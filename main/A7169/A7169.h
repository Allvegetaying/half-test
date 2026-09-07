


/*!
    \file    A7169.h
    \brief   the header file of A7169

*/

#ifndef __A7169_H
#define __A7169_H


#include "blt_gpio.h"
#include "A7169reg.h"
#include "A7169config.h"
#include "stdbool.h"
#include "stdint.h"
#define AGC_ON  1

#define PRINTF  rt_kprintf

#define A7169_DelayMS     WaitMs
#define A7169_DelayUS     WaitnUs

//#define A7169GPIO_INIT()      GPIO_SPI_INTT()

#define A7169_CS_INIT           SCS_MODE_OUT
#define A7169_CLK_INIT          SCK_MODE_OUT
#define A7169_GIO_INIT          GIOD_MODE_INPUT
#define A7169_CS_L()  					SCS_L
#define A7169_CS_H()  					SCS_H
#define A7169_DIO_H() 		    	SDIO_H
#define A7169_DIO_L() 					SDIO_L
#define A7169_DIO_READ()        	SDIO_READ
#define A7169_CLK_H()		    		SCK_H
#define A7169_CLK_L() 					SCK_L
#define A7169_OutMode()  				MOSI_MODE_OUT
#define A7169_InMode() 	 				MOSI_MODE_INPUT   //DIOÊäÈë
#define GIO1S                   GIOD_READ
//#define GIO2S                  	GPIO_Pin_read(GPIO2)

#define RF_NORMAL_FRAME_LEN     12

typedef struct {
    uint8_t vendor_type;
    uint8_t sensor_type;
    uint32_t sensor_id;
    uint8_t acceleration_raw;
    uint8_t temperature_raw;
    uint8_t pressure_raw;
    uint8_t status;
    float acceleration_g;
    int16_t temperature_c;
    float pressure_kpa;
} rf_normal_data_t;

void entry_deep_sleep_mode(void);
void wake_up_from_deep_sleep_mode(void);
uint8_t A7169_ParseNormalData(const uint8_t *data, uint8_t len, rf_normal_data_t *out);
uint8_t A7169_GetData(uint8_t *buf,int len);
void A7169_RxFifoReset(void);
void RxPacket(void);
uint8_t InitRF(void);
uint8_t A7169_POR();
uint8_t A7169_Config(void);
uint8_t A7169_WriteID(void);
uint8_t A7169_Cal(void);
void A7169_WriteFIFO(void);
void entry_deep_sleep_mode(void);
void wake_up_from_deep_sleep_mode(void);
uint8_t  RSSI_Measurement(void);
uint16_t get_rssi(void);
#endif




