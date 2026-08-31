


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
void entry_deep_sleep_mode(void);
void wake_up_from_deep_sleep_mode(void);
uint8_t A7169_GetData(uint8_t *buf,int len);
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




