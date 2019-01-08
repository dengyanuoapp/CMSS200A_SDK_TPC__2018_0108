/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *       History Record:
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(DEAL_KEY_MSG)

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
        result = deal_timeout_msg();
        break;

    case AP_KEY_PLAY | AP_KEY_UP: //play���������ش���
        result = deal_play_key();
        break;

    case AP_KEY_MODE | AP_KEY_UP: //AP�л��ȼ�
        result = deal_mode_key();
        break;

    case AP_KEY_MUTE | AP_KEY_UP: //mute����
        mute_flag = !mute_flag;
        if (!mute_flag)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL);
        }
        else
        {
            fmSendCommand(FM_MUTE, SetMUTE, NULL);
        }
        need_draw = TRUE;
        break;

    case AP_KEY_NEXT | AP_KEY_UP:
        result = deal_next_prev_key(AP_KEY_NEXT);
        break;

    case AP_KEY_PREV | AP_KEY_UP:
        result = deal_next_prev_key(AP_KEY_PREV);
        break;

    case AP_KEY_NEXT | AP_KEY_LONG:
        result = deal_next_prev_longkey(AP_KEY_NEXT);
        break;

    case AP_KEY_PREV | AP_KEY_LONG:
        result = deal_next_prev_longkey(AP_KEY_PREV);
        break;

    case AP_KEY_VOL | AP_KEY_UP: //��������
        result = deal_radio_vol_msg(NULL);
        break;

    case AP_KEY_VOLADD | AP_KEY_UP:
    case AP_KEY_VOLADD | AP_KEY_LONG:
        result = deal_radio_vol_msg(Key_Next);
        break;

    case AP_KEY_VOLSUB | AP_KEY_UP:
    case AP_KEY_VOLSUB | AP_KEY_LONG:
        result = deal_radio_vol_msg(Key_Prev);
        break;

    case AP_KEY_MODE | AP_KEY_LONG:
        longkey_flag = 1;
        deal_long_mode_key();
        break;

    case AP_KEY_CLOCK | AP_KEY_UP: //���õ�ǰʱ��
        result = set_clock_ui(1);
        if (result != 0)
        {
            return result;
        }
        result = 0;
        SetCurUI_Type(FMFreq_UI);
        break;

    case AP_MSG_LINEIN_IN:
        result = RESULT_LINEIN;
        break;

    case AP_KEY_CHANSUB | AP_KEY_UP:
    case AP_KEY_CHANSUB | AP_KEY_LONG:
        deal_chan_msg(AP_KEY_PREV);
        break;

    case AP_KEY_EQ | AP_KEY_UP:
        result = deal_eq_msg(Key_None);
        break;
    case AP_KEY_CHANADD | AP_KEY_UP:
    case AP_KEY_CHANADD | AP_KEY_LONG:
        deal_chan_msg(AP_KEY_NEXT);
        break;
#ifdef AP_RADIO_REPORT_EN
    case AP_KEY_VOL | AP_KEY_LONG:
    case AP_KEY_CLOCK | AP_KEY_LONG:
        longkey_flag = 1;
        fmSendCommand(FM_MUTE, SetMUTE, NULL);
        exit_fmin_channel();
        if(key == (AP_KEY_VOL | AP_KEY_LONG))
        {
            result = report_frequency(&g_FMStatus, report_buf);
        }
        else
        {
            result = report_time_info(0, TRUE);
        }
        init_fmin_channel();
        if(result != 0)
        {
            return result;
        }
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
        break;
#endif
    case AP_KEY_AUTOSLEEP | AP_KEY_UP:
        set_autosleep_time(Key_None);
        break;

    default: //�ȼ�����
        if ((key >= (AP_KEY_NUM0 | AP_KEY_UP)) && (key <= (AP_KEY_NUM9 | AP_KEY_UP)))
        {
            deal_digital_key(key);
        }
        else
        {
            result = ap_handle_hotkey(key);
        }
        break;
    }
    return result;
}

/********************************************************************************
 * Description : ����mode��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_mode_key(void)
{
    if (CurUI_Type != FMFreq_UI)
    {
        if (CurUI_Type == FMInputFreq_UI)
        {
            deal_with_input_freq();
        }
        SetCurUI_Type(FMFreq_UI);
        return NULL;
    }
    return RESULT_MAIN;
}

/********************************************************************************
 * Description : ������mode��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_long_mode_key(void)
{
    if ((CurUI_Type != FMFreq_UI) && (CurUI_Type != FMChannel_UI))
    {
        return NULL;
    }
    if (g_radio_config.band_mode == Bank_Japan)
    {
        g_radio_config.band_mode = Bank_US_Europe;
        radio_param.param1 = g_radio_config.fmstation_us;

        memcpy(&FMStatusJP, &g_FMStatus, sizeof(g_FMStatus)); //����jpƵ������
        memcpy(&g_FMStatus, &FMStatusUS, sizeof(g_FMStatus)); //����usƵ������
    }
    else if (g_radio_config.band_mode == Bank_US_Europe)
    {
        g_radio_config.band_mode = Bank_Japan;
        radio_param.param1 = g_radio_config.fmstation_jp;

        memcpy(&FMStatusUS, &g_FMStatus, sizeof(fm_status_t)); //����usƵ������
        memcpy(&g_FMStatus, &FMStatusJP, sizeof(fm_status_t)); //����jsƵ������
    }

    fmSendCommand(FM_MUTE, SetMUTE, NULL); //���þ���
    fmSendCommand(FM_MID_PLAY, NULL, NULL); //���ŵ�̨
    fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

    need_draw = TRUE;
    return NULL;
}

/********************************************************************************
 * Description : ����chan + �� - ��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_chan_msg(uint8 key)
{
    if (CurUI_Type != FMFreq_UI)
    {
        //SetCurUI_Type(FMFreq_UI);
        return 0;
    }
    else if (key == NULL)
    {
        key = AP_KEY_NEXT;
    }

    if (key == AP_KEY_NEXT)
    {
        g_FMStatus.freq += 1;
        if ((g_FMStatus.freq >= USFREQ_MAX) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MIN;
        }
        else if ((g_FMStatus.freq >= JPFREQ_MAX) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MIN;
        }
    }
    else
    {
        g_FMStatus.freq -= 1;
        if ((g_FMStatus.freq <= USFREQ_MIN) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MAX;
        }
        else if ((g_FMStatus.freq <= JPFREQ_MIN) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MAX;
        }
    }
    //mute FM ģ�飬������BANK��ɸ���
    fmSendCommand(FM_MUTE, SetMUTE, NULL);

    fmSendCommand(FM_SETFREQ, (void *) g_FMStatus.freq, NULL);
    //release mute
    if (g_comval.volume != 0)
    {
        fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
    }
    fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
    fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

    fmSendCommand(FM_MID_SAVE, &radio_param, NULL);

    need_draw = TRUE;
    return NULL;
}

/********************************************************************************
 * Description : ����������
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_radio_vol_msg(uint8 key)
{
    //�Զ���̨���ֶ���̨����Ӧ��������
    if ((CurUI_Type == FMAutoSearch_UI) || (CurUI_Type == FMManualSearch_UI))
    {
        return NULL;
    }
    if((key == Key_Next) && (g_comval.volume == 0))
    {
        fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�����?
    }
    return deal_vol_msg(key, FMFreq_UI);
#if 0
    if (CurUI_Type != Volume_UI)
    {
        SetCurUI_Type(Volume_UI);
        return 0;
    }
    else if (key == NULL)
    {
        SetCurUI_Type(FMFreq_UI);
        return 0;
    }

    if (mute_flag)
    {
        mute_flag = FALSE;
        volume_callback(g_comval.volume);
    }

    if (key == AP_KEY_NEXT)
    {
        if (g_comval.volume < g_comval.VolumeMax)
        {
            g_comval.volume++;
        }
    }
    else
    {
        if (g_comval.volume > 0)
        {
            g_comval.volume--;
        }
    }
    volume_callback(g_comval.volume);

    need_draw = TRUE;
    return NULL;
#endif
}
/********************************************************************************
 * Description : ���������Ƶ��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_with_input_freq(void)
{
    uint16 freq;
    freq = DigitalBuf[3] + DigitalBuf[2] * 10 + DigitalBuf[1] * 100 + DigitalBuf[0] * 1000;
    if (check_freq_valid(freq))
    {
        g_FMStatus.freq = freq;
        //mute FM ģ�飬������BANK��ɸ���
        fmSendCommand(FM_MUTE, SetMUTE, NULL);

        fmSendCommand(FM_SETFREQ, (void *) g_FMStatus.freq, NULL);
        //release mute
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

        fmSendCommand(FM_MID_SAVE, &radio_param, NULL);

        return 1;
    }
    return 0;
}
/********************************************************************************
 * Description : ���Ƶ��ֵ�Ƿ�����Чֵ
 *
 * Arguments  :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************/
uint8 check_freq_valid(uint16 freq)
{
    if (g_radio_config.band_mode == Bank_US_Europe)
    {
        if ((freq <= USFREQ_MAX) && (freq >= USFREQ_MIN))
            return TRUE;
    }
    else if (g_radio_config.band_mode == Bank_Japan)
    {
        if ((freq <= JPFREQ_MAX) && (freq >= JPFREQ_MIN))
            return TRUE;
    }
    return FALSE;
}
/********************************************************************************
 * Description : ����ʱ��Ϣ
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_timeout_msg(void)
{
    //�Զ���̨���ֶ���̨����Ӧ��ʱ��Ϣ
    if ((CurUI_Type == FMAutoSearch_UI) || (CurUI_Type == FMManualSearch_UI))
    {
        return NULL;
    }
    if (CurUI_Type != FMFreq_UI)
    {
        if (CurUI_Type == FMInputFreq_UI)
        {
            deal_with_input_freq();
        }
        SetCurUI_Type(FMFreq_UI);
    }
    return NULL;
}
/********************************************************************************
 * Description : ����play�������Զ���̨�ͷ��Զ���̨֮���л�
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_play_key(void)
{
    uint8 result;

    if (CurUI_Type == FMFreq_UI)
    {
        fmSendCommand(FM_MUTE, SetMUTE, NULL);

        result = AutoSearchHandle(SEARCH_NEXT);
        if ((result != RESULT_NULL) && (result != RESULT_IGNORE) && (result != RESULT_RECORD))
        {
            return result;
        }

        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

        if (g_FMStatus.station)
        {
            SetCurUI_Type(FMChannel_UI);
        }
        else
        {
            SetCurUI_Type(FMFreq_UI);
        }
    }
    else
    {
        SetCurUI_Type(FMFreq_UI);
    }

    return result;
}

/********************************************************************************
 * Description : ����prev��next��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_next_prev_key(uint8 key)
{
    uint8 direct;

    switch (CurUI_Type)
    {
    case FMFreq_UI:
    case FMChannel_UI:
        if (key == AP_KEY_PREV)
        {
            direct = DOWN;
        }
        else
        {
            direct = UP;
        }
        fmSendCommand(FM_MUTE, SetMUTE, NULL);

        fmSendCommand(FM_MID_TURN, (void *) direct, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL);
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
        SetCurUI_Type(FMChannel_UI);
        break;

    case Volume_UI:
        deal_radio_vol_msg(key);
        break;

    case FMInputFreq_UI:
        break;

    default:
        break;
    }
    return NULL;
}

/********************************************************************************
 * Description : ����prev��next������
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_next_prev_longkey(uint8 key)
{
    uint8 direct;
    uint8 result;

    switch (CurUI_Type)
    {
    case FMFreq_UI:
    case FMChannel_UI:
        if (key == AP_KEY_PREV)
        {
            direct = SEARCH_LAST;
        }
        else
        {
            direct = SEARCH_NEXT;
        }
        fmSendCommand(FM_MUTE, SetMUTE, NULL);
        result = SearchHandle(direct);
        if ((result != RESULT_NULL) && (result != RESULT_IGNORE) && (result != RESULT_RECORD))
        {
            return result;
        }
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

        if (g_FMStatus.station)
        {
            SetCurUI_Type(FMChannel_UI);
        }
        else
        {
            SetCurUI_Type(FMFreq_UI);
        }
        return result;
        break;

    case Volume_UI:
        deal_radio_vol_msg(key);
        break;

    case FMInputFreq_UI:
        break;

    default:
        break;
    }
    return NULL;
}
/********************************************************************************
 * Description : �������ְ���
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_digital_key(uint8 key)
{
    uint8 key_val;
    uint8 i;
    if ((DigitalCount >= 4) || (DigitalCount == 0))
    {
        SetCurUI_Type(FMInputFreq_UI);
        memset(DigitalBuf, 0x0, 4);
    }
    key_val = (key & ~AP_KEY_UP) - AP_KEY_NUM0;//�����ְ�����Ϣת��������
    for (i = 0; i < 3; i++)
    {
        DigitalBuf[i] = DigitalBuf[i + 1];
    }
    DigitalBuf[3] = key_val;
    DigitalCount++;
    need_draw = TRUE;
    return NULL;
}

/*
 ********************************************************************************
 *             void    volume_callback(uint16 Vol)
 *
 * Description : �������õ�CALLBACK����
 *
 * Arguments   :    ��ǰ�����õ�����ֵ
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
/*void volume_callback(uint16 Vol)
 {
 //ʵʱ�ı�������ֵ
 g_comval.vol_display = (uint8) Vol;
 if (g_comval.VolumeMax != 0)
 {
 g_comval.volume = g_comval.vol_display * SOFTVOL_MAX / g_comval.VolumeMax;
 }
 fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
 if (g_comval.volume == 0)
 {
 fmSendCommand(FM_MUTE, SetMUTE, NULL);
 }
 else
 {
 fmSendCommand(FM_MUTE, releaseMUTE, NULL);
 }
 }*/
#if 0
/*
 ********************************************************************************
 *             void DealFadeIn(uint8 Volume)
 *
 * Description : �������봦����
 *
 * Arguments   :    ��ǰ����
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void DealFadeIn(uint8 Volume)
{
    uint8 i, j;
    uint16 nop = 0;
    for (i = 0; i <= Volume; i++)
    {
        for (j = 0; j < 100; j++)
        {
            nop++;
        }
        SetPAVolume(i);
    }
    ClearWatchDog();
}
/*
 ********************************************************************************
 *             void delay1(void)
 *
 * Description : ��ʱ����
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void delay1(void)
{
    uint16 i;
    for (i = 0; i < 1000; i++)
    {
        ; //����ʱ������ʱ
    }

}
/*
 ********************************************************************************
 *             void DealFadeInOut(void)
 *
 * Description : �˳�FMʱ������������
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void DealFadeInOut(void)
{
    uint8 vol_cnt;
    vol_cnt = g_comval.volume;
    while (vol_cnt > 0)
    {
        fmSendCommand(FM_MID_SETVOLUME, (void *) vol_cnt, NULL);
        fmSendCommand(FM_MUTE, releaseMUTE, NULL);
        vol_cnt--;
        delay1();
    }

}
#endif
