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
    //�ر��ⲿpa��Դ
    switch_pa_power(0);
    //�����Դָʾ��
    //    switch_pilot_light(RED_LIGHT, 0);
    ClearWatchDog();
    LEDClearScreen();
    //����ٹػ�״̬
    //   DRV_UnInstall(DRV_KY);
    VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (g_comval.SleepTime != 0)
    {
        g_comval.SleepTime = 0; //�˱���Ҫ��0��ʹ˯��ģʽ����һ�ν�һ����Ч��
        VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    }
    //�ȹ��ɳ���play�����Է������ػ����ֳ��ֿ���������
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
    //����udisk�˲���ֱ�ӹ��磬���Բ���Ҫ����Ƿ����usb�ߡ�
    //ֻ����DC5V,�������ⲻ��USB�ߺ�DC5V����ֱ�ӽ���s3ģʽ
    if (((retval & 0x80) != 0x80))
    {
        Enter_SoftStandby();//��VCC��VDD
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
    //����м��ⲻ��DC5Vʱ����ֱ�ӽ���s3ģʽ��
    return Enter_SoftStandby();//��VCC��VDD
#else
    //����s2ģʽ
    disable_charge_pump();
    retval = Enter_HardStandby();
    enable_charge_pump();
    return retval;
#endif
}

/********************************************************************************
 * Description : ��ȡӲ���ص�״̬
 *
 * Arguments   :
 *
 * Returns     : ��
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 get_hard_switch_status(void)
{
    uint8 sfr_bak, hard_switch_staus;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    hard_switch_staus = SYSTEM_ONOFF & 0x10;  //��ȡӲ���ص�״̬
    SFR_BANK = sfr_bak;
    return hard_switch_staus;
}


/********************************************************************************
 * Description : ����ٹػ�״̬
 *
 * Arguments   :
 *
 * Returns     : ��
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 enter_fake_standby(void)
{
    uint8 sfr_bank, device_flag;
    uint8 play_status, onoff_status;
    uint16 count = 0;
    hard_onoff_status = get_hard_switch_status();  //��ȡӲ���ص�״̬
    standby_count = 0;
    while(1)
    {
        ClearWatchDog();
        //��ֹӲ���ؿ��ٰζ�ʱ����û��⵽Ӳ���عر���Ϣ��Ӳ�����ֿ����������
        if((power_on_flag == 1) && (hard_onoff_status == 0))
        {
            return RESULT_MAIN;
        }
        //���������Ϣ
        if (alarmtimerflag == 1)
        {
            alarmtimerflag = 0;
            //���Ӳ���عرգ�����Ӧ����
            if ((g_comval.g_alarm.AlarmEnable != 0) && (hard_onoff_status == 0))
            {
                return RESULT_ALARM; //���ӹر�
            }
        }

        play_status = get_onoff_key_status();
        onoff_status = get_hard_switch_status();
        SFR_BANK = BANK_PMU;
        //�ж�Ӳ�����Ƿ��аζ�����
        if((hard_onoff_status == 0) && (TEST_CTL & 0x02) != 0)
        {
            TEST_CTL |= 0x02;
            hard_onoff_status = get_hard_switch_status();
            TM_DelayMS(10);
        }
        SFR_BANK = sfr_bank;
        device_flag = GetUsbStatus();
        if ((device_flag & 0x80) != 0x80)//���û��DC5V,��ֱ�ӽ���s3ģʽ
        {
            Enter_SoftStandby();
        }
        //��ⳤ��onoff����Ϣ
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
        //���Ӳ���شӹر�״̬�ζ�������״̬����ֱ�ӿ���
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
 * Description : �������ؼ�S3ģʽ����ʱVCC��VDDȫ���Ͽ�
 *
 * Arguments   :
 *
 * Returns     : ��
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
        //disable uvlob����źţ������ؼ��ʱȥ��DC5V����Զ�����
        SYSTEM_ONOFF = (uint8 ) (SYSTEM_ONOFF & 0x7f);
        SYSTEM_CTL = (uint8 ) (SYSTEM_CTL & 0xfe);//�Ͽ�vcc,VDD
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
 * Description : ����Ӳ���أ���S2ģʽ��ϵͳ�ܵ��ǵ�Ƶ32K
 *
 * Arguments   :
 *
 * Returns     : ��
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
        retval = Get_PowerON_Message();//��ȡ����PLAY����Ϣ���߲���USB�ߣ������ʱ����Ϣ
        switch (retval)
        {
        case RESULT_MAIN:
        case RESULT_UDISK:
        case RESULT_ALARM:
            RecoverPara();//�ָ���Ƶ��Ȼ�󿪻�
            enable_charge_pump();
            if (!DRV_Install("DRV_UI.DRV", 0))
            {
                while (1)
                {
                    ;//ȱʡ����
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
            Enter_SoftStandby();//��VCC��VDD
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
    CP_CTL0 |= 0x01; //��ɱ�ʹ��
    CP_CTL1 |= 0x08;	//charge pumpֱͨ����ʹ��
    SFR_BANK = sfr_bak;
}

/********************************************************************************
 * Description : �ָ���Ƶ���ָ��������õ�һЩ����
 *
 * Arguments   :
 *
 * Returns     : ��
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
    //�ָ���Ƶ24M��VDD1.6V
    //��Ҫ�Ƿ�ֹ�����Զ��ػ�ʱ��PLL���6M��VDDΪ1.4V����������û�лָ���
    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    MyEI();
}

/********************************************************************************
 * Description : ����STANDBY֮ǰ��������ؼĴ��������͹���
 *
 * Arguments   :
 *
 * Returns     : ��
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
 * Description : ��Ƶ32K�µ���ʱ�����Ϊ3MS
 *
 * Arguments   :
 *
 * Returns     : ��
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
 * Description : MCU24Mʱ����ʱ
 *
 * Arguments   :
 *
 * Returns     : ��
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

    WDCTL = (uint8) (WDCTL | 0x08);//��watchdog������
    WDCTL = (uint8) (WDCTL & 0x7f);//�ر�watchdog
    //�رն�ʱ��,�ر�2Hz�ж�
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
    //    MULTI_USED &= 0x7F;//�ر�UVDD
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
    CPUCLKCTL = (char) (CPUCLKCTL | 0x02);//�л�HOSCģʽ,��ƵĬ��24M
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();

}
/********************************************************************************
 * Description : ��ȡUSB��������״̬��
 *
 * Arguments  :
 *
 *
 * Returns     :dpdm_status=0X40��ʾ�������USB�ߣ�dpdm_status=0X80Ϊ�����
 *            ��
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
 * Description : ��ȡ��������Ϣ���γ�DC5V��Ϣ
 *
 * Arguments  :
 *
 *
 * Returns     :
 * RESULT_POWERON������PLAY������
 * RESULT_UDISK������PLAY�������Ҳ���USB�߿���
 * RESULT_STANDBY��DC5V�γ�
 * RESULT_TIMER ���忪��
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
    alarm_msg = RTC_CTL0 & 0x01; //�����ӱ�־���
    SFR_BANK = sfr_bak;
#else
    play_key_flag = SYSTEM_ONOFF & 0x18;
#endif
    SFR_BANK = sfr_bak;
    if(alarm_msg != 0)
    {
        SFR_BANK = BANK_RTC;
        RTC_CTL0 = RTC_CTL0 | 0x01;//�����ӱ�־���
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
    if ((play_key_flag != 0) && (hard_onoff_status == 0)) //��ⰴPLAY����Ϣ
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
 * Description : �ر����е��ж�
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
 * Description : �ָ��ж�
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

