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

//打开按键音
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
//对长按数字键进行复用处理
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
 * Description : 处理系统消息, 返回按键消息, 同时负责开/关背光
 *
 * Arguments   : key, 系统消息
 *
 * Returns     : 返回ap能够识别的消息, 如果没有系统消息, 返回 AP_KEY_NULL
 *
 * Notes       :

 * 按键时序:
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
    /*按键消息和上次的一样时的处理*/
    if (key == key_value)
    {
        /*背光没开时，等抬起键开背光，忽略按键*/
        g_rtc_counter = 0;
        g_standby_time = 0;
        key_count++;

        /*按住按键时，320ms发送一次相同按键消息*/
        if (key_count >= 4)
        {
            key_value = key;
            //处理数字键长按复用功能
            tmp_key = switch_key_func(key);
            if(tmp_key == NULL)
            {
                key = (uint8) (key | AP_KEY_LONG); //=1.2s,发长按键消息
            }
            else
            {
                key = tmp_key;
                //避免复用长按键多次处理
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
        //如果当前的按键为play键，则检测到onoff并没有松开，则丢弃发出up消息
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
            /*key_value的置需改变*/
            key = key_value | AP_KEY_UP;
        }
        key_value = key;

        break;
    case MSG_LOW_POWER:
        /*系统低电消息处理为强制关机*/
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
    default: /*新按键处理*/
        g_rtc_counter = 0;
        key_value = key;
        key_count = 1;

    } /*end of switch*/

    return key;
}

/*
 ********************************************************************************
 * Description : 处理热键消息, 返回结果消息
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
uint8 ap_handle_hotkey_core(uint8 key)
{
    switch (key)
    {
    case AP_KEY_PLAY | AP_KEY_LONG: //长按play发standby
    case AP_KEY_FUNCTION | AP_KEY_LONG:
        longkey_flag = 1;
    case AP_MSG_STANDBY: //自动关机
        return RESULT_STANDBY;
    case AP_KEY_USBTEST | AP_KEY_UP:
        //如果没有连接USB线，则不进入utest功能
        if(usb_on_flag == 0)
        {
            return NULL;
        }
        return RESULT_USBTEST;
    case AP_KEY_USBTEST | AP_KEY_LONG:
        longkey_flag = 1;
        return RESULT_PITCH;

        break;
    case AP_KEY_REC | AP_KEY_UP: //进入录音
        //这里的条件判断主要是为了处理数字长按键复用成录音键时，
        //防止退出当前ap后再次处理该按键
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
 * Description : 获取onoff键的状态
 *
 * Arguments   :
 *
 * Returns     : 无
 *
 * Notes       :
 *
 ********************************************************************************/

uint8 get_onoff_key_status(void)
{
    uint8 sfr_bak, onoff_staus;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    onoff_staus = SYSTEM_ONOFF & 0x08;  //获取onoff键的状态
    SFR_BANK = sfr_bak;
    return onoff_staus;
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

uint8 check_longkey_msg(uint8 key)
{
    if ((key == AP_MSG_SD_IN) || (key == AP_MSG_UH_IN) || (key == AP_MSG_SD_OUT) || (key == AP_MSG_UH_OUT) || (key
            == AP_MSG_WAIT_TIMEOUT))
    {
        return 1;
    }
    else if ((key & AP_KEY_UP) != 0)
    {
        //如果检测到onoff键还是按住的，则可能是误发出了up消息，则不作处理。
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
 * Description : int FW_Updata(void) 通过卡或U盘进行小机固件升级，当前仅支持卡升级
 *
 * Arguments   : 当前只支持卡升级
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


