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

#pragma name(FM_DEAL)

uint16 FMdeal(uint16 RunFMStatues)
{
    uint16 result = 0;
    uint8 key;
    uint8 count = 0;
    uint8 modekeystore = NULL;
    bool BandSwitch = TRUE, FirstWait = TRUE;
    RunFMStatues = RunFMStatues;
    //clear the message pool
    while (ap_get_message() != AP_KEY_NULL)
    {
        ; //�����Ϣ��
    }

    while (1)
    {
        if (BandSwitch == TRUE) //��ʼ�����ػ� BandSwitch
        {
            result = drawBandSwitch(); //���� ����ת������
            if (result != 0)
            {
                return result;
            }
            BandSwitch = FALSE;
            FirstWait = TRUE;
        }
        if (need_draw == TRUE) //��Ļ�ػ�
        {
            need_draw = FALSE;
            RefreshMainScreen();
        }
        if (FirstWait == TRUE)
        {
            FirstWait = FALSE;
            //ap_sleep(3);        //���ν���AP�ȴ�1.5S�ͷ�MUTE(����)
            fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL); //����������С
            fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL); //��ȡ״̬
            if (g_comval.volume != 0)
            {
                fmSendCommand(FM_MUTE, releaseMUTE, NULL); //fmSendCommand(MC_MUTE, releaseMUTE);
            }
            if (RunFMStatues == ALARM_TO_FM)
            {
                VMWrite(&g_radio_config, VM_AP_RADIO, sizeof(radio_config_t));
                return RESULT_FMREC_START;
            }
        }

        deal_pc_eqsrs(0);

        result = deal_key_msg();
        if (result)
        {
            return result;
        }
    }
    return key; //�������е�����
}

//���� ����ת������
uint16 drawBandSwitch(void)
{
    uint8 sfr_bak;

    if (g_radio_config.band_mode == Bank_US_Europe) //FM ����ģʽ �����Ϊ��ͨƵ��
    {
        radio_param.param1 = g_radio_config.fmstation_us; //US Tab
    }
    else
    {
        radio_param.param1 = g_radio_config.fmstation_jp; //JP Tab
    }

    (fm_status_t *) radio_param.param2 = &g_radio_config.FMStatus;//��ǰ���ŵĵ�̨��Ϣ

    fmSendCommand(FM_MID_RESTORE, &radio_param, NULL); //�ָ�

    fmSendCommand(FM_MID_SETVOLUME, 0, NULL); //��������Ϊ0

    if (!fmSendCommand(FM_OPEN, SEARCHLEVEL, NULL)) //�����ʧ�ܣ�Ӳ��������ʾ
    {
        LEDClearScreen();
        /* ERR */
        LEDDisplay(NUMBER1, NUM_E, 1);
        LEDDisplay(NUMBER2, NUM_R, 1);
        LEDDisplay(NUMBER3, NUM_R, 1);
        ap_sleep(3);
        fmSendCommand(FM_MID_SETVOLUME,  (void *) g_comval.volume, NULL);
        return RESULT_MAIN; //�������˵�
    }

    fmSendCommand(FM_MUTE, SetMUTE, NULL); //���þ���

    fmSendCommand(FM_MID_PLAY, NULL, NULL); //���ŵ�̨

    init_fmin_channel();

    sfr_bak = SFR_BANK;
    SFR_BANK = 0x13;
    I2S_CTL1 = 0;
    SFR_BANK = sfr_bak;

    return 0;
}
