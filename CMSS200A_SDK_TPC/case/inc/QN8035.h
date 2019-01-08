/*******************************************************************************
 *                              AS260A
 *                            Module: qn8035 driver
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     mikeyang     2011-11-03 15:00     1.0             build this file
 *******************************************************************************/

#ifndef _QNDRIVER_H
#define _QNDRIVER_H

#include "actos.h"

#define QND_READ(adr)    QND_ReadReg(adr)
#define QND_WRITE(adr, value)  QND_WriteReg(adr, value)

#define QND_REG_NUM_MAX                 85  // for qn8035

enum
{
    // MSB (8bits) of the word: RSSI Threshold
    // LSB (8bits) of the word: SNR Threshold
    // eg: 0x0a01 => RSSI_TH = 0x0a, SNR_TH = 0x01
    // notice: the rang of RSSI is 0x0A ~ 0x3F
    // notice: the rang of SNR is 0x00 ~ 0x3F
    CCA_SENSITIVITY_LEVEL_0 = 0x0A04,
    CCA_SENSITIVITY_LEVEL_1 = 0x0A04,
    CCA_SENSITIVITY_LEVEL_2 = 0x1405,
    CCA_SENSITIVITY_LEVEL_3 = 0x1406,
    CCA_SENSITIVITY_LEVEL_4 = 0x1E06,
    CCA_SENSITIVITY_LEVEL_5 = 0x1E07,
    CCA_SENSITIVITY_LEVEL_6 = 0x1E08,
    CCA_SENSITIVITY_LEVEL_7 = 0x1E09,
    CCA_SENSITIVITY_LEVEL_8 = 0x1E0A,   //the item is better in sz office
    CCA_SENSITIVITY_LEVEL_9 = 0x1E0B,
    CCA_SENSITIVITY_LEVEL_10 = 0x1E0C,
    CCA_SENSITIVITY_LEVEL_11 = 0x1E0D,
    CCA_SENSITIVITY_LEVEL_12 = 0x1E0E,
    CCA_SENSITIVITY_LEVEL_13 = 0x1E0F,
    CCA_SENSITIVITY_LEVEL_14 = 0x1E10,
    CCA_SENSITIVITY_LEVEL_15 = 0x1E11,
    CCA_SENSITIVITY_LEVEL_16 = 0x2312,
    CCA_SENSITIVITY_LEVEL_17 = 0x2313,
    CCA_SENSITIVITY_LEVEL_18 = 0x2814,
    CCA_SENSITIVITY_LEVEL_19 = 0x2D15  // top
};

/**********************************QN8035's clock source selection**************
1.setting QN8035's clock source and clock type,recommendation use 32768Hz clock as
  chip's clock.
2.this group value is 32768HZ clock as QN8035's clock source.
3.user need to modify according to actual hardware platform,notice clock unit is Hz.
4.clock formula:
  XTAL_DIV = Round(Clock/32768);
  PLL_DLT = Round((28500000*512*XTAL_DIV)/Clock)-442368
*******************************************************************************/
#define QND_SINE_WAVE_CLOCK             0x00    //inject sine-wave clock  
#define QND_DIGITAL_CLOCK               0x80    //inject digital clock,default is inject digital clock

//32768HZ
//#define QND_XTAL_DIV0                   0x01
//#define QND_XTAL_DIV1                   0x08
//#define QND_XTAL_DIV2                   0x5C


/*1MHZ
#define QND_XTAL_DIV0                   0x1F
#define QND_XTAL_DIV1                   0x00
#define QND_XTAL_DIV2                   0x38
*/
/*4MHZ
#define QND_XTAL_DIV0                   0x7A
#define QND_XTAL_DIV1                   0x00
#define QND_XTAL_DIV2                   0x54
*/
/*6MHZ
#define QND_XTAL_DIV0                   0xB7
#define QND_XTAL_DIV1                   0x00
#define QND_XTAL_DIV2                   0x54
*/
/*8MHZ
#define QND_XTAL_DIV0                   0xF4
#define QND_XTAL_DIV1                   0x00
#define QND_XTAL_DIV2                   0x54
*/
/*10MHZ
#define QND_XTAL_DIV0                   0x31
#define QND_XTAL_DIV1                   0x01
#define QND_XTAL_DIV2                   0x54
*/
/*12MHZ
#define QND_XTAL_DIV0                   0x6E
#define QND_XTAL_DIV1                   0x01
#define QND_XTAL_DIV2                   0x54
*/
/*16MHZ
#define QND_XTAL_DIV0                   0xB8
#define QND_XTAL_DIV1                   0x01
#define QND_XTAL_DIV2                   0x54
*/
/*20MHZ
#define QND_XTAL_DIV0                   0x62
#define QND_XTAL_DIV1                   0x02
#define QND_XTAL_DIV2                   0x54
*/
//24MHZ

#define QND_XTAL_DIV0                   0xDC
#define QND_XTAL_DIV1                   0x02
#define QND_XTAL_DIV2                   0x54

/*26MHZ
#define QND_XTAL_DIV0                   0x19
#define QND_XTAL_DIV1                   0x03
#define QND_XTAL_DIV2                   0x54
*/
/***************************************End************************************/

// tune
#define QND_FSTEP_50KHZ                 0
#define QND_FSTEP_100KHZ                1
#define QND_FSTEP_200KHZ                2


/****************************Chips ID definition*******************************/
#define CHIPID_QN8000                   0x00
#define CHIPID_QN8005                   0x20
#define CHIPID_QN8005B1                 0x21
#define CHIPID_QN8006                   0x30
#define CHIPID_QN8006LB                 0x71
#define CHIPID_QN8007B1                 0x11
#define CHIPID_QN8007                   0x10
#define CHIPID_QN8006A1                 0x30
#define CHIPID_QN8006B1                 0x31
#define CHIPID_QN8016                   0xe0
#define CHIPID_QN8016_1                 0xb0
#define CHIPID_QN8015                   0xa0
#define CHIPID_QN8065                   0xa0
#define CHIPID_QN8067                   0xd0
#define CHIPID_QN8065N                  0xa0
#define CHIPID_QN8027                   0x40
#define CHIPID_QN8025                   0x80
#define CHIPID_QN8035                   0x84
/***************************************End************************************/

/*******************************************************************************
Performance configuration
*******************************************************************************/
#define SMSTART_VAL                     19
#define HCCSTART_VAL                    33
#define SNCSTART_VAL                    55
//the smaller is this value,the more is number of Automatic scan channels ,fake channels is also increased .


/*******************************************************************************
 definition register
*******************************************************************************/
#define SYSTEM1                         0x00
#define CCA                             0x01
#define SNR                                0x02
#define RSSISIG                         0x03
#define STATUS1                         0x04
#define CID1                            0x05
#define CID2                            0x06
#define    CH                                0x07
#define    CH_START                        0x08
#define    CH_STOP                            0x09
#define    CH_STEP                            0x0A
#define    RDSD0                            0x0B
#define    RDSD1                            0x0C
#define    RDSD2                            0x0D
#define    RDSD3                            0x0E
#define    RDSD4                            0x0F
#define    RDSD5                            0x10
#define    RDSD6                            0x11
#define    RDSD7                            0x12
#define    STATUS2                            0x13
#define    VOL_CTL                            0x14
#define    XTAL_DIV0                        0x15
#define    XTAL_DIV1                        0x16
#define    XTAL_DIV2                        0x17
#define INT_CTRL                        0x18
#define SMP_HLD_THRD                    0x19
#define    RXAGC_GAIN                        0x1A
#define GAIN_SEL                        0x1B
#define    SYSTEM_CTL1                        0x1C
#define    SYSTEM_CTL2                        0x1D
#define RDSCOSTAS                        0x1E
#define REG_TEST                        0x1F
#define STATUS4                            0x20
#define    CCA1                            0x27
#define    SMSTART                            0x34
#define    SNCSTART                        0x35
#define    HCCSTART                        0x36
#define NCCFIR3                         0x40
#define REG_DAC                            0x4C
/***************************************End************************************/

/*******************************************************************************
 definition operation bit of register
*******************************************************************************/
#define CCA_CH_DIS                      0x01
#define CHSC                            0x02
#define RDSEN                           0x08
#define CH_CH                            0x03
#define CH_CH_START                     0x0c
#define CH_CH_STOP                      0x30
#define STNBY                           0x20
#define RX_MODE                         0x10
#define IMR                             0x40
#define RX_MONO_MASK                    0x04
#define RDS_RXUPD                       0x80
#define RDS_SYNC                  0x10
#define RDS_ERROR                0x0f                             // rdsd0 to rdsd3 error check
/***************************************End************************************/

#endif
