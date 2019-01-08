#include "ap_common.h"

#define VERIFY_IsNotPowerDown

#define RTCFlushAP() {if(RTCRUPD != 0xA5) RTCRUPD = 0xA5;while(RTCRUPD != 0x5A);}

#pragma name(KCFG_M)
extern uint32 systemtime;
extern time_t time;

extern date_t date;//ϵͳ����
//time_t time;//ϵͳʱ��
kval_t kval;

int8 near IsNotPowerDown(void);//�жϴ˴ο���֮ǰ�Ƿ����ػ�

int16 InitSystemtime(void)
{
    uint8 sfr_bak;
    uint8 InitDateTime;//�Ƿ���Ҫ��ʼ��ϵͳʱ��1:��Ҫ��ʼ��0:����Ҫ��ʼ��

    //��������, ʱ������
#ifdef VERIFY_IsNotPowerDown

    VMRead(&kval, VM_KERNEL, sizeof(kval_t));

    if (kval.magic == MAGIC_KVAL)
    {
        //�жϴ˴ο���֮ǰ�Ƿ����ػ�
        if (IsNotPowerDown() == 0) //����  0������   ��0��û����
        {
            InitDateTime = 1; //��Ҫ��ʼ��
        }
        else
        {
            InitDateTime = 0; //����Ҫ��ʼ��
        }
    }
    else //�������һ�ο���
    {
        InitDateTime = 1; //��Ҫ��ʼ��
    }

    if (InitDateTime == 1)
    {
        kval.rtcresetflag = 0x55; //���õ����־
        date.year = 2010;
        date.month = 01;
        date.day = 01;
        time.hour = 0;
        time.minute = 0;
        time.second = 1;

        TM_SetDate(&date); //���޸�systemtime   systemtime����ϵͳ��ʼֵ
        TM_SetTime(&time); //���޸�systemtime   systemtime����ϵͳ��ʼֵ
        //��systemtimeд�ص�VRAM��
        kval.magic = MAGIC_KVAL;
        kval.systemtime = systemtime; //��TM_SetDate���޸�

        VMWrite(&kval, VM_KERNEL, sizeof(kval_t));
        sfr_bak = SFR_BANK;
        SFR_BANK = BANK_RTC;
        //RTC_CTL0|=0x85;
        //fpga,  select cal_clk_division
        RTC_CTL0 |= 0x81;
        RTCFlushAP();
        SFR_BANK = sfr_bak;
    }
    else
    {

    }
    ClearWatchDog();

    //��VRAM��ȡ��systemtime
    systemtime = kval.systemtime;
#endif
    return 0xffff;

}

int8 near IsNotPowerDown(void)
{
    uint8 sfr_bak;

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    if(RTC_CTL0 & 0x80)
    {
        SFR_BANK = sfr_bak;
        return 1;
    }
    else
    {
        SFR_BANK = sfr_bak;
        return 0;
    }
}

