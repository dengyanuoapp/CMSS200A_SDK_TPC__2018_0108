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
#define  USOUND_CARD_ERROR        (RESULT_USER1+1)
#define  USOUND_SETDIR_ERROR   (RESULT_USER1+2)
#define  USOUND_CARD_FULL      (RESULT_USER1+3)
#define  USOUND_DIR_FULL          (RESULT_USER1+5)
#define  RESULT_CARD_OUT          (RESULT_USER1+6)
#define  RESULT_SAVE_FILE            (RESULT_USER1+7)
#define  USOUND_CREAT_FAIL       (RESULT_USER1+8)

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
#define SPACE_LOW           150L          // exFAT Support  //60L    //剩余扇区数
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
    AP_BT_32K = 0, AP_BT_64K, AP_BT_128K, AP_BT_192K, AP_BT_256K
} ap_bitrate_e;

/* 录音源枚举*/
typedef enum
{
    AP_MICIN = 0, AP_LINEIN, AP_FMIN
} ap_inputSelect_e;

//当前磁盘,目录,文件
typedef struct
{
    int8 disk; //'C','D','E','F'-internal, 'H','I','J','K'-externel
    int8 dirLayer;
    uint8 DirNo[8]; //dir no {1, 255, 36, 10....}
    int32 ClusterNo; //cluster no
    int32 DirEntry;
    uint8 path[12]; //8.3+'\0'
    uint8 filename[12]; //8.3+'\0'
} file_location_urec_t;

/* 录音VRAM 数据区备份结构*/
typedef struct
{
    uint16 maigc; //用来判断vm里的数据是否有效
    file_location_urec_t location;
    uint16 wav_num; //当前录音文件的编号
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

typedef uint16 (*MenuCallBack)(void);

//global
extern record_vars_t g_record_vars;
//extern fmrecord_vars_t fm_record_vars;
extern audio_format_t Rec_format;
extern function_menu_t menu;
extern wav_rec_status_t rec_stat; //录音状态
extern uint32 total_sec;

extern uint32 g_free_space; //剩余磁盘空间

extern resource_t inputSelect;

BOOL is_lowbatt(uint8 batt);
void ui_draw(void);
uint16 show_error(uint16 msg_id);
uint16 show_stop(BOOL auto_start);
uint16 show_rec(void);
uint16 show_pause(void);
uint16 do_rec(void);

extern void make_dirname(void);
extern void get_rec_source(void);
extern void parse_total_time(void);
extern void parse_total_num(void);
//extern void Open_RecRoute(void);
extern void DecodeOnePage(uint16 top, uint16 bottom, uint8 type, uint8 string_id);
extern BOOL SetCard();
extern BOOL SetUDisk();

extern BOOL reset_record_dir(void);

//extern uint16 Show_AVRLevel_Menu(void);
//extern uint16 Show_AVRDelay_Menu(void);

extern int16 do_pause(void);
extern void format_filename(void);
extern BOOL gen_filename(void);
extern int GenNewName_forUrec(file_location_urec_t *location);

#endif  /* AP_RECORD_H*/
