/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: show_pause.c,v 1.2 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */
#include "ap_record.h"

//#pragma renamecode(AP_PAUSE)
#pragma name(AP_PAUSE)

/*
 ********************************************************************************
 * Description : ��ͣ����
 *
 * Arguments   :
 *
 * Returns     : �����Ϣ��0
 RESULT_USER_STOP    //�û���������
 RESULT_LOW_POWER_STOP    //�͵�ѹ����
 *
 * Notes       :
 *
 ********************************************************************************
 */
int16 do_pause(void)
{
#if 0
    //uint16 result;

    //uint8 isUsbVoltage;
    uint8 key;

    BOOL need_draw_ui = TRUE;

    BOOL flash_state = TRUE; //��˸��״̬
    BYTE low_power_count = 0; //��õ͵�ѹ�Ĵ���
    //BYTE batt;

    while (1)
    {
	    #if 0
        if (FALSE != need_draw_ui)
        {
            //��ͣUSOUND¼��״̬
            LEDDisplay(LED_PLAY, 0xff, 0);
            LEDDisplay(LED_PAUSE, 0xff, 1);
            need_draw_ui = FALSE;
        }
	   #endif
        //handle msg
        key = ap_get_message();
        switch (key)
        {

        case AP_KEY_PLAY | AP_KEY_UP:
            //����usound¼��
            //LEDDisplay(LED_PAUSE, 0xff, 0);
            //LEDDisplay(LED_PLAY, 0xff, 1);
            mp3rSendCommand(MC_RECORD, 0);
            return 0;

        case AP_MSG_SD_OUT:
            if (g_record_vars.location.disk == 'H')
            {
                mp3rSendCommand(MC_STOP, 0);
                return RESULT_CARD_OUT;
            }
            break;

        case AP_KEY_MODE | AP_KEY_UP:
            /* ����¼���ļ� */
            mp3rSendCommand(MC_STOP, 0);
            return RESULT_SAVE_FILE;
            break;

        case AP_MSG_RTC:
            //show batt
            //batt = GetBattery();
            //ui_show_batt(batt);
            break;

        default:
            break;
        }//switch()
    }//while(1)
#endif
    return 0; //���������warning
}

