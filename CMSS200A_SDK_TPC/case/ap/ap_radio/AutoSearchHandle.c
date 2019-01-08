/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *        History Record:
 1.ȥ��ˢ�±�����ʾ��������̨ʱ�Ľ�����˸��, by Mars,2008-04-15
 *******************************************************************************
 */

#include "ap_radio.h"

#pragma name(AP_AUTOSEARCH)

void SequenceFreqlist(uint16 *buf, uint16 curfreq, uint8 totalnum);
void SaveStation(void);

/*
 ********************************************************************************
 *             void ShowStationCount(uint16 count)
 *
 * Description : ��ʾ��̨��.
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ShowStationCount(uint8 count)
{
    char buf[3];
    uint8 i;

    LEDClearScreen();

    LEDDisplay(NUMBER1, NUM_C, 1);
    LEDDisplay(NUMBER2, NUM_H, 1);
    itoa2(count, buf);
    for (i = 0; i < 2; i++)
    {
        buf[i] -= '0';
    }
    LEDDisplay(NUMBER3, buf[0], 1);
    LEDDisplay(NUMBER4, buf[1], 1);
}

/*
 ********************************************************************************
 *             void ShowFreqInFM2()
 *
 * Description : ��ʾȫ�Զ���̨����---��ʾƵ��
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ShowFreqInFM2(void)
{
    char buf[5];
    uint8 i;

    LEDClearScreen();

    LEDDisplay(LED_FM, 0xff, 1);
    itoa4(g_FMStatus.freq, buf);
    for (i = 0; i < 4; i++)
    {
        buf[i] -= '0';
    }

    if (g_FMStatus.freq > 1000)
    {
        LEDDisplay(NUMBER1, buf[0], 1);
    }
    LEDDisplay(NUMBER2, buf[1], 1);
    LEDDisplay(NUMBER3, buf[2], 1);
    LEDDisplay(NUMBER4, buf[3], 1);
}

/*
 ********************************************************************************
 *             bool Search_Show2(FM_direct_e Direct)
 *
 * Description : ������һ���ڵ�̨����û�е��µ�̨.
 *
 * Arguments   : Direct,Next��Last����.
 *
 * Returns     : TRUE:�ѵ���һ���µ�̨,FALSE:û���ѵ�.
 *
 * Notes       :
 *
 ********************************************************************************
 */
bool Search_Show2(FM_direct_e Direct)
{
    bool Search_Flag;
    int i;
    g_FMStatus.stereo_status = Stereo;
    g_FMStatus.station = 0;
    fmSendCommand(FM_MUTE, SetMUTE, NULL);
    fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);

    Search_Flag = fmSendCommand(FM_SEARCH, (void *) &g_FMStatus, (void *) Direct);
    if (Search_Flag) //��̨�ɹ���MUTE�ͷ�
    {
        AutoTabNum++;
        ShowStationCount(AutoTabNum);
        //fmSendCommand(FM_MUTE, releaseMUTE,NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
        if (fmSendCommand(FM_MID_SAVESTATION, NULL, NULL) != FALSE)
        {
            fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
        }
        //        for (i = 0; i < 0x7000; i++)
        //        {
        //        }
    }
    else
    {
        ShowFreqInFM2();
    }
    for (i = 0; i < 0x3999; i++)
    {
    }
    if (flag_r == 0)
    {
        return 0;
    }
    else
    {
        return Search_Flag;
    }
}

/*
 ********************************************************************************
 *             bool RestartSearch(uint8 level)
 *
 * Description : ��������ֵ,���¿�ʼ����.
 *
 * Arguments   : ����ֵ
 *
 * Returns     : TRUE:��������,FALSE:�ж�����,��������.
 *
 * Notes       :
 *
 ********************************************************************************
 */
#if 0
bool RestartSearch(uint8 level)
{
    if (level <= 2)
    {
        return FALSE;
    }
    fmSendCommand(FM_OPEN, level, NULL); //����������������ֵ����ʼ��������

#ifndef HARDWARE_SEEK
    fmSendCommand(FM_MID_RESTORE, & radio_param, NULL);
#endif
    return TRUE;
}
#endif

/*
 ********************************************************************************
 *             uint16 AutoSearchHandle(uint8 Direct)
 *
 * Description : ȫ�Զ�����
 *
 * Arguments   : Direct,Next��Last����.
 *
 * Returns     : ����п�ʶ����ȼ���Ϣ,  ���ؽ����Ϣ
 ���û�п�ʶ����ȼ���Ϣ,  ����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint16 AutoSearchHandle(uint8 Direct)
{
    int key = 0;
    bool need_draw = TRUE, need_restart = TRUE;
    BYTE Searchflag = 1;
    uint16 backup_freq, end_freq;
    uint8 result = 0;
    bool loop = TRUE;

    flag_r = 0;
    AutoTabNum = 0;
    memset(m_FMStationTab, 0, sizeof(uint16) * MAX_STATION_COUNT); //��ʼ��AutoSearch��������

    g_FMStatus.station = 0;
    g_FMStatus.station_count = 0;
    backup_freq = g_FMStatus.freq; //���ݵ�ǰƵ��
    // USƵ��
    if (g_radio_config.band_mode == Bank_US_Europe)
    {
        if (Direct == SEARCH_NEXT)
        {
            g_FMStatus.freq = USFREQ_MIN - 5; //870
            end_freq = USFREQ_MAX;
        }
        else
        {
            g_FMStatus.freq = USFREQ_MAX;
            end_freq = USFREQ_MIN;
        }
    }
    // JPƵ��
    else
    {
        if (Direct == SEARCH_NEXT)
        {
            g_FMStatus.freq = JPFREQ_MIN;
            end_freq = JPFREQ_MAX;
        }
        else
        {
            g_FMStatus.freq = JPFREQ_MAX;
            end_freq = JPFREQ_MIN;
        }
    }

    fmSendCommand(FM_MID_SAVE, &radio_param, NULL); //���g_radio_config�е�����
    charge_pump_onoff(0);
    while (loop)//while(1)
    {
        if (Direct != SEARCH_NULL)
        {
            if (Direct == SEARCH_NEXT) //NEXT��������
            {
                g_FMStatus.freq += 1;
                if ((g_FMStatus.freq >= USFREQ_MAX) && (g_radio_config.band_mode == Bank_US_Europe))
                {
                    loop = FALSE;
                }
                else if ((g_FMStatus.freq >= JPFREQ_MAX) && (g_radio_config.band_mode == Bank_Japan))
                {
                    loop = FALSE;
                }
                if (Search_Show2(UP))
                {
                    // return 0;
                    break;
                }
            }
            else if (Direct == SEARCH_LAST) //LAST��������
            {
                g_FMStatus.freq -= 1;
                if ((g_FMStatus.freq <= USFREQ_MIN) && (g_radio_config.band_mode == Bank_US_Europe))
                {
                    loop = FALSE;
                }

                else if ((g_FMStatus.freq <= JPFREQ_MIN) && (g_radio_config.band_mode == Bank_Japan))
                {
                    loop = FALSE;
                }
                if (Search_Show2(DOWN))
                {
                    //return 0;
                    break;
                }
            }
            else if (Direct == SEARCH_STOP)
            {
                Direct = SEARCH_NULL;
                break;
                //  return 0;
            }
        }
        if (AutoTabNum >= MAX_STATION_COUNT) //���ѵ�MAX_STATION_COUNT����̨,�����˳�.
        {
            loop = FALSE;
        }
        if (loop)
        {
            key = ap_get_message();
            switch (key)
            {
            case AP_KEY_NEXT | AP_KEY_UP: //NEXT����ʱ��NEXT����ֹͣ����
                break;

            case AP_KEY_PREV | AP_KEY_UP: //LAST����ʱ��LAST����ֹͣ����
                break;

            case AP_KEY_PLAY | AP_KEY_UP:
            case AP_KEY_MODE | AP_KEY_UP:
                Direct = SEARCH_STOP;
                break;

            case AP_MSG_LINEIN_IN:
                result = RESULT_LINEIN;
                break;

            case AP_MSG_ALARM:
                result = RESULT_ALARM;
                break;

            case AP_MSG_WAIT_TIMEOUT:
            case AP_MSG_UH_IN:
            case AP_MSG_SD_IN:
            case AP_MSG_USB_PLUGIN:
                break;

            default: //�ȼ�����
                result = ap_handle_hotkey(key);
                break;
            }
            if (result)
            {
                break;
            }
        }
    }
    charge_pump_onoff(1);
    if (g_FMStatus.station_count > 0x00) // �ж��Ƿ���������̨
    {
        /* ��ת����һ����̨ */
        g_FMStatus.freq = m_FMStationTab[0];
        fmSendCommand(FM_SETFREQ, (void *) g_FMStatus.freq, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);

        fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    }
    else
    {
        g_FMStatus.freq = backup_freq;
        fmSendCommand(FM_SETFREQ, (void *) g_FMStatus.freq, NULL);
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
        fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    }
    while (ap_get_message() != AP_KEY_NULL) //����Զ���̨�����е���Ϣ
    {
        ClearWatchDog(); //�����Ϣ��
    }
    return result;
}

/*
 ********************************************************************************
 *             void SaveStation(void)
 *
 * Description : �����������ĵ�̨
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
#if 0
void SaveStation(void)
{
    uint8 i;
    uint16 k;
    fmSendCommand(MC_GETFREQ, (uint16) & k); // �����̨
    //FMAutoTab[AutoTabNum] = k;
    AutoTabNum++;
    SequenceFreqlist(FMAutoTab, k, AutoTabNum);
    for (i = AutoTabNum; i < 30; i++)
    {
        if (radio_param.param1[i] == k) // ɾ���ظ��ĵ�̨.

        {
            radio_param.param1[i] = 0;
        }
    }

}
#endif

