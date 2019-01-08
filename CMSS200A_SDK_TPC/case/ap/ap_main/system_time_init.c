#include "ap_common.h"

#define VERIFY_IsNotPowerDown

#define RTCFlushAP() {if(RTCRUPD != 0xA5) RTCRUPD = 0xA5;while(RTCRUPD != 0x5A);}

#pragma name(KCFG_M)
extern uint32 systemtime;
extern time_t time;

extern date_t date;//系统日期
//time_t time;//系统时间
kval_t kval;

int8 near IsNotPowerDown(void);//判断此次开机之前是否掉电关机

int16 InitSystemtime(void)
{
    uint8 sfr_bak;
    uint8 InitDateTime;//是否需要初始化系统时间1:需要初始化0:不需要初始化

    //调试日期, 时间设置
#ifdef VERIFY_IsNotPowerDown

    VMRead(&kval, VM_KERNEL, sizeof(kval_t));

    if (kval.magic == MAGIC_KVAL)
    {
        //判断此次开机之前是否掉电关机
        if (IsNotPowerDown() == 0) //返回  0：掉电   非0：没掉电
        {
            InitDateTime = 1; //需要初始化
        }
        else
        {
            InitDateTime = 0; //不需要初始化
        }
    }
    else //升级完第一次开机
    {
        InitDateTime = 1; //需要初始化
    }

    if (InitDateTime == 1)
    {
        kval.rtcresetflag = 0x55; //设置掉电标志
        date.year = 2010;
        date.month = 01;
        date.day = 01;
        time.hour = 0;
        time.minute = 0;
        time.second = 1;

        TM_SetDate(&date); //会修改systemtime   systemtime采用系统初始值
        TM_SetTime(&time); //会修改systemtime   systemtime采用系统初始值
        //将systemtime写回到VRAM中
        kval.magic = MAGIC_KVAL;
        kval.systemtime = systemtime; //在TM_SetDate被修改

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

    //从VRAM中取出systemtime
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

