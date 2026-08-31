#ifndef __BLT_GPIO_H
#define __BLT_GPIO_H

#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

/*
 * A7169 SPI GPIO 引脚定义 (位操作方式)
 * 根据实际硬件接线修改以下引脚号
 */
#define A7169_CS_PIN        GPIO_NUM_5      // 片选 SCS
#define A7169_CLK_PIN       GPIO_NUM_6      // 时钟 SCK
#define A7169_DIO_PIN       GPIO_NUM_7      // 数据 SDIO (双向)
#define A7169_GIO1_PIN      GPIO_NUM_15     // GIO1 (状态指示)

/* 片选 SCS */
#define SCS_MODE_OUT        do { \
                                gpio_reset_pin(A7169_CS_PIN); \
                                gpio_set_direction(A7169_CS_PIN, GPIO_MODE_OUTPUT); \
                            } while(0)
#define SCS_L               gpio_set_level(A7169_CS_PIN, 0)
#define SCS_H               gpio_set_level(A7169_CS_PIN, 1)

/* 时钟 SCK */
#define SCK_MODE_OUT        do { \
                                gpio_reset_pin(A7169_CLK_PIN); \
                                gpio_set_direction(A7169_CLK_PIN, GPIO_MODE_OUTPUT); \
                            } while(0)
#define SCK_L               gpio_set_level(A7169_CLK_PIN, 0)
#define SCK_H               gpio_set_level(A7169_CLK_PIN, 1)

/* 数据 SDIO (双向) */
#define MOSI_MODE_OUT       gpio_set_direction(A7169_DIO_PIN, GPIO_MODE_OUTPUT)
#define MOSI_MODE_INPUT     gpio_set_direction(A7169_DIO_PIN, GPIO_MODE_INPUT)
#define SDIO_H              gpio_set_level(A7169_DIO_PIN, 1)
#define SDIO_L              gpio_set_level(A7169_DIO_PIN, 0)
#define SDIO_READ           gpio_get_level(A7169_DIO_PIN)

/* GIO1 输入 (状态指示) */
#define GIOD_MODE_INPUT     do { \
                                gpio_reset_pin(A7169_GIO1_PIN); \
                                gpio_set_direction(A7169_GIO1_PIN, GPIO_MODE_INPUT); \
                            } while(0)
#define GIOD_READ           gpio_get_level(A7169_GIO1_PIN)

/* 延时函数 */
#define WaitMs(ms)          vTaskDelay(pdMS_TO_TICKS(ms))
#define WaitnUs(us)         esp_rom_delay_us(us)

/* 打印函数 */
#define rt_kprintf          printf

/*
 * CRC16 (CCITT) 计算函数
 * @param buf  数据缓冲区
 * @param len  数据长度
 * @return CRC16 值
 */
static inline uint16_t CRC16(const uint8_t *buf, uint8_t len)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;
    while (len--)
    {
        crc ^= (*buf++);
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

#endif /* __BLT_GPIO_H */
