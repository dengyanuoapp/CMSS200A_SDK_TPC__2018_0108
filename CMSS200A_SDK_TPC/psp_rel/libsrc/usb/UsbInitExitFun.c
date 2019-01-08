/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UsbInitExitFun.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-20 08:09
 *********************************************************************************************************
 */
#include "udisklib.h"

#pragma name(INITEXITFUN_C)
static void USB_EP_Reset(void);
#pragma renamecode(?PR?INITEXITFUN_C)

//此定义为避免屏蔽掉打印后，udisk应用编译常量段定义未使用导致的编译错误。
#ifndef _PRINT_DEBUG_
const uint8 ConstSeg2 = 'A';
#endif

/*
 *********************************************************************************************************
 *                                           sUDiskRun
 *
 * Description: setup  communication  between PC and udisk
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : U盘开始与PC通信
 *********************************************************************************************************
 */
bool UDiskRun(int para)
{
    uint8 ret = 0;
    uint8 sfrbak,i;

    //char *pUsbInterrupt;
    //word USB_Isr_Addr;
    para++;

    //清全局变量,防止加密盘全局变量有误
    Suspend_Flag = 0x00;
    PrevRTCTimerValue1 = 0x00;
    PrevRTCTimerValue2 = 0x00;
    prv_connect_idle_tm = 0x00;

    //清除USB连线动作稳定标记,防止加密盘应用中进入错误的suspend状态
    usb_pwr_suspnd_fnsh = 0x00;
    //清reset计数,防止错误的进入Full speed模式 */
    usb_reset_enter_cnt = 0x00;
    //清idle状态(无bus reset)时间计数,
    //防止因usb_reset_enter_cnt被清掉导致重新对idle状态时间计数累加进而认为是插入到充电器中
    connect_idle_cnt = 0x00;

    sfrbak = SFR_BANK;

    OldIsrPtr = IRQ_Intercept((uint32) U20_ISRMain, IRQ_USB);

    // RTC calender enable
    SFR_BANK = BANK_RTC;
    if ((RTC_CTL0 & 0x80) != 0x80)
    {
        RTC_CTL0 = RTC_CTL0 | (uint8) 0x80;
        RTCRUPD = 0xa5;
        while (RTCRUPD != 0x5a)
        {
            ; /* do nothing */
        }
    }

    SFR_BANK = BANK_CMU_RMU;

    // debug add
    //    CPUCLKCTL = 0x02;

    CLKENCTL2 = CLKENCTL2 | 0x20; //DMA01234 clock enable
    MEMCLKENCTL0 = 0xff; //set memory CLK=CPU CLK
    MEMCLKSELCTL0 = 0x00;
    MEMCLKSELCTL1 = 0x8f;

    CLKENCTL1 = CLKENCTL1 | 0x40; //USBCLKEN enable

    MRCR2 = MRCR2 | 0x01; //DMA0/1/2/3/4 Reset
    MRCR2 = MRCR2 | 0x04; //USB Reset = nomal;
    i = 0x40;
    while(i--)
    {
        ; //do nothing
    }

    SFR_BANK = BANK_USB;
    USB_Efuse_Ref = USB_Efuse_Ref & 0xbf; //control ENB,Efuse is standby, and its standby current is 5uA max.
    i = 0x40;
    while(i--)
    {
        ; //do nothing
    }

    SFR_BANK = BANK_PMU;
    MULTI_USED = MULTI_USED | 0x80; //USBVDD LDO 软件 enable

    SFR_BANK = BANK_USB;
    DPDMCTRL = 0x1f; //enable DP DM 500K pull-up & disable DP DM pull-down
    i = 0x40;
    while(i--)
    {
        ; //do nothing
    }
    
    IDVBUSCTRL = 0x1f; //enable software Vbus, enable PHY clock

    //phy disable,FPGA invalid,note:disable is open	   //位置注意在	USBReset之前配置
    USB_PHYCTRL = USB_PHYCTRL & 0x3f;

    SFR_BANK = BANK_CMU_RMU;
    MRCR2 = MRCR2 | 0x08; //USBReset = nomal;
    SFR_BANK = BANK_USB;
    while ((USBSTATUS & 0x01) == 0x01)
    {
        ; /*do nothing*/
    }

    if ((DPDMCTRL & 0x40) != 0x00)
    {
        //Global Interrupt Enable
        IE0 = IE0 & (uint8) 0x7F;

        //enable USB INT
        AIE = AIE | 0x01;

        //phy set
        OTG_PHY_Setting(0xE7, 0x0B);
        OTG_PHY_Setting(0xE7, 0x1F);
        //    	OTG_PHY_Setting(0xE0,0x13);
        smDelay(1);

        USB_EP_Reset();
        EP1STADDRH = 0x03;
        EP1STADDRL = 0x80;

        EP2STADDRH = 0x0b;
        EP2STADDRL = 0x80;

        //Global Interrupt Enable
        IE0 = IE0 | (uint8) 0x80;

        //usb connection
        UsbCTRL_STUS = 0;

        ret = 1;
    }
    else
    {
#ifdef _PRINT_DEBUG_
        prints("L0\n");
#endif
    }

    SFR_BANK = sfrbak;
#ifdef _PRINT_DEBUG_
    prints("run\n");
#endif
    return ret;
}

/*
 ********************************************************************************
 *                           USB_EP_Reset
 * Description : USB endpoint(EP0,EPA,EPB) reset
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
static void USB_EP_Reset(void)
{
    USBEIRQ = 0x80;

    Hcin0maxpck = 0x40; //set EP0 maxpacket size 64byte

    Usbirq_hcusbirq = 0xff;
    Usbien_hcusbien = 0xB9; //Enable USB setup, suspend, reset, hs, connect/disconnect interrupt

    SHORTPCKIRQ = 0x20; //Enable Endpoint B short packet recieve interrupt
    SHORTPCKIRQ = 0x02;

    USBEIRQ = 0x08; //Turn on USB interrupt
}

/*
 *********************************************************************************************************
 *                                           UDiskExit
 *
 * Description: 退出与PC通信.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskExit(void)
{
    uint8 IE0bak;
    uint8 sfrbak,i;
#ifdef _PRINT_DEBUG_
    uprintf("exit Status=%x\n", USB_State);
#endif
    //close globle interrupt
    IE0bak = IE0;
    IE0 = IE0 & 0x7f;

    //close usb interrupt
    AIE = AIE & (uint8) 0xfe;

    sfrbak = SFR_BANK;

    SFR_BANK = BANK_USB;
    USBEIRQ = 0xc0;
    USBEIRQ = 0x00;

    //set Uram clk to MCU
    SFR_BANK = BANK_CMU_RMU;
    MEMCLKSELCTL1 = 0x00; //PCMRAM and URAM to MCU clk
    
	SFR_BANK = BANK_USB;
    Usbirq_hcusbirq = 0xff;
    Usbien_hcusbien = 0x00;
    SHORTPCKIRQ = 0x20;

    SFR_BANK = BANK_USB;
    /* software disconnect bit.  */
    UsbCTRL_STUS |= 0x40;

    SFR_BANK = BANK_CMU_RMU;
	MRCR2 = MRCR2 & 0xf7;  //USBReset
    i = 0x40;
    while(i--)
    {
        ; //do nothing
    }

    MRCR2 = MRCR2 | 0x08; //USBReset = nomal;
    SFR_BANK = BANK_USB;
    while ((USBSTATUS & 0x01) == 0x01)
    {
        ; /*do nothing*/
    }

    //phy enable,FPGA invalid ,note:enable is close
    USB_PHYCTRL = USB_PHYCTRL | (uint8) 0xc0;

    if (IsAdfuCondition == 0x00)
    {
        /*fixme:need*/
        IRQ_Release(OldIsrPtr, IRQ_USB);

        //卸载sd卡的驱动程序,卡处于multiread或multiwrite状态，再次初始化卡时会失败，vicky 20051121
        //bankb函数,升级完不能调用
        DRV_UnInstall(DRV_SDFF);

        if (EjectFlag == 0x00)
        {
            Udisk_Exit_Handle();
            //bankb函数，退出ap时调用，2006-1-14 16:39更换
            UD_Exit();
        }
        else
        {
            //如果发生写操作,把最后一次传输数据写入Flash
            UD_Update();
        }
    }
    else
    {
        //如果发生写操作,把最后一次传输数据写入Flash
        UD_Update();
    }

    DPDMCTRL = 0x10; // 恢复uhost检测

    //open globle interrupt
    IE0 = IE0bak;
    //restore SFR_BANK
    SFR_BANK = sfrbak;

    return 0x01;
}
