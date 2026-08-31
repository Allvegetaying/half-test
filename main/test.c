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
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"
#include "A7169/A7169.h"

// GPIO2 引脚定义
#define WAKEUP_GPIO_NUM      GPIO_NUM_2
#define CONTROL_GPIO_NUM1    GPIO_NUM_3
#define CONTROL_GPIO_NUM2    GPIO_NUM_4

// UART0 引脚定义
#define UART0_TX_PIN    43
#define UART0_RX_PIN    44
#define UART0_PORT      UART_NUM_0
#define UART0_BAUD_RATE 115200
#define BUF_SIZE        1024

// UART1 引脚定义
#define UART1_TX_PIN    17
#define UART1_RX_PIN    18
#define UART1_PORT      UART_NUM_1
#define UART1_BAUD_RATE 115200
#define BUF_SIZE        1024

// 传感器通信串口队列
static QueueHandle_t uart1_queue;
// 上位机通信串口队列
static QueueHandle_t uart0_queue;

// 事件标志组：统一管理各模块使能状态
static EventGroupHandle_t xEventFlags;
#define UART1_ENABLE_BIT   (1 << 0)   // UART1 使能位
#define RF_ENABLE_BIT      (1 << 1)   // 433 RF 使能位
#define UART1_DATA_READY   (1 << 2)   // UART1 数据就绪
#define RF_DATA_READY      (1 << 3)   // 433 数据就绪

// 数据对比缓冲区
static uint8_t uart1_cmp_buf[64];
static uint8_t uart1_cmp_len = 0;
static uint8_t rf_cmp_buf[64];
static uint8_t rf_cmp_len = 0;



// 函数声明
static uint8_t control_gpio_read_level(uint8_t gpio_num);


void GPIO_INIT()
{
    // 配置唤醒引脚为输出模式
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << WAKEUP_GPIO_NUM),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
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
    // 两边数据都就绪才对比
    EventBits_t bits = xEventGroupGetBits(xEventFlags);
    if (!(bits & UART1_DATA_READY) || !(bits & RF_DATA_READY))
        return;

    // 取最小长度对比
    uint8_t cmp_len = (uart1_cmp_len < rf_cmp_len) ? uart1_cmp_len : rf_cmp_len;

    if (cmp_len == 0) {
        uart0_send_string("FAIL: 数据长度为0\r\n");
        goto clear;
    }

    // 逐字节对比
    int match = 1;
    for (uint8_t i = 0; i < cmp_len; i++) {
        if (uart1_cmp_buf[i] != rf_cmp_buf[i]) {
            match = 0;
            break;
        }
    }

    if (match && uart1_cmp_len == rf_cmp_len) {
        uart0_send_string("PASS: 数据一致\r\n");
    } else {
        uart0_send_string("FAIL: 数据不一致\r\n");
        // 打印对比详情
        char msg[128];
        snprintf(msg, sizeof(msg), "UART1[%d]: ", uart1_cmp_len);
        uart0_send_string(msg);
        for (uint8_t i = 0; i < uart1_cmp_len; i++) {
            snprintf(msg, sizeof(msg), "0x%02X ", uart1_cmp_buf[i]);
            uart0_send_string(msg);
        }
        uart0_send_string("\r\n");
        snprintf(msg, sizeof(msg), "RF[%d]:    ", rf_cmp_len);
        uart0_send_string(msg);
        for (uint8_t i = 0; i < rf_cmp_len; i++) {
            snprintf(msg, sizeof(msg), "0x%02X ", rf_cmp_buf[i]);
            uart0_send_string(msg);
        }
        uart0_send_string("\r\n");
    }

clear:
    // 清除就绪标志，等待下一轮数据
    xEventGroupClearBits(xEventFlags, UART1_DATA_READY | RF_DATA_READY);
}

// 重置对比状态，准备下一轮检测
static void reset_compare_state(void)
{
    memset(uart1_cmp_buf, 0, sizeof(uart1_cmp_buf));
    uart1_cmp_len = 0;
    memset(rf_cmp_buf, 0, sizeof(rf_cmp_buf));
    rf_cmp_len = 0;
    xEventGroupClearBits(xEventFlags, UART1_DATA_READY | RF_DATA_READY);
    uart0_send_string("RESET: 已重置，等待下一轮检测\r\n");
}



// 按键重置任务 - GPIO3/GPIO4 按下时重置
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

//唤醒引脚控制
static void wakeup_gpio_set_level(uint32_t level)
{
    gpio_set_level(WAKEUP_GPIO_NUM, level);
}

// 定义状态
#define PIN_ACTIVE   1   // 激活状态
#define PIN_STANDBY  0   // 待机状态

// 检测引脚定义（避开 A7169 片选 GPIO5，改用 GPIO8）
#define DETECT_GPIO_NUM    GPIO_NUM_8

// 任务句柄
static TaskHandle_t xDetectTaskHandle = NULL;
static TaskHandle_t xWorkerTaskHandle = NULL;

// 引脚检测任务 - 读取电平并通知其他任务
static void pin_detect_task(void *pvParameters)
{
    uint8_t lastState = PIN_STANDBY;

    // 配置 GPIO5 为输入模式
    gpio_config_t io_conf = 
    {
        .pin_bit_mask = (1ULL << DETECT_GPIO_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    printf("引脚检测任务启动 (GPIO%d)\n", DETECT_GPIO_NUM);

    // 读取初始电平，同步 lastState 并发送初始通知
    uint8_t initLevel = gpio_get_level(DETECT_GPIO_NUM);
    lastState = initLevel;
    if (initLevel == 1) {
        printf("初始状态：高电平 -> 通知激活\n");
        xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
    } else {
        printf("初始状态：低电平 -> 通知待机\n");
        xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
    }

    while (1)
    {
        // 读取引脚电平
        uint8_t currentState = gpio_get_level(DETECT_GPIO_NUM);

        // 状态变化时才通知
        if (currentState != lastState) 
        {
            if (currentState == 1) 
            {
                printf("检测到高电平 -> 通知激活\n");
                // 发送激活通知
                xTaskNotify(xWorkerTaskHandle, PIN_ACTIVE, eSetValueWithOverwrite);
            } else 
            {
                printf("检测到低电平 -> 通知待机\n");
                // 发送待机通知
                xTaskNotify(xWorkerTaskHandle, PIN_STANDBY, eSetValueWithOverwrite);
            }
            lastState = currentState;
        }

        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms 检测一次
    }
}

static void worker_up_task(void *pvParameters)
{
    uint32_t state = PIN_STANDBY;
    printf("工作任务启动，等待通知...\n");
    while (1) {
        // 等待任务通知
        if (xTaskNotifyWait(0, 0, &state, portMAX_DELAY) == pdTRUE) 
        {
            if (state == PIN_ACTIVE) {
                printf(">>> 收到激活通知，开始工作！\n");
                wakeup_gpio_set_level(1);  // 设置唤醒引脚为高电平
                xEventGroupSetBits(xEventFlags, UART1_ENABLE_BIT | RF_ENABLE_BIT);  // 使能 UART1 和 433接收模式
                
            } else 
            {
                printf(">>> 收到待机通知，进入待机状态\n");
                wakeup_gpio_set_level(0);  // 设置唤醒引脚为低电平
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

//传感器数据接收函数
static void uart1_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t data[BUF_SIZE];

    while (1) {
        // 等待UART事件
        if (xQueueReceive(uart1_queue, &event, portMAX_DELAY)) {
            // 未使能时丢弃数据，不处理
            if (!(xEventGroupGetBits(xEventFlags) & UART1_ENABLE_BIT)) continue;
            memset(data, 0, sizeof(data));

            switch (event.type) {
                case UART_DATA:  // 收到数据
                    // 读取数据（防止越界）
                    if (event.size >= BUF_SIZE) event.size = BUF_SIZE - 1;
                    uart_read_bytes(UART1_PORT, data, event.size, pdMS_TO_TICKS(100));
                    data[event.size] = '\0';

                    // 调试：打印原始数据
                    printf("收到 %d 字节: ", event.size);
                    for (int i = 0; i < event.size; i++) {
                        printf("[%d]=%d(0x%02X) ", i, data[i], data[i]);
                    }
                    printf("\n");

                    if(event.size < 5)
                    {
                        // printf("数据不足5字节，跳过解析\n");
                        break;
                    }

                    // 解析数据
                    uint8_t ID = data[0];           //ID
                    uint8_t stress = data[1];       //压力
                    uint8_t temperature = data[2];  // 温度
                    uint8_t acceleration = data[3]; // 加速度
                    uint8_t battery_voltage = data[4]; // 电池电压

                    printf("解析: ID=%d, 压力=%d, 温度=%d, 加速度=%d, 电池电压=%d\n",
                           ID, stress, temperature, acceleration, battery_voltage);

                    // 存入对比缓冲区，标记就绪
                    uart1_cmp_len = (event.size < 64) ? event.size : 64;
                    memcpy(uart1_cmp_buf, data, uart1_cmp_len);
                    xEventGroupSetBits(xEventFlags, UART1_DATA_READY);
                    compare_and_report();
                    break;
                case UART_FIFO_OVF:  // FIFO溢出
                    printf("UART1 FIFO 溢出\n");
                    uart_flush_input(UART1_PORT);
                    xQueueReset(uart1_queue);
                    break;

                case UART_BUFFER_FULL:  // 缓冲区满
                    printf("UART1 缓冲区满\n");
                    uart_flush_input(UART1_PORT);
                    xQueueReset(uart1_queue);
                    break;

                case UART_PARITY_ERR:  // 校验错误
                    printf("UART1 校验错误\n");
                    break;

                case UART_FRAME_ERR:  // 帧错误
                    printf("UART1 帧错误\n");
                    break;

                default:
                    printf("UART1 事件类型: %d\n", event.type);
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

// 433 RF 数据接收任务
static void rf_recv_task(void *pvParameters)
{
    uint8_t rf_buf[64];

    printf("433 RF 接收任务启动\n");

    while (1) {
        if (!(xEventGroupGetBits(xEventFlags) & RF_ENABLE_BIT)) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        // 检查 GIO1S 引脚，高电平表示收到数据
        if (GIO1S) 
        {
            uint8_t len = A7169_GetData(rf_buf, 11);
            if (len > 0)
            {
                printf("433 收到 %d 字节: ", len);
                for (int i = 0; i < len; i++)
                {
                    printf("0x%02X ", rf_buf[i]);
                }
                printf("\n");

                // 存入对比缓冲区，标记就绪
                rf_cmp_len = (len < 64) ? len : 64;
                memcpy(rf_cmp_buf, rf_buf, rf_cmp_len);
                xEventGroupSetBits(xEventFlags, RF_DATA_READY);
                compare_and_report();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void app_main(void)
{
    GPIO_INIT();
    UART0_INIT();
    UART1_INIT();

    // 创建事件标志组 存放各种状态标志位
    xEventFlags = xEventGroupCreate();

    // 初始化 433 RF 模块
    if (InitRF() == 0) {
        printf("433 RF 初始化成功\n");
    } else {
        printf("433 RF 初始化失败\n");
    }
    // xTaskCreate(uart1_event_task, "uart1_event_task", 4096, NULL, 12, NULL);   //接收传感器数据
    // xTaskCreate(rf_recv_task, "rf_recv", 4096, NULL, 9, NULL);                 //接收433数据
    // // 先创建工作任务（接收通知方），确保句柄就绪
    // xTaskCreate(worker_up_task, "worker_up", 2048, NULL, 8, &xWorkerTaskHandle);
    // // 再创建引脚检测任务（发送通知方）
    // xTaskCreate(pin_detect_task, "pin_detect", 2048, NULL, 10, &xDetectTaskHandle);
    // // 创建按键重置任务
    // xTaskCreate(button_reset_task, "btn_reset", 2048, NULL, 7, NULL);
    // 主循环
    while (1)
    {
        uint8_t state = A7169_GetData(rf_cmp_buf, 11);
        if (state > 0)
        {
            printf("433 收到 %d 字节: ", state);
            for (int i = 0; i < state; i++)
            {
                printf("0x%02X ", rf_cmp_buf[i]);
            }
            printf("\n");
        }
      
        vTaskDelay(pdMS_TO_TICKS(100));  // 延时2秒
    }
}
