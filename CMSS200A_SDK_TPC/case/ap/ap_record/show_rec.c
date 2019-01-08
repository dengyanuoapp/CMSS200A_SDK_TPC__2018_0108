/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: show_rec.c,v 1.12 2009/10/12 03:44:16 dengyulin Exp $
 *******************************************************************************
 */
#include "actos.h"
#include "basal.h"
#include "enhanced.h"
#include "ap_common.h"
#include "ap_record.h"
#include "ap_report.h"

#pragma name(AP_REC)

uint8 deal_recoding_status(void);

/*
 ********************************************************************************
 * Description : 录音UI  界面显示
 *
 * Arguments   : NULL
 *
 * Returns     :  NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ui_draw(void)
{
    if (rec_status != rec_start)
    {
        LEDDisplay(LED_PLAY, 0xff, 0);
        LEDDisplay(LED_PAUSE, 0xff, 1);
    }
    else
    {
        LEDDisplay(LED_PAUSE, 0xff, 0);
        LEDDisplay(LED_PLAY, 0xff, 1);
    }
    /*  显示录音目标存储介质*/
    if (g_record_vars.location.disk == g_disk_char[1])
    {
        LEDDisplay(LED_USB, 0xff, 0);
        LEDDisplay(LED_SD, 0xff, 1);
    }
    else
    {
        LEDDisplay(LED_SD, 0xff, 0);
        LEDDisplay(LED_USB, 0xff, 1);
    }
    switch (CurUI_Type)
    {
    case (uint8) RecStop_UI:
        memset(g_now_time, 0x0, 0x04);
        LEDPuts(NUMBER1, g_now_time, 1);
        break;

    case (uint8) RecOn_UI:
    case (uint8) RecPause_UI:
        time_draw(1);
        break;
    default:
        break;
    }
}

/*
 ********************************************************************************
 * Description : 显示当前的录音时间
 *
 * Arguments   : mode   刷新模式  0--只刷当前时间   1--刷当前时间和剩余可录时间
 *
 * Returns     :  NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 time_draw(uint8 mode)
{
    uint32 nowtime_l; /* 获取当前的时间的32位变量 */
    time_t nowtime_s; /* 获取当前的时间的结构 */

    /* 显示当前录音的时间 */
    recSendCommand(MC_GETTIME, (void *) &nowtime_s);
    g_rec_time.hour = nowtime_s.hour;
    g_rec_time.minute = nowtime_s.minute;
    g_rec_time.second = nowtime_s.second;

    nowtime_l = (uint32) nowtime_s.hour * 3600 + nowtime_s.minute * 60 + nowtime_s.second;
    if (nowtime_l >= total_sec)
    {
        stop_rec(1);
        show_err_msg();
        return RESULT_MAIN;
    }
    //    itoa_3(nowtime_s.hour, &g_now_time[0])//macro qac
    //    g_now_time[3] = ':';
    itoa_2(nowtime_s.minute, &g_now_time[4]);//macro qac
    //    g_now_time[6] = ':';
    itoa_2(nowtime_s.second, &g_now_time[6]);//macro qac
    LEDPuts(NUMBER1, &g_now_time[4], mode);
    LEDDisplay(LED_COL, 0xff, 1);
    return NULL;
}
//切换录音状态
uint8 change_rec_status(void)
{
    uint8 retval = NULL;
    if (need_change_rec_status == 0)
    {
        return NULL;
    }
    need_change_rec_status = 0;
    if (rec_status == rec_start)
    {
        rec_status = rec_pause;
        recSendCommand(MC_GETTIME, (void *) &g_rec_time); //取现在时间
        recSendCommand(MC_PAUSE, 0);
        SetCurUI_Type(RecPause_UI);

    }
    else if (rec_status == rec_pause)
    {
        rec_status = rec_start;
        recSendCommand(MC_RECORD, 0);
        SetCurUI_Type(RecOn_UI);
    }
    else
    {
        rec_status = rec_start;
        retval = rec_init();
    }
    return retval;
}
void deal_play_msg(void)
{
    if (CurUI_Type == STANDBY_UI)
    {
        SetCurUI_Type(RecStop_UI);
        return;
    }
    if (CurUI_Type == RecOn_UI)
    {
        SetCurUI_Type(RecPause_UI);
    }
    else
    {
        SetCurUI_Type(RecOn_UI);
    }
    need_change_rec_status = TRUE;
}
//停止录音，并保存文件
void stop_rec(uint8 get_status)
{
    uint16 i = 0;
    if (rec_status == rec_stop)
    {
        return;
    }
    if (rec_status == rec_pause)
    {
        recSendCommand(MC_RECORD, 0);
    }
    recSendCommand(MC_STOP, 0);
    if (get_status == 1)
    {
        while (1)
        {

            i++;
            recSendCommand(MC_GETSTATUS, (void *) &rec_stat);
            //当录音停止后，只有等到接受到的消息为stop时，才能调用close，否则文件保存不成功
            if ((rec_stat.status == RECORDING_STOP) || (i >= 2000))
            {
                break;
            }
            if (deal_recoding_status() != NULL)
            {
                break;
            }
        }
    }
    recSendCommand(MC_CLOSE, 0); //关闭录音
    rec_status = rec_stop;
    if (g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        g_record_vars.mp2_num++;
    }
    else
    {
        g_record_vars.wav_num++;
    }
}
uint8 deal_mode_msg(void)
{
    if (rec_status == rec_stop)
    {
        if (g_record_vars.location.disk == g_disk_char[1])
        {
            if (USBH_Check_Udisk())
            {
                g_record_vars.location.disk = g_disk_char[0];
            }
            else
            {
                return 1;
            }

        }
        else
        {
            if (DRV_DetectUD(1) == 0x20)
            {
                g_record_vars.location.disk = g_disk_char[1];
            }
            else
            {
                return 1;
            }

        }
        if (CheckStoreOK() == 0)
        {
            return RESULT_MAIN;
        }
        //		need_change_rec_status=TRUE;
        SetCurUI_Type(RecStop_UI);
        return NULL;
    }
    else
    {
        stop_rec(1);
        SetCurUI_Type(RecStop_UI);
    }
    return NULL;
}

uint8 deal_recoding_status(void)
{
    uint8 retval = NULL;
    retWavrProceed_e mc_result = WRITE_NONE;
    if (rec_status != rec_start)
    {
        return NULL;
    }
    mc_result = wavrProceed();
    if (mc_result == WRITE_ONE) //已写一个page
    {
        g_file_size++;
        g_free_space--;
        if (g_free_space == 1)
        {
            stop_rec(1);
            show_err_msg();
            retval = deal_mode_msg();
            if (retval != 0)

            {
                retval = RESULT_MAIN;
            }
        }
    }
    else if (mc_result == WRITE_WRONG) //致命错误
    {
        while (ap_get_message() != NULL)
        {
            ;//qac
        }
        stop_rec(0);
        show_err_msg();
        retval = deal_mode_msg();
        if (retval != 0)

        {
            retval = RESULT_MAIN;
        }
    }
    else
    {

    }
    return retval;
}

/*
 ********************************************************************************
 * Description : 录音界面显示
 *
 * Arguments   : NULL
 *
 * Returns     : 结果消息或0
 RESULT_USER_STOP    //用户按键结束
 *
 * Notes       :
 *
 ********************************************************************************
 */

uint8 show_rec(void)
{
    uint8 key;
    uint8 result = NULL; //result for return
    retWavrProceed_e mc_result = WRITE_NONE;

    g_rec_time.hour = 0;
    g_rec_time.minute = 0;
    g_rec_time.second = 0;
    SetCurUI_Type(RecOn_UI);
    rec_status = rec_stop;
    need_draw = TRUE;
    need_change_rec_status = TRUE;
    review_disk = TRUE;
    if(g_record_vars.rec_source == AP_MICIN)
    {
        EnableAIN(AD_MIC);
        SetAINGain(0, g_record_vars.rec_InputGain);
    }
    else
    {
        EnableAIN(AD_FM);
        SetAINGain(g_record_vars.rec_InputGain / 2, 0 );
    }
    while (1)
    {
        result = change_rec_status();
        if (result != NULL)
        {
            return result;
        }
        if (need_draw)
        {
            ui_draw();
            need_draw = FALSE;
        }
        key = ap_get_message();
        switch (key)
        {
        case AP_MSG_RTC:
        case AP_MSG_WAIT_TIMEOUT:
            if (rec_status == rec_pause)
            {
                show_what = !show_what;
                time_draw(show_what);
                break;
            }
            if (rec_status == rec_start)
            {
                recSendCommand(MC_GETSTATUS, (void *) &rec_stat); //add by lei
                if (rec_stat.status > 0xf0)
                {
                    recSendCommand(MC_STOP, 0);
                    return 0;
                }
                result = time_draw(1);
            }
            break;
        case AP_KEY_PLAY | AP_KEY_UP:
            deal_play_msg();
            break;

        case AP_KEY_MODE | AP_KEY_UP:
            result = RESULT_MAIN;
            break;
        case AP_KEY_MODE | AP_KEY_LONG:
            longkey_flag = 1;
            result = deal_mode_msg();
            if (result != RESULT_MAIN)
            {
                result = NULL;
            }
            break;
        case AP_KEY_CLOCK | AP_KEY_UP:
            if (rec_status == rec_stop)
            {
                result = set_clock_ui(1);
                if (result != 0)
                {
                    return result;
                }
#if defined(RECORD_AP)
                //处理TTS播报时卡或u盘拔出没有及时处理的情况
                if(result == AP_MSG_SD_OUT || result == AP_MSG_UH_OUT)
                {
                    result = deal_mode_msg();
                }
#endif
                SetCurUI_Type(RecStop_UI);
            }
            break;
        case AP_KEY_CLOCK | AP_KEY_LONG:
#if defined(RECORD_AP)
            longkey_flag = 1;
            if (rec_status == rec_stop)
            {
                result = deal_clock_report();
            }
#endif
            break;
        case AP_KEY_AUTOSLEEP | AP_KEY_UP:
            if (rec_status == rec_stop)
            {
                set_autosleep_time(Key_None);
            }
            break;
        case AP_MSG_SD_OUT:
            if (g_record_vars.location.disk == g_disk_char[1])
            {
                rec_status = rec_start;
                stop_rec(0);
                show_err_msg();
                result = deal_mode_msg();
            }
            break;
        case AP_MSG_UH_OUT:
            if (g_record_vars.location.disk == g_disk_char[0])
            {
                rec_status = rec_start;
                stop_rec(0);
                show_err_msg();
                result = deal_mode_msg();
            }
            break;
        case AP_MSG_SD_IN:
            if (rec_status != rec_start)
            {
                result = RESULT_MUSIC_CPLAY;
            }
            break;
        case AP_MSG_UH_IN:
            if (rec_status != rec_start)
            {
                result = RESULT_MUSIC_UPLAY;
            }
            break;
        case AP_MSG_LINEIN_IN:
            if (rec_status != rec_start)
            {
                result = RESULT_LINEIN;
            }
            break;
        case AP_KEY_REC | AP_KEY_UP:
        case AP_KEY_REC | AP_KEY_LONG:
            if (rec_status == rec_stop)
            {
                result = ap_handle_hotkey(key);
            }
            break;
        default:
            result = ap_handle_hotkey(key);
            break;
        }//switch
        if (result != NULL)
        {
            stop_rec(1);
            break;
        }
        result = deal_recoding_status();
        if (result != NULL)
        {
            break;
        }
    }//while(1)

    return result;
}
#if defined(RECORD_AP)
uint8 deal_report_result(uint8 retval)
{
    if(retval == RESULT_MAIN)
    {
        return RESULT_MAIN;
    }
    else if(retval == RESULT_SD_OUT)
    {
        //原先录音到卡中，播报时拔出卡
        if (g_record_vars.location.disk == g_disk_char[1])
        {
            if(deal_mode_msg() != NULL)
            {
                return RESULT_MAIN;
            }
        }
        return NULL;
    }
    else if(retval == RESULT_UHOST_OUT)
    {
        //原先录音到U盘中，播报时拔出u盘
        if (g_record_vars.location.disk == g_disk_char[0])
        {
            if(deal_mode_msg() != NULL)
            {
                return RESULT_MAIN;
            }
        }
        return NULL;
    }
#if 0
    else if(retval == AP_MSG_UH_IN)
    {
        //原先录音到U盘中，播报时拔出u盘再插上u盘
        if (g_record_vars.location.disk == g_disk_char[0])
        {
            if (USBH_Check_Udisk())
            {
                if (CheckStoreOK() == 0)
                {
                    return RESULT_MAIN;
                }
                SetCurUI_Type(RecStop_UI);
            }
            else if(deal_mode_msg() != NULL)
            {
                return RESULT_MAIN;
            }
        }
    }
    else if(retval == AP_MSG_SD_IN)
    {
        //原先录音到卡盘中，播报时拔出卡再插上卡
        if (g_record_vars.location.disk == g_disk_char[1])
        {
            if (DRV_DetectUD(1) == 0x20)
            {
                if (CheckStoreOK() == 0)
                {
                    return RESULT_MAIN;
                }
                SetCurUI_Type(RecStop_UI);
            }
            else if(deal_mode_msg() != NULL)
            {
                return RESULT_MAIN;
            }
        }
    }
#endif
    return retval;
}
uint8 deal_clock_report(void)
{
    uint8 retval;
    disable_adda();
    retval = report_time_info(g_record_vars.location.disk, FALSE);
    enable_adda();
    return deal_report_result(retval);
}
#endif

