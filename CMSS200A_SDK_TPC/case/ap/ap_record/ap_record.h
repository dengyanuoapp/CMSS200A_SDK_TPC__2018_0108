/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main header file.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_record.h,v 1.5 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */

#ifndef AP_RECORD_H
#define AP_RECORD_H

#include "actos.h"
#include "ap_common.h"
#include "enhanced.h"
#include "basal.h"
#include "mmcmd.h"//模块层命令名定义
//#define	 TEST_HOST	1

//message
#define  RESULT_USER_STOP        (RESULT_USER1+1)    //用户按键结束
#define  RESULT_LOW_POWER_STOP   (RESULT_USER1+2)    //低电压结束
#define  RESULT_DIR_CHANGED      (RESULT_USER1+3)
#define  RESULT_TRACKED          (RESULT_USER1+5)    //分曲退出结果
//event
#define  AP_EVENT_CODEC_ERR    AP_EVENT_9    //codec 出错
//main(int param) 定义入口参数的值
#define  NON_FMREC      0                     //非FM录音,此时录音源由g_mrecord_vars.rec_source决定
#define  MICREC_START   1                   //RESULT_RECORD         //非FM进程下,按REC键直接进入MIC录音.
#define  FMREC_START    RESULT_FMREC_START    //FM录音.FM进程收听电台时按热键REC进行的FM录音
#define  FMREC_NOSTART  RESULT_FMREC_NOSTART  //FM录音.FM进程收听电台时选择"电台录音"菜单进行的FM录音
#define  FMREC_AUSTART  RESULT_AUTORECORD_START

//录音源常量定义(全局变量:g_rec_from会用到!)
#define  R_SPDIF          0x80
#define  R_FM             0x40
#define  R_LINEIN         0x20
#define  R_MIC            0x10

//defines
#define SPACE_LOW           150L          // exFAT Support //60L    //剩余扇区数
#define LOW_POWER_COUNT     4      //停止录音时低电压发生的次数
//#define FPGA_DEBUG_ADC


/* 录音格式枚举*/
typedef enum
{
    RECORD_FILE_TYPE_WAV = 0, RECORD_FILE_TYPE_MP2
} ap_rectype_e;

/* 录音采样率枚举*/
typedef enum
{
    AP_FS_8K = 0, AP_FS_12K, AP_FS_32K, AP_FS_48K
} ap_samplerate_e;

/* 录音比特率枚举*/
typedef enum
{
    AP_BT_32K = 0, AP_BT_64K, AP_BT_128K, AP_BT_192K, AP_BT_256K, AP_BT_384K
    /* FPGA 阶段, ADPCM 双声道为384KBPS */
} ap_bitrate_e;

/* 录音源枚举*/
typedef enum
{
    AP_MICIN = 0, AP_LINEIN, AP_FMIN
} ap_inputSelect_e;

/* 录音VRAM 数据区备份结构*/
typedef struct
{
    uint16 maigc; //用来判断vm里的数据是否有效
    file_location_t location;
    uint16 wav_num; //当前录音文件的编号
    uint16 mp2_num; //当前录音mp2文件编号

    /* 根据录音模式,  录音格式以及wav 编码方式确定FS,  Bitrate 及channel num */
    uint8 rec_mode; //0:  长时录音  1:  普通录音  2:  优质录音
    wav_encmode_t rec_encode; //wav 格式编码方式,  ADPCM  or PCM
    ap_rectype_e rec_type; //录音格式
    ap_inputSelect_e rec_source; //当前设置的录音源
    //uint8 channelnum;
    //uint8 Rec_Fsrate; //采样率
    //uint8 rec_bitrate; //0/1/2/3/4/5:32/48/64/96/128/192bps.(通过BitRate_Tab[]转换成module层的接口参数)

    uint8 rec_InputGain;//5
    uint8 rec_ADCGain;//3
    uint8 rec_DSPGain;//0

    uint8 rec_DelayTime;//15 * 20ms per
    //uint8 rec_AVREnable;//1 voice control ,0 turn off
    //uint8 rec_AVRDelay;//3
    //uint16 rec_AVRLevel;//700
} record_vars_t;

typedef struct
{
    uint16 hour;
    uint8 minute;
    uint8 second;
} time_t_ext;

typedef enum
{
    recording, wait_to_save, saving
} status_t;
typedef enum
{
    rec_stop, rec_start, rec_pause
} rec_status_t;

typedef enum
{
    RecStop_UI, RecOn_UI, RecPause_UI
} UI_Type_rec_t;

typedef uint16 (*MenuCallBack)(void);

//global
extern record_vars_t g_record_vars;
//extern fmrecord_vars_t fm_record_vars;
extern audio_format_t Rec_format;
extern wav_rec_status_t rec_stat; //录音状态
extern uint32 total_sec;
extern bool isFmRecord;
extern uint8 avrEnableSave;
extern ap_inputSelect_e inputSelSave;

extern uint8 g_file_num; //录音文件总个数
//extern uint8 g_total_time[];          //00:00:00
extern uint8 g_now_time[]; //00:00:00
extern time_t_ext g_total_time; //总时间显示
extern time_t g_rec_time; //实时显示录音的时间
extern uint8 g_file_name_time[16]; //"WAV01 2004/01/01"

extern uint32 g_free_space; //剩余磁盘空间
extern uint32 g_file_size; //当前录音文件的page数
extern uint16 g_rec_event;

extern resource_t inputSelect;

//extern const char BitRate_Tab[];
extern uint8 currentBitrate[4];

//extern char g_noise_level;      //for vox recording
//extern char FMAutoRecord_flag;
extern BOOL show_time;
//extern BOOL	unicodeFlagofFolder;

//extern const char rec_Fsrate[4]; //= {8,8,32,32};
//extern const char rec_FsrateCodec_WAV[4]; //= {_FS_8K,_FS_8K/*,_FS_12K,_FS_16K,_FS_24K,_FS_32K*/,_FS_32K,_FS_32K};
//extern const uint8 voice_ext[];
extern uint8 g_disk_char[];

extern const uint8 rec_source_tab[];//{ R_LINEIN, R_SPDIF, R_MIC };

extern const uint16 stop_string[];// = {RSTOP1, RECORDFILETYPE,RECORDSETTING, CHANGEVO};
extern const uint16 stop_string_card[];// = {RSTOP1, MSTOP2, RECORDFILETYPE, RECORDSETTING,CHANGEVO};

extern const uint8 rec_bitrate_sector_ratio[];
/* 录音比特率*/
extern ap_bitrate_e rec_bitrate;
/* 录音采样率*/
extern ap_samplerate_e Rec_Fsrate;
/* 录音通道数*/
extern channelnum_t rec_channel;

//extern uint8 dir_name[30];

extern bool need_fetch_space;

extern bool card_exist;
extern bool UDisk_exist;

/* 所设置的录音容量: 扇区为单位 */
extern DLL_IMP uint32 RecSpare;
/* 配置的可录扇区数 */
extern uint32 total_space_setting;

/* 单次进入录音，累计已录扇区数 */
extern uint32 total_space_used;

/* 单个文件的扇区数 */
extern uint32 onefile_space;
extern uint8 rec_status;
extern uint8 need_change_rec_status;
extern uint8 review_disk;
/* ADPCM 三种模式下比特率配置*/
extern const ap_bitrate_e bitrate_ap_adpcm[3];

/* PCM 三种模式下比特率配置*/
extern const ap_bitrate_e bitrate_ap_pcm[3];

/* MP2 三种模式下比特率配置*/
extern const ap_bitrate_e bitrate_ap_mp2[3];
extern const uint8 *g_dir_voice;
extern bool overflag;
uint8 rec_init(void);
BOOL CheckStoreOK(void);
uint8 deal_mode_msg(void);
uint8 StartMP2Record(void);
uint8 StartWAVRecord(void);

uint16 show_stop(BOOL auto_start);
uint16 show_pause(void);
uint16 do_rec(void);
void stop_rec(uint8 get_status);

//extern void make_dirname(void);
extern void get_rec_source(void);
extern void parse_total_time(void);
extern void parse_total_num(void);
//extern void Open_RecRoute(void);

extern BOOL reset_record_dir(bool need_init);

//extern uint16 Show_AVRLevel_Menu(void);
//extern uint16 Show_AVRDelay_Menu(void);
extern void confirm_rec_param(void);

extern bool recSendCommand(BYTE cmd, void *param);

/* main.c */
uint16 show_error(uint16 msg_id);
uint16 show_msg(uint16 msg_id);
BOOL SetCard();
BOOL SetUDisk();

/* show_rec.c */
void ui_draw(void);
uint8 time_draw(uint8 mode);
BOOL is_lowbatt(uint8 batt);
uint8 show_rec(void);
uint8 show_err_msg(void);
uint8 deal_clock_report(void);
void enable_adda(void);
void disable_adda(void);

extern void decAddainit(adin_t ain);
extern void decAddaexit(void);
#endif  /* AP_RECORD_H*/
