/*!
    \file    A7169.c
    \brief   A7169  driver

*/


#include "A7169.h"

uint16_t rssi = 0;
#define DR_19K2bps_100KIFBW      //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_10Kbps_50KIFBW       //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
//#define DR_10Kbps_100KIFBW      //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_50Kbps_50KIFBW       //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
//#define DR_100Kbps_100KIFBW     //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
//#define DR_150Kbps_150KIFBW     //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
//#define DR_250Kbps_250KIFBW     //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
//#define DR_500Kbps_500KIFBW     //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz

#ifdef DR_19K2bps_100KIFBW
const uint16_t A7169Config[] =       //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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
    0x1854,             //[0A]RX1 //18d4 ID允许错1 位  1854  不允许错
    0x7009,             //[0B]RX2
    0x4000,             //[0C]ADC
    0x0800,             //[0D]Pin Control
    0x6C00,             //[0E]Calibration
    0x20C0,             //[0F]Mode Control
};

const uint16_t A7169Config_PageA[] =     //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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
//    0x0013,
    0x001c,             //[PageA-0D]FIFO //0x4013  //0x0018
    //0x000F,
    0x1501,              //[PageA-0E]CODE  Preamble=2bytes, ID=2bytes
    //0x1507,             //[PageA-0E]CODE  Preamble=4bytes, ID=4bytes
    0x87E0,             //[PageA-0F]WCAL
};

const uint16_t A7169Config_PageB[] =     //433MHz, 19k2bps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0337,             //[PageB-00]TX2  //0337
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
    //
    0x0000,     //DCMON1 register,
    0x0000,     //DCMON2 register,
    0x0000,     //DCMON3 register,
    0x1143,     //MBUS1 register,
    0xc287,     //MBUS2 register,
    0x0000,     //MBUS3 register,
    0x1020,     //MBUS4 register,
    0x0080,     //MBUS5 register,
    0x0000,     //PN DC2 register,
    0x0000,     //VCB2 register,
    0x0000,     //PS MODE4 register
    0x0000	    //WOR3 register
};
#endif


#ifdef DR_10Kbps_50KIFBW

const uint16_t A7169Config[] =       //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 10kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config[] =       //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 10kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config[] =       //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 50kbps (IFBW = 50KHz, Fdev = 18.75KHz), Crystal=12.8MHz
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

const uint16_t A7169Config[] =       //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
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

const uint16_t A7169Config[] =       //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 150kbps (IFBW = 150KHz, Fdev = 56.25KHz), Crystal=19.2MHz
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

const uint16_t A7169Config[] =       //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 250kbps (IFBW = 250KHz, Fdev = 93.75KHz), Crystal=16MHz
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

const uint16_t A7169Config[] =       //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
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

const uint16_t A7169Config_PageA[] =  //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
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

const uint16_t A7169Config_PageB[] =  //433MHz, 500kbps (IFBW = 500KHz, Fdev = 187.5KHz), Crystal=16MHz
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



uint8_t       timer;
uint8_t       TimeoutFlag;
uint16_t      RxCnt;
uint32_t      Err_ByteCnt;
uint32_t      Err_BitCnt;
uint16_t      TimerCnt0;
uint8_t       *Uartptr;
uint8_t       UartSendCnt;
uint8_t       CmdBuf[11];
static uint8_t       tmpbuf[100];
uint8_t		  fb;
bool		  fb_ok;
uint16_t	  BODF;
static bool   RF_Init_Flg = false;
const uint8_t  BitCount_Tab[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
const uint8_t  ID_Tab[8] = {0x55, 0x56, 0x00, 0x00, 0xC7, 0x33, 0x45, 0xE7}; //ID code
const uint8_t  PN9_Tab[] =
{   0xFF, 0x83, 0xDF, 0x17, 0x32, 0x09, 0x4E, 0xD1,
    0xE7, 0xCD, 0x8A, 0x91, 0xC6, 0xD5, 0xC4, 0xC4,
    0x40, 0x21, 0x18, 0x4E, 0x55, 0x86, 0xF4, 0xDC,
    0x8A, 0x15, 0xA7, 0xEC, 0x92, 0xDF, 0x93, 0x53,
    0x30, 0x18, 0xCA, 0x34, 0xBF, 0xA2, 0xC7, 0x59,
    0x67, 0x8F, 0xBA, 0x0D, 0x6D, 0xD8, 0x2D, 0x7D,
    0x54, 0x0A, 0x57, 0x97, 0x70, 0x39, 0xD2, 0x7A,
    0xEA, 0x24, 0x33, 0x85, 0xED, 0x9A, 0x1D, 0xE0
};  // This table are 64bytes PN9 pseudo random code.

/**
*
*
* @brief  DIO读取
* @param NULL
* @return  NULL
*
**/
uint8_t A7169_DIO_IN(void)
{
    return A7169_DIO_READ() ;
}


/**
*
*
* @brief  读取一个字节
* @param NULL
* @return  NULL
*
**/

uint8_t A7169_ReadByte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for(i = 0; i < 8; i ++)
    {
        A7169_CLK_H();
        data <<= 1;
        if(A7169_DIO_IN())
        {
            data |= 1;
        }
        A7169_DelayUS(1);
        A7169_CLK_L();
        A7169_DelayUS(1);
    }

    return data;
}
/**
*
*
* @brief  写一个字节
* @param data  内容
* @return  NULL
*
**/
void A7169_WriteByte(uint8_t data)
{
    uint8_t i;

    for(i = 0; i < 8; i ++)
    {
        if(data & 0x80)
        {
            A7169_DIO_H();
        }
        else
        {
            A7169_DIO_L();
        }
        A7169_DelayUS(1);
        A7169_CLK_H();
        data <<= 1;
        A7169_DelayUS(1);
        A7169_CLK_L();
    }
}
/**
*
*
* @brief  写入控制寄存器
* @param RegAddr  地址
*        data     数据
* @return  NULL
*
**/

void A7169_WriteReg(uint8_t RegAddr, uint16_t data)
{
    RegAddr &= 0x0f;						//地址限制为BIT0-BIT3
    RegAddr |= CMD_Reg_W;				//写命令
    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(RegAddr);
    A7169_WriteByte(data >> 8);
    A7169_WriteByte(data);
    A7169_CS_H();
}
/**
*
*
* @brief 读控制寄存器
* @param RegAddr  地址
* @return  NULL
*
**/

uint16_t A7169_ReadReg(uint8_t RegAddr)
{
    uint16_t data = 0;
    RegAddr &= 0x0f;						//地址限制为BIT0-BIT3
    RegAddr |= CMD_Reg_R;				//读命令
    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(RegAddr);
    A7169_InMode();	//输入
    data = A7169_ReadByte();
    data <<= 8;
    data |= A7169_ReadByte();
    A7169_CS_H();

    return data;
}


void A7169_WritePageA(uint8_t address, uint16_t dataWord)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7169Config[CRYSTAL_REG]);
    A7169_WriteReg(CRYSTAL_REG, tmp);
    A7169_WriteReg(PAGEA_REG, dataWord);

}


uint16_t A7169_ReadPageA(uint8_t address)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7169Config[CRYSTAL_REG]);
    A7169_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7169_ReadReg(PAGEA_REG);
    return tmp;
}


void A7169_WritePageB(uint8_t address, uint16_t dataWord)
{
    uint16_t tmp;

    tmp = address;
    if ( tmp & 0x20 )
    {
        tmp = ( ( ( tmp & 0x1F ) << 7 ) | (A7169Config[CRYSTAL_REG]&~0x0F80) );
        A7169_WriteReg ( CRYSTAL_REG, tmp );
        A7169_WriteReg ( PIN_REG, ((A7169Config[PIN_REG]&~0x0001) | 0x01) );
    }
    else
    {
        tmp = ( ( ( tmp & 0x1F ) << 7 ) | (A7169Config[CRYSTAL_REG]&~0x0F80) );
        A7169_WriteReg ( CRYSTAL_REG, tmp );
        A7169_WriteReg ( PIN_REG, (A7169Config[PIN_REG]&~0x0001));
    }
    A7169_WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  A7169_ReadPageB
************************************************************************/
uint16_t A7169_ReadPageB(uint8_t address)
{
    uint16_t tmp;

    tmp = address;
    if ( tmp & 0x20 )
    {
        tmp = ( ( (  tmp & 0x1F ) << 7 ) | (A7169Config[CRYSTAL_REG]&~0x0F80) );
        A7169_WriteReg ( CRYSTAL_REG, tmp );
        A7169_WriteReg ( PIN_REG, ((A7169Config[PIN_REG]&~0x0001) | 0x01) );
    }
    else
    {
        tmp = ( ( ( tmp & 0x1F ) << 7 ) | (A7169Config[CRYSTAL_REG]&~0x0F80) );
        A7169_WriteReg ( CRYSTAL_REG, tmp );
        A7169_WriteReg ( PIN_REG, (A7169Config[PIN_REG]&~0x0001) );
    }
    tmp = A7169_ReadReg(PAGEB_REG);
    return tmp;
}

/*********************************************************************
** Strobe Command
*********************************************************************/

void A7169_StrobeCmd(uint8_t A7169_StrobeCmd)
{
    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(A7169_StrobeCmd);
    A7169_CS_H();
}

/*********************************************************************
** A7169_POR
*********************************************************************/
uint8_t  A7169_POR()
{

//    A7169GPIO_INIT();

    //power on only
    A7169_DelayMS(10);   			//for regulator settling time (power on only)

    A7169_StrobeCmd(CMD_RF_RST);  	//reset A7169 chip
    uint8_t cnt = 0;
    while(A7169_WriteID())		//check SPI
    {
        cnt++;
        if(cnt > 5)
            return 1;
        A7169_StrobeCmd(CMD_RF_RST);  //reset A7169 chip
    }
    A7169_WritePageA(PM_PAGEA, A7169Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    A7169_DelayMS(2);

    entry_deep_sleep_mode();		//deep sleep
    A7169_DelayMS(2);
    wake_up_from_deep_sleep_mode();	//wake up

    A7169_StrobeCmd(CMD_RF_RST);  	//reset A7169 chip
    cnt = 0;
    while(A7169_WriteID())		//check SPI
    {
        cnt++;
        if(cnt > 5)
            return 1;
        A7169_StrobeCmd(CMD_RF_RST);  //reset A7169 chip
    }
    A7169_WritePageA(PM_PAGEA, A7169Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    A7169_DelayMS(2);
    return 0;
}
/**
*
*
* @brief RF初始化
* @param NULL
* @return  NULL
*
**/
uint8_t InitRF(void)
{
    A7169_CS_INIT  ;
    A7169_CLK_INIT ;
    A7169_GIO_INIT ;
    A7169_OutMode();

    A7169_POR();	//power on only

    A7169_CS_H() ;
    A7169_CLK_L();
    A7169_DIO_H() ;


    A7169_DelayMS(1);            //delay 1ms for regulator stabilized
    A7169_StrobeCmd(CMD_RF_RST);  //reset A7169 chip
    A7169_DelayMS(1);

    if(A7169_Config())      //config A7169 chip
        return 1;

    A7169_DelayUS(8);          //delay 800us for crystal stabilized

    if(A7169_WriteID())     //write ID code
        return 1;

    if(A7169_Cal())         //IF and VCO Calibration
        return 1;
    A7169_WritePageA(GIO_PAGEA, (A7169Config_PageA[GIO_PAGEA] & 0xF000) | 0x0045);  //GIO1=FSYNC, GIO2=WTR
    A7169_StrobeCmd(CMD_RX);
    // RSSI_Measurement();
    RF_Init_Flg = true;

    return 0;
}
/**
*
*
* @brief 读取ID
* @param NULL
* @return  NULL
*
**/
uint32_t A7169ReadID(void)
{
    uint32_t data ;
    uint8_t i;

    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(CMD_ID_R);			//读ID命令
    A7169_InMode();	//输入
    data = 0;
    for(i = 0; i < 4; i ++)
    {
        data <<= 8;
        data |= A7169_ReadByte();
    }
    A7169_CS_H();

    return data;
}

/**
*
*
* @brief 写ID命令
* @param NULL
* @return  NULL
*
**/
void A7169WriteID(uint32_t ID)
{
    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(CMD_ID_W);			//写ID命令
    A7169_WriteByte(ID >> 24);
    A7169_WriteByte(ID >> 16);
    A7169_WriteByte(ID >> 8);
    A7169_WriteByte(ID >> 0);
    A7169_CS_H();
}


/*********************************************************************
** A7169_Config
*********************************************************************/
uint8_t A7169_Config(void)
{
    uint8_t i;
    uint16_t tmp;
//	for(i=0; i<8; i++)
//         A7169_WriteReg(i, A7169Config[i]);
//	for(i=10; i<16; i++)
//         A7169_WriteReg(i, A7169Config[i]);
//     for(i=0; i<16; i++)
//         A7169_WritePageA(i, A7169Config_PageA[i]);
//	for(i=0; i<5; i++)
//         A7169_WritePageB(i, A7169Config_PageB[i]);



    for(i = 0; i < 8; i++)
        A7169_WriteReg(i, A7169Config[i]);

    for(i = 10; i < 16; i++)
    {
        if((i == 14) && (fb_ok == 1))
            A7169_WriteReg(i, A7169Config[i] | (1 << 4));			//MIFS=1(Manual)
        else
            A7169_WriteReg(i, A7169Config[i]);
    }

    for(i = 0; i < 16; i++)
        A7169_WritePageA(i, A7169Config_PageA[i]);

    for(i = 0; i < 53; i++)
        A7169_WritePageB(i, A7169Config_PageB[i]);


    //for check
    tmp = A7169_ReadReg(SYSTEMCLOCK_REG);
    if(tmp != A7169Config[SYSTEMCLOCK_REG])
    {
        return 1;
    }

    return 0;
}


/************************************************************************
**  WriteID
************************************************************************/
uint8_t A7169_WriteID(void)
{
    uint8_t d1, d2, d3, d4;
    uint8_t i;
    A7169_CS_L();
    A7169_OutMode();
    A7169_WriteByte(CMD_ID_W);
    for(i = 0; i < 2; i++)
        A7169_WriteByte(ID_Tab[i]);
    A7169_CS_H();

    A7169_CS_L();
    A7169_WriteByte(CMD_ID_R);
    A7169_InMode();	//输入
    d1 = A7169_ReadByte();
    d2 = A7169_ReadByte();
    d3 = A7169_ReadByte();
    d4 = A7169_ReadByte();
    A7169_CS_H();
    if((d1 != ID_Tab[0]) || (d2 != ID_Tab[1]) || (d3 != ID_Tab[2]) || (d4 != ID_Tab[3]))
    {
        return 1;
    }

    return 0;
}
/*********************************************************************
** A7169_Cal
*********************************************************************/
uint8_t A7169_Cal(void)
{
    uint8_t i;
    uint8_t fb_old, fcd, fbcf;//IF Filter
    uint8_t vb, vbcf;         //VCO Current
    uint8_t vcb, vccf;        //VCO Band
    uint16_t tmp;
    static uint8_t fb_fail = 0;
    A7169_StrobeCmd(CMD_STBY);

    //IF calibration procedure @STB state
    if(fb_ok == 1)
    {
        A7169_WriteReg(MODE_REG, A7169Config[MODE_REG] | 0x0800);       //VCO Current Calibration
        do {
            tmp = A7169_ReadReg(MODE_REG);
        } while(tmp & 0x0800);
        tmp = (A7169Config[CALIBRATION_REG] & 0xFFE0);
        tmp = tmp | fb | (1 << 4);
        A7169_WriteReg(CALIBRATION_REG, tmp);
    }
    else
    {
        fb_fail = 0;

        for(i = 0; i < 3; i++)
        {
            A7169_WriteReg(MODE_REG, A7169Config[MODE_REG] | 0x0802);       //IF Filter & VCO Current Calibration
            do {
                tmp = A7169_ReadReg(MODE_REG);
            } while(tmp & 0x0802);

            //for check(IF Filter)
            tmp = A7169_ReadReg(CALIBRATION_REG);
            //printf("tmp %x\n",tmp);
            fb = tmp & 0x0F;
            //printf("fb %d\n",fb);
            fcd = (tmp >> 11) & 0x1F;
            //printf("fcd %x\n",fcd);
            fbcf = (tmp >> 4) & 0x01;
            //printf("fbcf %x\n",fbcf);

            if((fb < 4) || (fb > 8))
                fb_fail = 1;
            else
            {
                //printf("i %d\n",i);
                if(i == 0)
                    fb_old = fb;
                else
                {
                    //printf("els i %d\n",i);
                    if(fb != fb_old)
                        fb_fail = 1;
                }
            }

            if((fbcf) || (fb_fail))
            {
                //printf("fb if error\n");
                return 1;
            }
        }
    }

    //for check(VCO Current)
    tmp = A7169_ReadPageA(VCB_PAGEA);
    vcb = tmp & 0x0F;
    vccf = (tmp >> 4) & 0x01;
    if(vccf)
    {
        return 1;
    }


    //RSSI Calibration procedure @STB state
    A7169_WriteReg(ADC_REG, 0x4C00);                                    //set ADC average=64
    A7169_WriteReg(MODE_REG, A7169Config[MODE_REG] | 0x1000);           //RSSI Calibration
    do {
        tmp = A7169_ReadReg(MODE_REG);
    } while(tmp & 0x1000);
    A7169_WriteReg(ADC_REG, A7169Config[ADC_REG]);


    //VCO calibration procedure @STB state
//    A7169_WriteReg(PLL1_REG, A7169Config[PLL1_REG]);
//    A7169_WriteReg(PLL2_REG, A7169Config[PLL2_REG]);

    A7169_WriteReg(PLL1_REG, 0x0021);
    A7169_WriteReg(PLL2_REG, 0xE666);
//    A7169_WriteReg(CKO_PAGEA, 0xFA03);
    A7169_WriteReg(MODE_REG, A7169Config[MODE_REG] | 0x0004);       //VCO Band Calibration
    do {
        tmp = A7169_ReadReg(MODE_REG);
    } while(tmp & 0x0004);

    //for check(VCO Band)
    tmp = A7169_ReadReg(CALIBRATION_REG);
    vb = (tmp >> 5) & 0x07;
    vbcf = (tmp >> 8) & 0x01;
    if(vbcf)
    {
        return 1;
    }

    fb_ok = 1;
    return 0;
}
/**
*
*
* @brief 发送数据
* @param dat -- 发送数组首地址，leng -- 发送长度
* @return  NULL
*
**/

void A7169_sendata(uint8_t *dat, uint8_t len)
{
    uint8_t i;
    A7169_StrobeCmd(CMD_STBY);

    A7169_StrobeCmd(CMD_TFR);	          //重置Tx数据指针位置

    A7169_CS_L();
    A7169_WriteByte(CMD_FIFO_W);	  //“写FIFO缓存区”的命令
    for(i = 0; i < len; i++)
    {
        A7169_WriteByte(*(dat + i));
    }
    A7169_CS_H();

    A7169_StrobeCmd(CMD_TX);            //发送命令，
    A7169_DelayUS(10);
    while(GIO1S);                  //循环等待直至数据发送完毕，GIO2引脚将输出一个正脉冲信号

}


/*********************************************************************
** A7169_WriteFIFO
*********************************************************************/
void A7169_WriteFIFO(void)
{
    uint8_t i;

    A7169_StrobeCmd(CMD_TFR);     //TX FIFO address pointer reset

    A7169_CS_L();
    A7169_WriteByte(CMD_FIFO_W);   //TX FIFO write command
    for(i = 0; i < 64; i++)
        A7169_WriteByte(PN9_Tab[i]);
    A7169_CS_H();
}


// 曼彻斯特编码  5->00  6->01  9->10 A->11
//
uint8_t mancherCode[] = {
    0x55, 0x56, 0x59, 0x5a, // 0 1 2 3
    0x65, 0x66, 0x69, 0x6a, // 4 5 6 7
    0x95, 0x96, 0x99, 0x9a, // 8 9 A B
    0xa5, 0xa6, 0xa9, 0xaa, // C D E F
};

int  manchersetrAnlysis(uint8_t *des, uint8_t *src, int len)
{
    static uint8_t M_data = 0;
    for(int j = 0; j < len; j++)
    {

        for(int i = 0; i < 16; i++)
        {
            if(src[j * 2] == mancherCode[i])
                M_data = i;
        }

        for(int i = 0; i < 16; i++)
        {
            if(src[j * 2 + 1] == mancherCode[i])
                M_data = M_data << 4 ^ (i);
        }
        des[j] = M_data;
        M_data = 0;
    }
    return 0;
}

uint8_t ManchData(uint8_t *src, uint8_t *dec, int len)
{
    int i = 0;
    for(i = 0; i < len; i++)
    {
        dec[i * 2] = mancherCode[(src[i] >> 4)];
        dec[i * 2 + 1] =  mancherCode[(src[i] & 0x0f)];
    }
    return len;
}

//uint8_t ManchData(uint8_t *src,uint8_t *dec,int len)
//{
//    uint8_t res = 0;
//    int i = 0;
//    for(i = 0;i < len;i++)
//    {
//        dec[i*2] = mancherCode[(src[i]>>4)];
//        dec[i*2+1] =  mancherCode[(src[i]&0x0f)];
//    }
//    return len;
//}
uint8_t rf_rcv_flg()
{
    A7169_StrobeCmd(CMD_RX);
    return 0;
}


unsigned char CRC8(unsigned char *ptr, unsigned char len)
{
    unsigned char crc;
    unsigned char i;
    crc = 0;
    while(len--)
    {
        crc ^= *ptr++;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else crc <<= 1;
        }
    }
    return crc;
}

float getAver(int *buf, int len)
{
    float aver = 0;
    int sum = 0;
    for(int i = 0; i < len; i++)
    {
        sum += buf[i];
    }
    aver = sum / (len * 1.0);
    return aver;
}

static int _e6data[200];
static int _24data[200];

int  test_e6(int rssi)
{
    static int num1 = 0;
    _e6data[num1] = rssi;
    num1 ++;
    if(num1 > 100)
    {
        num1 = 0;
        memset(_e6data, 0, sizeof(_e6data));
    }

    return num1;
}

int  test_24(int rssi)
{
    static int num2 = 0;
    _24data[num2] = rssi;



    num2 ++;
    if(num2 > 100)
    {
        num2 = 0;
        memset(_24data, 0, sizeof(_24data));
    }

    return num2;
}
/*********************************************************************
** RxPacket
*********************************************************************/
bool check_RF_Init_Flg()
{
    return RF_Init_Flg;
}

uint8_t xor_check8(uint8_t *buf, uint8_t len)
{
	uint8_t x = 0;
	for(int i = 0; i < len; i++)
		x = x^(*(buf+i));
	return x;
}


uint8_t A7169_GetData(uint8_t *buf, int len)
{
    uint8_t res = 0;
    int decoded_len = len + 1;

//    rssi = RSSI_Measurement();
    // if (GIO1S == 0)
    //     return 0;

    static uint8_t data[64] = {0};
    memset(data, 0, sizeof(data));
	memset(tmpbuf,0,sizeof(tmpbuf));
    A7169_StrobeCmd(CMD_RFR);

    A7169_CS_L();
    A7169_WriteByte(CMD_FIFO_R);   //RX FIFO read command
    A7169_InMode();
    for(int i = 0; i < decoded_len * 2; i++)
        tmpbuf[i] = A7169_ReadByte();

    A7169_CS_H();
    uint8_t crc8 = 0;
//    printf("test: ");
//    for(int i= 0;i<11*2;i++)
//    	printf("%x ",tmpbuf[i]);
//    printf("\r\n");

    manchersetrAnlysis(data, tmpbuf, decoded_len);
    uint8_t rf_null[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    printf("test: ");
    for(int i= 0;i<12;i++)
    	printf("%x ",data[i]);
    printf("\r\n"); 

    if(memcmp(data, rf_null, 6) == 0)
    {
        A7169_StrobeCmd(CMD_RX);
        return 0;
    }
    uint8_t xor = 0;
	uint16_t crc16 = 0;
	crc16 = CRC16(data, 10);
    

	// if(data[10] == (crc16>>8) && (data[11] == (uint8_t)crc16) && crc16!=0  && (data[9]&0x20) == 0x20)//&& data[11] == crc16
    if(data[10] == (crc16>>8) && (data[11] == (uint8_t)crc16) && crc16!=0 )//&& data[11] == crc16
	{
		memcpy(buf, data, 12);
        res = 12;
        A7169_StrobeCmd(CMD_RX);
        return res;
	}
	crc16 = CRC16(data, 9);
	if(data[9] == (crc16>>8) && (data[10] == (uint8_t)crc16) && crc16!=0 && data[6] == 0x03) //10位数据 软件版本 \电量查询
	{
		memcpy(buf, data, 10);
		res = 10;
		A7169_StrobeCmd(CMD_RX);
		return res;
	}
    crc8 = CRC8(data, 10);

    if(data[10] == crc8 && crc8 != 0 && (data[9]&0x20) == 0x20) //11位数据 其他厂商数据
    {
        memcpy(buf, data, 11);
        res = 11;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }


    crc8 = CRC8(data, 9);
    if(data[9] == crc8 && crc8 != 0 && data[6] == 0x03) //10位数据 软件版本 \电量查询
    {
        memcpy(buf, data, 10);
        res = 10;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }

    xor = xor_check8(data,7);
	if(xor == data[7] && xor != 0 && data[0] != 0x0f && (data[5]&0x10) == 0x10 )
	{
		memcpy(buf, data, 8);
		res = 9;//万通
		A7169_StrobeCmd(CMD_RX);
		return res;
	}
    crc8 = CRC8(data, 7);
    if(crc8 == data[7] && crc8 != 0 && (data[6]&0x20) == 0x20 && data[0] != 0x0f) //8位数据 万通
    {
        memcpy(buf, data, 8);
        res = 8;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }

    A7169_StrobeCmd(CMD_RX);
    return res;
}


uint8_t A7169_GetData_test(uint8_t *buf, int len)
{
    uint8_t res = 0;
    uint16_t rssi = 0;
//    rssi = RSSI_Measurement();
    static uint8_t data[64] = {0};
    memset(data, 0, sizeof(data));

    A7169_StrobeCmd(CMD_RFR);

    A7169_CS_L();
    A7169_WriteByte(CMD_FIFO_R);   //RX FIFO read command
    A7169_InMode();
    for(int i = 0; i < (len + 1) * 2; i++)
        tmpbuf[i] = A7169_ReadByte();

    A7169_CS_H();
    uint8_t crc8 = 0;
//    PRINTF("测试未编码数据：");
//    for(int i= 0;i<11*2;i++)
//        PRINTF("%.2x ",tmpbuf[i]);
//    PRINTF("\r\n");


    manchersetrAnlysis(data, tmpbuf, len);
    uint8_t rf_null[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if(memcmp(data, rf_null, 6) == 0)
        return 0;

    crc8 = CRC8(data, 10);

    if(data[10] == crc8 && crc8 != 0) //11位数据 其他厂商数据
    {
        memcpy(buf, data, 11);
        res = 11;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }
    crc8 = CRC8(data, 9);
    if(data[9] == crc8 && crc8 != 0) //10位数据 软件版本
    {
        memcpy(buf, data, 10);
        res = 10;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }

    crc8 = CRC8(data, 7);
    if(crc8 == data[7] && crc8 != 0) //8位数据 万通
    {
        memcpy(buf, data, 8);
        res = 8;
        A7169_StrobeCmd(CMD_RX);
        return res;
    }

    return res;
}
//    {

//     printf("NOW_RSSIaver: RSSI %d RX: ",rssi);
//        for(int i= 0;i<len;i++)
//            printf("%.2x ",data[i]);
//        printf("\r\n");
//        RFDATA_filter(data,len);
//    }
//    if(crc8 == data[7] && ((data[6]&0x20) == 0x20))
//     {
//         int averlen = 0;
//         int aver = 0;
//         RFData_485_Report(data,7);
//          //printf("激励数据 NOW_RSSIaver: %d   RSSI %d RX: ",aver,rssi);
//            for(int i= 0;i<len;i++)
//                //printf("%.2x ",data[i]);
//            //printf("\r\n");
//         RFDATA_filter(data,len);
//  }

void RxPacket(void)
{
    uint16_t rssi = 0;

    A7169_StrobeCmd(CMD_RX);

    while((GIO1S == 0));       // wait receive completed
//    rssi = RSSI_Measurement();
//    rssi = (A7169_ReadReg(ADC_REG) & 0x00FF);        //read RSSI value(wanted signal RSSI)
//
    uint8_t i;
    uint8_t recv;
    uint8_t tmp;

    static uint8_t data[64] = {0};
    memset(data, 0, sizeof(data));
//    A7169_StrobeCmd(CMD_RX);
    RxCnt++;

    A7169_StrobeCmd(CMD_RFR);     //RX FIFO address pointer reset

    A7169_CS_L();
    A7169_WriteByte(CMD_FIFO_R);   //RX FIFO read command
    A7169_InMode();
    for(i = 0; i < 18; i++)
        tmpbuf[i] = A7169_ReadByte();

    A7169_CS_H();

//    printf("RSSI %.2x tmpbuf: ",rssi);
//    for(int i= 0;i<16;i++)
//        printf("%.2x ",tmpbuf[i]);
//    printf("\r\n");

    int len = 8;
    uint8_t crc8 = 0;
    manchersetrAnlysis(data, tmpbuf, len);
    crc8 = CRC8(data, len - 1);
}

/*********************************************************************
** entry_deep_sleep_mode
*********************************************************************/
void entry_deep_sleep_mode(void)
{
    A7169_StrobeCmd(CMD_RF_RST);              //RF reset
    A7169_WriteReg(PIN_REG, A7169Config[PIN_REG] | 0x0800);             //SCMDS=1
    A7169_WritePageA(PM_PAGEA, A7169Config_PageA[PM_PAGEA] | 0x0010);   //QDS=1
    A7169_StrobeCmd(CMD_SLEEP);               //entry sleep mode
    A7169_DelayUS(600);                      //delay 600us for VDD_A shutdown, C load=0.1uF
    A7169_StrobeCmd(CMD_DEEP_SLEEP);          //entry deep sleep mode
    A7169_DelayUS(200);                      //delay 200us for VDD_D shutdown, C load=0.1uF
}


/*********************************************************************
** wake_up_from_deep_sleep_mode
*********************************************************************/
void wake_up_from_deep_sleep_mode(void)
{
    A7169_StrobeCmd(CMD_STBY);    //wake up
    A7169_DelayMS(2);            //delay 2ms for VDD_D stabilized
    //InitRF();
}

/*********************************************************************
** BODF_Detect
*********************************************************************/
uint8_t BODF_Detect(void)
{
    uint16_t tmp;
    tmp = A7169_ReadPageA(WOR1_PAGEA);
    BODF = ((tmp >> 11) & 0x01);

    if(BODF)
    {
        return 1;
    }

    return 0;
}

/*********************************************************************
** RSSI_measurement
*********************************************************************/
uint8_t  RSSI_Measurement(void)
{
    uint16_t tmp;

    A7169_StrobeCmd(CMD_STBY);

    A7169_WritePageA(GIO_PAGEA, (A7169Config_PageA[GIO_PAGEA] & 0xF000) | 0x0045);  //GIO1=FSYNC, GIO2=WTR
#ifdef AGC_ON
    A7169_WritePageA(AGC1_PAGEA, (A7169Config_PageA[AGC1_PAGEA] | 0X0040)); //AGC
#endif
    A7169_WriteReg(ADC_REG, A7169Config[ADC_REG] | 0x8000);     //ARSSI=1

    A7169_StrobeCmd(CMD_RX);  //entry RX mode
    A7169_DelayMS(1);

#ifdef AGC_ON
    //AGC
//    while(GIO1S==0)      //Stay in RX mode until receiving ID code(ID detect ok)
//    {
//        tmp = (A7169_ReadReg(RX2_REG) );    //read RSSI value(environment RSSI)
//    }
    tmp = (A7169_ReadReg(RX2_REG));
#else
//    while(GIO1S==0)      //Stay in RX mode until receiving ID code(ID detect ok)
//    {
//        tmp = (A7169_ReadReg(ADC_REG) & 0x00FF);    //read RSSI value(environment RSSI)
//    }
    tmp = (A7169_ReadReg(ADC_REG) & 0x00FF);        //read RSSI value(wanted signal RSSI)
#endif

    return tmp;
}


uint16_t get_rssi()
{
    uint16_t tmp = 0;
    static bool flag = 0;
    if(!flag)
    {
        flag = 1;
        A7169_StrobeCmd(CMD_STBY);
        A7169_WritePageA(GIO_PAGEA, (A7169Config_PageA[GIO_PAGEA] & 0xF000) | 0x0045);  //GIO1=FSYNC, GIO2=WTR
#ifdef AGC_ON
        A7169_WritePageA(AGC1_PAGEA, (A7169Config_PageA[AGC1_PAGEA] | 0X0040)); //AGC
#endif
        A7169_WriteReg(ADC_REG, A7169Config[ADC_REG] | 0x8000);     //ARSSI=1

        A7169_StrobeCmd(CMD_RX);  //entry RX mode
        A7169_DelayMS(1);
        //printf("rssi init\n");
    }

    if(flag)
    {
//        //printf("test111\n");
#ifdef AGC_ON
        //AGC
        while(GIO1S == 0)    //Stay in RX mode until receiving ID code(ID detect ok)
        {
            tmp = (A7169_ReadReg(RX2_REG) );    //read RSSI value(environment RSSI)
            return 0;
        }
        tmp = (A7169_ReadReg(RX2_REG));
#else
        while(GIO1S == 0)    //Stay in RX mode until receiving ID code(ID detect ok)
        {
            tmp = (A7169_ReadReg(ADC_REG) & 0x00FF);    //read RSSI value(environment RSSI)
            return 0;
        }
        tmp = (A7169_ReadReg(ADC_REG) & 0x00FF);        //read RSSI value(wanted signal RSSI)
#endif
    }

    flag = false;
    return tmp;
}



