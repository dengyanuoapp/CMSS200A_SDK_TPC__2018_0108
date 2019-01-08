/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: sub functions 2 of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_common.h"

#pragma name(AP_CLOCK_1)
const uint8 month_days[] = //一年12个月每个月对应的基本天数
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void init_alarm_setting(void)
{
    SetCurUI_Type(AlarmSet_UI);
    //    VMRead(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));
    memcpy(&time, &g_comval.g_alarm.alarmTime, sizeof(time_t));
    //闹钟设置时，从第2项开始设置以便和时间设置统一
    set_what = 2;
    longkey_flag = TRUE;
}

/********************************************************************************
 * Description : 将BUF中的数值转换成现实可用的数字。
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 例如:DigitalCount=3,DigitalBuf[]={3,7,8},转换成实现可能的数字就是DigitalNum=378；
 *
 ********************************************************************************/
uint16 DigitalSwitchToRealNum(void)
{
    uint16 DigitalNum = 0;
    uint8 i;
    uint16 CurBitNum = 1;
    for (i = 1; i < DigitalCount; i++)
    {
        CurBitNum = CurBitNum * 10;
    }
    for (i = DigitalCount; i > 0; i--)
    {
        DigitalNum = DigitalNum + DigitalBuf[DigitalCount - i] * CurBitNum;
        CurBitNum = CurBitNum / 10;
    }
    return DigitalNum;
}

/********************************************************************************
 * Description : 实现通过数字值调整日期，时间的功能
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :

 *
 ********************************************************************************/
void DigitalAdjustTime(void)
{
    uint16 realNum = 0;
    uint8 days;
    realNum = DigitalSwitchToRealNum();
    switch (set_what)
    {
    case 0: //year
        if ((realNum <= 2098) && (realNum >= 2000))
        {
            date.year = realNum;
        }
        break;
    case 1: //month
        if ((realNum <= 12) && (realNum > 0))
        {
            date.month = (uint8) realNum;
        }
        break;
    case 2: //day
        days = month_days[date.month - 1]; //本月天数
        if (((date.year % 4) == 0) && (date.month == 2))
        {
            days++; //闰年+1
        }
        if ((realNum <= days) && (realNum > 0))
        {
            date.day = (uint8) realNum;
        }
        break;
    case 3: //hour
        if (realNum <= 23)
        {
            time.hour = (uint8) realNum;
        }
        break;
    case 4: //min
        if (realNum <= 59)
        {
            time.minute = (uint8) realNum;
        }
        break;
    default: //no in setting
        break;
    } //set_what
    need_draw = TRUE;
}
void deal_clock_next_prev(uint8 key)
{
    if (CurUI_Type == Clock_UI)
    {
        return;
    }
    if (CurUI_Type == Volume_UI)
    {
        deal_vol_msg(key, Clock_UI);
        return;
    }
    if (CurUI_Type == STANDBY_UI)
    {
        set_autosleep_time(key);
        return;
    }
    disable_flicker();
    if (CurUI_Type == AlarmSet_UI && set_what == 2)
    {
        g_comval.g_alarm.AlarmEnable = !g_comval.g_alarm.AlarmEnable;
    }
    //上下调节年份，日期，时间
    KeyAdjustTime(key);
}
/********************************************************************************
 * Description :根据按键值上下调整当前的日期或时间
 *
 * Arguments  :
 *           key：相应按键值
 * Returns     :
 *            无
 * Notes       :
 *
 ********************************************************************************/
void KeyAdjustTime(uint8 key)
{
    uint8 days;
    if (key == Key_Next)
    {
        switch (set_what)
        {
        case 0: //year
            if (date.year < 2098)
            {
                date.year++;
            }
            else
            {
                date.year = 2000;
            }
            break;
        case 1: //month
            if (date.month < 12)
            {
                date.month++;
            }
            else
            {
                date.month = 1;
            }
            break;
        case 2: //day
            days = month_days[date.month - 1]; //本月天数
            if (((date.year % 4) == 0) && (date.month == 2))
            {
                days++; //闰年+1
            }
            if (date.day < days)
            {
                date.day++;
            }
            else
            {
                date.day = 1;
            }
            break;
        case 3: //hour
            if (time.hour < 23)
            {
                time.hour++;
            }
            else
            {
                time.hour = 0;
            }
            break;
        case 4: //min
            if (time.minute < 59)
            {
                time.minute++;
            }
            else
            {
                time.minute = 0;
            }
            break;
        default: //no in setting
            break;
        } //set_what
    }
    else
    {
        switch (set_what)
        {
        case 0: //year
            if (date.year > 2000)
            {
                date.year--;
            }
            else
            {
                date.year = 2098;
            }
            break;
        case 1: //month
            if (date.month > 1)
            {
                date.month--;
            }
            else
            {
                date.month = 12;
            }
            break;
        case 2: //day
            days = month_days[date.month - 1]; //本月天数
            if (((date.year % 4) == 0) && (date.month == 2))
            {
                days++; //闰年+1
            }
            if (date.day > 1)
            {
                date.day--;
            }
            else
            {
                date.day = days;
            }
            break;
        case 3: //hour
            if (time.hour > 0)
            {
                time.hour--;
            }
            else
            {
                time.hour = 23;
            }
            break;
        case 4: //min
            if (time.minute > 0)
            {
                time.minute--;
            }
            else
            {
                time.minute = 59;
            }
            break;
        default: //no in setting
            break;
        } //set_what
    }
    need_draw = TRUE;
}
uint8 deal_clock_digital_key(uint8 key)
{
    uint8 key_val;
    uint8 maxcount;
    //光标停留在闹钟开关选项时，按数字键无效
    if (CurUI_Type == Clock_UI)
    {
        return NULL;
    }
    disable_flicker();
    if ((CurUI_Type == AlarmSet_UI) && (set_what == 2))
    {
        return NULL;
    }

    if (set_what == 0) //年份占四位显示
    {
        maxcount = 4;
    }
    else //月，日，时，分，秒占两位显示
    {
        maxcount = 2;
    }
    if ((DigitalCount >= maxcount) || (DigitalCount == 0))
    {
        SetCurUI_Type(CurUI_Type);
    }
    key_val = (key & ~AP_KEY_UP) - AP_KEY_NUM0;//将数字按键消息转换成数字
    DigitalBuf[DigitalCount] = key_val;
    DigitalCount++;
    need_draw = TRUE;
    return NULL;
}
void deal_clock_mode_msg(void)
{
    enable_flicker();
    if (CurUI_Type == Clock_UI)
    {
        SetCurUI_Type(TimeSet_UI);
        return;
    }
    else
    {
        if (DigitalCount != 0)
        {
            DigitalAdjustTime();
            DigitalCount = 0;
            memset(DigitalBuf, 0, 4);
        }
        //当切换到最后一项时，保存当前的设置
        if (set_what >= 4)
        {
            set_what = 3;
            show_what = 1;
            if (CurUI_Type == TimeSet_UI)
            {
                time.second = 1;
                TM_SetDate(&date);
                TM_SetTime(&time);
            }
            else
            {
                memcpy(&g_comval.g_alarm.alarmTime, &time, sizeof(time_t));
                if (g_comval.g_alarm.AlarmEnable != 0)
                {
                    TM_SetAlarmTime(&g_comval.g_alarm.alarmTime);
                }
                else
                {
                    disable_alarm();
                }
                VMWrite(&g_comval, VM_SYSTEM, sizeof(alarm_vars_t));
            }
            SetCurUI_Type(Clock_UI);
        }
        else
        {
            set_what++;
        }
    }
}
//使用设置项闪烁效果
void enable_flicker(void)
{
    show_ui_flag = 1;
    show_count = 0;
    need_draw = TRUE;
}
//取消设置项闪烁效果
void disable_flicker(void)
{
    show_ui_flag = 0;
    show_count = 0;
    show_what = 1;
    need_draw = TRUE;
}
//取消系统闹钟
void disable_alarm(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    RTC_CTL0 &= 0xBF;
    RTC_CTL0 |= 0x01;
    SFR_BANK = sfr_bak;
}

