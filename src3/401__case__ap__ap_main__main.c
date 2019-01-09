/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: main.c,v 1.2 2006/06/26 01:13:37 tangwh Exp $
 *******************************************************************************
 */
#include "actos.h"
#include "ap_common.h"
#include "display.h"
#include "uhdrv.h"
#include "AlarmDef.h"
#include "ap_main.h"
#include "key.h"

#pragma name(AP_MAIN)

vm_cfg_t g_modifyval;

uint8 ap_id[MAX_AP_NUM];
bool with_fm = FALSE; //存在fm模块
uint32 item_offset[8];
uint8 far sd_read_buf[512];
const uint8 FwcfgName[] = "FWCFG.BIN";

#ifdef MAIN_AP
char report_buf[16]; //存诸播报数据的buffer
info_report_t info =
{   report_buf, 0, TRUE};
#endif

uint8 check_gpioa6(void);
uint8 test_uart(void);
uint8 test_dpdm(void);

/*
 *******************************************************************************
 * Description : 初始化Modify设置变量
 *
 * Arguments   :   ap_result,存放 AP结果消息的数组指针
 *
 * Returns     :   NONE
 *
 * Notes       :
 *
 *******************************************************************************
 */
void ReadModifyInfor(void)
{
    uint8 i, j = 0;

    VMRead(&g_modifyval, VM_AP_FWCFG, sizeof(vm_cfg_t));

    if (g_modifyval.Magic != MAGIC_CFG)
    {
        //read modify information from FWCFG.BIN
        SD_FILE *fwcfgfp;
        fwcfgfp = SD_FOpen(FwcfgName, MODE_READ);
        /*读取配置文件头信息*/
        SD_FRead(fwcfgfp, sd_read_buf, sizeof(sd_read_buf)); //读取配置文件头信息

        //item_offset[0] --- parameters set item offset
        //item_offset[1] ---  key config item offset
        //item_offset[2] --- ap config item offset
        //item_offset[3] --- language config item offset
        //item_offset[4] --- file exchange config item offset, not used here
        //item_offset[5] --- function config item offset
        for (i = 0; i < 6; i++)
        {
            item_offset[i] = *(uint32 *) (sd_read_buf + 16 + i * SIZE_ONE_ITEM);
        }

        SD_FSeek(fwcfgfp, SEEK_SET, item_offset[0]); //读取参数类头信息
        SD_FRead(fwcfgfp, sd_read_buf, 512); //读取数值设置类信息

        //获取固件版本信息偏移位置
        item_offset[6] = *(uint32 *) (sd_read_buf + 5);
        /*读取数值类参数信息*/
        memcpy(&g_modifyval.SleepTime, &sd_read_buf[SIZE_PARA_HEAD], sizeof(value_set_t));
        memcpy(&g_modifyval.Volume, &sd_read_buf[SIZE_PARA_HEAD + SIZE_PARA_ONEITEM], sizeof(value_set_t));
        memcpy(&g_modifyval.VolDefault, &sd_read_buf[SIZE_PARA_HEAD + 2 * SIZE_PARA_ONEITEM], sizeof(value_set_t));
        memcpy(&g_modifyval.RecordGain, &sd_read_buf[SIZE_PARA_HEAD + 3 * SIZE_PARA_ONEITEM], sizeof(value_set_t));
        memcpy(&g_modifyval.RecordTimeMax, &sd_read_buf[SIZE_PARA_HEAD + 4 * SIZE_PARA_ONEITEM], sizeof(value_set_t));
        memcpy(&g_modifyval.Nor_RecordTimeMax, &sd_read_buf[SIZE_PARA_HEAD + 5 * SIZE_PARA_ONEITEM], sizeof(value_set_t));
        //        memcpy(&g_modifyval.RecordGain, &sd_read_buf[SIZE_PARA_HEAD + 4 * SIZE_PARA_ONEITEM], sizeof(value_set_t));

        /*读取按键配置信息*/
        SD_FSeek(fwcfgfp, SEEK_SET, item_offset[1]);
        SD_FRead(fwcfgfp, sd_read_buf, sizeof(key_cfg_t));

#if 0
        g_modifyval.KeyInfor.Total = ((key_cfg_t *) (sd_read_buf))->TotalKey;
        memcpy(g_modifyval.KeyInfor.Table, ((key_cfg_t *) (sd_read_buf))->KeyValue, 16);
#else
		g_modifyval.KeyInfor.Total = 3;
		g_modifyval.KeyInfor.Table[0] = Msg_KeyPlayPause;
		if (KEY_VOLSUB_TH < KEY_VOLADD_TH)
		{
			g_modifyval.KeyInfor.Table[1] = Msg_KeyVolSub;
			g_modifyval.KeyInfor.Table[2] = Msg_KeyVolAdd;
		}
		else
		{
			g_modifyval.KeyInfor.Table[1] = Msg_KeyVolAdd;
			g_modifyval.KeyInfor.Table[2] = Msg_KeyVolSub;
		}
#endif

        //	g_modifyval.Supper_clock= ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD + 3 * sizeof(func_item_t)]))->State;
        //	 g_modifyval.TTS_Report= ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD + 4 * sizeof(func_item_t)]))->State;

        /*读取ap配置类信息*/
        SD_FSeek(fwcfgfp, SEEK_SET, item_offset[2]);
        SD_FRead(fwcfgfp, sd_read_buf, 512);
        for (i = 0, j = 0; i < sd_read_buf[0]; i++)
        {
            if (((ap_infor_t *) (&sd_read_buf[SIZE_APCFG_HEAD + SIZE_APINFOR_ITEM * i]))->Status != 0)
            {
                //有效AP
                g_modifyval.ApInfor.Table[j]
                    = ((ap_infor_t *) (&sd_read_buf[SIZE_APCFG_HEAD + SIZE_APINFOR_ITEM * i]))->Index;
                j++;
                /*找出Active 的序号*/
                if (j == sd_read_buf[1])
                {
                    g_modifyval.ApInfor.Active = j;
                }

            }
        }
        g_modifyval.ApInfor.Total = j;

        /*读取语言配置信息*/
        //       SD_FSeek(fwcfgfp, SEEK_SET, item_offset[3]);
        //        SD_FRead(fwcfgfp, sd_read_buf, sizeof(lan_set_t));
        //        memcpy(&g_modifyval.LanInfor, sd_read_buf, sizeof(lan_set_t));

        /*读取功能配置信息*/
        SD_FSeek(fwcfgfp, SEEK_SET, item_offset[5]);
        SD_FRead(fwcfgfp, sd_read_buf, 512);
        g_modifyval.KeyToneFlag = ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD]))->State;
        g_modifyval.Support_WAV = ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD + 1 * sizeof(func_item_t)]))->State;
        g_modifyval.TTS_Report = ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD + 2 * sizeof(func_item_t)]))->State;
        g_modifyval.Support_NorRec = ((func_item_t *) (&sd_read_buf[SIZE_FUNC_HEAD + 3 * sizeof(func_item_t)]))->State;
        g_modifyval.Magic = MAGIC_CFG;
        SD_FClose(fwcfgfp);

        VMWrite(&g_modifyval, VM_AP_FWCFG, sizeof(vm_cfg_t));

    }
    memcpy(sKY_GetKeyTabAddress(), g_modifyval.KeyInfor.Table, g_modifyval.KeyInfor.Total);
    memcpy(ap_id, g_modifyval.ApInfor.Table, g_modifyval.ApInfor.Total);
    ClearWatchDog();
}

/*
 ********************************************************************************
 * Description : 初始化系统设置变量
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void read_var(void)
{
    BOOL result;

    result = VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (g_comval.magic != MAGIC_COMVAL)
    {
        memset(&g_comval, 0x0, sizeof(g_comval));
        g_comval.magic = MAGIC_COMVAL;
        g_comval.ap_result = ap_id[0];
        g_comval.SleepTime = g_modifyval.SleepTime.DefValue;

        g_comval.VolumeMax = g_modifyval.Volume.DefValue;
        g_comval.VolumeDef = g_modifyval.VolDefault.DefValue;
        if (g_comval.VolumeDef > g_comval.VolumeMax)
        {
            g_comval.VolumeDef = g_comval.VolumeMax;
        }
        g_comval.vol_display = g_comval.VolumeDef;
        g_comval.volume = g_comval.VolumeDef; // (g_comval.vol_display * SOFTVOL_MAX) / g_comval.VolumeMax;

        g_comval.FMBuildInFlag = g_modifyval.FMBuildInFlag;
        g_comval.RecordGain = g_modifyval.RecordGain.DefValue;
        g_comval.RecordTime_max = g_modifyval.RecordTimeMax.DefValue;
        g_comval.Nor_RecordTime_max = g_modifyval.Nor_RecordTimeMax.DefValue;
        g_comval.SuppTTS = g_modifyval.TTS_Report;
        g_comval.SuppNorRec = g_modifyval.Support_NorRec;
        g_comval.eq_type = none;
        g_comval.SuppKeyTone = g_modifyval.KeyToneFlag;
        g_comval.RecordType = g_modifyval.Support_WAV;

        g_comval.g_alarm.AlarmEnable = FALSE;
        g_comval.g_alarm.volume = 35;
        g_comval.g_alarm.alarmTime.hour = 12; //闹钟时
        g_comval.g_alarm.alarmTime.minute = 0; //闹钟分
        g_comval.g_alarm.alarmTime.second = 0;//闹钟分
        g_comval.g_alarm.alarmDate.month = 1; //闹钟月
        g_comval.g_alarm.alarmDate.day = 1; //闹钟日
        g_comval.g_alarm.alarmDate.year = 2013; //闹钟年
        g_comval.g_alarm.location.disk = 'U';
        strcpy(g_comval.g_alarm.location.filename, "alarm.mp3"); //使用内部alarm文件
        result = VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    }
}
/*
 ********************************************************************************
 * Description : pa电源GPIO按键初始化
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */

void init_pa_ctl_gpio(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    //使能pa 电源开关GPIO 口
    PA_POWER_GPIO_OUTPUT |= (1 << PA_POWER_GPIO_NUM);
    //将GPIOE2,3,4,5切换成GPIO口功能，以防默认为I2S是对FM的干扰
    GPIOEINEN |= 0x3c;
    SFR_BANK = sfr_bak;
}
/*
 ********************************************************************************
 * Description : 关闭EJTAG
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :由于I2C使用的GPIO口与EJTAG复用，所以两者只能选其一
 *
 ********************************************************************************
 */

void disable_ejtag(uint8 flag)
{
    uint8 sfr_bak;
    if (flag == 0)
    {
        return;
    }
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    MFP_CTL2 &= 0xfe;//disable ejtag
    //for test
    GPIOCOUTEN |= 0x08;
    GPIOCDAT &= 0xf7;
    SFR_BANK = sfr_bak;
    TM_DelayMS(100);
}

/*
 ********************************************************************************
 * Description : 主函数
 *
 * Arguments   :
 *
 * Returns     : 结果消息或0
 *
 * Notes       :
 *
 ********************************************************************************
 */

int16 main(int16 param)
{
    int16 result;
	uint8 test_uart_flag = FALSE;
	uint8 test_dpdm_flag = FALSE;
	param = param;

    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    ClearWatchDog();

	test_dpdm_flag = test_dpdm();
	
	if (test_dpdm_flag)
	{
		test_uart_flag = test_uart();
	}

	//if(check_gpioa6()==1)
	/*{
		if (!DRV_Install("DRV_UART.DRV", 0))
	    {
	        while (1)
	        {
	            ;
	        }
	    }
	}	*/

	if (!DRV_Install("KY_HARD.DRV", 0))
    {
        while (1)
        {
            ;
        }
    }
	
//    init_pa_ctl_gpio();
    //使能Pa之前先将外部pa电源disable，防止开机时出现papa电流声
//    switch_pa_power(0);
    //读取系统配置
    ReadModifyInfor();
    read_var();

    result = start_init(TRUE);

	if (test_uart_flag && test_dpdm_flag)
	{// drive earphone
		while(1)
		{
			ClearWatchDog();
			KY_Beep(1);
		}
	}
	
    //如果是长按onoff开机的话，需要将longkey_flag置起来，
    //以便过滤play键消息，以防再次进入standby
    if(get_onoff_key_status() != 0)
    {
        longkey_flag = 1;
    }


    while (1)
    {
        ClearWatchDog();
        result = AP_Loop_Exec(result);
    }//while(1)
    //never run here
}

/*打开PA开关*/
void  OpenPA(uint8 volume)
{

    pa_t paattr;
    //g_PAOpenFlag = TRUE;//标志PA已打开


    paattr.pa_in.dacin = 0;
    paattr.pa_in.fmin = 0;
    paattr.pa_in.linein = 0;
    paattr.pa_in.micin = 0;
    paattr.volume = (int8) get_dac_real_vol(volume);
    paattr.reserve[0] = (int8) get_dac_real_vol(volume);
#if 0
    //非直驱
    EnablePA(&paattr);
#else
    //直驱
    EnablePADDV(&paattr);
#endif
}
/*打开DAC开关*/
void  OpenDAC(void)
{
    dac_t dacattr;
    dacattr.dacselect = 0;
    dacattr.rate = 22;
    EnableDAC(&dacattr);
}

/********************************************************************************
 * Description :开机获取闹钟标记
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :

 *
 ********************************************************************************/
uint8 check_alarm_msg(void)
{
    uint8 sfr_bak;
    uint8 alarm_flag = 0;
    uint8 alarm_pending;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    alarm_pending = RTC_CTL0 & 0x40;//检测闹钟pending位是否置起来
    SFR_BANK = sfr_bak;
    if (alarm_pending == 0)
    {
        return alarm_flag;
    }
    //检测闹钟消息
    if (alarmtimerflag == 1)
    {
        alarmtimerflag = 0;
        if (g_comval.g_alarm.AlarmEnable != 0)
        {
            SFR_BANK = BANK_RTC;
            RTC_CTL0 = RTC_CTL0 | 0x01;//将闹钟标志清除
            SFR_BANK = sfr_bak;
            alarm_flag = 1;
        }
    }
    return alarm_flag;
}
/********************************************************************************
 * Description :检测硬开关是否处于关闭状态
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :不管小机不知以何种方式唤醒了，当检测到硬开关处于关闭状态时，
 *                都直接进入standby状态，以保证硬开关的统一性
 *
 *
 ********************************************************************************/

uint8 check_hardswitch_status(uint8 result)
{
    uint8 retval = NULL, sfr_bak, onoff_status;
    retval = result;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    onoff_status = SYSTEM_ONOFF & 0x10;
    SFR_BANK = sfr_bak;
    if(onoff_status != 0)
    {
        retval = RESULT_STANDBY;
    }
    return retval;
}
/********************************************************************************
 * Description :开机启动运行到ap以后的初始化
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :主要是打开pa,dac,开机检测闹钟消息，USB线是否存在等情况
 *
 *
 *
 ********************************************************************************/

uint8 start_init(uint8 first_init)
{
    uint8 result;
#if 0	// not check usb status, because the status is wrong when charging.
    uint8 retval;
    {
        retval = GetUsbCableStatus();
        //1)如果检测到USB线，则先进udisk
        //2)如果没有接usb线，则进入上一次进入standby时ap
        if ((retval & 0x40) == 0x40)
        {
            result = RESULT_USBAUDIO_PLAY;
        }
        else
        {
            result = RESULT_LINEIN;// g_comval.ap_result;
        }
    }
#else
	result = RESULT_USBAUDIO_PLAY;
#endif
    if(first_init == TRUE)
    {
        OpenDAC();
        power_on_flag = 0;
        linein_status = 0;
        usb_on_flag = 0;
        memset(&tmp_time, 0xFF, sizeof(time_t));
    //    OpenPA(g_comval.volume); //有按键音，开PA
        set_dac_volume(g_comval.volume);
    }

    close_usb_phyctrol();

    return result;
}

uint8 check_gpioa6(void)
{
	uint8 sfr_bak;
	uint16 check_count;
	sfr_bak = SFR_BANK;
	SFR_BANK = BANK_GPIO;
	//init gpioa6
	GPIOAPUEN |= 1<<6; 
	GPIOAOUTEN |= 1<<6;
	GPIOAINEN |= 1<<6;
	GPIOADAT |= 1<<6;
	while((GPIOADAT & (1<<6)) == 0x40)
	{
		TM_DelayMS(1);
		check_count++;
		if (check_count > 1000)
		{
			GPIOAOUTEN &= ~(1<<6);
			SFR_BANK =sfr_bak;
			return 0;
		}
	}	
	GPIOAOUTEN &= ~(1<<6);
	SFR_BANK =sfr_bak;
	return 1;
}

uint8 test_uart(void)
{
	uint8 sfr_bak;
	uint8 ret = FALSE;
	
	sfr_bak = SFR_BANK;
	
	SFR_BANK = BANK_GPIO;
	GPIOCOUTEN |= 0x10;
	GPIOCINEN  |= 0x08;
    GPIOCDAT   |= 0x10;
	if (GPIOCDAT & 0x08)
	{
		GPIOCDAT &= ~0x10;
		if ((GPIOCDAT & 0x08) == 0x00)
		{
			ret = TRUE;
		}
	}
	SFR_BANK = sfr_bak;

	return ret;
}

uint8 test_dpdm(void)
{
    uint8 sfr_bak, dpdm_status;
    uint8 usb_status;
	uint8 ret = FALSE;
	
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;
    dpdm_status = DPDMCTRL;

    DPDMCTRL = 0x1f;
    usb_status = 0x80;
    while(usb_status--)
    {
        ;
    }

    if((USBSTATUS & 0x18) != 0x18)
    {
		usb_status = 0xff;
        while(usb_status--)
        {
            ; 
        }
        if((USBSTATUS & 0x18) == 0x18)
        {
            ret = TRUE;
        }     
	}
    else
    {
        ret = TRUE;
    }
	
    DPDMCTRL = dpdm_status;
    SFR_BANK = sfr_bak;
    return ret;
}

