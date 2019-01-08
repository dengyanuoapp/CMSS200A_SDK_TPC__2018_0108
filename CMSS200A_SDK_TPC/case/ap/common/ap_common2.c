/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib file, part 2
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_common2.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */

#include "ap_common.h"
#pragma name(COM_COM2)
//�����ȼ���Ӧ���޸ĸñ�ʱ��ͬ���޸�ap_usound�е�
//usound_main.c �����ȼ���Ӧ��
const uint8 dac_vol[MAX_VOl_NUM] =
{
    0x00, 0x23, 0x36, 0x40,
    0x47, 0x50, 0x54, 0x57,
    0x5a, 0x60, 0x64, 0x67,
    0x6a, 0x70, 0x72, 0x74,
    0x76, 0x77, 0x79, 0x7a,
    0x7b, 0x80, 0x81, 0x82,
    0x83, 0x84, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b,
    0x90, 0x91, 0x92, 0x93,
    0x94, 0x95, 0x96, 0x97,
    0x98
};
void show_volume_ui(void)
{
    memset(temp_buffer, 0xff, 0x04);
    itoa2(g_comval.volume, temp_buffer);
    LEDPuts(NUMBER2, temp_buffer, 1);
}

/*
 ********************************************************************************
 * Description : ��ʼ����Ϣ����
 *
 * Arguments   : comval, ϵͳ�趨ֵ, =null ��ʾֻ��ʼ���ڲ�������
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void set_autosleep_time(uint8 key)
{
    uint8 str_buf[4];
    uint8 tmp_val;
    memset(str_buf, 0xff, 0x04);
    if (key == Key_None)
    {
        if (CurUI_Type != STANDBY_UI)
        {
            SetCurUI_Type(STANDBY_UI);
            LEDClearScreen();
            if (g_comval.SleepTime < 100)
            {
                itoa2(g_comval.SleepTime, str_buf);
            }
            else
            {
                itoa3(g_comval.SleepTime, str_buf);
            }
            LEDPuts(NUMBER2, str_buf, 1);
            need_draw = FALSE;
            return;
        }
        else
        {
            key = Key_Next;
        }
    }
    tmp_val = g_comval.SleepTime / 10;
    if (key == Key_Next)
    {
        if (tmp_val < 12)
        {
            tmp_val++;
        }
        else
        {
            tmp_val = 0;
        }
    }
    else
    {
        if (tmp_val > 0)
        {
            tmp_val--;
        }
        else
        {
            tmp_val = 12;
        }
    }
    g_comval.SleepTime = tmp_val * 10;
    if (g_comval.SleepTime < 100)
    {
        itoa2(g_comval.SleepTime, str_buf);
    }
    else
    {
        itoa3(g_comval.SleepTime, str_buf);
    }
    LEDPuts(NUMBER2, str_buf, 1);
    need_draw = FALSE;
    g_standby_time = 0;
}
/*
 ********************************************************************************
 * Description : ˯��ָ����ʱ��
 *
 * Arguments   : half_sec, ʱ��, 0.5��Ϊ��λ
 *
 * Returns     : ���ָ����ʱ����û��������Ϣ����, ����0
 ���ָ����ʱ������������Ϣ����, ���ظ���Ϣ
 * Notes       : ���ָ����ʱ������������Ϣ����, ˯�ߵ�ʱ�佫< ָ����ʱ��
 *
 ********************************************************************************
 */

uint8 ap_sleep(uint16 half_sec)
{
    uint8 n = 0; //rtc counter
    uint8 key, result;

    while (1)
    {
        key = ap_get_message();
        //sleep ʱ����timeout
        if ((key == AP_MSG_RTC) || (key == AP_MSG_CHARGING) || (key == AP_MSG_WAIT_TIMEOUT))
        {
            n++;
            if (n >= (uint8) half_sec)
            {
                result = 0;
                break;
            }
        }//lock ֻ��up, û��long/hold
        else if (key == AP_KEY_NULL)
        {
            continue;
        }
        else
        {
            result = ap_handle_hotkey_core(key);
            break;
        }
    }
    return result;
}
uint8 read_ap_comval(void)
{
    BOOL result;
    result = VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (g_comval.magic != MAGIC_COMVAL)
    {
        memset(&g_comval, 0x0, sizeof(g_comval));
        g_comval.magic = MAGIC_COMVAL;
        g_comval.ap_result = RESULT_MUSIC;

        g_comval.VolumeMax = 40;
        g_comval.VolumeDef = 35;
        if (g_comval.VolumeDef > g_comval.VolumeMax)
        {
            g_comval.VolumeDef = g_comval.VolumeMax;
        }
        g_comval.vol_display = 32;
        g_comval.volume = g_comval.vol_display; // (g_comval.vol_display * SOFTVOL_MAX) / g_comval.VolumeMax;
        g_comval.RecordGain = 6;
        g_comval.RecordTime_max = 10;
        g_comval.eq_type = none;
        result = VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    }
    return result;
}
void set_poweroff_HCL(void)
{
#if 0
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    RTC_CTL0 &= 0xe0;
    RTCRUPD = 0xa5;
    TM_DelayMS(1);
    while(RTCRUPD != 0x5a);
    SFR_BANK = sfr_bak;
#endif
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
uint8 deal_vol_msg(uint8 key, uint8 UI_Type)
{
    if (key == Key_None)
    {
        if (CurUI_Type != Volume_UI)
        {
            LEDClearScreen();
            SetCurUI_Type(Volume_UI);
            show_volume_ui();
            need_draw = FALSE;
            return NULL;
        }
        else
        {
            SetCurUI_Type(UI_Type);
        }
    }
    else
    {
        if (CurUI_Type != Volume_UI)
        {
            LEDClearScreen();
            SetCurUI_Type(Volume_UI);
        }
        if (key == Key_Next)
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
        set_dac_volume(g_comval.volume);
        show_volume_ui();
        need_draw = FALSE;
    }
    return NULL;
}
uint8 get_dac_real_vol(uint8 cur_vol)
{
    return dac_vol[cur_vol];
}
void dac_mute_onoff(uint8 on_off)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_AUIP;
    if(on_off != 0)
    {
        DAC_ANALOG0 |= 0x01; //dismute,connect
    }
    else
    {
        DAC_ANALOG0 &= 0xfe; //mute dac
    }
    SFR_BANK = sfr_bak;
}
/********************************************************************************
 * Description : ����pa����
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
void set_dac_volume(uint8 cur_vol)
{
    uint8 real_val;
    real_val = dac_vol[cur_vol];
    SetPAVolume_LR(real_val, real_val);
    if((mute_flag == 1) && (real_val != 0))
    {
        return;
    }
    dac_mute_onoff(real_val);
}

