/*
*******************************************************************************
*                ACTOS AP
*        ap config header  file
*
*        (c) Copyright, Actions Co,Ld.
*             All Right Reserved
*
*  $Id: ap_cfg.h,v 1.7 2009/07/30 14:54:29 mikeyang Exp $
*******************************************************************************
*/
#ifndef AP_CFG_H
#define AP_CFG_H

#ifdef MAIN_AP
#include "cfg_MainAP.h"
#endif

#ifdef UDISK_AP
#include "CFGUDISK_AP.h"
#endif

#ifdef MUSIC_AP
#include "cfgmusic_ap.h"
#endif

#ifdef RADIO_AP
#include "cfgRADIO_AP.h"
#endif
//define this flag when release
#define SupportCard              // 支持插卡 
#define SupportWMAREC          //支持WMA格式录音 
#define Msg_CTC2HZ             //使用CTC定时的2HZ消息
#define UNICODE_SUPPORT       //支持unicode多国语言驱动
#define NEWFORMATRES          //支持Excel输入Unicode编码新格式Res文件
//#define ALARM_CLOCK             //支持闹钟功能
//#define AUTO_FMREC              //支持FM定时自动录音功能
#define  FUN_SWITCH_KEY            /* 支持一键功能切换 */


#define SOFTVOL_MAX    40

//card base or nor base define
//#define USE_CARDBASE

//固件配置相关宏
#define SIZE_CONFIGFILE_HEAD     16                 //fwcfg.bin 头信息结构字节数
#define SIZE_ONE_ITEM      32              // cfg_item_t 结构字节数

#define SIZE_PARA_HEAD    16          //参数配置类头信息结构长度
#define SIZE_PARA_ONEITEM    48    //参数配置类单项配置项长度
#define SIZE_STRINGSET_ITEM    64     //字符串配置类结构单项长度

#define SIZE_APCFG_HEAD      16       // ap config 配置项头结构长度
#define SIZE_APINFOR_ITEM    32       // ap 配置项单项长度

#define SIZE_FUNC_HEAD     16       //功能配置项头结构长度

#define  SLEEPTIME_VALUE       0               //关机时间
#define  SLEEPTIME_MAX          120
#define  SLEEPTIME_MIN           0
#define  SLEEPTIME_STEP           10

#define  VOLUME_VALUE       40               //音量等级
#define  VOLUME_MAX          40
#define  VOLUME_MIN           0
#define  VOLUME_STEP           1

#define  DEFVOLUME_VALUE       40               //默认音量32
#define  DEFVOLUME_MAX          40
#define  DEFVOLUME_MIN           0
#define  DEFVOLUME_STEP           1

#define  RECGAIN_VALUE       15               //录音增益
#define  RECGAIN_MAX          15
#define  RECGAIN_MIN           0
#define  RECGAIN_STEP           1

#define  RECTIME_VALUE       3               //最大录音时间
#define  RECTIME_MAX          20
#define  RECTIME_MIN           0
#define  RECTIME_STEP           1

#define  NOR_RECTIME_VALUE       15               //默认 nor录音音量（单位为S）
#define  NOR_RECTIME_MAX          15			  //最大nor录音时间	 (秒)
#define  NOR_RECTIME_MIN           1
#define  NOR_RECTIME_STEP           1

#ifdef __C251__
/*固件配置相关数据结构定义*/
typedef struct
{
    uint8  Total; //支持的语言总数,最大25种语言
    uint8  Active;//当前默认语言的ID号
    uint8  Table[62]; //语言ID号数组，每个数组成员表示不同语言的ID，取值范围为为0-29，0xff表示到结尾
} lan_set_t;    // 64 bytes

typedef struct
{
    uint8  Total;      //支持的按键定义总数，最多支持16个按键
    uint8  Table[16];  //按键消息值定义数组，每个数组成员表示不同的按键消息
} key_set_t;   // 17 bytes

typedef struct
{
    uint8  Total;     //支持的AP的总数
    uint8  Active;    //进入主菜单默认ap的编号
    uint8 Table[16]; //AP对应的数组，每个数组成员两个字节，第一个表示AP编号,第二个表示AP状态
} ap_set_t;  // 18 bytes

typedef struct
{
    uint8  DefValue;  //默认值
    uint8  Max;      //最大值
    uint8  Min;      //最小值
    uint8  Step;     //步长
} value_set_t;  // 4 bytes

typedef struct
{
    uint16        Magic;
    lan_set_t     LanInfor;
    key_set_t     KeyInfor;
    ap_set_t      ApInfor;
    value_set_t   SleepTime;
    value_set_t   Volume;
    value_set_t   VolDefault;
    value_set_t   RecordGain;
    value_set_t   RecordTimeMax; //支持最大录音时间
    value_set_t   Nor_RecordTimeMax; //支持nor 最大录音时间,单位为秒
    uint8  DevVesion[32];
    uint8  FirmwareDate[32];
    uint8  FMBuildInFlag;	  //FM支持，0，不支持，1，支持
    uint8  ClassDFlag;	  //D类功放，外挂喇叭时使用，0，不支持，1，支持
    uint8  KeyToneFlag;//按键音功能支持   0， 不支持按键音； 1，支持
    uint8  Support_WAV;     // 支持WAV录音。0，只支持mp3录音，1-只支持WAV录音
    uint8  TTS_Report;     // TTS 播报功能支持   0， 不支持TTS   1， 支持
    uint8  Support_NorRec; // Nor 录音功能支持   0， 不支持Nor录音，使用卡或u盘介质录音    1， 支持Nor录音
} vm_cfg_t;


/*配置项数据结构定义*/
typedef struct
{
    char   CfgMagic[4];
    uint8  TotalItem;
    char   Reserv[11];
} file_cfg_head_t; // 16 bytes

typedef struct
{
    uint32 Offset;
    char   EName[14];
    char   CName[14];
} cfg_item_t; // 32 bytes


/*参数类配置项头信息数据结构定义*/
typedef struct
{
    uint32 ValueOffset;
    uint8  ValueTotal;
    uint32 StringOffset;
    uint8  StringTotal;
    char   Reserv[6];
} paraset_head_t; //16 bytes

/*数值类配置项数据结构定义*/
typedef struct
{
    uint8  DefValue;
    uint8  Max;
    uint8  Min;
    uint8  Step;
    uint8  Reserv[12];
    char   EName[16];
    char   CName[16];
} value_t; //48 bytes

/*字符串类配置项数据结构定义*/
typedef struct
{
    char   StringCfg[32];
    char   EName[16];
    char   CName[16];
} string_t; //64 bytes

/*按键类配置项数据结构定义*/
typedef struct
{
    uint8  TotalKey;
    uint8  TotalItem;
    char   Reserv[14];
    char   KeyValue[16];
} key_cfg_t; //32 bytes

typedef struct
{
    uint8  Value;
    uint8  Status; //0 表示可配置的按键，1表示不可配置
    char   Description[14];
} key_infor_t; //16 bytes

/*AP配置项头信息数据结构定义*/
typedef struct
{
    uint8  Total;
    uint8  Active;
    char   Reserv[14];
} ap_cfg_head_t; //16 bytes

/*ap 配置信息数据结构*/
typedef struct
{
    uint8  Index;
    uint8  Status;
    char   EName[15];
    char   CName[15];
} ap_infor_t; //32 bytes

typedef struct
{
    uint8  Total; //支持的语言总数,最大25种语言
    uint8  Active;//当前默认语言的ID号
    uint8  Table[62]; //语言ID号数组，每个数组成员表示不同语言的ID，取值范围为为0-29，0xff表示到结尾
} lang_infor_t; //64 bytes

typedef struct
{
    uint8  Total; //当前可替换的固件文件
    uint8  Reserv[15]; //保留
} file_ex_t; //16 bytes

typedef struct
{
    uint8  Total; //当前可配置项的总数
    uint8  Reserv[15]; //保留
} func_cfg_t; //16 bytes

typedef struct
{
    uint8  State; //功能支持项的状态，0，不支持，1，支持。
    uint8  Reserv[15]; //保留
    uint8  EName[16];  //英文名
    uint8  CName[16];  //中文名
} func_item_t; //48 bytes

#endif      /*end of #ifdef __C251__*/

#endif

