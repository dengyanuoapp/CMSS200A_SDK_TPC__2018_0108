/*
 ********************************************************************************
 *                       ACTOS
 *                  ap_radio head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : ap_radio.h
 * By     : mike
 * Version: 1> v1.00     first version     2004-1-12 13:45
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _APRADIO_H
#define _APRADIO_H
#include "actos.h"
#include "ap_common.h"
#include "filesys.h"
#include "basal.h"
#include "stringH.h"
//#include "fmradio.h"
#include "fm_drv.h"
#include "ap_report.h"
#include "eqtool.h"

//硬件最大音量
#define  MAXVOLUME		    40
//默认搜台门限定义
#define  SEARCHLEVEL        4
//电台列表中支持的最大保存数
#define  MAX_STATION_COUNT  40

/* 各波段频率范围定义,以100khz为单位 */
#define  USFREQ_MAX         0x0438               //108000khz
#define  USFREQ_MIN         0x036B               //87500khz
#define  JPFREQ_MAX         0x0384               //90000khz
#define  JPFREQ_MIN         0x02F8               //76000khz
/* 城市电台列表频率范围( 中国 ) */
#define  CITYFREQ_MAX       0x0438             //108000khz
#define  CITYFREQ_MIN       0x036B            //87500khz          0x036B = 875
typedef enum
{
    FM_TO_RADIO = 0, //  0, 进入FM,进行电台收听
    ALARM_TO_FM, //  1, Alarm进入FM
    FM_TO_STANDBY,
    //  2, 使FM进入Standby
} fm_param_e;

typedef enum
{
    SEARCH_NULL = 0, //不搜索
    SEARCH_NEXT = 10, //next方向搜索
    SEARCH_LAST = 20, //last方向搜索
    SEARCH_STOP = 50,
    //搜索终止
} FM_search_direct_e; //手动搜台(源程序是用此值，目前不知道作用，先保留)


/* RDS 功能开关枚举 */
typedef enum
{
    RDS_Close, //RDS 关
    RDS_Open
    //RDS 开
} radio_RDS_e;

typedef struct
{
    uint16 FM_CurrentFreq; //当前频率
    FM_Audio_e FM_Stereo_Status; //0，立体声；非0，单声道
} FMStatus_Tab_t;

/* radio ui配置参数 */
typedef struct
{
    /* 魔术数*/
    uint16 magic;
    /* 预设列表( 普通频段)*/
    uint16 fmstation_us[MAX_STATION_COUNT];
    /* 预设列表( 日本频段)*/
    uint16 fmstation_jp[MAX_STATION_COUNT];
    /* 当前播放的电台信息*/
    fm_status_t FMStatus;
    /* 当前播放波段, Band_MAX 表示当前播放为城市电台*/
    radio_band_e band_mode;
    /* 是否使能RDS功能开关 */
    radio_RDS_e RDS_fun;
} radio_config_t;

/* radio 城市电台结构 */
typedef struct
{
    /* 当前城市电台频点*/
    uint16 fmstation_city;
    /* 当前播放的城市电台名称 */
    char station_name[40];
} radio_citylist_t;

/* FM菜单项	 */
typedef enum
{
    MENU_SAVE, /* 保存电台*/
    MENU_RECORD, /* 电台录音*/
    MENU_DELETE, /* 删除电台*/
    MENU_DELETE_ALL, /* 删除全部*/
    MENU_AUTO_SEARCH, /* 自动搜台*/
    MENU_NORMAL_BAND, /* 普通频段*/
    MENU_JAPAN_BAND, /* 日本频段*/
    MENU_RDS, /* RDS 功能*/

    MENU_FM_RECORD, //for FM Record by ccm

    MENU_EXIT, /* 退出*/

    MENU_COUNT,
    //FM菜单项数目
} radio_menu_e;

/* 菜单函数指针类型 */
typedef uint16 (*pfunc_menu)(uint16 *);

/* 菜单函数指针组成的结构体 */
typedef struct
{
    pfunc_menu menu_save; /* 保存电台*/
    pfunc_menu menu_record; /* 电台录音*/
    pfunc_menu menu_delete; /* 删除电台*/
    pfunc_menu menu_delete_all; /* 删除全部*/
    pfunc_menu menu_auto_search; /* 自动搜台*/
    pfunc_menu menu_normal_band; /* 普通频段*/
    pfunc_menu menu_japan_band; /* 日本频段*/
    pfunc_menu menu_rds; /* RDS 功能*/
    pfunc_menu menu_fm_record; //for FM Record by ccm
    pfunc_menu menu_exit; /* 退出*/
} radio_menufunc_t;

/* FM命令函数指针类型 */
typedef uint8 (*pfunc_cmd)(void *, void *);

/* FM命令选项 */
typedef enum
{
    /* FM 中间层命令宏, 部分涉及到调用FM驱动接口 */
    FM_OPEN = 0, //安装fm驱动, 进行模组初始化
    FM_CLOSE, //模组进入standby, 并卸载fm驱动
    FM_HARDWARE, //FM 硬件相关设置
    FM_SETFREQ, //设置单个频点播放
    FM_MUTE, //FM静音或解除静音
    FM_SETBAND, //设置波段
    FM_SETTH, //设置搜台门限
    FM_SEARCH, //设置软件搜台
    FM_SEEK, //启动seek (硬件搜台)
    FM_GETSEEKFLAG, //获取seek状态
    FM_BREAKSEEK, //退出seek

    FM_GETFREQ, //获取当前频率值
    FM_GETINTENSITY, //获取当前电台信号强度
    FM_GETSTATUS, //获取当前电台状态

    //中间逻辑层:与FM驱动无关
    FM_MID_PLAY, //播放电台
    FM_MID_SAVE, //保存当前电台
    FM_MID_SAVESTATION, //保存当前电台对应的电台表项号。
    FM_MID_FINDSTATION, //查找当前电台对应的电台表项号。
    FM_MID_LOADFREQ, //根据电台表项号加载频率。
    FM_MID_RESTORE, //还原当前电台
    FM_MID_TURN, //换台
    FM_MID_DELETE, //删除当前电台
    FM_MID_DELALL, //删除全部电台
    FM_MID_SELCITYINIT, //确定所选城市的第一个城市电台
    FM_MID_CITYSWITCH, //在所选城市中上下切换城市电台

    FM_MID_SETVOLUME, //设置声音
    FM_MID_STEPGRID, //电台频率微调，以100KHZ为步进

    CMD_COUNT,
    //FM命令数目
} radio_cmd_e;

/* Fm 的程序数据指针组成 结构体 */
typedef struct
{
    uint16 *param1; //电台表地址指针，20个（US模式）/20个（Japan模式）
    uint16 *param2; //当前电台信息的地址指针
} FMRADIO_param_t;

/* 数据声明 */
//main.c
extern radio_config_t g_radio_config; //radio ui 的配置项信息
extern uint8 EnterFlag; //应用进入模式. 0xff--只是为使FM模组进入standby
extern uint8 AutoTabNum; //自动搜台过程中,当前所搜电台的电台号
extern fm_status_t g_FMStatus; //搜台过程中获取的状态
extern uint8 seekflag; //硬件搜台标记
extern radio_citylist_t g_city_station; //radio 城市电台列表结构信息
extern uint16 *pfreq_play; //指向当前所播放的电台列表表首
extern radio_RDS_e use_RDS; //RDS关闭打开状态
extern fm_status_t FMStatusUS; //US电台信息暂存,8 bytes
extern fm_status_t FMStatusJP; //Japan电台信息暂存,8bytes
extern FMRADIO_param_t radio_param; //当前电台信息
extern uint8 flag_r; //搜台是否搜到第一个台之后返回标志位

//从VRAM中读出的EQ,SRS参数表，通过EQ工具设置
extern EQ_VM_t g_eq_para;

extern int8 AuCodecRun;
extern int8 AuCodecEffect;
extern int16 SentenceDectTime;
extern uint8 SentenceDectEnergy;
extern int8 SetenceFlag;
extern uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
extern uint32 m_decSpdifddrSav;

extern int8 last_SetenceFlag;

#ifdef AP_RADIO_REPORT_EN
extern char report_buf[16]; //存诸播报数据的buffer
#endif

//basal_fmradio.c
extern uint16 m_FMStationTab[MAX_STATION_COUNT];
extern void charge_pump_onoff(uint8 flag);

/* main.c */
uint16 FMinit(uint16 param);
void read_var(void);
void FMexit(uint16 result);
uint16 drawBandSwitch(void);

/* sub function */
extern uint16 fm_menu_save(uint16 *arg);  /* 保存电台*/
extern uint16 fm_menu_record(uint16 *arg);  /* 电台录音*/
extern uint16 fm_menu_delete(uint16 *arg);  /* 删除电台*/
extern uint16 fm_menu_delete_all(uint16 *arg);  /* 删除全部*/
extern uint16 fm_menu_auto_search(uint16 *arg);  /* 自动搜台*/
extern uint16 fm_menu_normal_band(uint16 *arg);  /* 普通频段*/
extern uint16 fm_menu_japan_band(uint16 *arg);  /* 日本频段*/
extern uint16 fm_menu_rds(uint16 *arg);  /* RDS 功能*/
extern uint16 fm_menu_fm_record(uint16 *arg);  //for FM Record by ccm
extern uint16 fm_menu_exit(uint16 *arg);  /* 退出 */

/* display.c */
void RefreshMainScreen(void);

/* deal_key_message.c */
int16 deal_key_msg(void);
void volume_callback(uint16 Vol);
void DealFadeIn(uint8 Volume);
void DealFadeInOut(void);

/* fm_deal.c */
uint16 FMdeal(uint16 RunFMStatues);

//SearchHandle.c
extern void ShowFreqInFM(void);
extern uint16 SearchHandle(BYTE Direct);
extern void DealFadeIn(uint8 Volume);
extern uint16 ChangeVolume(void);
extern void DealFadeInOut(void);
extern uint16 AutoSearchHandle(uint8 Direct);

/* Basal_fmradio.c */
uint8 fmSendCommand(radio_cmd_e cmd, void *param1, void *param2);

uint8 deal_mode_key(void);
uint8 deal_long_mode_key(void);
uint8 deal_chan_msg(uint8 key);
uint8 deal_radio_vol_msg(uint8 key);
uint8 deal_with_input_freq(void);
uint8 check_freq_valid(uint16 freq);
uint8 deal_timeout_msg(void);
uint8 deal_play_key(void);
uint8 deal_next_prev_key(uint8 key);
uint8 deal_next_prev_longkey(uint8 key);
uint8 deal_digital_key(uint8 key);

//eq set
void volume_callback(uint16 Vol);
void init_fmin_channel(void);
void exit_fmin_channel(void);
uint8 deal_eq_msg(uint8 key);

/* Analog */
void decinitAnalog(adin_t ain);
void decstartAnalog(adin_t ain);
void decAnalogexit(void);

#endif/*_APRADIO_H*/
