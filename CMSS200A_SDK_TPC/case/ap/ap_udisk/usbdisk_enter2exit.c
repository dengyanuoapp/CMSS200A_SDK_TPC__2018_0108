/********************************************************************************
 *                              AS211A
 *                            Module: USB MAIN
 *                 Copyright(c) 2001-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      wanghaijing 2010-3-10         1.0                build this file
 ********************************************************************************/
/*!
 * \file     usbdisk_enter2exit.c
 * \brief
 * \author   wanghaijing
 * \par      GENERAL DESCRIPTION:
 *               ������ļ���������
 * \par      EXTERNALIZED FUNCTIONS:
 *               �����������õ������ģ��
 *
 *      Copyright(c) 2001-2010 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2010-3-27
 *******************************************************************************/
#include "Udisk.h"
extern uint8 UdiskStatus;
extern uint8 HoldDispState;
extern uint8 IRUSBExit;
extern uint8 reckeypress;
#pragma name(AP_ENTEREXIT)
bool UDiskInitAll(int para)
{
    bool ret;
    //    /*�͵�ģʽ�½��г��*/
    //    if(GetBattery() < 0x13)
    //    {
    //       sKY_ChargeSet(1);
    //       sKY_ChargeGetandSet();
    //    }

    //��ram��clk���л���mcu clk,����Ӧ���˳���u��,clk���Ե���ʹ������.
    RamToMcu();

    if (DRV_DetectUD(0x00) == 0xff)
    {
        return 0;
    }
    else
    {
        DRV_ActiveUD(0x00, MODE_FAST); //����ģʽud����
    }

    //read comval
    VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval)); //��VRAM

    //==================================================================================
    //**********************���º������ô����ܸĶ�***************************************
    //==================================================================================
    UDiskSetDiskNum(3); //��������ģʽ

    AlwaysShowCardDisk(0); //�����Ƿ���ʾ���̷�  //reagan modify 2009-1-6 13:39

    //U�̳�ʼ�ջ�,�������:callback����������...
    UDiskInit(0, g_comval.udisk_setting);

    ret = UDiskRun(para); //����USB����
    return ret;
}

extern void _nop_(void);
void power_control(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;

    SFR_BANK = BANK_CMU_RMU;
    MRCR3 |= 0x08;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    MRCR2 |= 0x08;
    _nop_();
    _nop_();

    SFR_BANK = BANK_PMU;
    //VDD_DCDC_CTL = (VDD_DCDC_CTL & 0xC1)|0x16;
    BDG_CTL |= 0x40;
    EFUSE_CTL |= 0x40;
    SFR_BANK = BANK_USB;
    USB_PHYCTRL |= 0xC0;
    USB_Efuse_Ref |= 0x40;
    SFR_BANK = BANK_CMU_RMU;
    MRCR1 &= 0xf7;
    //MCSR2 &= 0xfc;
    //    MCSR3 &= 0xbf;
    //MCSR5 &= 0xe7;

    SFR_BANK = sfr_bak;
}

int16 UdiskExitAll(void)
{
    int16 ret;

    UDiskExit();

    DRV_ActiveUD(0x00, MODE_SLOW);

    //��Mode�˳�
    if (UdiskStatus == 0x00)
    {
        if (reckeypress == 1)
        {
            ret = RESULT_USBAUDIO_PLAY;
        }
        else if(reckeypress == 2)
        {
            ret = RESULT_USBTEST;
        }
        else
        {
            ret = RESULT_NULL;
        }
    }
    //�����˳�
    else if (UdiskStatus == 0x50)
    {
        ret = RESULT_UPGRADE;
    }
    else
    {
        //����,����
        ret = 0;
    }
    //MODE������  UHOST��CARD��FM��USBAUDIO��UDISKUSB  ѭ���л�
    // ��Ӧ����ң�������Power��
    if (IRUSBExit)
    {
        ret = RESULT_NULL;
        //        ret = RESULT_MUSIC_PLAY; //	 RESULT_MUSIC_PLAY SD Card ׼��ʱ�䲻�������ֽ��� FM
    }
    power_control();
    return ret;	//RESULT_USBAUDIO_PLAY
}

void RamToMcu(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;

    SFR_BANK = BANK_CMU_RMU;
#if 0
    MCSR11 = (uint8) (MCSR11 & 0x04);
    MCSR5 = (uint8) ((MCSR5 & 0xe1) | 0x34); //JRAM3Clk:MCU clk; PCMRAM:MCU clk;
    MCSR6 = (uint8) ((MCSR6 & 0xf0) | 0x0a); //MURAM2:MCU clk; JRAM1Clk:MCU clk;
    MCSR7 = 0x2a; //B1 Clk:MCU clk; MURAM1HClk:MCU clk; JRAM2Clk:MCU clk;
    MCSR8 = (uint8) ((MCSR8 & 0x03) | 0x48); //MURAM1MClk:MCU clk; MURAM1LClk:MCU clk;
    MCSR9 = 0x4a; //URAMClk:MCU clk; B2 Clk:MCU clk; DRAM251BClk:MCU clk;
    MCSR11 = MCSR11 | 0xfb;
#endif

    SFR_BANK = sfr_bak;

}
