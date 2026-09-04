/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
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
#include "string.h"
#include "A7169/A7169.h"
#include "protocol.h"

// 传感器唤醒脚与传感器 TX 共线，工装侧共用 UART1 RX
#define SENSOR_WAKE_UART_GPIO GPIO_NUM_39
#define WAKEUP_GPIO_NUM       SENSOR_WAKE_UART_GPIO
#define CONTROL_GPIO_NUM1    GPIO_NUM_5
#define CONTROL_GPIO_NUM2    GPIO_NUM_6
#define CONTROL_GPIO_NUM3    GPIO_NUM_40

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

// 数据对比缓冲区
static char uart1_chip_id[PROTO_CHIP_ID_MAX + 1];
static char rf_chip_id[PROTO_CHIP_ID_MAX + 1];
static proto_rx_t uart1_rx;
static SemaphoreHandle_t uart1_rx_mutex;
static volatile uint8_t wakeup_break_expected = 0;

// 函数声明
static uint8_t control_gpio_read_level(uint8_t gpio_num);
static void on_uart1_frame(const char *line, void *ctx);
static void uart1_rx_reset(void);
static void uart1_process_rx_bytes(const uint8_t *data, int len, const char *source);
static int uart1_poll_buffered_data(const char *source);

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

    //初始化GPIO40
    gpio_config_t io_conf3 = {
        .pin_bit_mask = (1ULL << GPIO_NUM_40),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf3);

    gpio_set_level(GPIO_NUM_40, 1);  // 默认高电平，释放传感器 TX 共线脚

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

// UART1发送字符串
static void uart1_send_string(const char* str)
{
    uart_write_bytes(UART1_PORT, str, strlen(str));
}

// UART1接收数据
static int uart1_receive_data(uint8_t* data, size_t max_len)
{
    int len = uart_read_bytes(UART1_PORT, data, max_len, pdMS_TO_TICKS(100));
    return len;
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
    char frame[96];

    if (proto_build_semi_result(frame, sizeof(frame), uart1_chip_id, result) > 0)
        uart0_send_string(frame);

    printf("SEMI_RESULT UART_ID=%s RF_ID=%s RESULT=%u\n",
           uart1_chip_id, rf_chip_id, result);

    uart1_chip_id[0] = '\0';
    rf_chip_id[0] = '\0';
    xEventGroupClearBits(xEventFlags, UART1_DATA_READY | RF_DATA_READY);
}

// 重置对比状态，准备下一轮检测
static void reset_compare_state(void)
{
    uart1_chip_id[0] = '\0';
    rf_chip_id[0] = '\0';
    xEventGroupClearBits(xEventFlags, UART1_DATA_READY | RF_DATA_READY);
    uart0_send_string("RESET: 已重置，等待下一轮检测\r\n");
}

// 按键重置任务 
static void button_reset_task(void *pvParameters)
{
    printf("按键重置任务启动 (GPIO%d, GPIO%d)\n", CONTROL_GPIO_NUM1, CONTROL_GPIO_NUM2);

    while (1) 
    {
        uint8_t level1 = control_gpio_read_level(CONTROL_GPIO_NUM1);
        uint8_t level2 = control_gpio_read_level(CONTROL_GPIO_NUM2);

        // 任一按键按下（低电平）触发重置
        if (level1 == 0 || level2 == 0) 
        {
            reset_compare_state();
            // 等待按键释放，避免重复触发
            while (control_gpio_read_level(CONTROL_GPIO_NUM1) == 0 || control_gpio_read_level(CONTROL_GPIO_NUM2) == 0) 
            {
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
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
// 独立于 UART1_DATA_READY，避免被 compare_and_report 提前清除
static volatile uint8_t wake_recv_flag = 0;

#define DETECT_GPIO_NUM    GPIO_NUM_4

// A7169 GIO1 中断引脚定义 (GPIO10)
#define A7169_GIO1_IRQ_PIN  GPIO_NUM_10

// 任务句柄
static TaskHandle_t xDetectTaskHandle = NULL;
static TaskHandle_t xWorkerTaskHandle = NULL;
static TaskHandle_t xRfRecvTaskHandle = NULL;  // 433 RF接收任务句柄

// 引脚检测任务
static void pin_detect_task(void *pvParameters)
{
    uint8_t lastState = PIN_STANDBY;

    // 配置 GPIO5 为输入模式
    gpio_config_t io_conf = 
    {
        .pin_bit_mask = (1ULL << DETECT_GPIO_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    printf("引脚检测任务启动 (GPIO%d)\n", DETECT_GPIO_NUM);

    // 读取初始电平
    uint8_t initLevel = gpio_get_level(DETECT_GPIO_NUM);
    lastState = initLevel;
    if (initLevel == 0) {
        printf("初始状态：GPIO%d=0 -> 通知激活\n", DETECT_GPIO_NUM);
        xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
    } else {
        printf("初始状态：GPIO%d=1 -> 通知待机\n", DETECT_GPIO_NUM);
        xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
    }

    while (1)
    {
        // 读取引脚电平
        uint8_t currentState = gpio_get_level(DETECT_GPIO_NUM);
        // 状态变化时才通知
        if (currentState != lastState)
        {
            if (currentState == 0)
            {
                printf("GPIO%d 变0 -> 通知激活\n", DETECT_GPIO_NUM);
                // 发送激活通知
                xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
            } 
            else
            {
                printf("GPIO%d 变1 -> 通知待机\n", DETECT_GPIO_NUM);
                xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
            }
            lastState = currentState;
        }
        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms 检测一次
    }
}

// 传感器唤醒序列：每500ms拉低一次，共3次；期间收到数据视为成功，否则判定失败
static void sensor_wakeup_sequence(void)
{
    xEventGroupSetBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT);
    uart1_chip_id[0] = '\0';
    rf_chip_id[0] = '\0';
    xEventGroupClearBits(xEventFlags, UART1_DATA_READY | RF_DATA_READY);
    wake_recv_flag = 0;
    wakeup_gpio_set_level(1);
    vTaskDelay(pdMS_TO_TICKS(10));
    uart_flush_input(UART1_PORT);
    xQueueReset(uart1_queue);
    uart1_rx_reset();

    for (int i = 0; i < WAKEUP_ATTEMPTS; i++)
    {
        wakeup_gpio_set_level(0);
        printf(">>> 唤醒第%d次：拉低唤醒引脚\n", i + 1);
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
    xEventGroupClearBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT);
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
                xEventGroupClearBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT);  // 失能 UART1 和 433接收模式
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
    if (!data || len <= 0)
        return;

    printf("UART1 %s收到 %d 字节: ", source ? source : "", len);
    for (int i = 0; i < len; i++) {
        printf("[%d]=%d(0x%02X) ", i, data[i], data[i]);
    }
    printf("\n");

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

    printf("UART1 %s缓冲区已有 %u 字节\n", source ? source : "", (unsigned)buffered);

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
            if (!(xEventGroupGetBits(xEventFlags) & UART1_ENABLE_BIT)) continue;
            memset(data, 0, sizeof(data));
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
                        printf("UART1 BREAK 事件来自本轮唤醒低电平，保留后续接收队列\n");
                    }
                    else
                    {
                        printf("UART1 BREAK 事件，清理异常低电平产生的串口状态\n");
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

    printf("433 RF 接收任务启动 (中断模式)\n");

    while (1) {
        if (!(xEventGroupGetBits(xEventFlags) & RF_ENABLE_BIT))
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        // 等待 GPIO10 中断通知 (超时 100ms)
        if (xTaskNotifyWait(0, 0, &notify_value, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            // 收到中断通知，读取数据
            uint8_t len = A7169_GetData(rf_buf, RF_NORMAL_FRAME_LEN - 1);
            if (len > 0)
            {
                printf("433 收到 %d 字节: ", len);
                for (int i = 0; i < len; i++)
                {
                    printf("0x%02X ", rf_buf[i]);
                }
                printf("\n");

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
static void adc_read_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    xTaskCreate(button_reset_task, "button_reset", 2048, NULL, 5, NULL);
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
