/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: draw_logo.c,v 1.1.1.1 2006/05/22 03:24:01 selina Exp $
 *******************************************************************************
 */
#include "actos.h"
#include "ap_common.h"
#include "ap_main.h"

#pragma name(AP_EXEC_LOOP)
#ifdef MAIN_AP
uint8 report_cur_ap(uint8 report_msg)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS != 0)
    {
        report_buf[0] = ' ';
        report_buf[1] = report_msg;
        report_buf[2] = ' ';
        report_buf[3] = 0;
        check_report_state(&info);
        retval = wait_report_end(NULL);
    }
    return retval;
}
#endif
void save_cur_ap(void)
{
    g_comval.ap_result = ap_result;
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
}
void close_USBAndCard_device(void)
{
    USB_Detect_Ctrl(0);
    Card_Detect_Ctrl(0x01, 100, 0);
}
void open_USBAndCard_device(void)
{
    USB_Detect_Ctrl(1);
    Card_Detect_Ctrl(0x01, 100, 1);
}

uint8 check_key(void)
{
	uint8 sfr_bak;
    uint8 key_val = 0;
	uint8 i = 0;
	uint8 count = 0;
	uint8 bat_adc = 0;

	uint8 lradc = 0;
	uint8 adc_array[2];
	
	sfr_bak = (uint8) SFR_BANK;

	adc_array[0] = KEY_PLAY_TH;
	if (KEY_VOLSUB_TH < KEY_VOLADD_TH)
	{
		
		adc_array[1] = KEY_VOLADD_TH;
	}
	else
	{
		adc_array[1] = KEY_VOLSUB_TH;
	}

	TM_DelayMS(100);
	SFR_BANK = BANK_PMU;
	while(1)
	{
		ClearWatchDog();

#ifdef SUPPORT_APPLE_KEY
		lradc = LRADC4_DATA & 0x3f;
#else
		lradc = LRADC5_DATA & 0x3f;		
#endif

		if((lradc > adc_array[0]) && (lradc <= adc_array[1]))
		{
			i++;
		}

		count++;
		if(count >= 50)
		{
			break;
		}
	}
	SFR_BANK=sfr_bak;
	if(i >= 20)
	{
		return 1; // means detected vol- pressing down for a while.
	}
	else
	{
		return 0;
	}
}
/*
 ********************************************************************************
 * Description : 第二层AP间切换
 *
 * Arguments   : 执行ap名称
 *
 * Returns     :  ap 返回值
 *
 *
 * Notes       :
 *
 ********************************************************************************
 */
int16 AP_Loop_Exec(int16 ap_name)
{
    int16 result;
    int16 ResultCheck = 0;
    bool need_exit = FALSE;
    uint8 result_from_plug = 0;
    uint8 i;
    result = ap_name;
    ap_result = g_comval.ap_result;
    save_cur_ap();
    while(ap_get_message() != NULL)
    {
        ClearWatchDog();
    }
	
	if(check_key() == 1)
	{
		result = RESULT_UDISK;
	}
	
    while (1)
    {
        switch (result)
        {
        case RESULT_MUSIC_PLAY:
        case RESULT_MUSIC:
        case RESULT_MUSIC_CPLAY:
        case RESULT_MUSIC_UPLAY:
            ap_result = RESULT_MUSIC;
            save_cur_ap();
            result = ExecAP("music.ap", result);
            break;
        case RESULT_RADIO:
        case RESULT_FMREC2FM:
        case RESULT_FMREC2STANDBY:
            ap_result = RESULT_RADIO;
            save_cur_ap();
            result = ExecAP("radio.ap", result);
            break;
        case RESULT_LINEIN:
            if(result_from_plug == FALSE)
            {
                ap_result = RESULT_LINEIN;
            }
            save_cur_ap();
            result = ExecAP("linein.ap", 0);
            if(result == RESULT_LINEIN)
            {
                if(ap_result == RESULT_LINEIN)
                {
                    result = RESULT_MAIN;
                }
                else
                {
                    result = ap_result;
                }
            }
            else if(result == RESULT_MAIN)
            {
                if(result_from_plug == TRUE)
                {
                    ap_result = RESULT_LINEIN;
                }
            }
            else if(result == RESULT_USBTEST)
            {
                ap_result = RESULT_LINEIN;
            }
            break;
        case RESULT_ALARM:
            result = ExecAP("alarm.ap", result);
            break;
        case RESULT_CLOCK:
            ap_result = RESULT_CLOCK;
            save_cur_ap();
#ifdef MAIN_AP
            result = report_cur_ap(EXTEND_SYS_TIME);
            if (result == RESULT_NULL)
            {
                result = set_clock_ui(0);
            }
#else
            result = set_clock_ui(0);
#endif
            VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
            break;

        case RESULT_RECORD: //开始录音
        case RESULT_FMREC_START:
#ifdef MAIN_AP
            //如果从FM进入录音，这里不做语音播报功能，以防
            //切换到其他ap时，FM的监听功能没有关闭
            if(result == RESULT_FMREC_START)
            {
                ResultCheck = RESULT_NULL;
            }
            else
            {
                ResultCheck = report_cur_ap(EXTEND_RECORD);
            }
            if (ResultCheck == RESULT_NULL)
            {
                result = ExecAP("record.ap", result);
            }
            else
            {
                result = ResultCheck;
            }
#else
            result = ExecAP("record.ap", result);
#endif
            break;
        case RESULT_PITCH:
            result = ExecAP("ap_rec_p.ap", result);
            break;

        case RESULT_USBAUDIO_PLAY: //Mode键功能，由FM切换到USB 音箱
            result = USBDevice(USB_Audio);
            break;
        case RESULT_UDISK:
        case RESULT_USBUDISK_PLAY: //Mode键功能，由USB 音箱切换到 UDISK
            result = USBDevice(USB_disk);
            break;
        case RESULT_USBTEST: //加载音效调试通信模块
            result = ExecAP("usbtest.ap", result);
            break;
        case RESULT_STANDBY:
            g_comval.ap_result = ap_result;
            VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
            result = ExecAP("standby.ap", 0);
            if(result != RESULT_ALARM)
            {
                result = start_init(FALSE);
            }
            break;

        default:
            result = RESULT_NULL;
            break;
        }
        //先过滤所有的消息，以免检测到不必要的消息
        while (ap_get_message() != NULL)
        {
        };
        /*支持插卡时从应用退出需卸载插卡驱动*/
        DRV_UnInstall(DRV_STG);
        VMRead(&g_modifyval, VM_AP_FWCFG, sizeof(vm_cfg_t));
        if (g_modifyval.Magic != MAGIC_CFG)
        {
            ReadModifyInfor();
        }
        else
        {
            memcpy(ap_id, g_modifyval.ApInfor.Table, g_modifyval.ApInfor.Total);
            memcpy(sKY_GetKeyTabAddress(), g_modifyval.KeyInfor.Table, g_modifyval.KeyInfor.Total);
        }
        read_var();
        //检测到linein线插入时，将该位置起来
        if(result == RESULT_LINEIN)
        {
            result_from_plug = TRUE;
        }
        //一键切换时，获取下一个ap值
        if (result == RESULT_MAIN)
        {
            for (i = 0; i < g_modifyval.ApInfor.Total; i++)
            {
                if (ap_result == ap_id[i])
                {
                    if (i == (g_modifyval.ApInfor.Total - 1))
                    {
                        result = ap_id[0];
                    }
                    else
                    {
                        result = ap_id[i + 1];
                    }
                    break;
                }
            }
            if (i >= g_modifyval.ApInfor.Total)
            {
                result = ap_id[0];
            }
            g_comval.ap_result = result;
            VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
        }
        if (result == RESULT_NULL)
        {
            result = ap_result;
        }
    }
    return result;
}

uint16 near USBDevice(int16 State)
{
    uint16 result;
#if 0	// not check usb status, because the status is wrong when charging.
    uint8 retval;
    retval = GetUsbCableStatus(); //USB线是否连接
    //if((retval & 0xc0) == 0xc0)
    if((retval & 0x40) == 0x40)
    {
		if(longkey_flag == 1)
        {
            longkey_flag = 0;
        }
        result = USBDeviceService(State); // 进入USB相应功能
    }
    else
    {
        result = ap_id[0];
    }
    usb_on_flag = 0;
    check_plug_device_msg(&usb_on_flag, USB_IN);
#else
	result = USBDeviceService(State); 
#endif
    return result;
}

uint16 near USBDeviceService (int16 State)
{
    uint8 result = RESULT_NULL;
    uint8 temp;
	uint8 sfr_bak;
	
    if(State == USB_disk)
    {
        UHOST_Power_EN(sfr_bak);
        UHOST_Power_OPEN(sfr_bak);
        open_USBAndCard_device();

        SetPLL(PLL_36MHZ);
        SetMemBankCtl_CODE(temp);
        result = ExecAP("usbdisk.ap", 0);
        RestoreMemBankCtl(temp);
        SetPLL(PLL_48MHZ);
    }
    else if(State == USB_Audio)
    {
        result = ExecAP("usbsound.ap", 0);
    }
    while(ap_get_message() != NULL)
    {
        ClearWatchDog();
    }
    return result;
}

