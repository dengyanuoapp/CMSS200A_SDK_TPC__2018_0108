#include "drv_key.h"
#include "ap_common.h"

#pragma name(KEY_RCODE)

uint8 oldkey;
uint8 tmpkey;
uint8 tmp_count, hold_count;
uint8 wait_keyup;

uint8 const key_map[16];
uint8 IR_KEY;
uint8 Low_power_count;
uint8 TheCharge; //充电标识，0,低电；1，充电中，2.满电。3.有DC5V
uint8 onoff_flag;
uint8 cur_keyval;

#ifdef EVB_KEY
#define MAX_KEYVAL  7
#else
#ifdef ONE_KEY
#define MAX_KEYVAL  1
#else
#define MAX_KEYVAL  3
#endif
#endif

uint8 adctab[MAX_KEYVAL];
#ifdef SUPPORT_APPLE_KEY
uint8 adctab_A[MAX_KEYVAL];
#endif

const uint8 insert_msg[MAX_TYPE] =
{ AP_MSG_LINEIN_IN, AP_MSG_MIC_IN, AP_MSG_USB_PLUGIN};
const uint8 pull_out_msg[MAX_TYPE] =
{ AP_MSG_LINEIN_OUT, AP_MSG_MIC_OUT, USB_PLUGIN_FINISH };

//获取LRADC1值，用以判断低电和满电
//低电：小于3.3V    ，约等于0X50，小于等于这个值认为低电
//接近低电提示:小于3.4V,约等于0x54
//大于4.2V   约等于0X76，大于等于这个值认为满电
const uint8 bat_vol[3] =
{
    0x50, 0x54, 0x76
};

void key_scan(void)
{
    uint8 sfr_bak;
    uint8 key_val = 0, key_offset;
    uint8 row, tmp_adc;
#ifdef EVB_KEY
	uint8 lradc1_data;
#endif
	uint8 lradc4_data;
	uint8 lradc5_data;
#ifdef SUPPORT_APPLE_KEY
	uint8 det_lradc4_key   = FALSE;
#endif
	uint8 det_lradc5_key = FALSE;
	
    sfr_bak = (uint8) SFR_BANK;

#ifndef	NONE_ONOFF
    //检测on/off按键
    SFR_BANK = BANK_PMU;
    if ((SYSTEM_ONOFF & 0x08) != 0)
    {
		key_val = 1;		
        goto get_key;
    }
#endif

#ifdef EVB_KEY
	SFR_BANK = BANK_PMU;
    //检测LRADC1(sem_con)
    lradc1_data = LRADC1_DATA & 0x3f;
    lradc5_data = LRADC5_DATA & 0x3f;
    //优化检测LRADC1,如果lRADC1有按键键，则不再检测LRADC5
    if(lradc1_data < adctab[MAX_KEYVAL - 2])
    {
        key_offset = 0;
        tmp_adc = lradc1_data;
    }
    else if(lradc5_data < adctab[MAX_KEYVAL - 1])
    {
        key_offset = MAX_KEYVAL - 1;
        tmp_adc = lradc5_data;
    }
    else
    {
        key_val = NO_KEY;
        goto get_key;
    }
#else
	SFR_BANK = BANK_PMU;

#ifdef SUPPORT_APPLE_KEY
	lradc4_data = LRADC4_DATA & 0x3F;
	if(lradc4_data < adctab[MAX_KEYVAL - 1])
	{
		key_offset = 0;
		tmp_adc = lradc4_data;
		det_lradc4_key = TRUE;
		goto check_key;
	}
	else
	{
		key_val = NO_KEY;
	}
#endif

	lradc5_data = LRADC5_DATA & 0x3f;
#ifdef SUPPORT_APPLE_KEY
	if(lradc5_data < adctab_A[MAX_KEYVAL - 1])
#else
	if(lradc5_data < adctab[MAX_KEYVAL - 1])
#endif
    {
        key_offset = 0;
        tmp_adc = lradc5_data;
		det_lradc5_key = TRUE;
		goto check_key;
    }
    else
    {
        key_val = NO_KEY;
    }

	if (key_val == NO_KEY)
	{
		goto get_key;
	}
#endif

check_key:
	if (det_lradc5_key == TRUE)
	{
		for(row = 0; row < MAX_KEYVAL; row++)
		{
#ifdef SUPPORT_APPLE_KEY
			if(tmp_adc < adctab_A[row])
#else
			if(tmp_adc < adctab[row])
#endif
			{
				key_val = row + 1 + key_offset;
				goto get_key;
			}		
		}
	}

#ifdef SUPPORT_APPLE_KEY
	if (det_lradc4_key == TRUE)
	{
	    for(row = 0; row < MAX_KEYVAL; row++)
	    {
			if(tmp_adc < adctab[row])
	        {
				key_val = row + 1 + key_offset;
	            goto get_key;
			}		
	    }
	}
#endif

    key_val = NO_KEY;
	
get_key:
    if (key_val == Msg_KeyNull)
    {
        key_val = NO_KEY;
    }
    if (key_val != oldkey) //如果本次扫描值不等于上次按键值
    {
        if (key_val == tmpkey) //如果本次键值等于前次扫描值
        {
            tmp_count++; //累加扫描次数
            if (tmp_count >= 2) //扫描三次，是同一键值，确定有按键按下
            {
               if (key_val != NO_KEY) //如果本次扫描捕获键值不为空
                {
                    oldkey = tmpkey;
                    wait_keyup = 1;
					PutSysMsg(key_map[oldkey - 1]);
                }
                else //本次键值为NO_KEY
                {
                    if (IR_KEY)
                    {
                        if (tmp_count == 14)
                        {
                            wait_keyup = 0;
                            tmp_count = 0; //清扫描次数
                            PutSysMsg(Msg_KeyShortUp); //发keyup消息
                            IR_KEY = 0;
                            hold_count = 0;
                            oldkey = NO_KEY;
                            tmpkey = NO_KEY;
                        }
                    }
                    else
                    {
                        if (wait_keyup) //等待KEYUP
                        {
                            wait_keyup = 0;
                            tmp_count = 0; //清扫描次数
                            PutSysMsg(Msg_KeyShortUp); //发keyup消息
                            hold_count = 0;
                            oldkey = NO_KEY;
                            tmpkey = NO_KEY;
                        }

                    }
                }
            }
        }
        else //如果本次扫描值不等于前次扫描值，则重置键值，重新计数
        {
            tmpkey = key_val;
            //if (!wait_keyup)
            {
                tmp_count = 0;
            }
        }
    }
    else //如果本次键值等于上次按键值,累计16次(320ms)，发一次按键消息
    {
        if (key_val != NO_KEY) //
        {
            hold_count++;

            if (hold_count == 12)
            {
                hold_count = 0;
                PutSysMsg(key_map[oldkey - 1]); //发键值消息
            }
        }
        else
        {
            tmpkey = key_val;
            hold_count = 0;
            if (!wait_keyup)
            {
                tmp_count = 0;
            }
        }
    }
    SFR_BANK = sfr_bak;
}

void check_bat_status(void)
{
#if 0
    uint8 sfr_bak;
    uint8 batval;
    sfr_bak = SFR_BANK;
    //如果有DC5V,则不需要判断低电状态
    SFR_BANK = BANK_PMU;
    batval = BATADC_DATA & 0x7f;
    SFR_BANK = sfr_bak;
    if(batval <= bat_vol[1])
    {
        //        onoff_flag = !onoff_flag;
        TheCharge = 0;
        if(batval <= bat_vol[0])
        {
            Low_power_count++;
        }
        else
        {
            Low_power_count = 0;
        }
        if (Low_power_count > 0x10)
        {
            PutSysMsg(MSG_LOW_POWER); //send 2-hz msg
        }
    }
    else if(batval >= bat_vol[2])
    {
        //        onoff_flag = 0;
        Low_power_count = 0;
        TheCharge = 2;
    }
    else
    {
        //        onoff_flag = 1;
        Low_power_count = 0;
        TheCharge = 1;
    }
    //    switch_red_light(onoff_flag);
#endif
}

uint8 get_usbcable_status(void)
{
#if 1
    uint8 sfr_bak, dpdm_status;
    uint8 usb_status;

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
        if((USBSTATUS & 0x18) != 0x18) //软件去抖
        {
            usb_status = 0x40;
        }
        else
        {
            usb_status = 0x00;
        }
    }
    else
    {
        usb_status = 0x00;
    }
    DPDMCTRL = dpdm_status;

    SFR_BANK = BANK_PMU;
    usb_status |= (TEST_CTL & 0x80);

    SFR_BANK = sfr_bak;
    return usb_status;
#else
    uint8 sfr_bak;
    cmd = cmd;
    bool dpdm_status;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;
    dpdm_status = DPDMCTRL & 0x40;
    SFR_BANK = BANK_PMU;
    dpdm_status |= (TEST_CTL & (uint8)0x80);
    SFR_BANK = sfr_bak;
    return dpdm_status;
#endif
}
uint8 drv_check_plug_device_msg(uint8 api_no, uint8 *cur_status, Plug_Device_t plug_dev_type)
{
#if 0
    uint8 status, utest_status;
    uint8 sfr_bak;
    api_no = api_no;
    sfr_bak = SFR_BANK;
    if(plug_dev_type == LINEIN)
    {
		#if 0
        SFR_BANK = BANK_PMU;
        status = LRADC1_DATA & 0x3f;
        SFR_BANK = sfr_bak;
        //过渡处于按键区域的ADC值
        if(status <= adctab[MAX_KEYVAL - 2])
        {
            return AP_MSG_RTC;
        }
        if ((status > adctab[MAX_KEYVAL - 2]) && (status <= adctab[MAX_KEYVAL - 1]))
        {
            status = 0;//说明linein线存在
        }
        else
        {
            status = 1;
        }
		#endif
    }
    else if(plug_dev_type == MIC_IN)
    {
        //如果该GPIO口一直都是高，说明没有mic线在，如果为低，说明有mic线在。
        //   SFR_BANK = BANK_GPIO;
        // status = MIC_IN_GPIO_DAT & (1 << MIC_IN_GPIO_NUM);
        //    status=1;
        //    SFR_BANK = sfr_bak;
    }
    else if(plug_dev_type == USB_IN)
    {
        SFR_BANK = BANK_USB;
        utest_status = Usbien_hcusbien;
        SFR_BANK = sfr_bak;
        //usb处utest状态时，不进行udisk状态检测
        if (utest_status == 0xb9)
        {
            return AP_MSG_RTC;
        }
        status =0;//get_usbcable_status();// GetUsbCableStatus();
        //1)如果检测到USB线，则先进udisk
        //2)如果没有接usb线，先检测uhost,再检测卡
        //3)即无uhost也无卡则进FM
        if (((status & 0x40) == 0x40))
        {
            status = 0;
        }
        else
        {
            status = 1;
        }
    }
    else
    {
        return AP_MSG_RTC;
    }
    //Line线已经接上，并且检测到status被置起来了，则说明LINEIN线或MIC线已经拔掉
    if ((*cur_status == 1) && (status != 0))
    {
        *cur_status = 0;
        return pull_out_msg[plug_dev_type];
    } //Line线没有接上，并且检测到status被清零，则说明LINEIN线或MIC线插入，需要切换到linein或MIC功能
    else if ((*cur_status == 0) && (status == 0))
    {
        *cur_status = 1;
        return insert_msg[plug_dev_type];
    }//Line线没有接上，并且检测到status被置起来了，则说明不存在LINEIN线或MIC线
    else if ((*cur_status == 0) && (status != 0))
    {
        return AP_MSG_RTC;
    }
    else //Line线已经接上，并且检测到status被清零，则说明LINEIN线或MIC线一直存在
    {
        return AP_MSG_RTC;
    }
#else
	return AP_MSG_RTC;
#endif
}

//cts isr, send	2hz msg
void ctc_isr(void)
{
    //send 2-hz msg
    uint8 sfr_bak;

    PutSysMsg(Msg_HalfSecond); //send 2-hz msg
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_RTC;

    CTCCTL |= 1 << 7; //enable ctc
    SFR_BANK = sfr_bak;
    //check_bat_status();
}

void adjust_key_sequence()
{
	adctab[0] = KEY_PLAY_TH;
	if (KEY_VOLSUB_TH < KEY_VOLADD_TH)
	{
		adctab[1] = KEY_VOLSUB_TH;
		adctab[2] = KEY_VOLADD_TH;
	}
	else
	{
		adctab[1] = KEY_VOLADD_TH;
		adctab[2] = KEY_VOLSUB_TH;
	}

#ifdef SUPPORT_APPLE_KEY
	adctab_A[0] = KEY_PLAY_TH_A;
	if (KEY_VOLSUB_TH_A < KEY_VOLADD_TH_A)
	{
		adctab_A[1] = KEY_VOLSUB_TH_A;
		adctab_A[2] = KEY_VOLADD_TH_A;
	}
	else
	{
		adctab_A[1] = KEY_VOLADD_TH_A;
		adctab_A[2] = KEY_VOLSUB_TH_A;
	}
#endif
}
