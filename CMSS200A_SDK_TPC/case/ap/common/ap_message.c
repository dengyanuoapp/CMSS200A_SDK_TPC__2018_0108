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
 * Description : ����ϵͳ��Ϣ, ���ذ�����Ϣ, ͬʱ����/�ر���
 ap_get_message_core() �����
 *
 * Arguments   :
 *
 * Returns     : ����ap�ܹ�ʶ�����Ϣ, ���û��ϵͳ��Ϣ, ���� AP_KEY_NULL
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
 * Description : �����ȼ���Ϣ, ���ؽ����Ϣ
 ap_handle_hotkey_core() �����
 *
 * Arguments   : key, ������Ϣ
 *
 * Returns     : ����п�ʶ����ȼ���Ϣ,  ���ؽ����Ϣ
 ���û�п�ʶ����ȼ���Ϣ,  ����0
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
 * Description :������PLAY���ػ����򳤰�Play���ػ�����Ϣ
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 �������Ӧ�ĳ�����Ҫ���д���ʱ�������Ƚ�longkey_flag��Ϊ1.���򲻽����ر���
 *			����RTC�жϻ᲻�Ϸ������������ڼ�ⳤ����Ϣʱ��Ҫ���˵�RTC��Ϣ��
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
    /*2HZ��ʱ����ʡ��ģʽ*/
    if (g_comval.SleepTime != 0) //�Զ��ػ�
    {
        g_standby_time++;
        if (g_standby_time == g_comval.SleepTime * 120)
        {
            g_standby_time = 0;
            g_comval.SleepTime = 0;
            return AP_MSG_STANDBY;
        }
    }
    /*2HZ��ʱ�޲�������*/
    if ((g_rtc_counter % 6) == 0)
    {
        return AP_MSG_WAIT_TIMEOUT;
    }
    //���������Ϣ
    if (alarmtimerflag == 1)
    {
        alarmtimerflag = 0;
        if (g_comval.g_alarm.AlarmEnable != 0)
        {
            return AP_MSG_ALARM; //���ӹر�
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
 * Description :��Ӳ���ؼ��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :TEST_CTL bit1Ϊ1�ǣ���ʾӲ���عرն���
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


