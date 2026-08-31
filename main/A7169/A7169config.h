#ifndef __A7169CONFIG_H
#define __A7169CONFIG_H
#include "stdint.h"


//#define DR_19K2bps_100KIFBW      //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_10Kbps_50KIFBW       //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
//#define DR_10Kbps_100KIFBW      //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_50Kbps_50KIFBW       //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
//#define DR_100Kbps_100KIFBW     //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_150Kbps_150KIFBW     //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
//#define DR_250Kbps_250KIFBW     //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
//#define DR_500Kbps_500KIFBW     //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz

#ifdef DR_19K2bps_100KIFBW
const uint16_t A7169Config[]=        //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x089C,             //[00]System Clock
    0x0821,             //[01]PLL1
    0xD405,             //[02]PLL2
    0x0000,             //[03]PLL3
    0x0A20,             //[04]PLL4
    0x0000,             //[05]PLL5
    0x0000,             //[06]PLL6
    0x0027,             //[07]Crystal
    0x0000,             //[08]PageA
    0x0000,             //[09]PageB
    0x18D4,             //[0A]RX1
    0x7009,             //[0B]RX2
    0x4000,             //[0C]ADC
    0x0800,             //[0D]Pin Control
    0x6C00,             //[0E]Calibration
    0x20C0,             //[0F]Mode Control
};

const uint16_t A7169Config_PageA[]=      //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0xF706,             //[PageA-00]TX1
    0xFC00,             //[PageA-01]WOR1
    0xF811,             //[PageA-02]WOR2
    0x0907,             //[PageA-03]RFI
    0x8A10,             //[PageA-04]PM
    0x0303,             //[PageA-05]RTH
    0x400F,             //[PageA-06]AGC
    0x0DC0,             //[PageA-07]AGC2
    0x4001,             //[PageA-08]GPIO
    0xFA01,             //[PageA-09]CKO
    0x0048,             //[PageA-0A]VCB
    0x6921,             //[PageA-0B]CHG1 //430.001
    0x0F21,             //[PageA-0C]CHG2 //435.001
    0x003F,             //[PageA-0D]FIFO
    0x1507,             //[PageA-0E]CODE
    0x87E0,             //[PageA-0F]WCAL
};

const uint16_t A7169Config_PageB[]=      //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0337,             //[PageB-00]TX2
    0x83D7,             //[PageB-01]IF1
    0x0000,             //[PageB-02]IF2
    0x0000,             //[PageB-03]ACK
    0x0000,             //[PageB-04]ART
    0x3C00,             //[PageB-05]SYN
    0xF177,             //[PageB-06]RCCAL
    0x0000,             //[PageB-07]ACKFIFO
    0x0064,             //[PageB-08]PNCFG1
    0x0010,             //[PageB-09]PNCFG2
    0x0001,             //[PageB-0A]PNCFG3
    0x0000,             //[PageB-0B]PNCFG4
    0x0001,             //[PageB-0C]PNCFG5
    0x0000,             //[PageB-0D]TCODE
    0x0000,             //[PageB-0E]PN DC
    0x0036,             //[PageB-0F]PNCFG6
    0x0C24,             //[PageB-10]PNCFG7
    0xA521,             //[PageB-11]PNCFG8
    0xF045,             //[PageB-12]PNCFG9
    0x003F,             //[PageB-13]PNCFG10
    0x4442,             //[PageB-14]Misc_CFG1
    0x0729,             //[PageB-15]Misc_CFG2
    0x0040,             //[PageB-16]Misc_CFG3
    0x0021,             //[PageB-17]PLL7
    0xD405,             //[PageB-18]PLL8
    0x0000,             //[PageB-19]PS MODE1
    0x0000,             //[PageB-1A]PS MODE2
    0x0000,             //[PageB-1B]PS MODE3
    0x0000,             //[PageB-1C]TX3
    0x0060,             //[PageB-1D]Misc_CFG4
    0x0000,             //[PageB-1E]PNCFG11
    0x0000,             //[PageB-1F]PNCFG12
    0x0000,             //[PageB-20]PNCFG13
    0x0000,             //[PageB-21]PNCFG14
    0x0000,             //[PageB-22]PNCFG15
    0x0000,             //[PageB-23]PNCFG16
    0x0000,             //[PageB-24]PNCFG17
    0x000F,             //[PageB-25]PNCFG18
    0x0000,             //[PageB-26]CSMA1
    0x0000,             //[PageB-27]CSMA2
    0x0000,             //[PageB-27]TXPA
};
#endif


#ifdef DR_10Kbps_50KIFBW

const uint16_t A7169Config[]=        //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x0803,     //SYSTEM CLOCK register,
    0x6021,     //PLL1 register,
    0xDA05,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D0,     //RX1 register,     IFBW=50KHz  
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0xF606,     //TX1 register,     Fdev = 18.75kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,      CST=1
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x0001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6921,     //CHG1 register,    430MHz
    0x0F21,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x0337,     //TX2 register,
    0x8200,     //IF1 register,     Enable Auto-IF, IF=100KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4442,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0042,     //Misc_CFG3 register,
    0x0021,     //PLL7 register,
    0xDA05,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000	    //WOR3 register
};

#endif


#ifdef DR_10Kbps_100KIFBW

const uint16_t A7169Config[]=        //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x1201,     //SYSTEM CLOCK register,
    0x6021,     //PLL1 register,
    0xDA05,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D4,     //RX1 register,     IFBW=100KHz 
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0xF706,     //TX1 register,     Fdev = 37.5kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,      CST=1
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x0001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6921,     //CHG1 register,    430MHz
    0x0F21,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0337,     //TX2 register,
    0x8400,     //IF1 register,     Enable Auto-IF, IF=200KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4442,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0042,     //Misc_CFG3 register,
    0x0021,     //PLL7 register,
    0xDA05,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};

#endif


#ifdef DR_50Kbps_50KIFBW

const uint16_t A7169Config[]=        //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x0003,     //SYSTEM CLOCK register,
    0x6021,     //PLL1 register,
    0xDA05,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D0,     //RX1 register,     IFBW=50KHz  
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0xF606,     //TX1 register,     Fdev = 18.75kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,      CST=1
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x0001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6921,     //CHG1 register,    430MHz
    0x0F21,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x0337,     //TX2 register,
    0x8200,     //IF1 register,     Enable Auto-IF, IF=100KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4442,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0042,     //Misc_CFG3 register,
    0x0021,     //PLL7 register,
    0xDA05,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};

#endif


#ifdef DR_100Kbps_100KIFBW

const uint16_t A7169Config[]=        //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0001,     //SYSTEM CLOCK register,
    0x6021,     //PLL1 register,
    0xDA05,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D4,     //RX1 register,     IFBW=100KHz
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0xF706,     //TX1 register,     Fdev = 37.5kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x0001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0004,     //VCB register,
    0x6921,     //CHG1 register,    430MHz
    0x0F21,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0337,     //TX2 register,
    0x8400,     //IF1 register,     Enable Auto-IF, IF=200KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4442,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0040,     //Misc_CFG3 register,
    0x0021,     //PLL7 register,
    0xDA05,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};

#endif



#ifdef DR_150Kbps_150KIFBW

const uint16_t A7169Config[]=        //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
{
    0x0001,     //SYSTEM CLOCK register,
    0x3216,     //PLL1 register,
    0x9158,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D8,     //RX1 register,     IFBW=150KHz 
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
{
    0xF706,     //TX1 register,     Fdev = 56.25kHz
    0x0000,     //WOR1 register,
    0xF800,     //WOR2 register,
    0x1907,     //RFI register,     Enable Tx Ramp up/down  
    0x1B70,     //PM register,
    0x0302,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0AC0,     //AGC2 register, 
    0x4001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6616,     //CHG1 register,    430MHz
    0x2A16,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
{
    0x8337,     //TX2 register,
    0x8400,     //IF1 register,     Enable Auto-IF, IF=300KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4442,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0040,     //Misc_CFG3 register,
    0x0016,     //PLL7 register,
    0x9158,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};

#endif


#ifdef DR_250Kbps_250KIFBW

const uint16_t A7169Config[]=        //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
{
    0x0000,     //SYSTEM CLOCK register,
    0x6A1B,     //PLL1 register,
    0x14D0,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18DC,     //RX1 register,     IFBW=250KHz 
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
{
    0xF70C,     //TX1 register,     Fdev = 93.75kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x8001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6E1A,     //CHG1 register,    430MHz
    0x231B,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
{
    0x0337,     //TX2 register,
    0x8800,     //IF1 register,     Enable Auto-IF, IF=500KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4443,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0040,     //Misc_CFG3 register,
    0x001D,     //PLL7 register,
    0x6004,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};

#endif


#ifdef DR_500Kbps_500KIFBW     

const uint16_t A7169Config[]=        //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
{
    0x0000,     //SYSTEM CLOCK register,
    0x181B,     //PLL1 register,
    0x14D0,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0000,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0025,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18CC,     //RX1 register,     IFBW=250KHz 
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x6C00,     //CALIBRATION register,
    0x60C0      //MODE CONTROL register,    Use FIFO mode
};

const uint16_t A7169Config_PageA[]=   //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
{
    0xF718,     //TX1 register,     Fdev = 187.5kHz
    0xFC00,     //WOR1 register,
    0xF811,     //WOR2 register,
    0x0907,     //RFI register,     Enable Tx Ramp up/down  
    0x8A10,     //PM register,
    0x0303,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0DC0,     //AGC2 register, 
    0x8001,     //GIO register,     GIO1=WTR
    0xFA01,     //CKO register
    0x0048,     //VCB register,
    0x6E1A,     //CHG1 register,    430MHz
    0x231B,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x87E0      //WCAL register,
};

const uint16_t A7169Config_PageB[]=   //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
{
    0x0337,     //TX2 register,
    0x8800,     //IF1 register,     Enable Auto-IF, IF=500KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000,     //ART register,
    0x3800,     //SYN register,
    0xF177,     //RCCAL register,
    0x0000,     //ACKFIFO register,
    0x0064,     //PNCFG1 register,
    0x0010,     //PNCFG2 register,
    0x0001,     //PNCFG3 register,
    0x0000,		  //PNCFG4 register,
    0x0001,     //PNCFG5 register,
    0x0000,     //TCODE register,
    0x0000,     //PN DC register,
    0x0036,     //PNCFG6 register,
    0x0C24,     //PNCFG7 register,
    0xA521,     //PNCFG8 register,
    0xF045,     //PNCFG9 register,
    0x003F,     //PNCFG10 register,
    0x4443,     //Misc_CFG1 register,
    0x0729,     //Misc_CFG2 register,
    0x0050,     //Misc_CFG3 register,
    0x001B,     //PLL7 register,
    0x14D0,     //PLL8 register,
    0x0000,     //PS MODE1 register,
    0x0000,     //PS MODE2 register,
    0x0000,     //PS MODE3 register,
    0x0000,     //TX3 register,
    0x0060,     //Misc_CFG4 register,
    0x0000,     //PNCFG11 register,
    0x0000,     //PNCFG12 register,
    0x0000,     //PNCFG13 register,
    0x0000,     //PNCFG14 register,
    0x0000,     //PNCFG15 register,
    0x0000,     //PNCFG16 register,
    0x0000,     //PNCFG17 register,
    0x000F,     //PNCFG18 register,
    0x0000,     //CSMA1 register,
    0x0000,     //CSMA2 register,
    0x0000,     //TXPA register,
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x0000,     //MBUS1 register,
    0x0000,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x0000,     //MBUS4 register,
    0x0000,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000     	//WOR3 register
};
#endif

#endif





