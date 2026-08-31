#ifndef __A7169REG_H
#define __A7169REG_H

#define SYSTEMCLOCK_REG     0x00
#define PLL1_REG            0x01
#define PLL2_REG            0x02
#define PLL3_REG            0x03
#define PLL4_REG            0x04
#define PLL5_REG            0x05
#define PLL6_REG            0x06
#define CRYSTAL_REG         0x07
#define PAGEA_REG           0x08
#define PAGEB_REG           0x09
#define RX1_REG             0x0A
#define RX2_REG             0x0B
#define ADC_REG             0x0C
#define PIN_REG             0x0D
#define CALIBRATION_REG     0x0E
#define MODE_REG            0x0F

#define TX1_PAGEA           0x00
#define WOR1_PAGEA          0x01
#define WOR2_PAGEA          0x02
#define RFI_PAGEA           0x03
#define PM_PAGEA            0x04
#define RTH_PAGEA           0x05
#define AGC1_PAGEA          0x06
#define AGC2_PAGEA          0x07
#define GIO_PAGEA           0x08
#define CKO_PAGEA           0x09
#define VCB_PAGEA           0x0A
#define CHG1_PAGEA          0x0B
#define CHG2_PAGEA          0x0C
#define FIFO_PAGEA          0x0D
#define CODE_PAGEA          0x0E
#define WCAL_PAGEA          0x0F

#define TX2_PAGEB           0x00
#define IF1_PAGEB           0x01
#define IF2_PAGEB           0x02
#define ACK_PAGEB           0x03
#define ART_PAGEB           0x04
#define SYN_PAGEB           0x05
#define RCCAL_PAGEB         0x06
#define ACKFIFO_PAGEB       0x07
#define PNCFG1_PAGEB        0x08
#define PNCFG2_PAGEB        0x09
#define PNCFG3_PAGEB        0x0A
#define PNCFG4_PAGEB        0x0B
#define PNCFG5_PAGEB        0x0C
#define TCODE_PAGEB         0x0D
#define PN_DC_PAGEB         0x0E
#define PNCFG6_PAGEB        0x0F
#define PNCFG7_PAGEB        0x10
#define PNCFG8_PAGEB        0x11
#define PNCFG9_PAGEB        0x12
#define PNCFG10_PAGEB       0x13
#define Misc_CFG1_PAGEB     0x14
#define Misc_CFG2_PAGEB     0x15
#define Misc_CFG3_PAGEB     0x16
#define PLL7_PAGEB          0x17
#define PLL8_PAGEB          0x18
#define PSMODE1_PAGEB       0x19
#define PSMODE2_PAGEB       0x1A
#define PSMODE3_PAGEB       0x1B
#define TX3_PAGEB           0x1C
#define Misc_CFG4_PAGEB     0x1D
#define PNCFG11_PAGEB       0x1E
#define PNCFG12_PAGEB       0x1F
#define PNCFG13_PAGEB       0x20
#define PNCFG14_PAGEB       0x21
#define PNCFG15_PAGEB       0x22
#define PNCFG16_PAGEB       0x23
#define PNCFG17_PAGEB       0x24
#define PNCFG18_PAGEB       0x25
#define CSMA1_PAGEB         0x26
#define CSMA2_PAGEB         0x27
#define TXPA_PAGEB          0x28
#define DCMON1_PAGEB        0x29
#define DCMON2_PAGEB        0x2A	
#define DCMON3_PAGEB        0x2B
#define MBUS1_PAGEB         0x2C
#define MBUS2_PAGEB         0x2D
#define MBUS3_PAGEB         0x2E
#define MBUS4_PAGEB         0x2F
#define MBUS5_PAGEB         0x30
#define PN_DC2_PAGEB        0x31
#define VCB2_PAGEB          0x32
#define PS_MODE4_PAGEB      0x33
#define WOR3_PAGEB          0x34


#define CMD_Reg_W           0x00  //000x,xxxx control register write
#define CMD_Reg_R           0x80  //100x,xxxx control register read
#define CMD_ID_W            0x20  //001x,xxxx ID write
#define CMD_ID_R            0xA0  //101x,xxxx ID Read
#define CMD_FIFO_W          0x40  //010x,xxxx TX FIFO Write
#define CMD_FIFO_R          0xC0  //110x,xxxx RX FIFO Read
#define CMD_RF_RST          0xFF  //x111,xxxx RF reset
#define CMD_TFR             0x60  //0110,xxxx TX FIFO address pointrt reset
#define CMD_RFR             0xE0  //1110,xxxx RX FIFO address pointer reset

#define CMD_SLEEP           0x10  //0001,0000 SLEEP mode
#define CMD_IDLE            0x12  //0001,0010 IDLE mode
#define CMD_STBY            0x14  //0001,0100 Standby mode
#define CMD_PLL             0x16  //0001,0110 PLL mode
#define CMD_RX              0x18  //0001,1000 RX mode
#define CMD_TX              0x1A  //0001,1010 TX mode
//#define CMD_DEEP_SLEEP    0x1C  //0001,1100 Deep Sleep mode(tri-state)
#define CMD_DEEP_SLEEP      0x1F  //0001,1111 Deep Sleep mode(pull-high)


#endif




