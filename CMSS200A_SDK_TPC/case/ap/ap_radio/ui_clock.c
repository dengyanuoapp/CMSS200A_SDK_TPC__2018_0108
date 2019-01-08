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
#include "ap_radio.h"

#pragma name(AP_CLOCK)
extern void deal_clock_next_prev(uint8 key);

const uint8 alarm_str[2][4] =
{ "OFF", "ONZ" };
void ShowTimer(void)
{
    TM_GetDate(&date);
    TM_GetTime(&time);
    if (date.year > 2098)
    {
        date.year = 2000;
        date.month = 0x01;
        date.day = 0x01;
        time.hour = 0;
        time.minute = 0;
        time.second = 1;
        TM_SetDate(&date);
        TM_SetTime(&time);
    }

    if (memcmp(&tmp_time, &time, sizeof(time_t)) != 0)
    {
        memcpy(&tmp_time, &time, sizeof(time_t));
        itoa_2(time.hour, &temp_buffer[0]);
        itoa_2(time.minute, &temp_buffer[2]);
        temp_buffer[4] = 0x0;
        LEDPuts(NUMBER1, temp_buffer, 1);
    }
    LEDDisplay(LED_COL, 0xff, show_what);
}
void show_time_setting(void)
{
    switch (set_what)
    {
    case 0://设置年份
        if (show_what == 0)
        {
            memset(temp_buffer, 0xff, 0x04);
        }
        else
        {
            if (DigitalCount != 0)
            {
                memset(temp_buffer, 0x0, 0x04);
                memcpy(&temp_buffer[4 - DigitalCount], DigitalBuf, DigitalCount);
                break;
            }
            itoa4(date.year, temp_buffer);
        }
        break;
    case 1://设置月份
        if (show_what == 0)
        {
            memset(temp_buffer, 0xff, 0x02);
        }
        else
        {
            if (DigitalCount != 0)
            {
                memset(temp_buffer, 0x0, 0x02);
                memcpy(&temp_buffer[2 - DigitalCount], DigitalBuf, DigitalCount);
            }
            else
            {
                itoa2(date.month, temp_buffer);
            }
        }
        itoa2(date.day, &temp_buffer[2]);
        break;
    case 2://设置日期(时间设置)或设置闹钟开关(闹钟设置)
        if (CurUI_Type == TimeSet_UI)
        {
            itoa2(date.month, temp_buffer);
            if (show_what == 0)
            {
                memset(&temp_buffer[2], 0xff, 0x02);
            }
            else
            {
                if (DigitalCount != 0)
                {
                    memset(&temp_buffer[2], 0x0, 0x02);
                    memcpy(&temp_buffer[4 - DigitalCount], DigitalBuf, DigitalCount);
                    break;
                }
                itoa2(date.day, &temp_buffer[2]);
            }
        }
        else
        {
            if (show_what == 0)
            {
                memset(temp_buffer, 0xff, 0x03);
            }
            else
            {
                memcpy(temp_buffer, alarm_str[g_comval.g_alarm.AlarmEnable], 0x03);
            }
            LEDPuts(NUMBER2, temp_buffer, 1);
            return;
        }
        break;
    case 3://设置时间中的时
        if (show_what == 0)
        {
            memset(temp_buffer, 0xff, 0x02);
        }
        else
        {
            if (DigitalCount != 0)
            {
                memset(temp_buffer, 0x0, 0x02);
                memcpy(&temp_buffer[2 - DigitalCount], DigitalBuf, DigitalCount);
                break;
            }
            else
            {
                itoa2(time.hour, temp_buffer);
            }

        }
        itoa2(time.minute, &temp_buffer[2]);
        break;
    case 4://设置时间中的分
        itoa2(time.hour, temp_buffer);
        if (show_what == 0)
        {
            memset(&temp_buffer[2], 0xff, 0x02);
        }
        else
        {
            if (DigitalCount != 0)
            {
                memset(&temp_buffer[2], 0x0, 0x02);
                memcpy(&temp_buffer[4 - DigitalCount], DigitalBuf, DigitalCount);
                break;
            }
            itoa2(time.minute, &temp_buffer[2]);
        }
        break;
    }
    if (set_what >= 3)
    {
        LEDDisplay(LED_COL, 0xff, 1);
    }
    LEDPuts(NUMBER1, temp_buffer, 1);
}

void show_clock_ui(void)
{
    switch(CurUI_Type)
    {
    case Clock_UI:
        //显示当前的系统时间
        ShowTimer();
        break;
    case TimeSet_UI:
    case AlarmSet_UI:
        //显示时间设置或闹钟设置界面
        show_time_setting();
        break;
    default:
        break;
    }
}
uint8 set_clock_loop(uint8 auto_return)
{
    uint8 key, retval = NULL;
    while (1)
    {
        if (need_draw == TRUE)
        {
            show_clock_ui();
            need_draw = FALSE;
        }
        key = ap_get_message(); //读键值
        switch (key)
        {
        case AP_MSG_RTC:
            if (show_ui_flag == 0)
            {
                show_count++;
                if (show_count > 4)
                {
                    show_ui_flag = 1;
                    show_count = 0;
                }
            }
            else
            {
                show_what = !show_what;
                need_draw = TRUE;
            }
            break;
        case AP_MSG_WAIT_TIMEOUT:
            if ((CurUI_Type == TimeSet_UI) || (CurUI_Type == AlarmSet_UI))
            {
                show_what = !show_what;
                need_draw = TRUE;
                break;
            }
            else if (CurUI_Type != Clock_UI)
            {
                SetCurUI_Type(Clock_UI);
            }
            else if(auto_return != 0)
            {
                return RESULT_NULL;
            }
            break;
        case AP_KEY_MODE | AP_KEY_UP:
            return RESULT_MAIN;
        case AP_KEY_CLOCK | AP_KEY_UP:
            deal_clock_mode_msg();
            need_draw = TRUE;
            break;
        case AP_KEY_MODE | AP_KEY_LONG:
            if (CurUI_Type == Clock_UI)
            {
                longkey_flag = 1;
                init_alarm_setting();
                need_draw = TRUE;
            }
            break;
#ifdef AP_RADIO_REPORT_EN
        case AP_KEY_VOL | AP_KEY_LONG:
        case AP_KEY_CLOCK | AP_KEY_LONG:
            longkey_flag = 1;
            fmSendCommand(FM_MUTE, SetMUTE, NULL);
            exit_fmin_channel();
            if(key == (AP_KEY_VOL | AP_KEY_LONG))
            {
                retval = report_frequency(&g_FMStatus, report_buf);
            }
            else
            {
                retval = report_time_info(0, TRUE);
            }
            init_fmin_channel();
            if(retval != 0)
            {
                return retval;
            }
            if (g_comval.volume != 0)
            {
                fmSendCommand(FM_MUTE, releaseMUTE, NULL); //解除静音
            }
            fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
            break;
#endif

            break;
        case AP_KEY_PLAY | AP_KEY_UP:
            if (CurUI_Type != Clock_UI)
            {
                SetCurUI_Type(Clock_UI);
            }
            else
            {
                if (auto_return != 0)
                {
                    return RESULT_NULL;
                }
            }
            break;
        case AP_KEY_NEXT | AP_KEY_UP:
            key = Key_Next;
        case AP_KEY_PREV | AP_KEY_UP:
            deal_clock_next_prev(key);
            need_draw = TRUE;
            break;
        case AP_KEY_VOL | AP_KEY_UP:
            key = Key_None;
            goto dealvolkey;
        case AP_KEY_VOLADD | AP_KEY_UP:
        case AP_KEY_VOLADD | AP_KEY_LONG:
            key = Key_Next;
        case AP_KEY_VOLSUB | AP_KEY_UP:
        case AP_KEY_VOLSUB | AP_KEY_LONG:
dealvolkey:
            deal_vol_msg(key, Clock_UI);
            break;
        case AP_KEY_AUTOSLEEP | AP_KEY_UP:
            set_autosleep_time(Key_None);
            break;
        default:
            if ((key >= (AP_KEY_NUM0 | AP_KEY_UP)) && (key <= (AP_KEY_NUM9 | AP_KEY_UP)))
            {
                deal_clock_digital_key(key);
            }
            else
            {
                retval = ap_handle_hotkey_core(key);
            }
            break;
        }
        if (retval != NULL)
        {
            break;
        }
    }
    return retval;
}
uint8 set_clock_ui(uint8 auto_return)
{
    uint8 retval;
    need_draw = TRUE;
    set_what = 3;
    show_what = 1;
    show_count = 0;
    show_ui_flag = 1;
    LEDClearScreen();
    SetCurUI_Type(Clock_UI);
    retval = set_clock_loop(auto_return);
    show_count = 0;
    show_ui_flag = 0;
    return retval;
}

