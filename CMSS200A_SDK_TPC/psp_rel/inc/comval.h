/*
********************************************************************************
*                       ACTOS
*                  common value define head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : comval.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-12-2 18:37
********************************************************************************
********************************************************************************
*/
#ifndef _COMVAL_H
#define _COMVAL_H

#include "sysdef.h"
#include "TypeExt.h"




//以下magic用于判断vm里的数据是否合法,uint16 类型
#define MAGIC_KVAL              0xdeed
#define MAGIC_COMVAL            0xdead
#define MAGIC_MUSIC             0xbeef
#define MAGIC_VOICE             0xfee0
#define MAGIC_RECORD            0x3d3d
#define MAGIC_FMRADIO           0xad01
#define MAGIC_SETTING           0xbaba
#define MAGIC_UDISK             0xee77
#define MAGIC_TESTER            0x9801
#define MAGIC_STANDBY           0x3935
#define MAGIC_UPGRADE           0x3951
#define MAGIC_MRECORD           0xcc00       //for M-Record by ccm
#define MAGIC_VIEWPIC		0x4a50		//for picture view
#define	MAGIC_READER		0x7478		//for reader "tx"
#define	MAGIC_AMV		0x616d		//for amv file "am"
#define    MAGIC_CFG        0x55aa        //for modify information
//language id
#ifdef PC
#define LAN_ID_SCHINESE        0           //中文简体
#define LAN_ID_TCHINESE        2           //中文繁体
#endif
//#define LAN_ID_ENGLISH         1           //英文
//#define LAN_ID_TCHINESE        2           //中文繁体
//#define LAN_ID_JAPANESE        3           //日文
//#define LAN_ID_KOREAN         4            //韩文
//#define LAN_ID_RUSSIAN         5            //俄罗斯语
//#define LAN_ID_RABBINIC        6           //西伯莱文
//#define LAN_ID_THAI                7            //泰国语
//#define LAN_ID_FRENCH             8           //法语
//#define LAN_ID_GERMANY           9          //德国语
//#define LAN_ID_ITALIAN             10          //意大利语
//#define LAN_ID_DUTCH              11          //荷兰语
//#define LAN_ID_PORTUGUESE        12          //葡萄牙语
//#define LAN_ID_SPANISH             13         //西班牙语
//#define LAN_ID_SWEDISH            14         //瑞典语
//#define LAN_ID_CZECHISH           15        //捷克语
//#define LAN_ID_POLAND             16         //波兰语
//#define LAN_ID_FINNISH             17        //芬兰语
//#define LAN_ID_DENISH              18         //丹麦语
//#define LAN_ID_HUNGARIAN         19         //匈牙利语
//#define LAN_ID_SLOVAKIAN         20         //斯洛伐克语
//batt type
#define BATT_TYPE_ALKALINE    0
#define BATT_TYPE_NIH        1
#define BATT_TYPE_LITHIUM    2

#ifdef __C251__

//************************************
//系统公共变量定义
//************************************
typedef struct
{
    //magic
    uint16 magic;    //"KVAL"
    //系统起始时间(半秒为单位)
    //系统当前的时间等于系统起始时间加系统time counter的时间
    uint32 systemtime;
    uint32 gwtime;
    int8  rtcresetflag;//默认为正常，rtcresetflag=0x00，如果rtc掉电rtcresetflag=0x55
} kval_t;

typedef struct
{
    //magic
    uint16 magic;

    //系统起始时间(半秒为单位)
    //系统当前的时间等于系统起始时间加系统time counter的时间
    uint32 systemtime;
    //系统时间比率
    uint16  reserve1;
    //int16 RTCRate;

    //对比度选择6~21
    uint8 DisplayContrast;

    //背景灯亮时间,默认值为0ah=5秒
    uint8 LightTime;

    //standby定时
    uint8 StandbyTime;

    //sleep定时
    uint8 SleepTime;

    //界面语言, 简体:0, 英文:1, 繁体:2
    int8 langid;

    //复读模式
    uint8 ReplayMode;

    //联机模式
    uint8 OnlineMode;

    uint8 BatteryType;                       //modify by gongee 2004-05-24 09:21
    uint8 FMBuildInFlag;                     //add battery select and fm select flag

    //录音类型,0:语言录音(V-Record),1:音乐录音(M-Record). add by ccm 2004-06-08
    uint8 RecordType;

    //背光灯颜色  7Color BackLight. For s2 by ccm
    uint8 BLightColor;

    //bit0~bit1: Udisk序列号支持:0:no sn; 1:unified sn; 2:random sn;
    //bit4: Udisk认证模式支持: 1:进入Udisk认证模式,启动时间加快,支持suspend和resume等USB认证功能
    int8 udisk_setting;

    uint8 Lightmode;	//0: 变暗模式 1: 变黑模式
    uint8 SuppCard;      //支持卡选择  0:不支持  1:支持

    uint8 MTPFormatType; //在MTP下格式化磁盘类型选择  0:FAT16  1:FAT32


    uint8 FMTag;
    //        int8 EarProtect_flag;
    //        int8 EarProtectThreshold;
    //        int8 attenuation_flag;
    //        int16  auto_switchoff_time;

    uint8 SuppKeyTone;           //支持按键音
    uint8 KeyTone;				//add by mzh 2007.3.14 0: no key tone 1:have key tone
    uint8  BackLightMax;  //背光最大亮度调节
    uint8  BackLightDef;  //默认背光亮度

    uint8  VolumeMax;    //音量最大级数调节
    uint8 VolumeDef;    //默认音量值
    uint8 ClassDFlag;   //D类功放支持
    uint8 EarphoneFlag; //耳机直驱支持
    uint8 RecordGain;
    uint8 RecordTime_max;
    uint8 volume; //当前音量实际值
    uint8 vol_display; //当前音量显示值
    uint8  SuppTTS;     //是否支持TTS
    uint8  tts_fun;       //tts播报功能开关
    uint8  SuppNorRec;   //Nor录音支持
    uint8  SuppMuscFav;    //音乐收藏夹支持
    //        int8 reserve[32-30];
} comval_t;

//初始化 comval  的宏
#define COMVAL_INIT(val) \
do{ \
        (val).DisplayContrast = 11; \
        (val).langid = LAN_ID_SCHINESE; \
        (val).LightTime = 3; \
        (val).magic = MAGIC_COMVAL; \
        (val).ReplayMode = 0; \
        (val).SleepTime = 0; \
        (val).StandbyTime = 30; \
        (val).OnlineMode = 0; \
        (val).systemtime = 0L; \
        (val).BatteryType = 3; \
        (val).FMBuildInFlag = 0; \
        (val).RecordType = 0; \
        (val).BLightColor = 4; \
        (val).Onlinedev = 0; \
        (val).EarProtect_flag = 0; \
        (val).EarProtectThreshold = 22; \
        (val).attenuation_flag = 0; \
        (val).auto_switchoff_time = 0; \
}while(0)

#endif/*__C251__*/


#endif/*_COMVAL_H*/
