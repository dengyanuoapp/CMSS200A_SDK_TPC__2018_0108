/*
 ********************************************************************************
 *                       ACTOS
 *                  driver demo
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : Hdrv_HardInit.c
 * By     :	fiona
 * Version: 1> v1.00     first version     date	2010/10/12
 ********************************************************************************
 */
#include "drv_led.h"
//#define EVB_MODE
#pragma name(HARD_INIT)
/************************************************************/

////********************************************************/
///*    void Hdrv_SetMfpToEmif(void);
//** FUNCTION:     Hdrv_SetMfpToEmif
//**
//** Description:  select lcd segment and com
//**
//**  input
//**     none
//**
//**  output
//**     none
//**********************************************************/
void Hdrv_SetMfpToEmif(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    //set MFP_GPIO Bank
    SFR_BANK = BANK_GPIO;
#ifdef EVB_MODE

    //select segment and com
    //select led_com[5,6,7]
    MFP_CTL0 &= 0x81;
    //select led_com[1,2,3,4]
    MFP_CTL1 = 0x15;
    //select led_com0
    MFP_CTL2 &= 0xf7;
    MFP_CTL2 |= 0x04;
    //select LED SEG4,5,6,7
    MFP_CTL3 &= 0xC0;
    MFP_CTL3 |= 0x15;
    //select LED SEG0,1,2,3
    MFP_CTL4 = 0x55;
#else
    //select led_com[1,2,3,4]
    MFP_CTL1 = 0x15;
    //select led_com0
    MFP_CTL2 &= 0xf7;
    MFP_CTL2 |= 0x04;
    //select LED SEG4,5,6,7
    MFP_CTL3 &= 0xC0;
    MFP_CTL3 |= 0x15;
    //select LED SEG0,1,2,3
    MFP_CTL4 = 0x55;
#endif
    SFR_BANK = sfr_bak;
}

////********************************************************/
///*    void Hdrv_SetLED(void);
//** FUNCTION:     Hdrv_SetLED
//**
//** Description:  lcd controller reset and lcd clock select
//**
//**  input
//**     none
//**
//**  output
//**     none
//**********************************************************/
void Hdrv_SetLED(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    //set BANK_CMU_RMU
    SFR_BANK = BANK_CMU_RMU;
    //enable LCD & SEG LCD clock,模块时钟使能
    CLKENCTL2 |= 0x10;
    // touch key controller reset,模块reset
    MRCR3 &= 0xFD;
    MRCR3 |= 0x02;

    //select LED &SEG LCD clock,模块时钟设置
    LED_SEGLCDCLKCTL &= 0xF0;
    LED_SEGLCDCLKCTL |= 0x03;
    SFR_BANK = sfr_bak;
}

////********************************************************/
///*    void Hdrv_HardInit(void);
//** FUNCTION:     Hdrv_HardInit
//**
//** Description:  初始化LCM的硬件设备
//**
//**  input
//**     none
//**
//**  output
//**     none
//**********************************************************/
void Hdrv_HardInit(void)
{
	return;
#if 0
    uint8 sfr_bak = SFR_BANK;
    //lcd controller reset and lcd clock select
    Hdrv_SetLED();
    //select lcd segment and com
    Hdrv_SetMfpToEmif();
    SFR_BANK = BANK_LCD;
    //lcd_led模式选择
    LCD_MODE = LCD_MODE & (~0x7) | 0x07;
    //打开segment
    LCD_MODE |= 0x80;
    //输出使用
    LCD_MODE |= 0x10;
    SFR_BANK = BANK_GPIO;
#if 1
    //使用数字部分点LED灯
    //降低驱动功能，也是就改变IO口的驱动等级
    //    PADDRV0=0x0;
    //    PADDRV1=0x0;
    LED_SEG_BIAS_EN &= 0xF7;
#else
    //使用恒流源点亮LED灯
    LED_SEG_BIAS_EN &= 0xF8;
    LED_SEG_BIAS_EN |= 0x0C;
    LED_SEG_RC_EN = 0x7f;
    PADDRV0 = 0xff;
    SFR_BANK = BANK_PMU;
    TEST_CTL |= 0x10;
#endif

    SFR_BANK = sfr_bak;
#endif
}
