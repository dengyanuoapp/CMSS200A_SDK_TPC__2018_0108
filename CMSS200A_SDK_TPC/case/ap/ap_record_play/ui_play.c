#include "ap_record_play.h"

#pragma name(UI_PLAY)

//#define MUSIC_MP3

Open_param_t g_Openparam;
breakPT_t BreakPTSave;
uint8 *name = "TEST    WAV";

void ShowNowTime(void);
void ui_draw_play_music(void);
uint8 deal_play_status(void);
bool musicpSendCommand_bank(uint8 cmd, void *param);


uint8 ui_play(void)
{
    uint8 result = 0;
    uint8 need_draw;
    uint8 key;

    memset(&BreakPTSave, 0, sizeof(breakPT_t));
    g_Openparam.filename = g_record_vars.location.filename;//�ļ�����ַ
    g_Openparam.BreakPTSave = &BreakPTSave;//���ݲ�����ַ
    g_Openparam.typeerror = 0;//Ĭ��Ϊ������ʽ�ļ�
    g_Openparam.SoftVolumeMax = 0;
    g_Openparam.FadeInTime = 0x03;
#ifndef MUSIC_MP3
    g_Openparam.musicframeoffset = 0x200;
#endif

    if(!DRV_ActiveDrive(g_record_vars.location.disk, 1))
    {
        result = RESULT_MAIN;
        goto play_exit;
    }
    if((g_record_vars.location.disk != 'M') && \
            (FALSE == FS_CD(FS_CD_ROOT))) //���⴦������Ŀ¼��ʱ����FS_SetCurDir����
    {
        result = RESULT_MAIN;
        goto play_exit;
    }

    musicpSendCommand_bank(MC_OPEN, (void *) &g_Openparam);

    if(!musicpSendCommand_bank(MC_LOAD, NULL))
    {
        result = RESULT_CURRENT_AP;
        goto play_exit;
    }

    if (!musicpSendCommand_bank(MC_PLAY, (void *) Nor_open))
    {
        musicpSendCommand_bank(MC_STOP, NULL);
        musicpSendCommand_bank(MC_CLOSE, NULL);
        result = RESULT_CURRENT_AP;
        goto play_exit;
    }

    while(1)
    {
        if (need_draw)
        {
            ui_draw_play_music();
            need_draw = FALSE;
        }

        key = ap_get_message();
        switch (key)
        {
        case AP_MSG_RTC:
        case AP_MSG_WAIT_TIMEOUT:
            ShowNowTime();
            break;

        case AP_KEY_PLAY | AP_KEY_UP:
            result = RESULT_CURRENT_AP;
            break;

            /*case AP_KEY_VOLADD | AP_KEY_UP:
            case AP_KEY_VOLADD | AP_KEY_LONG:
                if (g_comval.volume < g_comval.VolumeMax)
                {
                    g_comval.volume++;
                    set_dac_volume(g_comval.volume);
                }
                break;
            case AP_KEY_VOLSUB | AP_KEY_UP:
            case AP_KEY_VOLSUB | AP_KEY_LONG:
                if (g_comval.volume > 0)
                {
                    g_comval.volume--;
                    set_dac_volume(g_comval.volume);
                }
                break;*/

        case AP_KEY_MODE | AP_KEY_UP:
            result = RESULT_MAIN;
            break;

        case AP_MSG_SD_OUT:
            if (g_record_vars.location.disk == g_disk_char[1])
            {
                show_err_msg();
                result = RESULT_MAIN;
            }
            break;
        case AP_MSG_UH_OUT:
            if (g_record_vars.location.disk == g_disk_char[0])
            {
                show_err_msg();
                result = RESULT_MAIN;
            }
            break;

        case AP_MSG_SD_IN:
            result = RESULT_MUSIC_CPLAY;
            break;
        case AP_MSG_UH_IN:
            result = RESULT_MUSIC_UPLAY;
            break;
        case AP_MSG_LINEIN_IN:
            result = RESULT_LINEIN;
            break;

        case AP_KEY_USBTEST | AP_KEY_LONG:
            /* do nothing */
            break;

        default:
            result = ap_handle_hotkey(key);
            break;
        }//switch
        if (result != NULL)
        {
            musicpSendCommand_bank(MC_STOP, NULL);
            musicpSendCommand_bank(MC_CLOSE, NULL);
            break;
        }
        result = deal_play_status();
        if (result != NULL)
        {
            musicpSendCommand_bank(MC_STOP, NULL);
            musicpSendCommand_bank(MC_CLOSE, NULL);
            break;
        }
    }
play_exit:
    return result;
}

/*
 ********************************************************************************
 * Description : ��ʾ��ǰ��ʱ��
 *
 * Arguments   : mode   ˢ��ģʽ  0--ֻˢ��ǰʱ��   1--ˢ��ǰʱ���ʣ���¼ʱ��
 *
 * Returns     :  NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ShowNowTime(void)
{
    Full_time_t curtime_buf;
    time_t BreakPTDisTime;
    uint8 temp_buffer[5];

#ifdef MUSIC_MP3
    if (mp3pSendCommand(MC_GETTIME, (void *)&curtime_buf) == 0) //ֱ�ӵ���apSendCommand��ֹ��bank��AP_UI_PLY�����г�ȥ�����ڸ߱����ʸ���ʱ��ֹ�п�������
#else
    if (wavpSendCommand(MC_GETTIME, (void *)&curtime_buf) == 0)
#endif
    {
        return;
    }
    BreakPTDisTime.hour = curtime_buf.TimeHour;
    BreakPTDisTime.minute = curtime_buf.TimeMinute;
    BreakPTDisTime.second = curtime_buf.TimeSecond;

    itoa_2(BreakPTDisTime.minute, temp_buffer);
    itoa_2(BreakPTDisTime.second, &temp_buffer[2]);
    LEDPuts(NUMBER1, temp_buffer, 1);
    LEDDisplay(LED_COL, 0xff, 1);
}

/*
 ********************************************************************************
 * Description : UI������ʾ
 *
 * Arguments   : NULL
 *
 * Returns     :  NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ui_draw_play_music(void)
{
    uint8 tmp;

    LEDDisplay(LED_PAUSE, 0xff, 0);
    LEDDisplay(LED_PLAY, 0xff, 1);

    /*  ��ʾ¼��Ŀ��洢���� */
    if(g_record_vars.location.disk == 'M')
    {
        LEDDisplay(LED_USB, 0xff, 0);
        LEDDisplay(LED_SD, 0xff, 0);
    }
    else
    {
        tmp = (g_record_vars.location.disk == 'H') ? 1 : 0;
        LEDDisplay(LED_USB, 0xff, !tmp);
        LEDDisplay(LED_SD, 0xff, tmp);
    }

    ShowNowTime();
}

bool musicpSendCommand_bank(uint8 cmd, void *param)
{
    bool result = FALSE;

#ifdef MUSIC_MP3
    result = mp3pSendCommand(cmd, param);
#else
    result = wavpSendCommand(cmd, param);
#endif

    return result;
}


/*
 ********************************************************************************
 *             void deal_play_status(void)
 *
 * Description : �����Ź����е��쳣���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.����ǲ���״̬�£���ֱ�ӷ���
 *			   2.������ų�����ת��������������
 *			   3.������ŵ�β����ת���ļ�β����������
 *			   4.������ŵ�ͷ����ת���ļ�ͷ����������
 *			   5.��������²���ˢ�²���ʱ��
 ********************************************************************************
 */
uint8 deal_play_status(void)
{
    uint8 retval = 0;
    music_play_status_t status_buf;

    apSendCommand(MC_GETSTATUS, Music_WAV, (void *)(&status_buf)); //ֱ�ӵ���apSendCommand��ֹ��bank��AP_UI_PLY�����г�ȥ�����ڸ߱����ʸ���ʱ��ֹ�п�������
    if (status_buf.status == PLAYING_ERROR)
    {
        ap_sleep(3);
        if ((g_record_vars.location.disk == 'U') && !USBH_Check_Udisk())
        {
            retval = RESULT_MAIN;
        }
        else if (g_record_vars.location.disk == 'H' && (DRV_DetectUD(1) != 0x20))
        {
            retval = RESULT_MAIN;
        }
        else
        {
            retval = RESULT_CURRENT_AP;
        }
    }
    else if (status_buf.status == PLAYING_REACH_END)
    {
        retval = RESULT_CURRENT_AP;
    }
    else if (status_buf.status == PLAYING_REACH_HEAD)
    {
        retval = 0;
    }
    return retval;
}

