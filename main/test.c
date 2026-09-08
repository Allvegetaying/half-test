/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "string.h"
#include "A7169/A7169.h"
#include "protocol.h"

// 传感器唤醒脚与传感器 TX 共线，工装侧共用 UART1 RX
#define SENSOR_WAKE_UART_GPIO GPIO_NUM_39
#define WAKEUP_GPIO_NUM       SENSOR_WAKE_UART_GPIO
#define CONTROL_GPIO_NUM1    GPIO_NUM_41
#define CONTROL_GPIO_NUM2    GPIO_NUM_42
#define GPIO40_FLOAT_INPUT   GPIO_NUM_40

#define BATTERY_ADC_GPIO              GPIO_NUM_1
#define BATTERY_ADC_UNIT              ADC_UNIT_1
#define BATTERY_ADC_CHANNEL           ADC_CHANNEL_0
#define BATTERY_ADC_ATTEN             ADC_ATTEN_DB_12
#define BATTERY_ADC_BITWIDTH          ADC_BITWIDTH_DEFAULT
#define BATTERY_ADC_SAMPLE_COUNT      16
#define BATTERY_ADC_SAMPLE_PERIOD_MS  1000

// Battery voltage = ADC pin voltage * NUM / DEN. Change this for the board divider.
#define BATTERY_DIVIDER_NUM           5011
#define BATTERY_DIVIDER_DEN           5000
#define BATTERY_ADC_REF_MV            3300
#define BATTERY_ADC_MAX_RAW           4095

// UART0 引脚定义
#define UART0_TX_PIN    43
#define UART0_RX_PIN    44
#define UART0_PORT      UART_NUM_0
#define UART0_BAUD_RATE 115200
#define BUF_SIZE        1024

// UART1 引脚定义
#define UART1_TX_PIN    17
#define UART1_RX_PIN    ((int)SENSOR_WAKE_UART_GPIO)
#define UART1_PORT      UART_NUM_1
#define UART1_BAUD_RATE 19200
#define BUF_SIZE        1024

// 传感器通信串口队列
static QueueHandle_t uart1_queue;
// 上位机通信串口队列
static QueueHandle_t uart0_queue;

// 事件标志组
static EventGroupHandle_t xEventFlags;
#define UART1_ENABLE_BIT   (1 << 0)   // UART1 使能位
#define RF_ENABLE_BIT      (1 << 1)   // 433 RF 使能位
#define UART1_DATA_READY   (1 << 2)   // UART1 数据就绪
#define RF_DATA_READY      (1 << 3)   // 433 数据就绪
#define ADC_ENABLE_BIT     (1 << 4)   // ADC 采样使能位

// 数据对比缓冲区
static char uart1_chip_id[PROTO_CHIP_ID_MAX + 1];
static proto_semi_t uart1_semi;
static char rf_chip_id[PROTO_CHIP_ID_MAX + 1];
static proto_rx_t uart1_rx;
static SemaphoreHandle_t uart1_rx_mutex;
static volatile uint8_t wakeup_break_expected = 0;
static volatile uint8_t wake_recv_flag = 0;
static adc_oneshot_unit_handle_t battery_adc_handle = NULL;
static adc_cali_handle_t battery_adc_cali_handle = NULL;
static bool battery_adc_cali_enabled = false;
/* 最近一次成功的 ADC 实测电池电压(mV)，-1 表示当前检测轮尚未采到有效值 */
static volatile int battery_adc_last_mv = -1;

// 函数声明
static uint8_t control_gpio_read_level(uint8_t gpio_num);
static void on_uart1_frame(const char *line, void *ctx);
static void uart1_rx_reset(void);
static void uart1_process_rx_bytes(const uint8_t *data, int len, const char *source);
static int uart1_poll_buffered_data(const char *source);
static esp_err_t battery_adc_init(void);
static esp_err_t battery_adc_read(int *raw_avg, int *battery_mv);

uint8_t state=0;

void GPIO_INIT()
{
    // 唤醒/传感器TX共线脚默认释放，UART1_INIT 后切到 UART RX 接收
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << WAKEUP_GPIO_NUM),
        .mode = GPIO_MODE_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // 配置两路开关引脚为输入模式
    gpio_config_t io_conf2 = {
        .pin_bit_mask = (1ULL << CONTROL_GPIO_NUM1) | (1ULL << CONTROL_GPIO_NUM2),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf2);

    gpio_config_t io_conf3 = {
        .pin_bit_mask = (1ULL << GPIO40_FLOAT_INPUT),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf3);

}

// 传感器通信串口
void UART1_INIT(void)
{
    // 配置UART1参数
    const uart_config_t uart_config = 
    {
        .baud_rate = UART1_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART1_PORT, BUF_SIZE * 2, BUF_SIZE, 20, &uart1_queue, 0);
    uart_param_config(UART1_PORT, &uart_config);
    uart_set_pin(UART1_PORT, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    printf("UART1 初始化完成 (TX: GPIO%d, RX: GPIO%d, 波特率: %d)\n",
           UART1_TX_PIN, UART1_RX_PIN, UART1_BAUD_RATE);
}
//上位机通信串口
void UART0_INIT(void)
{
    // 配置UART0参数
    const uart_config_t uart_config = 
    {
        .baud_rate = UART0_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART0_PORT, BUF_SIZE * 2, BUF_SIZE, 20, &uart0_queue, 0);
    uart_param_config(UART0_PORT, &uart_config);
    uart_set_pin(UART0_PORT, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    printf("UART0 初始化完成 (TX: GPIO%d, RX: GPIO%d, 波特率: %d)\n",
           UART0_TX_PIN, UART0_RX_PIN, UART0_BAUD_RATE);
}

// UART0发送字符串
static void uart0_send_string(const char* str)
{
    uart_write_bytes(UART0_PORT, str, strlen(str));
}

// 数据对比与结果上报
static void compare_and_report(void)
{
    EventBits_t bits = xEventGroupGetBits(xEventFlags);
    if (!(bits & UART1_DATA_READY) || !(bits & RF_DATA_READY))
        return;

    uint8_t result = proto_chip_id_equal(uart1_chip_id, rf_chip_id) ? 0 : 1;
    char frame[PROTO_FRAME_MAX + 1];

    /* 上报帧的 BAT_V 用工装 ADC 实测电压替换传感器自报值（本检测轮已采到才用） */
    proto_semi_t semi = uart1_semi;
    int adc_mv = battery_adc_last_mv;
    if (adc_mv >= 0)
        semi.bat_v = (float)adc_mv / 1000.0f;

    if (proto_build_semi_result(frame, sizeof(frame), result, &semi) > 0)
        uart0_send_string(frame);

    printf("SEMI_RESULT UART_ID=%s RF_ID=%s RESULT=%u",
           uart1_chip_id, rf_chip_id, result);
    if (adc_mv >= 0)
        printf(" BAT_ADC=%d.%03dV\n", adc_mv / 1000, adc_mv % 1000);
    else
        printf(" BAT_SENSOR=%.2fV(无ADC采样，沿用传感器上报)\n", uart1_semi.bat_v);

    uart1_chip_id[0] = '\0';
    memset(&uart1_semi, 0, sizeof(uart1_semi));
    rf_chip_id[0] = '\0';
    xEventGroupClearBits(xEventFlags,
                         UART1_DATA_READY | RF_DATA_READY |
                         UART1_ENABLE_BIT | RF_ENABLE_BIT |
                         ADC_ENABLE_BIT);
}

// 重置对比状态，准备下一轮检测
static void reset_compare_state(void)
{
    uart1_chip_id[0] = '\0';
    memset(&uart1_semi, 0, sizeof(uart1_semi));
    rf_chip_id[0] = '\0';
    wake_recv_flag = 0;
    xEventGroupClearBits(xEventFlags,
                         UART1_DATA_READY | RF_DATA_READY |
                         UART1_ENABLE_BIT | RF_ENABLE_BIT |
                         ADC_ENABLE_BIT);
    uart0_send_string("RESET: 已重置，等待下一轮检测\r\n");
}


static void uart1_restore_rx_pin(void)
{
     uart_set_pin(UART1_PORT, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //  gpio_pullup_en(UART1_RX_PIN);
    // gpio_pulldown_dis(UART1_RX_PIN);
}

//唤醒/传感器TX共线脚控制：拉低时切 GPIO 开漏输出，释放时恢复 UART RX
static void wakeup_gpio_set_level(uint32_t level)
{
    if (level == 0)
    {
        wakeup_break_expected = 1;
        uart_disable_rx_intr(UART1_PORT);
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << WAKEUP_GPIO_NUM),
            .mode = GPIO_MODE_OUTPUT_OD,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);
        gpio_set_level(WAKEUP_GPIO_NUM, 0);
        return;
    }

    gpio_set_level(WAKEUP_GPIO_NUM, 1);
    uart1_restore_rx_pin();
    uart_enable_rx_intr(UART1_PORT);
}

// 定义状态
#define PIN_ACTIVE   1   // 激活状态
#define PIN_STANDBY  0   // 待机状态

// 唤醒序列参数
#define WAKEUP_ATTEMPTS      10     // 唤醒拉低次数
#define WAKEUP_LOW_PULSE_MS  50     // 单次拉低脉冲宽度(ms)
#define WAKEUP_LISTEN_MS     1500   // 每次拉低释放后的监听窗口(ms)
                                     // DUT 从"被唤醒 -> 上电启动 -> 回传第一帧"通常要数百 ms，
                                     // 窗口太短时，迟到的响应会在下一轮唤醒的清队列操作里被冲掉。

// 唤醒次数内传感器是否已回传数据


// A7169 GIO1 中断引脚定义 (GPIO10)
#define A7169_GIO1_IRQ_PIN  GPIO_NUM_10

// 任务句柄
static TaskHandle_t xDetectTaskHandle = NULL;
static TaskHandle_t xWorkerTaskHandle = NULL;
static TaskHandle_t xRfRecvTaskHandle = NULL;  // 433 RF接收任务句柄

// GPIO41+GPIO42 双引脚检测任务：同时低电平启动，同时高电平等待下一轮
#define START_GPIO1   CONTROL_GPIO_NUM1   // GPIO41
#define START_GPIO2   CONTROL_GPIO_NUM2   // GPIO42

static void pin_detect_task(void *pvParameters)
{
    printf("双引脚检测任务启动 (GPIO%d + GPIO%d)\n", START_GPIO1, START_GPIO2);
    printf("规则：两引脚同时为低 -> 启动采集；同时为高 -> 等待下一轮\n");

    uint8_t last_state = PIN_STANDBY;

    // 读取初始状态
    uint8_t lv1 = control_gpio_read_level(START_GPIO1);
    uint8_t lv2 = control_gpio_read_level(START_GPIO2);
    if (lv1 == 0 && lv2 == 0)
    {
        printf("初始状态：GPIO%d=0 GPIO%d=0 -> 通知激活\n", START_GPIO1, START_GPIO2);
        last_state = PIN_ACTIVE;
        xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
    }
    else
    {
        printf("初始状态：GPIO%d=%d GPIO%d=%d -> 通知待机\n", START_GPIO1, lv1, START_GPIO2, lv2);
        last_state = PIN_STANDBY;
        xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
    }

    while (1)
    {
        lv1 = control_gpio_read_level(START_GPIO1);
        lv2 = control_gpio_read_level(START_GPIO2);

        if (lv1 == 0 && lv2 == 0 && last_state != PIN_ACTIVE)
        {
            // 两引脚同时低电平 -> 激活
            printf("GPIO%d=0 GPIO%d=0 -> 通知激活\n", START_GPIO1, START_GPIO2);
            last_state = PIN_ACTIVE;
            xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
        }
        else if (lv1 == 1 && lv2 == 1 && last_state != PIN_STANDBY)
        {
            // 两引脚同时高电平 -> 待机，准备下一轮
            printf("GPIO%d=1 GPIO%d=1 -> 通知待机（等待下一轮）\n", START_GPIO1, START_GPIO2);
            last_state = PIN_STANDBY;
            reset_compare_state();
            xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
        }
        // 其他情况（一个高一个低）保持当前状态不变

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// 传感器唤醒序列：每500ms拉低一次，共3次；期间收到数据视为成功，否则判定失败
static void sensor_wakeup_sequence(void)
{
    uart1_chip_id[0] = '\0';
    memset(&uart1_semi, 0, sizeof(uart1_semi));
    rf_chip_id[0] = '\0';
    xEventGroupClearBits(xEventFlags,
                         UART1_DATA_READY | RF_DATA_READY |
                         UART1_ENABLE_BIT | RF_ENABLE_BIT |
                         ADC_ENABLE_BIT);
    wake_recv_flag = 0;
    battery_adc_last_mv = -1;   /* 新一轮开始，旧轮 ADC 值作废 */
    wakeup_gpio_set_level(1);
    vTaskDelay(pdMS_TO_TICKS(10));
    uart_flush_input(UART1_PORT);
    xQueueReset(uart1_queue);
    uart1_rx_reset();
    xEventGroupSetBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT | ADC_ENABLE_BIT);

    for (int i = 0; i < WAKEUP_ATTEMPTS; i++)
    {
        wakeup_gpio_set_level(0);
        vTaskDelay(pdMS_TO_TICKS(WAKEUP_LOW_PULSE_MS));
        uart_flush_input(UART1_PORT);
        xQueueReset(uart1_queue);
        uart1_rx_reset();
        wakeup_gpio_set_level(1);
        for (int left = WAKEUP_LISTEN_MS; left > 0 && !wake_recv_flag; left -= 50)
        {
            uart1_poll_buffered_data("轮询");
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        if (wake_recv_flag)
        {
            printf(">>> 唤醒第%d次收到数据，判定成功\n", i + 1);
            return;
        }
    }

    wakeup_gpio_set_level(1);
    printf(">>> WAKEUP_FAIL：%d次唤醒、每次监听%ums内均未收到数据\n",
           WAKEUP_ATTEMPTS, WAKEUP_LISTEN_MS);
    uart0_send_string("WAKEUP_FAIL\r\n");
    xEventGroupClearBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT | ADC_ENABLE_BIT);
}
static void worker_up_task(void *pvParameters)
{
    uint32_t state = PIN_STANDBY;
    printf("工作任务启动，等待通知...\n");
    while (1)
    {
        // 等待任务通知
        if (xTaskNotifyWait(0, 0, &state, portMAX_DELAY) == pdTRUE)
        {
            if (state == PIN_ACTIVE)
            {
                printf(">>> 收到激活通知，开始工作！\n");
                sensor_wakeup_sequence();  // 执行唤醒序列（拉低3次，未收到数据则判定失败）
            }
            else
            {
                printf(">>> 收到待机通知，进入待机状态\n");
                wakeup_gpio_set_level(1);  // 设置唤醒引脚为高电平
                xEventGroupClearBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT | ADC_ENABLE_BIT);  // 失能 UART1 和 433接收模式
            }
        }
    }
}
//两路开关引脚读取
static uint8_t control_gpio_read_level(uint8_t gpio_num)
{
    return gpio_get_level(gpio_num);
}

// GPIO10 中断服务程序 (A7169 GIO1)
static void IRAM_ATTR gpio10_isr_handler(void* arg)
{
    state=1;
    
    if (xRfRecvTaskHandle != NULL) 
    {
        xTaskNotifyFromISR(xRfRecvTaskHandle, 1, eSetValueWithOverwrite, NULL);
    }
}

// 初始化 GPIO10 为中断模式
static void GPIO10_IRQ_INIT(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << A7169_GIO1_IRQ_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // 下降沿触发中断
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(A7169_GIO1_IRQ_PIN, gpio10_isr_handler, (void*) A7169_GIO1_IRQ_PIN);

    printf("GPIO10 中断初始化完成 (A7169 GIO1, 下降沿触发)\n");
}

static void on_uart1_frame(const char *line, void *ctx)
{
    (void)ctx;
    EventBits_t bits = xEventGroupGetBits(xEventFlags);
    if (!(bits & UART1_ENABLE_BIT) || (bits & UART1_DATA_READY))
        return;

    uint16_t calc = 0;
    if (proto_crc_check(line, &calc) != 0)
    {
        printf("UART1 CRC 校验失败 (计算值 0x%04X): %s\n", calc, line);
        return;
    }

    proto_semi_t st;
    if (proto_parse_semi(line, &st) != 0)
    {
        printf("UART1 非 SEMI_TEST 或解析失败: %s\n", line);
        return;
    }

    printf("UART1 解析: MODEL=%s CHIP_ID=%s PRESS=%.1f TEMP=%.1f ACC_Z=%.2f BAT_V=%.2f\n",
           st.model, st.chip_id, st.press, st.temp, st.acc_z, st.bat_v);

    // 原始帧存入对比缓冲区
    strncpy(uart1_chip_id, st.chip_id, sizeof(uart1_chip_id) - 1);
    uart1_chip_id[sizeof(uart1_chip_id) - 1] = '\0';
    uart1_semi = st;
    xEventGroupSetBits(xEventFlags, UART1_DATA_READY);
    wake_recv_flag = 1;   // 标记唤醒序列已收到数据
    compare_and_report();
}

static void uart1_rx_reset(void)
{
    if (uart1_rx_mutex && xSemaphoreTake(uart1_rx_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        uart1_rx.len = 0;
        uart1_rx.buf[0] = '\0';
        xSemaphoreGive(uart1_rx_mutex);
        return;
    }

    uart1_rx.len = 0;
    uart1_rx.buf[0] = '\0';
}

static void uart1_process_rx_bytes(const uint8_t *data, int len, const char *source)
{
    (void)source;

    if (!data || len <= 0)
        return;

    if (uart1_rx_mutex)
        xSemaphoreTake(uart1_rx_mutex, portMAX_DELAY);

    proto_rx_feed(&uart1_rx, data, len, on_uart1_frame, NULL);

    if (uart1_rx_mutex)
        xSemaphoreGive(uart1_rx_mutex);
}

static int uart1_poll_buffered_data(const char *source)
{
    uint8_t data[BUF_SIZE];
    size_t buffered = 0;
    int total = 0;

    if (uart_get_buffered_data_len(UART1_PORT, &buffered) != ESP_OK || buffered == 0)
        return 0;

    while (buffered > 0)
    {
        size_t want = buffered;
        if (want >= BUF_SIZE)
            want = BUF_SIZE - 1;

        int len = uart_read_bytes(UART1_PORT, data, want, 0);
        if (len <= 0)
            break;

        uart1_process_rx_bytes(data, len, source);
        total += len;

        if ((size_t)len >= buffered)
            break;
        buffered -= (size_t)len;
    }

    return total;
}

//传感器数据接收函数
static void uart1_discard_event_data(const uart_event_t *event, uint8_t *data, size_t data_size)
{
    if (!event || !data || data_size == 0)
        return;

    if (event->type == UART_DATA)
    {
        size_t remaining = event->size;
        while (remaining > 0)
        {
            size_t chunk = remaining;
            if (chunk > data_size)
                chunk = data_size;

            int len = uart_read_bytes(UART1_PORT, data, chunk, 0);
            if (len <= 0)
                break;

            remaining -= (size_t)len;
        }
        uart1_rx_reset();
        return;
    }

    if (event->type == UART_FIFO_OVF ||
        event->type == UART_BUFFER_FULL ||
        event->type == UART_BREAK)
    {
        wakeup_break_expected = 0;
        uart_flush_input(UART1_PORT);
        xQueueReset(uart1_queue);
        uart1_rx_reset();
    }
}

static void uart1_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t data[BUF_SIZE];
    while (1) 
    {
        // 等待UART事件
        if (xQueueReceive(uart1_queue, &event, portMAX_DELAY)) 
        {
            // 未使能时丢弃数据，不处理
            memset(data, 0, sizeof(data));
            if (!(xEventGroupGetBits(xEventFlags) & UART1_ENABLE_BIT))
            {
                uart1_discard_event_data(&event, data, sizeof(data));
                continue;
            }
            switch (event.type) 
            {
                case UART_DATA:  // 收到数据
                    // 读取数据
                    if (event.size >= BUF_SIZE) event.size = BUF_SIZE - 1;
                    int len = uart_read_bytes(UART1_PORT, data, event.size, pdMS_TO_TICKS(100));
                    uart1_process_rx_bytes(data, len, "事件");
                    break;
                case UART_FIFO_OVF:  // FIFO溢出
                    printf("UART1 FIFO 溢出\n");
                    uart_flush_input(UART1_PORT);
                    xQueueReset(uart1_queue);
                    uart1_rx_reset();
                    break;

                case UART_BUFFER_FULL:  // 缓冲区满
                    printf("UART1 缓冲区满\n");
                    uart_flush_input(UART1_PORT);
                    xQueueReset(uart1_queue);
                    uart1_rx_reset();
                    break;

                case UART_PARITY_ERR:  // 校验错误
                    printf("UART1 校验错误\n");
                    break;

                case UART_FRAME_ERR:  // 帧错误
                    printf("UART1 帧错误\n");
                    break;

                case UART_BREAK:
                    if (wakeup_break_expected)
                    {
                        wakeup_break_expected = 0;
                    }
                    else
                    {
                        uart_flush_input(UART1_PORT);
                        xQueueReset(uart1_queue);
                        uart1_rx_reset();
                    }
                    break;

                default:
                    printf("UART1 事件类型: %d\n", event.type);
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

// 433 RF 数据接收任务 (中断方式)
static void rf_recv_task(void *pvParameters)
{
    uint8_t rf_buf[64];
    uint32_t notify_value;
    bool rf_enabled_prev = false;

    printf("433 RF 接收任务启动 (中断模式)\n");

    while (1)
    {
        bool rf_enabled = (xEventGroupGetBits(xEventFlags) & RF_ENABLE_BIT) != 0;

        if (!rf_enabled)
        {
            // 未使能：天线仍在收帧，A7169 收到帧会把 GIO1 拉低并一直锁存。
            // 若不清走，下次使能后 GIO1 无法产生下降沿，中断接收会失效，故静默排空复位。
            if (GIO1S == 0)
                A7169_RxFifoReset();

            // 丢弃 RF 关闭期间残留的中断通知，避免使能后读到陈旧数据
            xTaskNotifyWait(0, UINT32_MAX, &notify_value, 0);
            rf_enabled_prev = false;
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        // 由关到开的瞬间：先排空残留帧、复位 GIO1，保证后续真帧能触发下降沿中断
        if (!rf_enabled_prev)
        {
            A7169_RxFifoReset();
            printf(">>> RF 已使能，接收通路已复位，等待433数据中断...\n");
        }
        rf_enabled_prev = true;

        // 等待 GPIO10 中断通知 (超时 100ms)
        if (xTaskNotifyWait(0, 0, &notify_value, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            // 收到中断通知，读取数据
            uint8_t len = A7169_GetData(rf_buf, RF_NORMAL_FRAME_LEN - 1);
            if (len > 0)
            {
                if (xEventGroupGetBits(xEventFlags) & RF_DATA_READY)
                    continue;

                rf_normal_data_t rf_data;
                if (A7169_ParseNormalData(rf_buf, len, &rf_data))
                {
                    printf("433 normal data: ID=%02X%02X%08" PRIX32
                           " ACC=%.1fg TEMP=%dC PRESS=%.1fkPa "
                           "vendor=0x%02X type=0x%02X status=0x%02X\n",
                           rf_data.vendor_type,
                           rf_data.sensor_type,
                           rf_data.sensor_id,
                           rf_data.acceleration_g,
                           rf_data.temperature_c,
                           rf_data.pressure_kpa,
                           rf_data.vendor_type,
                           rf_data.sensor_type,
                           rf_data.status);

                    if (proto_build_rf_chip_id(rf_chip_id, sizeof(rf_chip_id),
                                               rf_data.vendor_type,
                                               rf_data.sensor_type,
                                               rf_data.sensor_id) > 0)
                    {
                        xEventGroupSetBits(xEventFlags, RF_DATA_READY);
                        compare_and_report();
                    }
                }
            }
        }
    }
}
// ADC采集任务
static esp_err_t battery_adc_init(void)
{
    adc_unit_t unit = ADC_UNIT_1;
    adc_channel_t channel = ADC_CHANNEL_0;
    esp_err_t ret = adc_oneshot_io_to_channel(BATTERY_ADC_GPIO, &unit, &channel);
    if (ret != ESP_OK) {
        return ret;
    }

    if (unit != BATTERY_ADC_UNIT || channel != BATTERY_ADC_CHANNEL) {
        return ESP_ERR_INVALID_STATE;
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = BATTERY_ADC_UNIT,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ret = adc_oneshot_new_unit(&init_config, &battery_adc_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };

    ret = adc_oneshot_config_channel(battery_adc_handle, BATTERY_ADC_CHANNEL, &channel_config);
    if (ret != ESP_OK) {
        adc_oneshot_del_unit(battery_adc_handle);
        battery_adc_handle = NULL;
        return ret;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = BATTERY_ADC_UNIT,
        .chan = BATTERY_ADC_CHANNEL,
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };

    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &battery_adc_cali_handle);
    if (ret == ESP_OK) {
        battery_adc_cali_enabled = true;
    }
#endif

    return ESP_OK;
}

static esp_err_t battery_adc_read(int *raw_avg, int *battery_mv)
{
    if (!battery_adc_handle || !raw_avg || !battery_mv) {
        return ESP_ERR_INVALID_ARG;
    }

    int raw_sum = 0;
    for (int i = 0; i < BATTERY_ADC_SAMPLE_COUNT; i++) {
        int raw = 0;
        esp_err_t ret = adc_oneshot_read(battery_adc_handle, BATTERY_ADC_CHANNEL, &raw);
        if (ret != ESP_OK) {
            return ret;
        }
        raw_sum += raw;
    }

    *raw_avg = raw_sum / BATTERY_ADC_SAMPLE_COUNT;

    int adc_mv = 0;
    if (battery_adc_cali_enabled)
    {
        esp_err_t ret = adc_cali_raw_to_voltage(battery_adc_cali_handle, *raw_avg, &adc_mv);
        if (ret != ESP_OK) {
            return ret;
        }
    } else {
        adc_mv = (*raw_avg * BATTERY_ADC_REF_MV) / BATTERY_ADC_MAX_RAW;
    }

    *battery_mv = (adc_mv * BATTERY_DIVIDER_NUM) / BATTERY_DIVIDER_DEN;
    return ESP_OK;
}

static void adc_read_task(void *pvParameters)
{
    (void)pvParameters;

    esp_err_t ret = battery_adc_init();
    if (ret != ESP_OK) {
        printf("Battery ADC init failed: %s\n", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    printf("Battery ADC ready: GPIO%d ADC%d_CH%d\n",
           BATTERY_ADC_GPIO, BATTERY_ADC_UNIT + 1, BATTERY_ADC_CHANNEL);

    while (1)
    {
        xEventGroupWaitBits(xEventFlags, ADC_ENABLE_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        int raw = 0;
        int battery_mv = 0;
        ret = battery_adc_read(&raw, &battery_mv);
        if (ret == ESP_OK && (xEventGroupGetBits(xEventFlags) & ADC_ENABLE_BIT))
        {
            battery_adc_last_mv = battery_mv;
            printf("BAT=%d.%03dV raw=%d\n", battery_mv / 1000, battery_mv % 1000, raw);
        }
        vTaskDelay(pdMS_TO_TICKS(BATTERY_ADC_SAMPLE_PERIOD_MS));
    }
}
void app_main(void)
{
    GPIO_INIT();
    UART0_INIT();
    UART1_INIT();

    // 创建事件标志组 存放各种状态标志位
    xEventFlags = xEventGroupCreate();
    uart1_rx_mutex = xSemaphoreCreateMutex();
    if (InitRF() == 0)
    {
        printf("433 RF 初始化成功\n");
    } else
    {
        printf("433 RF 初始化失败\n");
    }
    GPIO10_IRQ_INIT();
    xTaskCreate(rf_recv_task, "rf_recv", 4096, NULL, 9, &xRfRecvTaskHandle);
    xTaskCreate(uart1_event_task, "uart1_event", 4096, NULL, 8, NULL);
    xTaskCreate(worker_up_task, "worker_up", 2048, NULL, 6, &xWorkerTaskHandle);
    xTaskCreate(pin_detect_task, "pin_detect", 2048, NULL, 7, &xDetectTaskHandle);
    // 设置唤醒引脚为高电平，通知进入工作状态
    // xEventGroupSetBits(xEventFlags, RF_ENABLE_BIT);
    //创建ADC采集任务
    xTaskCreate(adc_read_task, "adc_read", 2048, NULL, 4, NULL);
    while (1)
    {
        // 主循环空闲，数据接收由 rf_recv_task 通过中断方式处理
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
