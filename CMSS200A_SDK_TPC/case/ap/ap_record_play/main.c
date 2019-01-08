/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: main.c,v 1.10 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */
#include "ap_record_play.h"
#include "uhdrv.h"

#pragma name(AP_MAIN)
#define DISK_C 0
#define DISK_H 1

/********************常量定义***********************/

/******************全局变量定义*********************/

//保留项
//uint8 DiskFlag = 0;
//uint16 near g_rec_event = 0;

bool show_time = TRUE;

/*  当前目录下录音文件总个数*/
uint8 near g_file_num;

/* 可录总秒数*/
uint32 total_sec = 0;

/* 配置的可录扇区数 */
uint32 total_space_setting = 0;

/* 单次进入录音，累计已录扇区数 */
uint32 total_space_used = 0;

/* 单个文件的扇区数 */
uint32 onefile_space = 0;
uint16 near g_rec_event = 0;

uint32 near g_free_space; //剩余磁盘空间//remaining disk space

/* 当前录音文件的page数*/
//uint8 near g_total_time[11]; //000:00:00
uint8 near g_now_time[10]; //000:00:00
uint8 near g_file_name_time[16]; //"WAV001 04/01/01"

/* 当前录音比特率ASCILL , 用于显示*/
uint8 near currentBitrate[4];

record_vars_t near g_record_vars;
//uint8 near g_rec_from; //当前的录音源 by ccm

/*  可录音总时间*/
time_t_ext near g_total_time;
/*  实时显示录音的时间*/
time_t near g_rec_time;
/* 录音状态*/
wav_rec_status_t rec_stat;

//resource_t inputSelect =
//{ 0 };

/* 录音参数结构*/
audio_format_t Rec_format;
//bool unicodeFlagofFolder;

/* 是否FM  录音*/
bool isFmRecord;
/* 空间是否已满*/
bool overflag;
uint8 avrEnableSave;
//ap_inputSelect_e inputSelSave;

//uint8 dir_name[30];

/* 录音比特率*/
ap_bitrate_e rec_bitrate;
/* 录音采样率*/
ap_samplerate_e Rec_Fsrate;
/* 录音通道数*/
channelnum_t rec_channel;
/* 指向当前默认录音目录*/
const uint8 *g_dir_voice;

/* 磁盘状态bitmap */
uint8 g_DiskStateFlag = 0;

/* 是否需要取剩余容量 */
bool need_fetch_space;

bool card_exist = FALSE;
bool UDisk_exist = FALSE;
uint8 rec_status;//当前录音状态。0-停止状态;1-录音状态;2-暂停状态
uint8 need_change_rec_status;//是否需要转换录音状态标志

//能量检测配置值
uint32 Mval = 200000;//阈值，建议值1024
uint32 Mblock = 38;//建议值60，接近1秒  Mblock=(time*256000/8)/512

uint8 g_disk_char[MAX_DISK_NUM] =
{ 'U', 'H', 'M'};

const uint8 rec_str[4] =
{ "REC" };
const uint8 nodevice_str[5] =
{ "NDEV" };
const uint8 err_str[4] =
{ "ERR" };

//#pragma renamecode(AP_MAIN)


/*
 ********************************************************************************
 * Description : 使能DAC
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near openDAC_main(void)
{
    dac_t dacattr;
    dacattr.dacselect = 0;
    dacattr.rate = FS_44K1;//默认
    EnableDAC(&dacattr);

}

/*
 ********************************************************************************
 * Description : 关闭录音通道
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near Close_RecRoute(void)
{
    if (!g_comval.KeyTone)
    {

    }
}

/*
 ********************************************************************************
 * Description : Record 应用参数初始化
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near read_var(void)
{
    BOOL result;
    result = read_ap_comval();

    {
        memset(&g_record_vars, 0, sizeof(g_record_vars));
        g_record_vars.maigc = MAGIC_RECORD;
        g_record_vars.location.disk = 0;
        g_record_vars.rec_source = AP_MICIN;        //默认选择MIC录音

        g_record_vars.rec_type = RECORD_FILE_TYPE_WAV;
        g_record_vars.rec_encode = ENCODE_WAV_PCM;
        g_record_vars.rec_mode = 2;

        g_record_vars.wav_num = 1;
        g_record_vars.mp2_num = 1;
        g_record_vars.rec_InputGain = g_comval.RecordGain;
        g_record_vars.rec_ADCGain = 3;
        g_record_vars.rec_DSPGain = 0;
        g_record_vars.rec_DelayTime = 15;
    }
}


/*
 ********************************************************************************
 * Description : Record 应用入口主函数
 *
 * Arguments   :
 param:
 0, 不自动开始录音
 1, 自动开始录音
 RESULT_FMREC_NOSTART,  FM录音,   不自动开始
 RESULT_FMREC_START,  FM录音, 自动开始
 *
 *
 * Returns     : 结果消息或0
 *
 * Notes       :
 *
 ********************************************************************************
 */
int main(int param)
{
    BOOL mc_result;
    BOOL ReturnToFm = 0;
    uint8 result = 0;
    uint8 sfr_bank;
    overflag = FALSE;

    SetPLL(PLL_48MHZ);

    need_fetch_space = TRUE;
    read_var();

    LEDClearScreen();
    LEDPuts(NUMBER2, rec_str, 1);

    /* 配置只支持NOR FLASH */
    if(g_comval.SuppNorRec == 1)
    {
        g_record_vars.location.disk = 'M';
    }
    else
    {
        if(USBH_Check_Udisk())
        {
            g_record_vars.location.disk = 'U';
        }
        else if (DRV_DetectUD(1) == 0x20)
        {
            g_record_vars.location.disk = 'H';
        }
    }

    /* 没有设备 */
    if(g_record_vars.location.disk == 0)
    {
        LEDPuts(NUMBER1, nodevice_str, 1);
        return RESULT_NULL;
    }

    set_dac_volume(g_comval.volume);

    while(1)
    {
        enable_adda();
        result = show_rec();
        if((result != 0) && (result != RESULT_CURRENT_AP))
        {
            break;
        }
        disable_adda();

        result = ui_play();
        if((result != 0) && (result != RESULT_CURRENT_AP))
        {
            break;
        }
    }

    if((g_record_vars.location.disk == 'H') || (g_record_vars.location.disk == 'U'))
    {
        DeleteRecFile();
    }

    SetPLL(PLL_48MHZ);

    mc_result = VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));

    if (result == RESULT_MAIN)
    {
        if (param != RESULT_FMREC_START)
        {
            result = RESULT_NULL;
        }
        else
        {
            result = RESULT_FMREC2FM;
        }
    }
    sfr_bank = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    MRCR2 &= 0xdf;
    SFR_BANK = sfr_bank;

    return result;
}
void enable_adda(void)
{
    if(g_record_vars.rec_source != AP_FMIN)
    {
        return;
    }
    decAddainit(AD_FM);
    set_dac_volume(g_comval.volume);
}
void disable_adda(void)
{
    if(g_record_vars.rec_source != AP_FMIN)
    {
        return;
    }
    decAddaexit();
}

uint8 show_err_msg(void)
{
    LEDClearScreen();
    LEDPuts(NUMBER2, err_str, 1);
    ap_sleep(4);
    return RESULT_MAIN;
}

