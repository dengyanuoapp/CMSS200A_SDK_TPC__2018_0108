/*
 ********************************************************************************
 *                       ACTOS
 *               standby ap of sdk 3.0
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : main.c
 * By     : Gongee
 * Version: 1> v1.00     first version     1/12/2004 11:10PM
 ********************************************************************************
 * $Id: main.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 ********************************************************************************
 */
#include "ap_common.h"
#include "actos.h"

//#define FAKE_HARD_SWITCH
#pragma name(AP_STANDBY)
uint8 far intBuf[2];
uint8 far PMUBuf[9];
uint8 far RTCBuf[9];
uint8 far USBBuf[2];
uint8 far RESETBuf[6];
uint8 far CMUBuf[8];
uint16 far standby_count;
uint8 far hard_onoff_status;

uint8 Enter_HardStandby(void);
uint8 Enter_SoftStandby(void);
uint8 enter_fake_standby(void);
uint8 GetUsbStatus(void);
void time_delay(uint8 time);
uint8 Get_PowerON_Message(void);
void RecoverPara(void);
extern void _nop_(void);
void MCU_delay(uint8 time);
void LOSC_delay(void);
void SetParaToStandby(void);
void MyDI(void);
void MyEI(void);
void SetRESET(void);
void RecoverRESET(void);
void SetRTC(void);
void RecoverRTC(void);
void SetPMU(void);
void RecoverPMU(void);
void SetCMU(void);
void SetUSBEfuse(void);
void RecoverUSBEfuse(void);
void RecoverHOSC(void);
void reg_update(void);
void Recover_CMU(void);
uint8 get_ir_msg(void);
void disable_charge_pump(void);
void enable_charge_pump(void);
void Delay_us(uint8 delay_ms);
uint8 get_onoff_key_status(void);

int16 main(int16 param)
{
    uint8 retval = 0;
    uint8 rtc_count = 0;
    uint8 key = 0 ;

    param = param;
    //关闭外部pa电源
    switch_pa_power(0);
    //灭掉电源指示灯
    //    switch_pilot_light(RED_LIGHT, 0);
    ClearWatchDog();
    LEDClearScreen();
    //进入假关机状态
    //   DRV_UnInstall(DRV_KY);
    VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (g_comval.SleepTime != 0)
    {
        g_comval.SleepTime = 0; //此变量要清0，使睡眠模式设置一次仅一次有效。
        VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    }
    //先过渡长按play键，以防长按关机后又出现开机的问题
    while(ap_get_message() != NULL)
    {
    };
    while(1)
    {
        ClearWatchDog();
        if(get_onoff_key_status() == 0)
        {
            break;
        }
    }
    longkey_flag = 0;
    standby_count = 0;
    retval = GetUsbStatus();
    //由于udisk端不会直接供电，所以不需要检测是否插着usb线。
    //只需检测DC5V,如果检测检测不到USB线和DC5V，则直接进入s3模式
    if (((retval & 0x80) != 0x80))
    {
        Enter_SoftStandby();//断VCC，VDD
    }
#if 1

    retval = enter_fake_standby();
    if(retval != NULL)
    {
        LEDPuts(NUMBER1, "LOAD", 1);
        switch_pa_power(1);
        power_on_flag = 0;
        return retval;
    }
    //如果中间检测不到DC5V时，则直接进入s3模式。
    return Enter_SoftStandby();//断VCC，VDD
#else
    //进入s2模式
    disable_charge_pump();
    retval = Enter_HardStandby();
    enable_charge_pump();
    return retval;
#endif
}

/********************************************************************************
 * Description : 获取硬开关的状态
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 get_hard_switch_status(void)
{
    uint8 sfr_bak, hard_switch_staus;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    hard_switch_staus = SYSTEM_ONOFF & 0x10;  //获取硬开关的状态
    SFR_BANK = sfr_bak;
    return hard_switch_staus;
}


/********************************************************************************
 * Description : 进入假关机状态
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 enter_fake_standby(void)
{
    uint8 sfr_bank, device_flag;
    uint8 play_status, onoff_status;
    uint16 count = 0;
    hard_onoff_status = get_hard_switch_status();  //获取硬开关的状态
    standby_count = 0;
    while(1)
    {
        ClearWatchDog();
        //防止硬开关快速拔动时，还没检测到硬开关关闭消息，硬开关又开起来的情况
        if((power_on_flag == 1) && (hard_onoff_status == 0))
        {
            return RESULT_MAIN;
        }
        //检测闹钟消息
        if (alarmtimerflag == 1)
        {
            alarmtimerflag = 0;
            //如果硬开关关闭，则不响应闹钟
            if ((g_comval.g_alarm.AlarmEnable != 0) && (hard_onoff_status == 0))
            {
                return RESULT_ALARM; //闹钟关闭
            }
        }

        play_status = get_onoff_key_status();
        onoff_status = get_hard_switch_status();
        SFR_BANK = BANK_PMU;
        //判断硬开关是否有拔动动作
        if((hard_onoff_status == 0) && (TEST_CTL & 0x02) != 0)
        {
            TEST_CTL |= 0x02;
            hard_onoff_status = get_hard_switch_status();
            TM_DelayMS(10);
        }
        SFR_BANK = sfr_bank;
        device_flag = GetUsbStatus();
        if ((device_flag & 0x80) != 0x80)//如果没有DC5V,则直接进入s3模式
        {
            Enter_SoftStandby();
        }
        //检测长按onoff键消息
        if((play_status != 0) && (hard_onoff_status == 0))
        {
            count = 0;
            standby_count++;
            TM_DelayMS(10);
            if ((standby_count > 40) && (get_hard_switch_status() == 0))
            {
                longkey_flag = 1;
                return RESULT_MAIN;
            }
        }
        //如果硬开关从关闭状态拔动到开机状态，则直接开机
        if(hard_onoff_status == 0x10)
        {
            onoff_status = get_hard_switch_status();
            if(onoff_status == 0)
            {
                while(1)
                {
                    count++;
                    Delay_us(1);
                    play_status = get_onoff_key_status();
                    onoff_status = get_hard_switch_status();
                    if((onoff_status == 0) && (play_status == 0) && (count > 600))
                    {
                        return RESULT_MAIN;
                    }
                    if((count > 600) || (play_status != 0) || (onoff_status != 0))
                    {
                        count = 0;
                        break;
                    }
                }

            }
        }
        else if(play_status == 0)
        {
            count = 0;
            standby_count = 0;
        }
    }
    return NULL;
}
/********************************************************************************
 * Description : 进入软开关既S3模式，这时VCC，VDD全部断开
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/
uint8 Enter_SoftStandby(void)
{
    uint8 sfr_bak;
    uint8 count = 0;
    DisablePA();
    DisableDAC();
    DisableAIN();
    while(1)
    {
        ClearWatchDog();
        disable_charge_pump();
        SFR_BANK = BANK_PMU;
        //disable uvlob检测信号，否则电池检电时去掉DC5V后会自动唤醒
        SYSTEM_ONOFF = (uint8 ) (SYSTEM_ONOFF & 0x7f);
        SYSTEM_CTL = (uint8 ) (SYSTEM_CTL & 0xfe);//断开vcc,VDD
        SFR_BANK = sfr_bak;
        TM_DelayMS(1);
        count++;
        if(count > 10)
        {
            break;
        }
    }
    enable_charge_pump();
    return NULL;
}

/********************************************************************************
 * Description : 进入硬开关，既S2模式，系统跑的是低频32K
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/
uint8 Enter_HardStandby(void)
{
    uint8 retval, sfr_bank;
    sfr_bank = SFR_BANK;
    SFR_BANK = BANK_PMU;
    hard_onoff_status = SYSTEM_ONOFF & 0x10;
    SFR_BANK = sfr_bank;
    UHOST_Power_CLOSE(sfr_bank);
    DRV_UnInstall(DRV_UI);
    DRV_UnInstall(DRV_KY);
    SetParaToStandby();
    while(1)
    {
        ClearWatchDog();
        retval = Get_PowerON_Message();//获取长按PLAY键消息或者插入USB线，充电器时的消息
        switch (retval)
        {
        case RESULT_MAIN:
        case RESULT_UDISK:
        case RESULT_ALARM:
            RecoverPara();//恢复高频，然后开机
            enable_charge_pump();
            if (!DRV_Install("DRV_UI.DRV", 0))
            {
                while (1)
                {
                    ;//缺省简体
                }
            }
            if (!DRV_Install("KY_HARD.DRV", 0))
            {
                while (1)
                {
                    ;
                }
            }
            LEDPuts(NUMBER1, "LOAD", 1);
            switch_pa_power(1);
            return retval;
            break;

        case RESULT_STANDBY:
            Enter_SoftStandby();//断VCC，VDD
            break;
        default:
            break;
        }
        if ((retval != 0) && (retval != RESULT_STANDBY))
        {
            break;
        }
    }
    return RESULT_MAIN;
}

void time_delay(uint8 time)
{
    uint16 i;
    uint32 timer;
    for (i = 0; i < 10 * time; i++)
    {
        timer = 8000;
        ClearWatchDog();
        while (timer > 0)
        {
            --timer;
        }
    }

}
void disable_charge_pump(void)
{
    uint8 sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    //disable charge pump
    CP_CTL0 &= 0xFE;
    CP_CTL1 &= 0xF7;
    SFR_BANK = sfr_bak;
}
void enable_charge_pump(void)
{
    uint8 sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    //enable charge pump
    CP_CTL0 |= 0x01; //电荷泵使能
    CP_CTL1 |= 0x08;	//charge pump直通功能使能
    SFR_BANK = sfr_bak;
}

/********************************************************************************
 * Description : 恢复高频，恢复其他设置的一些参数
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/
void RecoverPara(void)
{
    RecoverHOSC();
    Recover_CMU();
    RecoverRESET();
    RecoverUSBEfuse();
    RecoverRTC();
    RecoverPMU();
    //恢复高频24M，VDD1.6V
    //主要是防止黑屏自动关机时，PLL设成6M，VDD为1.4V，开机起来没有恢复。
    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    MyEI();
}

/********************************************************************************
 * Description : 进入STANDBY之前，设置相关寄存器，降低功耗
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/
void SetParaToStandby(void)
{
    DisablePA();
    DisableDAC();
    DisableAIN();
    SetRTC();
    SetRESET();
    SetUSBEfuse();
    SetPMU();
    MyDI();
    SetCMU();
}

/********************************************************************************
 * Description : 低频32K下的延时，大概为3MS
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/

void LOSC_delay(void)
{
    uint8 time = 0x10;
    while (1)
    {
        time--;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        ClearWatchDog();
        if (time == 0)
        {
            break;
        }

    }
}
/********************************************************************************
 * Description : MCU24M时的延时
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/
void MCU_delay(uint8 time)
{
    uint8 tmp_time = time;
    while (1)
    {
        tmp_time--;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        ClearWatchDog();
        if (tmp_time == 0)
        {
            break;
        }
    }
}
void SetRESET(void)
{
    SFR_BANK = 0x01;
    RESETBuf[0] = MRCR1;
    RESETBuf[1] = MRCR2;
    RESETBuf[2] = MRCR3;
    RESETBuf[3] = CLKENCTL0;
    RESETBuf[4] = CLKENCTL1;
    RESETBuf[5] = CLKENCTL2;

    //reset audio code,accelerator,sd/mmc car ,uart,irc and spi controller
    MRCR1 = 0x02;
    //reset RAM&ROM BIST,SPDIF,ADC and IIS RX,DAC and IIS TX,PWM ,DMA01234,set USB to normal
    MRCR2 = 0x0C;//USB normal
    //reset touch key and ASRC controller
    MRCR3 = 0x0;
    //disable audio codec clock,spdif RX  clock,DAC clock,SD card clock,IISIN special colck and FM clock
    CLKENCTL0 = 0x0;
    //disable PWM,IR clcok,SPI clock,URAT clock
    CLKENCTL1 = 0x40;
    //disable touch key clock,ASRC clock,DMA01234 clock,LCD&SEGLCD clock,ACC clock
    CLKENCTL2 = 0x0;

    _nop_();
    _nop_();
    _nop_();
}

void RecoverRESET(void)
{
    SFR_BANK = 0x01;
    MRCR1 = RESETBuf[0];
    MRCR2 = RESETBuf[1];
    MRCR3 = RESETBuf[2];
    CLKENCTL0 = RESETBuf[3];
    CLKENCTL1 = RESETBuf[4];
    CLKENCTL2 = RESETBuf[5];
    _nop_();
    _nop_();
    _nop_();
}

void SetUSBEfuse(void)
{
    SFR_BANK = 0x07;
    USBBuf[0] = USB_Efuse_Ref;
    USBBuf[1] = USB_PHYCTRL;
    USB_Efuse_Ref = (uint8) (USB_Efuse_Ref | 0x40);//bit6 set to 1, efuse is standby,and its standby current is 5UA max
    USB_PHYCTRL = (uint8) (USB_PHYCTRL | 0x80);//enable phy_PLLEn
    USB_PHYCTRL = (uint8) (USB_PHYCTRL | 0x40);//enable phy_dallallen
    _nop_();
    _nop_();
}

void RecoverUSBEfuse(void)
{
    SFR_BANK = 0x07;
    USB_Efuse_Ref = USBBuf[0];
    USB_PHYCTRL = USBBuf[1];
    _nop_();
    _nop_();
}
void SetRTC(void)
{
    SFR_BANK = 0x0c;
    RTCBuf[0] = CTCCTL;
    RTCBuf[1] = CTCCNTL;
    RTCBuf[2] = CTCCNTH;
    RTCBuf[3] = RTC_CTL0;
    RTCBuf[4] = WDCTL;
    RTCBuf[5] = RTC_CTL1;
    RTCBuf[6] = CTCCTL2;
    RTCBuf[7] = CTCCNTL2;
    RTCBuf[8] = CTCCNTH2;
    RTC_CTL0 = 0x0;// (uint8) (RTC_CTL0& 0xE7);
    RTC_CTL0 |= 0x80;

    WDCTL = (uint8) (WDCTL | 0x08);//清watchdog计数器
    WDCTL = (uint8) (WDCTL & 0x7f);//关闭watchdog
    //关闭定时器,关闭2Hz中断
    RTC_CTL1 &= 0x1F;
    //    CTCCTL &= 0x7f;
    //    CTCCTL2 &= 0x7f;
    reg_update();
    _nop_();
}

void RecoverRTC(void)
{
    SFR_BANK = 0x0c;
    CTCCTL = RTCBuf[0];
    CTCCNTL = RTCBuf[1];
    CTCCNTH = RTCBuf[2];
    RTC_CTL0 = RTCBuf[3];
    WDCTL = RTCBuf[4];
    RTC_CTL1 = RTCBuf[5];
    CTCCTL2 = RTCBuf[6];
    CTCCNTL2 = RTCBuf[7];
    CTCCNTH2 = RTCBuf[8];
    reg_update();
    _nop_();
    _nop_();
}
void SetPMU(void)
{
    SFR_BANK = 0x05;
    PMUBuf[0] = MULTI_USED;
    PMUBuf[1] = LDOPD_CTL;
    PMUBuf[2] = EFUSE_CTL;
    PMUBuf[3] = BDG_CTL;
    PMUBuf[4] = CP_CTL0;
    PMUBuf[5] = FS_CTL;
    PMUBuf[6] = PMUADC_CTL;
    PMUBuf[7] = TEST_CTL;
    PMUBuf[8] = SYSTEM_ONOFF;
    //    MULTI_USED &= 0x7F;//关闭UVDD
    LDOPD_CTL = 0x0;
    EFUSE_CTL = 0x0;
    BDG_CTL &= 0xEF;
    CP_CTL0 &= 0xFE;
    FS_CTL &= 0x7F;
    MULTI_USED &= 0xF7;
    PMUADC_CTL &= 0xDF;
    PMUADC_CTL &= 0xBF;
    TEST_CTL &= 0xEF;
    SYSTEM_ONOFF &= 0x7f;
    _nop_();
    MCU_delay(0xff);
}
void RecoverPMU(void)
{
    SFR_BANK = 0x05;
    MULTI_USED = PMUBuf[0];
    LDOPD_CTL = PMUBuf[1];
    EFUSE_CTL = PMUBuf[2];
    BDG_CTL = PMUBuf[3];
    CP_CTL0 = PMUBuf[4];
    FS_CTL = PMUBuf[5];
    PMUADC_CTL = PMUBuf[6];
    TEST_CTL = PMUBuf[7];
    SYSTEM_ONOFF = PMUBuf[8];
}
void SetCMU(void)
{
    SFR_BANK = 0x01;
    CMUBuf[0] = MEMCLKSELCTL0;
    CMUBuf[1] = MEMCLKSELCTL1;
    CMUBuf[2] = MEMCLKENCTL0;
    CMUBuf[3] = AUDIO_PLL_CTL;
    CMUBuf[4] = MEMCLKSELCTL0;
    CMUBuf[5] = HCL_CLK_CTL;
    CMUBuf[6] = LOSC_CTL;
    CMUBuf[7] = MCU_PLL_SSC_CTL;
    MEMCLKSELCTL0 = 0x0;
    MEMCLKSELCTL1 = 0x0;
    MEMCLKENCTL0 = 0x0;
    AUDIO_PLL_CTL &= 0xFE;//disable AudioPLL
    HCL_CLK_CTL &= 0xc0;
    LOSC_CTL &= 0xFE;//DISABLE DUAL PIN LOSC
    CPUCLKCTL = 0x02;
    MCU_PLL_CTL &= 0xDF;
    MCU_PLL_SSC_CTL &= 0xfe;
    reg_update();
#if 1
    SFR_BANK = 0x01;
    CPUCLKCTL &= 0xFD;//SELECT LOSC

    MCU_delay(0xff);
    MCU_PLL_CTL &= 0x7f;//HOSC Disable
#endif
    _nop_();
}
void Recover_CMU(void)
{
    SFR_BANK = 0x01;
    MEMCLKSELCTL0 = CMUBuf[0];
    MEMCLKSELCTL1 = CMUBuf[1];
    MEMCLKENCTL0 = CMUBuf[2];
    AUDIO_PLL_CTL = CMUBuf[3];
    MEMCLKSELCTL0 = CMUBuf[4];
    HCL_CLK_CTL = CMUBuf[5];
    LOSC_CTL = CMUBuf[6];
    MCU_PLL_SSC_CTL = CMUBuf[7];
    reg_update();
}

void reg_update(void)
{
    SFR_BANK = 0x0c;
    RTCRUPD = 0xa5;
    while (RTCRUPD != 0x5a)
        ;
    MCU_delay(0x0f);
}
void RecoverHOSC(void)
{
    SFR_BANK = 0x01;
    MCU_PLL_CTL = (char) (MCU_PLL_CTL | 0x80); //HOSC Enable
    MCU_delay(0xFF);
    _nop_();
    _nop_();
    CPUCLKCTL = (char) (CPUCLKCTL | 0x02);//切换HOSC模式,高频默认24M
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();

}
/********************************************************************************
 * Description : 获取USB或充电器的状态，
 *
 * Arguments  :
 *
 *
 * Returns     :dpdm_status=0X40表示插入的是USB线，dpdm_status=0X80为充电器
 *            无
 * Notes       :
 *
 ********************************************************************************/

uint8 GetUsbStatus(void)
{
    uint8 sfr_bak, dpdm_status;
    uint8 usb_status;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;
    dpdm_status = DPDMCTRL;
    DPDMCTRL = 0x1f;
    usb_status = 0x80;
    while(usb_status--)
    {
        ;
    }
    if((USBSTATUS & 0x18) != 0x18)
    {
        usb_status = 0xff;
        while(usb_status--)
        {
            ;
        }
        if((USBSTATUS & 0x18) != 0x18)
        {
            usb_status = 0x40;
        }
        else
        {
            usb_status = 0x00;
        }
    }
    else
    {
        usb_status = 0x00;
    }
    DPDMCTRL = dpdm_status;

    SFR_BANK = BANK_PMU;
    usb_status |= (TEST_CTL & 0x80);

    SFR_BANK = sfr_bak;
    return usb_status;

}

/********************************************************************************
 * Description : 获取开机的消息及拔出DC5V消息
 *
 * Arguments  :
 *
 *
 * Returns     :
 * RESULT_POWERON：长按PLAY键开机
 * RESULT_UDISK：长按PLAY键，并且插入USB线开机
 * RESULT_STANDBY：DC5V拔出
 * RESULT_TIMER 闹铃开机
 * Notes       :
 *
 ********************************************************************************/
uint8 Get_PowerON_Message(void)
{
    uint8 sfr_bak;
    uint8 play_key_flag, device_flag, fake_onoff_flag, alarm_msg;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
#ifndef FAKE_HARD_SWITCH
    play_key_flag = SYSTEM_ONOFF & 0x08;
    fake_onoff_flag = SYSTEM_ONOFF & 0x10;
    if((hard_onoff_status == 0) && (TEST_CTL & 0x02) != 0)
    {
        hard_onoff_status = 1;
        TEST_CTL |= 0x02;
    }
    SFR_BANK = BANK_RTC;
    alarm_msg = RTC_CTL0 & 0x01; //将闹钟标志清除
    SFR_BANK = sfr_bak;
#else
    play_key_flag = SYSTEM_ONOFF & 0x18;
#endif
    SFR_BANK = sfr_bak;
    if(alarm_msg != 0)
    {
        SFR_BANK = BANK_RTC;
        RTC_CTL0 = RTC_CTL0 | 0x01;//将闹钟标志清除
        SFR_BANK = sfr_bak;
        if(hard_onoff_status == 0)
        {
            return RESULT_ALARM;
        }
    }
    device_flag = GetUsbStatus();
    if (((device_flag & 0x40) != 0x40) && ((device_flag & 0x80) != 0x80))
    {
        return RESULT_STANDBY;
    }
#ifndef FAKE_HARD_SWITCH
    if ((play_key_flag != 0) && (hard_onoff_status == 0)) //检测按PLAY键消息
    {
        standby_count++;
        if (standby_count > 5)
        {
            standby_count = 1;
            return RESULT_MAIN;
        }
    }
    else if(play_key_flag == 0)
    {
        standby_count = 0;
        if((fake_onoff_flag == 0) && (hard_onoff_status != 0))
        {
            return RESULT_MAIN;
        }

    }
#else
    if(play_key_flag == 0)
    {
        return RESULT_MAIN;
    }
#endif
    return 0;
}
/********************************************************************************
 * Description : 关闭所有的中断
 *
 * Arguments  :
 *
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************/
void MyDI(void)
{
    intBuf[0] = IE0;
    intBuf[1] = AIE;
    IE0 = 0x0;
    AIE = 0x0;
}
/********************************************************************************
 * Description : 恢复中断
 *
 * Arguments  :
 *
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************/
void MyEI(void)
{
    IE0 = intBuf[0];
    AIE = intBuf[1];
}
void Delay_us(uint8 delay_ms)
{
    uint8  j;
    while (delay_ms != 0)
    {
        for (j = 0; j < 255; j++)
        {
            _nop_();
        }
        delay_ms--;
    }
}

