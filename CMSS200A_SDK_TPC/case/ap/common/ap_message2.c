/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib message file
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_message2.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */

#include "ap_common.h"

#define START_KEY_NUM AP_KEY_NUM0
#define END_KEY_NUM     AP_KEY_NUM9
#pragma name(COM_MSG2)
const uint8 lowpower_str[] =
{ "LBAT" };
const uint8 tmp_key_val[] =
{
    AP_KEY_USBTEST | AP_KEY_LONG, AP_KEY_AUTOSLEEP | AP_KEY_UP, AP_KEY_USBTEST | AP_KEY_UP, AP_KEY_VOL | AP_KEY_LONG,
    AP_KEY_LOOP | AP_KEY_UP, AP_KEY_CLOCK | AP_KEY_LONG, AP_KEY_CLOCK | AP_KEY_UP,
    AP_KEY_EQ | AP_KEY_UP, AP_KEY_REC | AP_KEY_LONG, AP_KEY_REC | AP_KEY_UP
};

uint8 ap_handle_hotkey_core(uint8 key);
uint8 FW_Card_Updata(void);
void show_lowpower_msg(void)
{
    LEDClearScreen();
    if (show_what != 0)
    {
        LEDPuts(NUMBER1, lowpower_str, 1);
    }
}

//�򿪰�����
void Open_KeyTone(uint8 keytone)
{
#if 0
    if (keytone != 0)
    {
        KY_Beep(keytone);
    }
#else
    keytone = keytone;
#endif
}
//�Գ������ּ����и��ô���
uint8 switch_key_func(uint8 key)
{
    if ((key >= START_KEY_NUM) && (key <= END_KEY_NUM ))
    {
        return tmp_key_val[key - START_KEY_NUM];
    }
    return NULL;
}

/*
 ********************************************************************************
 * Description : ����ϵͳ��Ϣ, ���ذ�����Ϣ, ͬʱ����/�ر���
 *
 * Arguments   : key, ϵͳ��Ϣ
 *
 * Returns     : ����ap�ܹ�ʶ�����Ϣ, ���û��ϵͳ��Ϣ, ���� AP_KEY_NULL
 *
 * Notes       :

 * ����ʱ��:
 *  0.....1.2....1.5........... (s)
 *  key    long   hold    up    (>1.2s)
 *  key  up            (<1.2s)
 *
 * key rate: 0.3s
 *
 ********************************************************************************
 */
uint8 ap_get_message_core(uint8 key)
{
    uint8 tmp_key;
    /*������Ϣ���ϴε�һ��ʱ�Ĵ���*/
    if (key == key_value)
    {
        /*����û��ʱ����̧��������⣬���԰���*/
        g_rtc_counter = 0;
        g_standby_time = 0;
        key_count++;

        /*��ס����ʱ��320ms����һ����ͬ������Ϣ*/
        if (key_count >= 4)
        {
            key_value = key;
            //�������ּ��������ù���
            tmp_key = switch_key_func(key);
            if(tmp_key == NULL)
            {
                key = (uint8) (key | AP_KEY_LONG); //=1.2s,����������Ϣ
            }
            else
            {
                key = tmp_key;
                //���⸴�ó�������δ���
                if(holdkey_flag == TRUE)
                {
                    longkey_flag = 1;
                }
            }
            holdkey_flag = TRUE;
        }
        return key;
    }

    switch (key)
    {
    case Msg_KeyShortUp:
        key_count = 0;
        g_rtc_counter = 0;
        g_standby_time = 0;
        //�����ǰ�İ���Ϊplay�������⵽onoff��û���ɿ�����������up��Ϣ
        if((key_value == Msg_KeyPlayPause) && (get_onoff_key_status() != 0))
        {
            break;
        }
        Open_KeyTone(g_comval.SuppKeyTone);
        if (holdkey_flag == TRUE)
        {
            key = AP_KEY_LONGUP;
            holdkey_flag = FALSE;
        }
        else
        {
            /*key_value������ı�*/
            key = key_value | AP_KEY_UP;
        }
        key_value = key;

        break;
    case MSG_LOW_POWER:
        /*ϵͳ�͵���Ϣ����Ϊǿ�ƹػ�*/
        lowpower_count++;
        if (lowpower_count > 6)
        {
            key = AP_MSG_STANDBY;
        }
        else
        {
            show_what = !show_what;
            show_lowpower_msg();
            key = NULL;
        }
        break;
    case AP_MSG_UH_OUT:
        close_usb_phyctrol();
        break;
    case MSG_USB_UNSTICK:
        key = AP_KEY_NULL;
        break;
    default: /*�°�������*/
        g_rtc_counter = 0;
        key_value = key;
        key_count = 1;

    } /*end of switch*/

    return key;
}

/*
 ********************************************************************************
 * Description : �����ȼ���Ϣ, ���ؽ����Ϣ
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
uint8 ap_handle_hotkey_core(uint8 key)
{
    switch (key)
    {
    case AP_KEY_PLAY | AP_KEY_LONG: //����play��standby
    case AP_KEY_FUNCTION | AP_KEY_LONG:
        longkey_flag = 1;
    case AP_MSG_STANDBY: //�Զ��ػ�
        return RESULT_STANDBY;
    case AP_KEY_USBTEST | AP_KEY_UP:
        //���û������USB�ߣ��򲻽���utest����
        if(usb_on_flag == 0)
        {
            return NULL;
        }
        return RESULT_USBTEST;
    case AP_KEY_USBTEST | AP_KEY_LONG:
        longkey_flag = 1;
        return RESULT_PITCH;

        break;
    case AP_KEY_REC | AP_KEY_UP: //����¼��
        //����������ж���Ҫ��Ϊ�˴������ֳ��������ó�¼����ʱ��
        //��ֹ�˳���ǰap���ٴδ���ð���
        if(holdkey_flag == TRUE)
        {
            longkey_flag = 1;
        }
        return RESULT_RECORD;
    case AP_MSG_SD_IN:
        return RESULT_MUSIC_CPLAY;
    case AP_MSG_UH_IN:
        return RESULT_MUSIC_UPLAY;
    case AP_MSG_ALARM:
        return RESULT_ALARM;
    case AP_MSG_LINEIN_IN:
        return RESULT_LINEIN;
    case AP_KEY_REC | AP_KEY_LONG:
        longkey_flag = 1;
        FW_Card_Updata();
        SetPLL(PLL_48MHZ);
        return 0;
    case AP_MSG_USB_PLUGIN:
        return RESULT_UDISK;
        break;
    default:
        return 0;
    }
}
/********************************************************************************
 * Description : ��ȡonoff����״̬
 *
 * Arguments   :
 *
 * Returns     : ��
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 get_onoff_key_status(void)
{
    uint8 sfr_bak, onoff_staus;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    onoff_staus = SYSTEM_ONOFF & 0x08;  //��ȡonoff����״̬
    SFR_BANK = sfr_bak;
    return onoff_staus;
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

uint8 check_longkey_msg(uint8 key)
{
    if ((key == AP_MSG_SD_IN) || (key == AP_MSG_UH_IN) || (key == AP_MSG_SD_OUT) || (key == AP_MSG_UH_OUT) || (key
            == AP_MSG_WAIT_TIMEOUT))
    {
        return 1;
    }
    else if ((key & AP_KEY_UP) != 0)
    {
        //�����⵽onoff�����ǰ�ס�ģ���������󷢳���up��Ϣ����������
        if(get_onoff_key_status() != 0)
        {
            return 1;
        }
        longkey_flag = 0;
        return 0;
    }
    else if (key == AP_MSG_RTC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
 ********************************************************************************
 * Description : int FW_Updata(void) ͨ������U�̽���С���̼���������ǰ��֧�ֿ�����
 *
 * Arguments   : ��ǰֻ֧�ֿ�����
 *
 * Returns     : 1 = updata ok,0 = updata fail
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FW_Card_Updata(void)
{
    UD_RW sdcard_rw;
    uint16 *data_p;
    uint16 checksum, i;

    if (DRV_DetectUD(1) != 0x20)
    {
        return 0;
    }
    else
    {
        SetPLL(PLL_24MHZ);
        if (DRV_ActiveUD(1, MODE_SLOW) != 0x00)
        {
            sdcard_rw.lba = 0x01;
            sdcard_rw.sramaddr = 0x2800;
            sdcard_rw.reserv = 0x01;
            sdcard_rw.srampage = 0x00;
            UD_SectorRead(&sdcard_rw);
            data_p = 0x2800;
            if (data_p[254] != 0xaa55)
            {
                return 0;
            }
            checksum = 0x00;
            for (i = 0x00; i <= 254; i++)
            {
                checksum += data_p[i];
            }
            checksum += 0x1234;
            if (checksum != data_p[255])
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
        LEDClearScreen();
        LEDPuts(NUMBER1, "UPDA", 1);
        switch_pa_power(0);
        SpiNor_FW_Upgrd();
        switch_pa_power(1);
        return 1;
    }
}
#if 0
uint8 deal_standby_msg(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    TEST_CTL |= 0x02;
    SFR_BANK = sfr_bak;
    //    set_poweroff_HCL();
    return AP_MSG_STANDBY;
}
#endif


