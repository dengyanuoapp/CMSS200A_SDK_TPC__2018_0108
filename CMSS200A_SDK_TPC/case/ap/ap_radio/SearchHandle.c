/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *       History Record:
 *         1.����Զ���̨ʱ������˸�����⣬by Mars��2008-04-15
 *******************************************************************************
 */

#include "ap_radio.h"

#pragma name(AP_SEARCH)

extern fm_status_t g_FMStatus;

void ShowFreqInFM(void); //FM��������ʾ
bool Search_Show(FM_direct_e Direct); //�Զ���̨ʱ����ʾ

/*
 ********************************************************************************
 *             uint16 SearchHandle(uint8 Direct)
 *
 * Description : �ֶ���̨
 *
 * Arguments   :    Direct,Next��Last����.
 *
 * Returns     : 	����п�ʶ����ȼ���Ϣ,  ���ؽ����Ϣ
 ���û�п�ʶ����ȼ���Ϣ,  ����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint16 SearchHandle(BYTE Direct)
{
    uint16 result = 0;
    uint16 key;
    uint16 start_freq;//��ʼƵ��
    flag_r = 1;
    //��ʼƵ��
    start_freq = g_FMStatus.freq;
    charge_pump_onoff(0);
    while (1)
    {
        if (Direct != SEARCH_NULL)
        {
            if (Direct == SEARCH_NEXT) //NEXT��������
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
                if (Search_Show(UP))
                {
                    //return 0;
                    break;
                }
                if(start_freq == g_FMStatus.freq)
                {
                    //return 0;
                    break;
                }
            }
            else if (Direct == SEARCH_LAST) //LAST��������
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
                if (Search_Show(DOWN))
                {
                    // return 0;
                    break;
                }
                if(start_freq == g_FMStatus.freq)
                {
                    // return 0;
                    break;
                }
            }
            else if (Direct == SEARCH_STOP)
            {
                Direct = SEARCH_NULL;
                break;
                // return 0;
            }
        }
        key = ap_get_message();
        switch (key)
        {
        case AP_MSG_STANDBY:
            //do nothing
            break;

        case AP_KEY_NEXT | AP_KEY_UP: //NEXT����ʱ��NEXT����ֹͣ����
            Direct = SEARCH_STOP;
            break;

        case AP_KEY_PREV | AP_KEY_UP: //LAST����ʱ��LAST����ֹͣ����
            Direct = SEARCH_STOP;
            break;

        case AP_KEY_PLAY | AP_KEY_UP:
        case AP_KEY_MODE | AP_KEY_UP:
            Direct = SEARCH_STOP;
            break;

        case AP_MSG_LINEIN_IN:
            result = RESULT_LINEIN;
            break;

        case AP_MSG_WAIT_TIMEOUT:
            break;
        case AP_MSG_UH_IN:
        case AP_MSG_SD_IN:
        case AP_MSG_USB_PLUGIN:
            result = 0;
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
    charge_pump_onoff(1);
    while (ap_get_message() != AP_KEY_NULL) //����Զ���̨�����е���Ϣ
    {
        ClearWatchDog(); //�����Ϣ��
    }
    return result;
}

/*
 ********************************************************************************
 *             bool Search_Show(FM_direct_e Direct)
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
bool Search_Show(FM_direct_e Direct)
{
    bool Search_Flag;
    int i;

    g_FMStatus.station = 0;
    fmSendCommand(FM_MUTE, SetMUTE, NULL);
    fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);

    Search_Flag = fmSendCommand(FM_SEARCH, (void *) &g_FMStatus, (void *) Direct);
    if (Search_Flag) //��̨�ɹ��󣬱����̨
    {
        fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
        if (fmSendCommand(FM_MID_SAVESTATION, NULL, NULL) != FALSE)
        {
            fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
        }
    }
    ShowFreqInFM();
    for (i = 0; i < 0x3999; i++)
    {
    }
    if (flag_r == 0)
    {
        return 0;
    }
    else
        return Search_Flag;
}

/*
 ********************************************************************************
 *             void ShowFreqInFM()
 *
 * Description : ��ʾ��̨��������
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void ShowFreqInFM(void)
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
