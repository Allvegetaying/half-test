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
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"

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

void GPIO_INIT()
{
    // 配置GPIO2为输出模式
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << WAKEUP_GPIO_NUM) || (1ULL << CONTROL_GPIO_NUM1) || (1ULL << CONTROL_GPIO_NUM2),  // 选择GPIO2
        .mode = GPIO_MODE_OUTPUT,                 // 输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,        // 禁用上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE,    // 禁用下拉
        .intr_type = GPIO_INTR_DISABLE            // 禁用中断
    };
    gpio_config(&io_conf);

}

// 传感器通信串口
void UART1_INIT(void)
{
    // 配置UART1参数
    const uart_config_t uart_config = {
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
    const uart_config_t uart_config = {
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

//唤醒引脚控制
static void wakeup_gpio_set_level(uint32_t level)
{
    gpio_set_level(WAKEUP_GPIO_NUM, level);
}

//两路开关控制引脚
static void control_gpio_set_level(uint32_t level)
{
    gpio_set_level(CONTROL_GPIO_NUM1, level);
    gpio_set_level(CONTROL_GPIO_NUM2, level);
}

//传感器数据接收函数
static void uart1_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t data[BUF_SIZE];

    while (1) {
        // 等待UART事件
        if (xQueueReceive(uart1_queue, &event, portMAX_DELAY)) {
            memset(data, 0, sizeof(data));

            switch (event.type) {
                case UART_DATA:  // 收到数据
                    // 读取数据
                    uart_read_bytes(UART1_PORT, data, event.size, pdMS_TO_TICKS(100));
                    data[event.size] = '\0';

                    // 调试：打印原始数据
                    printf("收到 %d 字节: ", event.size);
                    for (int i = 0; i < event.size; i++) {
                        printf("[%d]=%d(0x%02X) ", i, data[i], data[i]);
                    }
                    printf("\n");

                    // 回显
                    uart1_send_string("Echo: ");
                    uart1_send_string((char*)data);
                    uart1_send_string("\r\n");

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

// //上位机接收函数（应答的时候使用）
// static void uart0_event_task(void *pvParameters)
// {
//     uart_event_t event;
//     uint8_t data[BUF_SIZE];

//     while (1) {
//         // 等待UART事件
//         if (xQueueReceive(uart0_queue, &event, portMAX_DELAY)) {
//             memset(data, 0, sizeof(data));

//             switch (event.type) {
//                 case UART_DATA:  // 收到数据
//                     // 读取数据
//                     uart_read_bytes(UART1_PORT, data, event.size, pdMS_TO_TICKS(100));
//                     data[event.size] = '\0';

//                     // 在VSCode串口监视器显示
//                     printf("UART1 收到 %d 字节: %s\n", event.size, data);

//                     uart1_send_string("Echo: ");
//                     uart1_send_string((char*)data);
//                     uart1_send_string("\r\n");
//                     // uint8_t command =0; 
//                     if(event.size == 0)
//                     {
//                         printf("UART1 收到空数据\n");
//                         memset(data, 0, sizeof(data));
//                         break;
//                     }
//                     uint8_t ID = 0;
//                     uint8_t stress = 0;   
//                     uint8_t  temperature = 0;
//                     uint8_t  acceleration = 0;
//                     uint8_t  battery_voltage =0;
//                     ID = data[0];      //ID
//                     stress = data[1]; //压力
//                     temperature = data[2]; // 温度
//                     acceleration = data[3]; // 加速度
//                     battery_voltage = data[4]; // 电池电压
//                     printf("UART1 解析数据: ID=%d, 压力=%d, 温度=%d, 加速度=%d, 电池电压=%d\n",
//                            data[0], data[1], data[2], data[3], data[4]);    
//                     break;
//                 case UART_FIFO_OVF:  // FIFO溢出
//                     printf("UART1 FIFO 溢出\n");
//                     uart_flush_input(UART1_PORT);
//                     xQueueReset(uart1_queue);
//                     break;

//                 case UART_BUFFER_FULL:  // 缓冲区满
//                     printf("UART1 缓冲区满\n");
//                     uart_flush_input(UART1_PORT);
//                     xQueueReset(uart1_queue);
//                     break;

//                 case UART_PARITY_ERR:  // 校验错误
//                     printf("UART1 校验错误\n");
//                     break;

//                 case UART_FRAME_ERR:  // 帧错误
//                     printf("UART1 帧错误\n");
//                     break;

//                 default:
//                     printf("UART1 事件类型: %d\n", event.type);
//                     break;
//             }
//         }
//     }
//     vTaskDelete(NULL);
// }
//向上位机发送结果（uart1，协议待定需要与厂商进行对接一下）
// static void uart1_send_string(const char* str)
// {
//     uart_write_bytes(UART1_PORT, str, strlen(str));
// }

void app_main(void)
{
    GPIO_INIT();
    UART0_INIT();
    UART1_INIT();

    // xTaskCreate(uart0_event_task, "uart0_event_task", 4096, NULL, 12, NULL);
    xTaskCreate(uart1_event_task, "uart1_event_task", 4096, NULL, 12, NULL);

    // 主循环
    while (1)
    {
        // uart1_send_string("Hello, World!\r\n");
        vTaskDelay(pdMS_TO_TICKS(2000));  // 延时2秒
    }
}
