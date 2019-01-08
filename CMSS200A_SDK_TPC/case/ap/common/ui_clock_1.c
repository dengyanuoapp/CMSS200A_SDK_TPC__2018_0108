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
const uint8 month_days[] = //һ��12����ÿ���¶�Ӧ�Ļ�������
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void init_alarm_setting(void)
{
    SetCurUI_Type(AlarmSet_UI);
    //    VMRead(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));
    memcpy(&time, &g_comval.g_alarm.alarmTime, sizeof(time_t));
    //��������ʱ���ӵ�2�ʼ�����Ա��ʱ������ͳһ
    set_what = 2;
    longkey_flag = TRUE;
}

/********************************************************************************
 * Description : ��BUF�е���ֵת������ʵ���õ����֡�
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 ����:DigitalCount=3,DigitalBuf[]={3,7,8},ת����ʵ�ֿ��ܵ����־���DigitalNum=378��
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
 * Description : ʵ��ͨ������ֵ�������ڣ�ʱ��Ĺ���
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
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
        days = month_days[date.month - 1]; //��������
        if (((date.year % 4) == 0) && (date.month == 2))
        {
            days++; //����+1
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
    //���µ�����ݣ����ڣ�ʱ��
    KeyAdjustTime(key);
}
/********************************************************************************
 * Description :���ݰ���ֵ���µ�����ǰ�����ڻ�ʱ��
 *
 * Arguments  :
 *           key����Ӧ����ֵ
 * Returns     :
 *            ��
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
            days = month_days[date.month - 1]; //��������
            if (((date.year % 4) == 0) && (date.month == 2))
            {
                days++; //����+1
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
            days = month_days[date.month - 1]; //��������
            if (((date.year % 4) == 0) && (date.month == 2))
            {
                days++; //����+1
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
    //���ͣ�������ӿ���ѡ��ʱ�������ּ���Ч
    if (CurUI_Type == Clock_UI)
    {
        return NULL;
    }
    disable_flicker();
    if ((CurUI_Type == AlarmSet_UI) && (set_what == 2))
    {
        return NULL;
    }

    if (set_what == 0) //���ռ��λ��ʾ
    {
        maxcount = 4;
    }
    else //�£��գ�ʱ���֣���ռ��λ��ʾ
    {
        maxcount = 2;
    }
    if ((DigitalCount >= maxcount) || (DigitalCount == 0))
    {
        SetCurUI_Type(CurUI_Type);
    }
    key_val = (key & ~AP_KEY_UP) - AP_KEY_NUM0;//�����ְ�����Ϣת��������
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
        //���л������һ��ʱ�����浱ǰ������
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
//ʹ����������˸Ч��
void enable_flicker(void)
{
    show_ui_flag = 1;
    show_count = 0;
    need_draw = TRUE;
}
//ȡ����������˸Ч��
void disable_flicker(void)
{
    show_ui_flag = 0;
    show_count = 0;
    show_what = 1;
    need_draw = TRUE;
}
//ȡ��ϵͳ����
void disable_alarm(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    RTC_CTL0 &= 0xBF;
    RTC_CTL0 |= 0x01;
    SFR_BANK = sfr_bak;
}

