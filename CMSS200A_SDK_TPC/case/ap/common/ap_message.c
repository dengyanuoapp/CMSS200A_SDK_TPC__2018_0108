/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib message file, realtime part
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_message.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */
//

#include "ap_common.h"

//#define linein_min_adcval   0x34
//#define linein_max_adcval   0x3f


#pragma name(COM_MSG)
uint8 deal_RTC_msg(void);
uint8 check_ararm_msg(void);
/*
 ********************************************************************************
 * Description : 处理系统消息, 返回按键消息, 同时负责开/关背光
 ap_get_message_core() 的入口
 *
 * Arguments   :
 *
 * Returns     : 返回ap能够识别的消息, 如果没有系统消息, 返回 AP_KEY_NULL
 *
 * Notes       :
 ********************************************************************************
 */

uint8 ap_get_message(void)
{
    uint8 key;
#ifdef Msg_CTC2HZ
    do
    {
        key = GetSysMsg();
        ClearWatchDog();
    }
    while(key == MSG_RTC2HZ);
    if(key == Msg_HalfSecond)
    {
        key = MSG_RTC2HZ;
    }
#else
    key = GetSysMsg();
#endif
    if (key == Msg_KeyNull)
    {
        return AP_KEY_NULL;
    }
    else
    {
        if (key == MSG_RTC2HZ)
        {
            return deal_RTC_msg();
        }
        key = ap_get_message_core(key);
        if (DealLongKey(key) == 0)
        {
            return AP_KEY_NULL;
        }
        else
        {
            return key;
        }
    }
}

/*
 ********************************************************************************
 * Description : 处理热键消息, 返回结果消息
 ap_handle_hotkey_core() 的入口
 *
 * Arguments   : key, 按键消息
 *
 * Returns     : 如果有可识别的热键消息,  返回结果消息
 如果没有可识别的热键消息,  返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 ap_handle_hotkey(uint8 key)
{
    if (key == AP_KEY_NULL)
    {
        return 0;
    }
    else
    {
        return ap_handle_hotkey_core(key);
    }
}

/********************************************************************************
 * Description :处理长按PLAY键关机，或长按Play键关机的消息
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 如果对相应的长按键要进行处理时，必须先将longkey_flag置为1.否则不进行特别处理。
 *			由于RTC中断会不断发出来，所以在检测长按消息时，要过滤掉RTC消息。
 ********************************************************************************/
uint8 DealLongKey(uint8 key)
{
    if (longkey_flag == 0)
    {
        return 1;
    }
    return check_longkey_msg(key);
}
uint8 deal_RTC_msg(void)
{
    uint8 retval;
    g_rtc_counter++;
    /*2HZ计时进入省电模式*/
    if (g_comval.SleepTime != 0) //自动关机
    {
        g_standby_time++;
        if (g_standby_time == g_comval.SleepTime * 120)
        {
            g_standby_time = 0;
            g_comval.SleepTime = 0;
            return AP_MSG_STANDBY;
        }
    }
    /*2HZ计时无操作返回*/
    if ((g_rtc_counter % 6) == 0)
    {
        return AP_MSG_WAIT_TIMEOUT;
    }
    //检测闹钟消息
    if (alarmtimerflag == 1)
    {
        alarmtimerflag = 0;
        if (g_comval.g_alarm.AlarmEnable != 0)
        {
            return AP_MSG_ALARM; //闹钟关闭
        }
    }
#if 0
    retval = check_hard_switch();
    if(retval != 0)
    {
        return retval;
    }
#endif
    retval = check_plug_device_msg(&usb_on_flag, USB_IN);
    if(retval != AP_MSG_RTC)
    {
        return retval;
    }
    return check_plug_device_msg(&linein_status, LINEIN);
}

/********************************************************************************
 * Description :假硬开关检测
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :TEST_CTL bit1为1是，表示硬开关关闭动作
 *
 ********************************************************************************/

uint8 check_hard_switch(void)
{
    uint8 sfr_bak, retval = NULL, tmp_val;
    uint8 fake_onoff;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    tmp_val = TEST_CTL & 0x02;
    fake_onoff = SYSTEM_ONOFF & 0x10;
    SFR_BANK = sfr_bak;
    if((tmp_val != 0) || (fake_onoff == 0x10))
    {
        if(tmp_val != 0)
        {
            power_on_flag = 1;
        }
        //        retval = deal_standby_msg();
    }
    return retval;
}


