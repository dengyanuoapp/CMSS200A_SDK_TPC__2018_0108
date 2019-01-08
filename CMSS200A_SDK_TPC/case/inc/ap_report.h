/*
 *******************************************************************************
 *                        SDK30F_D1
 *                     music player header  file
 *
 *                (c) Copyright 2002, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File: ap_music.h
 * By  : rcmai
 * Version: 1> v1.00     first version     2004-3-19 8:28
 *******************************************************************************
 *******************************************************************************
 */
#ifndef REPORT_H
#define REPORT_H
#include "actos.h"//系统头文件
#include "mmcmd.h"//模块层命令名定义
#include "basal.h"//basal module 头文件
#include "enhanced.h"//enhanced module 头文件
#include "ap_common.h"//控件头文件
#include "uhdrv.h"
#include "report_res.h"

#include "fm_drv.h"

#define TIME_REPORT_EN          //时间播报使能定义
//#define DATE_REPORT_EN          //日期播报使能定义

#ifdef MAIN_AP
#define AP_MAIN_REPORT_EN       //主界面对应应用播报使能定义
#endif
#ifdef MUSIC_AP
#define AP_MUSIC_PEPORT_EN      //音乐播报使能定义
#define AP_MUSIC_NAME           //音乐歌曲名播报使能定义
#endif
#ifdef RADIO_AP
#define AP_RADIO_REPORT_EN      //收音播报使能定义
#endif
#ifdef LINEIN_AP
#define AP_LINEIN_REPORT_EN     //LINEIN中的播报使能定义
#endif
#ifdef USOUND_AP
#define AP_USOUND_REPORT_EN     //USB音箱中的播报使能定义
#endif

typedef void (*tts_func)(char *);

typedef struct
{
    char *report_buf;                       //存诸播报数据的指针，指向播报的数据。
    char tts_lib_flag;                      //当前TTS类型,1为标准库，0为词条
    char open_pa_flag;                      //是否打开PA的标志位
} info_report_t;

//data
extern BOOL report_stats;                   //当前播放状态  0:停止  1:播放
extern BOOL pa_close_flag;                  //播报前PA的状态
extern unsigned char ext_report_msg;    //返回的其它消息值。位图消息

//ext_report_msg 的位图信息
#define REPORT_SD_OUT   0X1
#define REPORT_UD_OUT   0X2
#define REPORT_SD_IN    0X4
#define REPORT_UD_IN    0X8

//report的信息
#define REPORT_ERROR    -1
#define REPORT_OK       0

//extern bool g_PAOpenFlag;

/*Function*/
void init_report_param(void);
unsigned char stop_report(void);
void start_report(info_report_t *info);
BOOL check_report_state(info_report_t *info);
uint8 wait_report_end(void *param);
void report_str_fix(char *str);
BOOL report_res(WORD id, char *str);
int report_res_tag(WORD id, void *param);
void conver_time_to_string(char *buffer);
void report_date(char *buffer);
int report_music_tag(char *report_buf, char cur_disk);
int convert_music_id3_string(char *g_TagInfoBuf, char type);
int convert_music_name_string(char *name);
int report_frequency(fm_status_t *fm_status, char *str);
int report_fix_info(char type, char pa_open_flag, void *param);
int report_tag(char *report_buf);
void conver_time_to_string(char *buffer);
BOOL check_report_state(info_report_t *info);
uint8 report_time_info(uint8 disk_char, uint8 pa_open_flag);
#endif
