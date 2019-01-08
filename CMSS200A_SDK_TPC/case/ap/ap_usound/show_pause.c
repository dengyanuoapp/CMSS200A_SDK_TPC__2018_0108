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
 * Description : 暂停界面
 *
 * Arguments   :
 *
 * Returns     : 结果消息或0
 RESULT_USER_STOP    //用户按键结束
 RESULT_LOW_POWER_STOP    //低电压结束
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

    BOOL flash_state = TRUE; //闪烁的状态
    BYTE low_power_count = 0; //获得低电压的次数
    //BYTE batt;

    while (1)
    {
	    #if 0
        if (FALSE != need_draw_ui)
        {
            //暂停USOUND录音状态
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
            //继续usound录音
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
            /* 保存录音文件 */
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
    return 0; //避免编译器warning
}

