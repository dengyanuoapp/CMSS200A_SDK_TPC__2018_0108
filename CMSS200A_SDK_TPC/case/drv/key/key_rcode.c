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
uint8 TheCharge; //����ʶ��0,�͵磻1������У�2.���硣3.��DC5V
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

//��ȡLRADC1ֵ�������жϵ͵������
//�͵磺С��3.3V    ��Լ����0X50��С�ڵ������ֵ��Ϊ�͵�
//�ӽ��͵���ʾ:С��3.4V,Լ����0x54
//����4.2V   Լ����0X76�����ڵ������ֵ��Ϊ����
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
    //���on/off����
    SFR_BANK = BANK_PMU;
    if ((SYSTEM_ONOFF & 0x08) != 0)
    {
		key_val = 1;		
        goto get_key;
    }
#endif

#ifdef EVB_KEY
	SFR_BANK = BANK_PMU;
    //���LRADC1(sem_con)
    lradc1_data = LRADC1_DATA & 0x3f;
    lradc5_data = LRADC5_DATA & 0x3f;
    //�Ż����LRADC1,���lRADC1�а����������ټ��LRADC5
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
    if (key_val != oldkey) //�������ɨ��ֵ�������ϴΰ���ֵ
    {
        if (key_val == tmpkey) //������μ�ֵ����ǰ��ɨ��ֵ
        {
            tmp_count++; //�ۼ�ɨ�����
            if (tmp_count >= 2) //ɨ�����Σ���ͬһ��ֵ��ȷ���а�������
            {
               if (key_val != NO_KEY) //�������ɨ�貶���ֵ��Ϊ��
                {
                    oldkey = tmpkey;
                    wait_keyup = 1;
					PutSysMsg(key_map[oldkey - 1]);
                }
                else //���μ�ֵΪNO_KEY
                {
                    if (IR_KEY)
                    {
                        if (tmp_count == 14)
                        {
                            wait_keyup = 0;
                            tmp_count = 0; //��ɨ�����
                            PutSysMsg(Msg_KeyShortUp); //��keyup��Ϣ
                            IR_KEY = 0;
                            hold_count = 0;
                            oldkey = NO_KEY;
                            tmpkey = NO_KEY;
                        }
                    }
                    else
                    {
                        if (wait_keyup) //�ȴ�KEYUP
                        {
                            wait_keyup = 0;
                            tmp_count = 0; //��ɨ�����
                            PutSysMsg(Msg_KeyShortUp); //��keyup��Ϣ
                            hold_count = 0;
                            oldkey = NO_KEY;
                            tmpkey = NO_KEY;
                        }

                    }
                }
            }
        }
        else //�������ɨ��ֵ������ǰ��ɨ��ֵ�������ü�ֵ�����¼���
        {
            tmpkey = key_val;
            //if (!wait_keyup)
            {
                tmp_count = 0;
            }
        }
    }
    else //������μ�ֵ�����ϴΰ���ֵ,�ۼ�16��(320ms)����һ�ΰ�����Ϣ
    {
        if (key_val != NO_KEY) //
        {
            hold_count++;

            if (hold_count == 12)
            {
                hold_count = 0;
                PutSysMsg(key_map[oldkey - 1]); //����ֵ��Ϣ
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
    //�����DC5V,����Ҫ�жϵ͵�״̬
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
        if((USBSTATUS & 0x18) != 0x18) //���ȥ��
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
        //���ɴ��ڰ��������ADCֵ
        if(status <= adctab[MAX_KEYVAL - 2])
        {
            return AP_MSG_RTC;
        }
        if ((status > adctab[MAX_KEYVAL - 2]) && (status <= adctab[MAX_KEYVAL - 1]))
        {
            status = 0;//˵��linein�ߴ���
        }
        else
        {
            status = 1;
        }
		#endif
    }
    else if(plug_dev_type == MIC_IN)
    {
        //�����GPIO��һֱ���Ǹߣ�˵��û��mic���ڣ����Ϊ�ͣ�˵����mic���ڡ�
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
        //usb��utest״̬ʱ��������udisk״̬���
        if (utest_status == 0xb9)
        {
            return AP_MSG_RTC;
        }
        status =0;//get_usbcable_status();// GetUsbCableStatus();
        //1)�����⵽USB�ߣ����Ƚ�udisk
        //2)���û�н�usb�ߣ��ȼ��uhost,�ټ�⿨
        //3)����uhostҲ�޿����FM
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
    //Line���Ѿ����ϣ����Ҽ�⵽status���������ˣ���˵��LINEIN�߻�MIC���Ѿ��ε�
    if ((*cur_status == 1) && (status != 0))
    {
        *cur_status = 0;
        return pull_out_msg[plug_dev_type];
    } //Line��û�н��ϣ����Ҽ�⵽status�����㣬��˵��LINEIN�߻�MIC�߲��룬��Ҫ�л���linein��MIC����
    else if ((*cur_status == 0) && (status == 0))
    {
        *cur_status = 1;
        return insert_msg[plug_dev_type];
    }//Line��û�н��ϣ����Ҽ�⵽status���������ˣ���˵��������LINEIN�߻�MIC��
    else if ((*cur_status == 0) && (status != 0))
    {
        return AP_MSG_RTC;
    }
    else //Line���Ѿ����ϣ����Ҽ�⵽status�����㣬��˵��LINEIN�߻�MIC��һֱ����
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
