/********************************************************************************
 *                              AS211A
 *                            Module: USB MAIN
 *                 Copyright(c) 2001-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>         <time>              <version >             <desc>
 *      wanghaijing      2011-1-28 15:27     1.0                build this file
 ********************************************************************************/
/*!
 * \file     usbdisk.c
 * \brief
 * \author   wanghaijing
 * \par      GENERAL DESCRIPTION:
 *               这里对文件进行描述
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2010 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2010-3-10
 *******************************************************************************/

#include "usound.h"
#include "ap_record.h"

void read_var(void);
//extern int ChangeVolume(void);
extern void key_timer_count(void);
extern void init_LRADC_CTL(void);
extern uint8 check_vbus(void);

#pragma name(AP_MAIN)
//=====================================================
extern uint8 g_PAOpenFlag;
//uint8 USB_Status;
uint8 max_gain; /*max_gain is Q4.4 format*/
ap_comval_t g_comval;
bool need_draw;
int16 g_result;
bool urec_en;
uint8 key_timer_id;
uint8 first_flag;
uint8 old_index;

#if 1
#if 0
const uint8 vol_value[49]= 
{
    0x00, 0x03, 0x06, 0x09, 0x10, 0x13, 0x16,
    0x19, 0x20, 0x23, 0x26, 0x29, 0x30, 0x33,
    0x36, 0x39, 0x40, 0x43, 0x46, 0x49, 0x50,
    0x53, 0x56, 0x59, 0x5B, 0x61, 0x63, 0x65,
    0x67, 0x69, 0x6B, 0x71, 0x73, 0x75, 0x77,
    0x79, 0x7B, 0x81, 0x83, 0x85, 0x87, 0x89,
    0x8B, 0x91, 0x93, 0x95, 0x97, 0x99, 0x9B,
};
#else
#if 0
const uint8 vol_value[17][2] =
{
    {0x69, 0x00}, {0x75, 0x00}, {0x80, 0x00}, {0x88, 0x00}, {0x93, 0x00}, {0x9B, 0x00}, {0x86, 0x01},
    {0x92, 0x01}, {0x99, 0x01}, {0x8B, 0x02}, {0x96, 0x02}, {0x92, 0x03}, {0x99, 0x03}, {0x96, 0x04},
    {0x94, 0x05}, {0x94, 0x06}, {0x9A, 0x06}/*{0x9B, 0x06}*/
};
#endif
const uint8 vol_value[17][2] =
{
    {0x69, 0x00}, {0x75, 0x00}, {0x80, 0x00}, {0x88, 0x00}, {0x93, 0x00}, {0x9B, 0x00}, {0x86, 0x01},
    {0x92, 0x01}, {0x99, 0x01}, {0x8B, 0x02}, {0x91, 0x02},{0x96, 0x02}, {0x92, 0x03}, {0x97, 0x03},
    {0x99, 0x03},{0x9A, 0x03},{0x93, 0x04}/*, {0x99, 0x04},{0x94, 0x05},*/ /*{0x94, 0x06}, {0x9A, 0x06}*/
};

#endif
#else
const uint8 vol_value[49][2] =
{
    {0x69, 0x00}, {0x6B, 0x00}, {0x72, 0x00}, {0x75, 0x00}, {0x77, 0x00}, {0x79, 0x00}, {0x80, 0x00},
    {0x83, 0x00}, {0x85, 0x00}, {0x88, 0x00}, {0x8A, 0x00}, {0x90, 0x00}, {0x93, 0x00}, {0x96, 0x00},
    {0x98, 0x00}, {0x9B, 0x00}, {0x81, 0x01}, {0x83, 0x01}, {0x86, 0x01}, {0x88, 0x01}, {0x8A, 0x01},
    {0x92, 0x01}, {0x94, 0x01}, {0x96, 0x01}, {0x99, 0x01}, {0x9B, 0x01}, {0x87, 0x02}, {0x8B, 0x02},
    {0x91, 0x02}, {0x93, 0x02}, {0x96, 0x02}, {0x99, 0x02}, {0x9B, 0x02}, {0x92, 0x03}, {0x94, 0x03},
    {0x96, 0x03}, {0x99, 0x03}, {0x9B, 0x03}, {0x93, 0x04}, {0x96, 0x04}, {0x99, 0x04}, {0x9B, 0x04},
    {0x94, 0x05}, {0x97, 0x05}, {0x99, 0x05}, {0x94, 0x06}, {0x96, 0x06}, {0x99, 0x06}, {0x9B, 0x06}
};

#endif


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

usound_param ud_param =
//{SAMPLE_96K, SAMPLE_48K, BIT_DEPTH16}; //下传98k, 上传48k, 位深24
//{SAMPLE_96K, SAMPLE_48K, BIT_DEPTH24}; //下传98k, 上传48k, 位深24
{SAMPLE_48K,SAMPLE_48K,BIT_DEPTH24}; //下传48k, 上传48k, 位深16

int16 main(int16 para)
{
    //uint8  key = 0;
    //BOOL  Audioloop;
    para++;
	first_flag = TRUE;
	
    //Audioloop = 1;
    need_draw = 1;
    max_gain = 0x10;
    g_PAOpenFlag = 0x00;
    //    LEDClearScreen();
    //    LEDPuts(NUMBER1, p_device_name, 1);
    //注销卡插拔检测定时器, 避免杂音
    //CardSetParam(0x01, 100, CARD_UNDEC);

    read_var();

    //    us_open_dac();
    //    us_open_pa();
   // SetPAVolume_LR(dac_vol[g_comval.volume], dac_vol[g_comval.volume]);
    init_LRADC_CTL();
//    TM_DelayMS(200);
//    TM_DelayMS(200);
//    TM_DelayMS(200);
//    TM_DelayMS(200);

start_check_usound:
    if(usound_init(&ud_param) == 0)  //AP初始化
    {
        usound_exit();
        //        DisablePA();
        //重新挂卡插拔检测定时器
        //CardSetParam(0x01, 100, CARD_DEC);
        return RESULT_NULL;//RESULT_MUSIC;
    }
    // ChangeVolume();	 //设置默认音量值
//	SetPAVolume_LR(dac_vol[g_comval.volume], dac_vol[g_comval.volume]);
    OutPutVolume(g_comval.volume);
    //~~~~~~~~~~~~~~~~~~~~~
    //usb sound loop
    //~~~~~~~~~~~~~~~~~~~~~
	key_timer_id =0xFF;// TM_SetTimer((uint32) key_timer_count, 4);
    g_result = Usound_loop();
	TM_KillTimer(key_timer_id);
    usound_exit();
    //  DisablePA();
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval)); //回写音量值等信息
    VMWrite(&g_record_vars, VM_AP_AMV, sizeof(record_vars_t));

	if(g_result==RESULT_MAIN)
	{
		goto start_check_usound;
	}
    //重新挂卡插拔检测定时器
    //CardSetParam(0x01, 100, CARD_DEC);
    if(g_result != RESULT_STANDBY)
    {
        g_result = RESULT_NULL;
    }

    return g_result;
}

void init_LRADC_CTL(void)
{
    uint8 sfr_bak;

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
	AD_Select1 &= 0xfc;
    AD_Select1 |= 0x01;

	// init GPIOA5, the state decide the mode of earphone detecting.
	GPIOAPUEN  |= 1<<5; 
	GPIOAINEN  |= 1<<5;
	GPIOADAT   |= 1<<5;
	GPIOAOUTEN &= ~(1<<5);	

	SFR_BANK = BANK_PMU;
	PMUADC_CTL |= 0x01<<5;
    SFR_BANK = sfr_bak;
}

void read_var(void)
{
    bool result;

    //read comval
    result = VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    //ASSERT(result);

    //Usound 录音所用VRAM区
    result = VMRead(&g_record_vars, VM_AP_AMV, sizeof(g_record_vars));
    if ((FALSE == result) || (g_record_vars.maigc != MAGIC_RECORD))
    {
        g_record_vars.maigc = MAGIC_RECORD;

        g_record_vars.location.disk = 'H';
        g_record_vars.rec_source = AP_LINEIN;

        g_record_vars.wav_num = 1;
        g_record_vars.rec_InputGain = g_comval.RecordGain;
        g_record_vars.rec_ADCGain = 3;
        g_record_vars.rec_DSPGain = 0;
        g_record_vars.rec_DelayTime = 15;

        //g_record_vars.rec_AVREnable = 0;
        //g_record_vars.rec_AVRDelay = 3;
        //g_record_vars.rec_AVRLevel = 5;//3400;
    }
}

void us_open_dac(void)
{
    dac_t dac_attr;

    dac_attr.dacselect = 0;
    dac_attr.rate = FS_44K1;
    EnableDAC(&dac_attr);
}

void us_open_pa(void)
{
    pa_t pa_attr;

    if (g_PAOpenFlag == 0x00)
    {
        pa_attr.pa_in.dacin = 1;
        pa_attr.pa_in.fmin = 0;
        pa_attr.pa_in.linein = 0;
        pa_attr.pa_in.micin = 0;
        pa_attr.volume = 0;
        EnablePA(&pa_attr);
        g_PAOpenFlag = 0x01;
    }
    //SetPAVolume(32);
}

/*===========================================
 functions: OutPutVolume(int vol)
 input:    int vol: 要输出的音量
 output:  void
 功能： 把音量值送到IO口，
 ============================================*/
void OutPutVolume(uint8 vol)
{
    uint8 temp;
    uint8 sfr_bak;

    temp = vol;

    if (temp > g_comval.VolumeMax)
    {
        temp = g_comval.VolumeMax;
    }
    else if (temp < 0)
    {
        temp = 0;
    }
    //    SetPAVolume_LR(temp, temp);
    SetPAVolume_LR(dac_vol[temp], dac_vol[temp]);

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_AUIP;
    if(temp != 0)
    {
        DAC_ANALOG0 |= 0x01; //dismute,connect
    }
    else
    {
        DAC_ANALOG0 &= 0xfe; //mute dac
    }
    SFR_BANK = sfr_bak;
}

void set_vol(uint8 index)
{
	uint8 dac_vol;
	uint8 pa_vol;
    uint8 sfr_bak;
	
    sfr_bak  = SFR_BANK;				
	SFR_BANK = BANK_DAC_ADC;
		
#if 1
	dac_vol = vol_value[index][0];
	pa_vol = vol_value[index][1]; 
	
	PA_VOLUME = (PA_VOLUME & 0xf8) | (pa_vol & 0x07);
	DAC_VOLUME0 = dac_vol;
	DAC_VOLUME1 = dac_vol;
#else
	dac_vol = vol_value[index];
	DAC_VOLUME0 = dac_vol;
	DAC_VOLUME1 = dac_vol;
#endif

	SFR_BANK = sfr_bak;
}

void change_vol(uint8 index)
{
    uint8 sfr_bak;
//	uint8 i, j;

    sfr_bak  = SFR_BANK;											
    SFR_BANK = BANK_AUIP;

    if (index != 0)
    {
        DAC_ANALOG0 |= 0x01; //dismute
    }
    else
    {
        DAC_ANALOG0 &= 0xfe; //mute 
    }

#if 0
	if (first_flag == TRUE)
	{
		first_flag = FALSE;
		i = index * 3;
		set_vol(i);
	}
	else
	{
		if (index > old_index)
		{	// increase
			for (j=3; j>0; j--)
			{
				i = index * 3 - (j - 1);
				set_vol(i);
				TM_DelayMS(50);
			}
		}
		else if (index < old_index)
		{	// decrease
			for (j=3; j>0; j--)
			{
				i = index * 3 + (j - 1);
				set_vol(i);
				TM_DelayMS(50);
			}			
		}
		else
		{
			;
		}
		
	}	
	old_index = index;
#else
	set_vol(index);
#endif

	SFR_BANK = sfr_bak;
}
