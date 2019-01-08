/*
 ********************************************************************************
 *                       ACTOS
 *                      usbsound
 *
 *                (c) Copyright 2002-2009, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : usound_tast.c
 * By     : wanghaijing
 * Version: v1.00     first version    2011-1-28 15:55  by wanghaijing
 ********************************************************************************
 */
#include "usound.h"

#pragma name(AP_TASK)
extern uint8 g_PAOpenFlag;
extern uint8 usb_state;

#pragma renamecode(?PR?AP_TASK)

#define  NUMWIDTH      22
#define  TXTPOSTY      58

SD_FILE *res_fp;
region_t res_region;
uint8 ui_auto_select = FALSE; //是否自动确认
uint32 pic_entry_offset;
uint32 string_entry_offset;
uint8 p_device_name[] = "USND";
uint8 g_light_time = 0; //关背光时间,0.5秒为单位
uint8 g_light_mode = 0; //关亮度模式，0：变暗，1：变黑
uint8 g_contrast_num = 0; //对比度
uint16 g_sleep_time = 0; //睡眠时间,0.5秒为单位
uint16 g_standby_time = 0; //自动关机时间,0.5秒为单位
uint16 g_rtc_counter = 0; //rtc 消息计数
uint8 g_light_flag = TRUE;//背景灯开关标志
uint8 g_charge_counter = 0; //检测充电状态的计数器，插上USB就开始检测第一次
uint16 g_low_counter = 0;//报低电计数器
uint16 key_count = 0; //当前按键消息发生的次数
uint8 key_value = Msg_KeyNull; //当前按键的值
uint16 lowPower_counter = 0;//低电发生次数
uint8 file_name[12]; //存放文件短名数字
uint8 cur_value = 0; //当前的电池电量值，主要用于充电显示
uint8 batvalue[5] =
{ 0x5c, 0x5f, 0x64, 0x69, 0x6e };
uint8 double_key_flag = 0;
uint8 double_key_count = 0;

uint8 long_key_flag = 0;
uint8 push_flag = 0;
uint8 FF_Play_mode=0;
uint8 headphone_in_flag = TRUE;
extern uint8 key_timer_id;
extern int16 prepare_urec(void);
uint8 ap_get_message_core(uint8 key);
uint8 deal_usound_vol_msg(uint8 key);
uint8 check_headphone_in(void);
void ignore_start_key(void);
uint8 check_headphone_status(void);
uint8 check_vbus(void);
void enable_pmos(void);

void key_timer_count(void)
{
	if (double_key_flag != 0)
	{
		double_key_count++;
        //printh(double_key_count);
		if (double_key_count > 15)
		{
			if (double_key_flag == 2)
			{
				//prints("\n two");
				printh(0x292);
				usound_key_msg(0x10);	
				//usound_key_msg(0x03);
			}
			else if (double_key_flag == 3)
			{
				/*prints("\n three");
				usound_key_msg(0x20);*/
			    printh(0x3223);
				usound_key_msg(0x20);	
				//usound_key_msg(0x05);
			}
			else
			{
				//prints("\n one");
                printh(0xc11);
				usound_key_msg(0x08);
				//usound_key_msg(0x04);
			}		
			double_key_flag = 0;
			key_value=0xff;
		}
	}
	else
	{	
		double_key_count = 0;
	}
}

uint8 check_headphone_in(void)
{
	uint8 lradc1_val;
	uint8 sfr_bak;
	uint8 ret = 0;
	uint8 mode;
	return 0;	// circuit board is wrong, can't detect earphone;
	
	sfr_bak = SFR_BANK;
	SFR_BANK = BANK_GPIO;
	if ((GPIOADAT & (1<<5)) == 0x20)
	{
		mode = 1;
	}
	else
	{
		mode = 0;
	}
	
	SFR_BANK = BANK_PMU;
	lradc1_val = LRADC1_DATA;
	if (mode)
	{
		if (lradc1_val > 0x1A)
		{//	headphone in
			ret = 0;
		}
		
		if (lradc1_val < 0x04)
		{//	headphone out
			ret = 1;
		}
	}
	else
	{
		if (lradc1_val > 0x1A)
		{//	headphone out
			ret = 1;
		}
		
		if (lradc1_val < 0x04)
		{//	headphone in
			ret = 0;
		}
	}
	SFR_BANK = sfr_bak;
	
	return ret;
}

uint8 check_headphone_status(void)
{
	if (headphone_in_flag == FALSE)
	{
		if (check_headphone_in() == 0)
		{//	headphone in				
			if(key_timer_id != 0xFF)
			{
				TM_KillTimer(key_timer_id);
				key_timer_id = 0xFF;
			}
			ignore_start_key();
			key_timer_id = TM_SetTimer((uint32) key_timer_count, 4);
			headphone_in_flag = TRUE;
			return 1;
		}
	}
	else
	{
		if(key_timer_id == 0xFF)
		{
			key_timer_id = TM_SetTimer((uint32) key_timer_count, 4);
		}

		if (check_headphone_in() != 0)
		{//	headphone out
			headphone_in_flag = FALSE;
			if(key_timer_id != 0xFF)
			{
				TM_KillTimer(key_timer_id);
				key_timer_id = 0xFF;
			}

			return 1;
		}
	}
	return 0;
}

int16 Usound_loop(void)
{
    uint8 key_v = 0;
    uint8 key = 0;
    int16 retval;
    BOOL Audioloop = 1;

    //初始化不使能USOUND录音
    urec_en = FALSE;
    memset(file_name, 0xff, 12);
    while (Audioloop != 0)
    {
        ClearWatchDog();

        if (0x00 != (usb_state & 0x04)) //拔线处理
        {
            Audioloop = FALSE;
            retval = RESULT_MAIN;
            continue;
        }
		if(vol_change_flag==1)
		{
			vol_change_flag=0;		   
     		change_vol(cur_vol_index);  
		}

		/* check headphone status */
		check_headphone_status();

		/* skip key if headphone not detected */
		if(headphone_in_flag==TRUE)
		{
			key_v = ap_get_message();
		}
		else
		{
			GetSysMsg();
			key_v=NULL;
		}
		
        switch (key_v)
        {
            /* mode键作为usound录音使能键 */
		case AP_MSG_WAIT_TIMEOUT:
			if (key != 0)
            {
                key = 0;
                need_draw = TRUE;
            }
			break;
        case AP_KEY_MODE | AP_KEY_UP:
            //打开USOUND录音
            key = 0;
            //LEDClearScreen();
            //LEDPuts(NUMBER1, p_device_name, 1);
            //LEDDisplay(LED_PLAY, 0xff, 1);
            retval = prepare_urec();
            if (retval == 0)
            {
                retval = usound_rec();
                mp3rSendCommand(MC_CLOSE, 0);
                if (retval == RESULT_MAIN)
                {
                    retval = RESULT_NULL;
                    Audioloop = 0;
                    break;
                }
            }
            //urec_en = 0;
            need_draw = TRUE;
            break;

            /* 长按mode返回main AP */
        case AP_KEY_MODE | AP_KEY_LONG:
            longkey_flag = 1;
            Audioloop = 0;
            retval = RESULT_NULL;
            break;

        case AP_KEY_PLAY | AP_KEY_LONG:			
			if (push_flag == 0)
			{ 
				if(FF_Play_mode==1)
				{
				//	prints("\n FF KEY LONG");
					usound_key_msg(0x40);	
				}
				else if(FF_Play_mode==2)
				{
				//	prints("\n FB KEY LONG");
					usound_key_msg(0x80);	
				}
				else
				{
				//	prints("\n MUTE KEY LONG");
				//	usound_key_msg(0x04);
				}				
				push_flag=1;
			}			
			break;
        case AP_KEY_PLAY | AP_KEY_UP:
			if(FF_Play_mode!=0)
			{	
				if(FF_Play_mode == 1)
				{
				//	prints("\n FF KEY UP");
					usound_key_msg(0x40);
				}
				else if(FF_Play_mode == 2)
				{
				//	prints("\n FB KEY UP");
					usound_key_msg(0x80);
				}else
				{
				//	prints("\n MUTE KEY UP");				
				}
				FF_Play_mode=0;
				break;
			}
            if (key != 0)
            {
                key = 0;
                need_draw = TRUE;
            }
            else
            {
            //    usound_key_msg(0x08);
            }
            break;
			
        case AP_KEY_NEXT | AP_KEY_UP:
            usound_key_msg(0x10);
            break;

        case AP_KEY_PREV | AP_KEY_UP:
            usound_key_msg(0x20);
            break;

        case AP_KEY_VOLADD | AP_KEY_UP:
		//case AP_KEY_VOLADD | AP_KEY_LONG:
            key = 1;
            deal_usound_vol_msg(key);
            break;
        case AP_KEY_VOLSUB | AP_KEY_UP:
		//case AP_KEY_VOLSUB | AP_KEY_LONG:
            key = 2;
            deal_usound_vol_msg(key);
            break;
        case AP_KEY_VOL | AP_KEY_UP:
            deal_usound_vol_msg(0);
            break;

#ifdef FUN_SWITCH_KEY
        case AP_KEY_FUNCTION | AP_KEY_UP: //Mode键，UHost\Card\FM\USB音箱 按下循环切换
            Audioloop = 0;
            retval = RESULT_USBUDISK_PLAY;
            break;
#endif

            //case AP_MSG_CHARGING:
            // ui_show_batt(GetBattery());
            //break;

            /*
             case AP_KEY_FUNCTION | AP_KEY_UP:
             MYDUMP("P",0);
             Audioloop = 0;
             g_result = RESULT_MUSIC_PLAY;
             break;
             */
        default:
            break;
        }
    }
    return retval;
}

void volume_callback(int16 Vol)
{

    //实时改变音量的值
    g_comval.volume = Vol; // (uint8) Vol * SOFTVOL_MAX / g_comval.VolumeMax;
    OutPutVolume(g_comval.volume);
}
/********************************************************************************
 * Description : 处理音量键
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_usound_vol_msg(uint8 key)
{
    if (key == Key_Next)
    {
		usound_key_msg(0x01);
		#if 0
        if (g_comval.volume < g_comval.VolumeMax)
        {
            g_comval.volume++;
			usound_key_msg(0x01);
        }
		#endif
    }
    else if (key == Key_Prev)
    {
		usound_key_msg(0x02);
		#if 0
        if (g_comval.volume > 0)
        {
            g_comval.volume--;
			usound_key_msg(0x02);
        }
		#endif
    }
    //volume_callback(g_comval.volume);
    need_draw = TRUE;
    return NULL;
}

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
		key = ap_get_message_core(key);
        return key;
    }
}

//
#if 0
void slider_atoi4(uint8 *str, int16 count)
{
    int16 tmp, level;

    if (count < 0)
    {
        *str = '-';
        count = 0 - count;
        str++;
    }

    if (count >= 1000)
    {
        level = 4;
    }
    else if (count >= 100)
    {
        level = 3;
    }
    else if (count >= 10)
    {
        level = 2;
    }
    else
    {
        level = 1;
    }
    if (level >= 4)
    {
        tmp = (count / 1000) % 10;
        *str = (uint8) (tmp + '0');
        str++;
    }
    if (level >= 3)
    {
        tmp = (count / 100) % 10;
        *str = (uint8) (tmp + '0');
        str++;
    }
    if (level >= 2)
    {
        tmp = (count / 10) % 10;
        *str = (uint8) (tmp + '0');
        str++;
    }
    tmp = count % 10;
    *str = (uint8) (tmp + '0');
    *(str + 1) = '\0';
}
#endif

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
#if 0
uint8 ap_handle_hotkey_core(uint8 key)
{
    /* if (usbplugflag == AP_MSG_USB_PLUGIN_FLAG)
     {
     return RESULT_UDISK;
     }*/
    switch (key)
    {
    case AP_KEY_PLAY | AP_KEY_LONG: //长按play发standby
    case AP_MSG_STANDBY: //自动关机								 
    case AP_MSG_FORCE_STANDBY: //使用上两个会报label duplicate
		long_key_flag=0;
		break;
        return RESULT_STANDBY;

    case AP_KEY_MODE | AP_KEY_LONG: //长按menu进入main
        return RESULT_MAIN;

    case RESULT_AUTO_BACK:
        return RESULT_AUTO_BACK;

    case AP_KEY_AB | AP_KEY_UP: //进入录音
        return RESULT_RECORD;
    case AP_MSG_SD_OUT:
        //ui_show_CardOut();
        return RESULT_SD_OUT;
    case AP_MSG_LOWBAT:
        //			ui_show_lowbat();
        return RESULT_REDRAW;

        /*case AP_MSG_LOCK: //locked
         if (ui_run_realtime == TRUE) //实时模式不显示lock
         {
         return 0;
         }
         else
         {
         ui_show_lock(1); //这里不处理热键,防止嵌套
         return RESULT_REDRAW;
         }*/
#ifdef CHECKALARM
    case AP_MSG_ALARM:
        return RESULT_ALARM;
    case AP_MSG_FM:
        return RESULT_ALARMFM;
#endif
#ifndef LINEIN_AP
    case AP_MSG_LINEIN:
        return RESULT_LINEIN;
#endif
    default:
        //here just drop the key
        return 0;
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
#endif

//
void Lightmodeset(bool OnOff)
{
    OnOff = OnOff;
    //StandbyScreen(OnOff);
}
/********************************************************************************
 * Description : 获取充电器的状态
 *
 * Arguments  :
 *
 *
 * Returns     :dpdm_status=0X80为充电器
 *            无
 * Notes       :
 *
 ********************************************************************************/
uint8 GetDC5VStatus(void)
{
    uint8 sfr_bak;
    uint8 dpdm_status;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    //dpdm_status = (uint8 ) (SYSTEM_VOL & 0x80);
    SFR_BANK = sfr_bak;
    return dpdm_status;
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
    //    static WORD key_count=0;      //当前按键消息发生的次数
    //    static uint8 key_value = Msg_KeyNull;   //当前按键的值
#ifdef EAR_PROTECT
    bool counter_flag = FALSE;
#endif
    //    uint8 sfr_bak;
    uint8 sysOnOff = 0;
    /*按键消息和上次的一样时的处理*/
    if (key == key_value)
    {
        /*背光没开时，等抬起键开背光，忽略按键*/
        g_rtc_counter = 0;
        key_count++;
        if (!g_light_flag)
        {
            key_count = 0;
            return NULL;
        }
		if(key_count>=2)
		{
			if(((double_key_flag==1)||(double_key_flag==2))&&(key == AP_KEY_PLAY))
            {	           	
				FF_Play_mode=double_key_flag;
				double_key_flag=0;
				long_key_flag = 1;
				key = (uint8) (key | AP_KEY_LONG); //》1.2s,发Key hold消息 
			//	prints("\n FF_Play");
				return key;
            }
		}
        /*按住按键时，320ms发送一次相同按键消息*/
		if (key_count >= 4)
        {
            key = (uint8) (key | AP_KEY_LONG); //》1.2s,发Key hold消息           
			long_key_flag = 1;
        }
        else
        {

        }
        return key;
    }

    switch (key)
    {
    case Msg_KeyShortUp:
        key_count = 0;
        g_rtc_counter = 0;
		if (long_key_flag != 0)
		{			
			long_key_flag = 0;
			push_flag = 0;
			key_value=0xff;
			if(FF_Play_mode!=0)
			{
			//	prints("\n ap_Play");
				return (AP_KEY_PLAY | AP_KEY_UP);
			}
			else
			{
				return NULL;
			}
			
		}
		
        /*key_value的置需改变*/
		if (key_value == AP_KEY_PLAY)
		{
			if (double_key_flag == 0)
			{
				double_key_count = 0;
			}
			double_key_flag++;
			if (double_key_flag > 3)
			{
				double_key_flag = 0;
			}
			else
			{
			    key_value=0xff;
				return NULL;
			}
		}
        key = key_value | AP_KEY_UP;
        key_value = key;
        if (!g_light_flag)
        {
            Lightmodeset(TRUE);//开背光
            g_light_flag = TRUE;
            /*输出按键音*/
            //Open_KeyTone(g_comval.KeyTone);
            return NULL;
        }
        break;

    case MSG_RTC2HZ:
        g_rtc_counter++;
#ifdef EAR_PROTECT
        if(g_EarProtect_flag == 1)
        {
            counter_flag = ear_protect_callback();
            if(counter_flag == TRUE)
            {
                g_decrease_time++;
            }
            else
            {
                g_decrease_time = 0;
            }

            g_half_hour_counter++;
            g_autoswitch_counter++;
        }
#endif

        

        /*2HZ计时无操作返回*/
        if ((g_rtc_counter % 16) == 0)
        {
            return AP_MSG_WAIT_TIMEOUT;
        }

        /*AP处理的2HZ消息*/
        if (key == MSG_RTC2HZ)
        {
#ifdef CHECKALARM	 //在AP工程里定义
            if (alarmtimerflag == 1)
            {
                if (g_comval.g_alarm.curAlarmType == ALARAMType)
                {
                    if (g_comval.g_alarm.FMRadioEnable)
                    {
                        //RSetAlarmTimer(g_alarm.FMRadioTime,FMType);
                        g_comval.g_alarm.curAlarmType = FMType;
                        TM_SetAlarmTime(&g_comval.g_alarm.FMRadioTime);
                        VMWrite(&g_comval, VM_SYSTEM);
                    }
                    key = CheckAlarmMsg();
                    if(key == NULL)
                    {
                        key = AP_MSG_RTC;
                    }
                    else
                    {
                        key = AP_MSG_ALARM;
                        Lightmodeset(TRUE);//开关背光（TRUE: 开, FALSE:关）
                    }
                }
                else
                {
                    if (g_comval.g_alarm.curAlarmType == FMType)
                    {
                        if (g_comval.g_alarm.AlarmEnable)
                        {
                            g_comval.g_alarm.curAlarmType = ALARAMType;
                            TM_SetAlarmTime(&g_comval.g_alarm.FMRadioTime);
                            VMWrite(&g_comval, VM_SYSTEM);
                        }
                        key = AP_MSG_FM;
                        Lightmodeset(TRUE);//开关背光（TRUE: 开, FALSE:关）
                    }
                    else
                    {

                        key = AP_MSG_RTC;
                    }
                }
            }
            else
            {
                if ((GetDC5VStatus() == 0) && (GetBattery() <= 0x5c))
                {
                    g_low_counter ++;
                    if(g_low_counter > 600)
                    {
                        g_low_counter = 0;
                        key = AP_MSG_LOWBAT;
                    }
                    else
                    {
                        key = AP_MSG_RTC;
                    }
                }
                else
                {
                    g_low_counter = 0;
                    key = AP_MSG_RTC;
                }
            }
#else
            g_low_counter = 0;
            key = AP_MSG_RTC;
#endif
        }

        /*充电检测2HZ计时，每30s监测一次充电状态*/
        if (GetDC5VStatus() == 0x80)
        {
            if (g_charge_counter == 60)
            {
                //if((GetBattery() <= 0x76)&&(sKY_ChargeSet(0) != 0x0))
                //{
                //	sKY_ChargeSet(1);
                //}
                //sKY_ChargeGetandSet();
                KY_GetChargeStatus();
                g_charge_counter = 0;
            }
            else
            {
                g_charge_counter++;
            }
        }
        /*
         #ifndef LINEIN_AP
         if(CheckLineIn())
         {
         key = AP_MSG_LINEIN;
         }
         #endif */
        break;

#if 0
    case MSG_LOW_POWER:
        /*系统低电消息处理为强制关机*/
        lowPower_counter++;
        if (lowPower_counter >= 3)
        {
            if(GetDC5VStatus() == 0x80)
            {
                lowPower_counter = 0;
                key = NULL;
            }
            else
            {
                key = AP_MSG_FORCE_STANDBY;
            }
        }
        else
        {
            key = NULL;
        }
        break;
#endif

    case Msg_KeyUnHold:
        key = AP_KEY_NULL;
        break;

    case MSG_SD_IN:/*插卡状态*/
        key = AP_MSG_SD_IN;
        if (!g_light_flag)
        {
            Lightmodeset(TRUE);//开背光
            g_light_flag = TRUE;
            g_rtc_counter = 0;
        }
        break;

#if 0
    case MSG_UH_IN:
        if(!g_light_flag)
        {
            Lightmodeset(TRUE);//开背光
            g_light_flag = TRUE;
            g_rtc_counter = 0;
        }
        key = AP_MSG_UH_IN;
        break;

    case MSG_UH_OUT:
        if(!g_light_flag)
        {
            Lightmodeset(TRUE);//开背光
            g_light_flag = TRUE;
            g_rtc_counter = 0;
        }
        key = AP_MSG_UH_OUT;
        break;
#endif

    case MSG_SD_OUT:/*拨卡状态*/
        key = AP_MSG_SD_OUT;
        break;	
    default: /*新按键处理*/
        g_rtc_counter = 0;
        key_count = 1;
        key_value = key;
        if (!g_light_flag)
        {
            /*如背灯没开，不处理消息*/
            key_count = 0;
            //g_rtc_counter = 0;
            //return NULL;
        }

        //        Open_KeyTone(g_comval.KeyTone);
    } /*end of switch*/

    return key;
}

void ignore_start_key(void)
{
	uint16 count=0;
	double_key_flag=0;
	while(1)
	{
		GetSysMsg();
		TM_DelayMS(10);
		count++;
		if(count>150)
		{
			break;
		}
	}

	double_key_flag=0;	
}

uint8 check_vbus(void)
{
	uint8 sfr_bak;
	uint8 ret = 0;

	sfr_bak = SFR_BANK;
	SFR_BANK = BANK_GPIO;

	if (GPIOADAT & (1<<5))
	{
		ret = 1;
	}
	
	SFR_BANK = sfr_bak;
	return ret; 
}

void enable_pmos(void)
{
	uint8 sfr_bak;

	sfr_bak = SFR_BANK;
	SFR_BANK = BANK_GPIO;

	GPIOADAT &= ~(1<<6);

	SFR_BANK = sfr_bak;
}

