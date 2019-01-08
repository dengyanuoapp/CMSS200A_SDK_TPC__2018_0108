/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#include "ap_linein.h"
#include "actos.h"
#include "alarmdef.h"

#pragma name(DEAL_KEY_MSG)

//uint8 deal_vol_msg(uint8 key);
bool CheckLineOut(void);
uint16 show_ui_func(uint8 mode)
{
    uint16 result = 0;
    need_draw = TRUE;

    mode = mode;
    SetCurUI_Type(Linein_UI);
    LEDClearScreen();
    while (1)
    {
        if (need_draw) //屏幕重画
        {
            ui_display();
            need_draw = FALSE;
        }
        deal_pc_eqsrs(0);
        result = deal_key_msg();
        if (result != 0)
            break;
    }
    return result;
}

int16 deal_key_msg(void)
{
    int16 result = 0;
    uint8 key;
    key = ap_get_message();
    switch (key)
    {
    case AP_MSG_RTC:
        break;
    case AP_MSG_WAIT_TIMEOUT:
        if (CurUI_Type != Linein_UI)
        {
            SetCurUI_Type(Linein_UI);
        }
        break;
    case AP_KEY_PLAY | AP_KEY_UP: //play按键作返回处理
        if (CurUI_Type != Linein_UI)
        {
            SetCurUI_Type(Linein_UI);
        }
        break;

    case AP_KEY_MODE | AP_KEY_UP: //AP切换热键
        return RESULT_MAIN;
        break;

    case AP_KEY_MUTE | AP_KEY_UP: //mute设置
        mute_flag = !mute_flag;
        if (mute_flag)
        {
            exit_linein_func();
        }
        else
        {
            init_linein_channel();
        }
        SetCurUI_Type(Linein_UI);
        break;

    case AP_KEY_VOL | AP_KEY_UP: //调节音量
        key = Key_None;
        goto set_vol;
        break;
    case AP_KEY_VOLADD| AP_KEY_UP: //调节音量
    case AP_KEY_VOLADD| AP_KEY_LONG: //调节音量
        key = Key_Next;
    case AP_KEY_VOLSUB| AP_KEY_UP: //调节音量
    case AP_KEY_VOLSUB| AP_KEY_LONG: //调节音量
set_vol:
        deal_vol_msg(key, Linein_UI);
        break;

    case AP_KEY_NEXT | AP_KEY_UP:
    case AP_KEY_NEXT | AP_KEY_LONG:
        key = Key_Next;
    case AP_KEY_PREV | AP_KEY_UP:
    case AP_KEY_PREV | AP_KEY_LONG:
        if(CurUI_Type == Volume_UI)
        {
            goto set_vol;
        }
        else if(CurUI_Type == EQ_UI)
        {
            goto set_eq;
        }
        else
        {
            break;
        }
        break;
    case AP_MSG_LINEIN_OUT:
        result = RESULT_LINEIN;
        break;
    case AP_KEY_CLOCK | AP_KEY_UP:
        result = set_clock_ui(1);
        if(result == NULL)
        {
            SetCurUI_Type(Linein_UI);
        }
        break;
    case AP_KEY_EQ | AP_KEY_UP:
        key = Key_None;
set_eq:
        result = deal_eq_msg(key);
        break;
    case AP_KEY_CLOCK | AP_KEY_LONG:
#if defined(LINEIN_AP)
        longkey_flag = TRUE;
        if(g_comval.SuppTTS != 0)
        {
            exit_linein_func();
            result = report_time_info(0, TRUE);
            init_linein_channel();
            if(result != 0)
            {
                return result;
            }
        }
#endif
        break;

    case AP_KEY_AUTOSLEEP:
        set_autosleep_time(Key_None);
        break;
    case AP_MSG_LINEIN_IN:
        //        result = RESULT_MAIN;
        break;

    default: //热键处理
        result = ap_handle_hotkey_core(key);
        break;
    }
    return result;
}
