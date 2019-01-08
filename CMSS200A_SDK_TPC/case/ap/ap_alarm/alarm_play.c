/*
 *******************************************************************************
 *                ACTOS AP
 *        system setting ap of sdk 3.0, ui part,
 *    this function is too big to fit in a bank with others
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: music_play.c,v 1.1 2008/10/12 09:40:36 stevenluo Exp $
 *******************************************************************************
 */
#include "actos.h"
#include "enhanced.h"
#include "ap_common.h"
#include "ui_alarm.h"

#define PLAYING_TIME   360  //3分钟
#define PAUSE_TIME      600//5分钟
#define MAX_PLAY_TIME 3//最后播放次数
#pragma name(ALARM_UI_PLY)
#define ALARMExtNum  6
extern uint8 key;
extern uint8 playing_flag;
extern uint8 need_draw;
extern Music_type_a g_currentMusicType;
extern uint8 g_TagInfoBuf[92];//用于放TAG信息的BUF
const uint8 Extalarmstr[ALARMExtNum][4] =
{
    "MP1", "MP2", "MP3", "WMA", /*"ASF",*/
    "WMV", "WAV"
};

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

bool AlarmMusicpSendCommand_bank(BYTE cmd, void *param)
{
    bool result = FALSE;
    //根据不同的音乐类型发不同的命令
    ClearWatchDog();
    switch (cmd)
    {
    case MC_PLAY:
        break;

    default:
        break;
    }
    switch (g_currentMusicType)
    {
    case Music_MP3: //mp3
        result = mp3pSendCommand(cmd, param);
        break;

    case Music_WMA: //wma
    case Music_WMV: //wmv
        result = wmapSendCommand(cmd, param);
        break;
    case Music_WAV:
        result = wavpSendCommand(cmd, param);

    default:
        break;
    }
    return result;
}

bool AlarmMusicpSendCommand(uint8 cmd, void *param)
{
    switch (cmd)
    {
    case MC_GETTIME:
    case MC_GETSAPLE:
    case MC_GETSTATUS:
    case MC_GETAUDIOFMT:
    case MC_FADEINOUT:
        return wavpSendCommand(cmd, param);
    default:
        return AlarmMusicpSendCommand_bank(cmd, param);
    }
    return FALSE;
}
Music_type_a ALARMCheckMusicType(uint8 *filename)
{
    uint8 cnt;
    uint8 ext[4];
    Music_type_a ret;
    memcpy(ext, filename + 8, 3);
    ext[0] = ((ext[0] >= 'a') && (ext[0] <= 'z')) ? (ext[0] - 'a' + 'A') : ext[0];
    ext[1] = ((ext[1] >= 'a') && (ext[1] <= 'z')) ? (ext[1] - 'a' + 'A') : ext[1];
    ext[2] = ((ext[2] >= 'a') && (ext[2] <= 'z')) ? (ext[2] - 'a' + 'A') : ext[2];
    ext[3] = 0;
    for (cnt = 0; cnt < Music_TypeMax; cnt++)
    {
        if (strcmp(ext, Extalarmstr[cnt]) == 0)
        {
            ret = Music_NON + cnt + 1;
            if ((ret == Music_MP1) || (ret == Music_MP2))
            {
                return Music_MP3;
            }
            else
            {
                return ret;
            }
        }
    }
    return Music_NON;
}

/* 获取首帧偏移, 避免将文件头也进行解码, 产生杂音 */
void AlarmGetInfo(void)
{
    ID3Info_t ID3;
    audio_format_t pFormat;
    memset(g_TagInfoBuf, 0, 92);
    ID3.TIT2_length = 30;
    ID3.TPE1_length = 30;
    ID3.TALB_length = 30;

    ID3.TIT2_buffer = &g_TagInfoBuf[0];
    ID3.TPE1_buffer = &g_TagInfoBuf[30];
    ID3.TALB_buffer = &g_TagInfoBuf[60];

    ID3.TENC_buffer = &g_TagInfoBuf[90];
    switch (g_currentMusicType)
    {
    case Music_WMA:
        g_Openparam.typeerror = wmaCheckType(NULL, &g_Openparam.musicframeoffset, &g_Openparam.musictotaltime,
                                             &pFormat, &ID3);
        break;

    case Music_WAV:
        g_Openparam.typeerror
            = wavCheckType(NULL, &g_Openparam.musicframeoffset, &g_Openparam.musictotaltime, &pFormat);
        break;

    default:
        break;
    }

}
uint8 alarm_play(uint8 type)
{
    if (type == 0)
    {
        AlarmMusicpSendCommand(MC_OPEN, (void *) &g_Openparam);
        if (AlarmMusicpSendCommand(MC_LOAD, NULL) == 0)
        {
            return 1;
        }
        //        AlarmMusicpSendCommand(MC_SETVOLUME, (void *) g_comval.g_alarm.volume);
        AlarmMusicpSendCommand(MC_PLAY, NULL);
    }
    else
    {
        mp3SDSendCommand(MC_OPEN, &g_Openparam);
        if (mp3SDSendCommand(MC_LOAD, NULL) == 0)
        {
            return 1;
        }
        //        mp3SDSendCommand(MC_SETVOLUME, (void *) g_comval.g_alarm.volume);
        mp3SDSendCommand(MC_PLAY, NULL);
    }
    return 0;
}
void alarm_stop(uint8 type)
{
    if (type == 0)
    {
        AlarmMusicpSendCommand(MC_STOP, NULL);
        AlarmMusicpSendCommand(MC_CLOSE, NULL);
    }
    else
    {
        mp3SDSendCommand(MC_STOP, NULL);
        mp3SDSendCommand(MC_CLOSE, NULL);
    }
    LEDDisplay(LED_PLAY, 0xff, 0);
    LEDDisplay(LED_PAUSE, 0xff, 1);
}
uint8 play_alarm_music(uint8 type)
{
    music_play_status_t status_buf;
    uint8 retval = NULL;
    uint8 key;
    playing_flag = 0;
    play_times = 1;
    need_draw = TRUE;
    show_what = 1;
    LEDClearScreen();
    set_dac_volume(g_comval.volume);
    while (1)
    {
        ClearWatchDog();
        if (need_draw)
        {
            ShowTimer();
            need_draw = FALSE;
        }
        if (playing_flag == 0)
        {
            if (alarm_play(type) != 0)
            {
                return RESULT_MAIN;
            }
            LEDDisplay(LED_PAUSE, 0xff, 0);
            LEDDisplay(LED_PLAY, 0xff, 1);
            playing_flag = 2;
            if (playing_count >= PAUSE_TIME)
            {
                play_times++;
                playing_count = 0;
            }

        }
        key = ap_get_message();
        switch (key)
        {
        case AP_MSG_RTC:
        case AP_MSG_WAIT_TIMEOUT:
            if (playing_flag != 0)
            {
                playing_count++;
                if (playing_flag == 1)
                {
                    //如果暂停情况下，则5分钟后继续播放
                    if (playing_count >= PAUSE_TIME)
                    {
                        playing_flag = 0;
                    }
                }
                else
                {
                    //播放状态下，门铃响3分钟后自动退出
                    if (playing_count >= PLAYING_TIME)
                    {
                        playing_count = 0;
                        alarm_stop(type);
                        if (play_times >= MAX_PLAY_TIME)
                        {
                            playing_flag = 0;
                            retval = RESULT_MAIN;
                        }
                        else
                        {
                            playing_flag = 1;
                        }
                    }
                }
            }
            show_what = !show_what;
            need_draw = TRUE;
            break;
        case AP_MSG_SD_IN:
        case AP_MSG_UH_IN:
        case AP_MSG_LINEIN_IN:
        case AP_MSG_LINEIN_OUT:
            break;
        case AP_MSG_SD_OUT:
            if (type != 0)
            {
                break;
            }
            if (g_comval.g_alarm.location.disk == 'H')
            {
                retval = RESULT_MAIN;
            }
            break;

        case AP_MSG_UH_OUT:
            if (type != 0)
            {
                break;
            }
            if (g_comval.g_alarm.location.disk == 'U')
            {
                retval = RESULT_MAIN;
            }
            break;
        case AP_KEY_PLAY | AP_KEY_UP:
        case AP_KEY_MODE | AP_KEY_UP: //取消退出
        case AP_KEY_MODE | AP_KEY_LONG:
            retval = RESULT_MAIN;
            break;
        default:
            if (((key & AP_KEY_UP) == AP_KEY_UP) || (key == AP_KEY_LONGUP) ||
                    (key == (AP_KEY_USBTEST | AP_KEY_LONG)))
            {
                if (playing_flag == 2)
                {
                    alarm_stop(type);
                    if (play_times >= MAX_PLAY_TIME)
                    {
                        playing_flag = 0;
                        retval = RESULT_MAIN;
                    }
                    else
                    {
                        playing_flag = 1;
                        playing_count = 0;
                    }
                }
            }
            else
            {
                retval = ap_handle_hotkey(key);
            }
            break;
        } //switch
        if (retval != NULL)
        {
            if (playing_flag == 2)
            {
                alarm_stop(type);
            }
            return retval;
        }
        if (playing_flag == 2)
        {
            if (type == 0)
            {
                AlarmMusicpSendCommand(MC_GETSTATUS, (void *) (&status_buf));
            }
            else
            {
                mp3SDSendCommand(MC_GETSTATUS, (&status_buf));
            }

            if (status_buf.status == PLAYING_REACH_END)
            {
                alarm_stop(type);
                //如果门铃不足3分钟时，则继续从头开始播放，直到3分钟时间到
                if (playing_count < PLAYING_TIME)
                {
                    playing_flag = 0;
                }
                else
                {
                    if (play_times >= 2)
                    {
                        playing_flag = 0;
                        return RESULT_MAIN;
                    }
                    else
                    {
                        playing_flag = 1;
                    }
                }
            }
            else if (status_buf.status == PLAYING_ERROR)
            {
                alarm_stop(type);
                return RESULT_MAIN;
            }
        }
    }//while
    return RESULT_MAIN;
}
