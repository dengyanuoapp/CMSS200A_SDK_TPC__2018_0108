/*
 *******************************************************************************
 *                ACTOS AP
 *        system setting ap of AS211A
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: main.c,v 1.2 2010/03/29 10:48:01 Mars Exp $
 *******************************************************************************
 */
#include "cfg.h"
#include "key.h"

#pragma name(CFG_MAIN)

/************************头信息定义**************************************/

//const char   CfgMagic[4]={"CFG"};
//const uint8  TotallItem = 5;
//const char   ReservHead1[11];
#define CFG_ITEM_MAX   6
#pragma userclass(nconst = CFG_HEAD)
const file_cfg_head_t FileHead =
{
    "CFG", CFG_ITEM_MAX,
    { 0xfe, 0xff, 0x00 }
};

const cfg_item_t CfgItem[CFG_ITEM_MAX] =
{
    { (uint32) (&ParaHead), "Parameters", "参数设置" },
    { (uint32) (&KeyCfg), "Key-press", "按键" },
    { (uint32) (&APCfg), "Application", "应用程序" },
    { (uint32) (&LanguageCfg), "language", "语言" },
    { (uint32) (&FileCfg), "File-exchange", "替换文件" },
    { (uint32) (&FuncCfg), "Functions", "功能配置" }
};
const char ReservHead2[0x200 - CFG_ITEM_MAX * 0x20 - 0x10];

#pragma userclass(nconst = CFG_PARA)

/******************参数设置********************************************/

#define PARA_VALUE_TOTAL    6
#define PARA_STRING_TOTAL   2

const paraset_head_t ParaHead =
{ (uint32) ValueSet, PARA_VALUE_TOTAL, (uint32) (&StringSet), PARA_STRING_TOTAL, "" };

const value_t ValueSet[PARA_VALUE_TOTAL] =
{
    { SLEEPTIME_VALUE, SLEEPTIME_MAX, SLEEPTIME_MIN, SLEEPTIME_STEP, "", "sleep time", "关机时间(分钟)" },
    { VOLUME_VALUE, VOLUME_MAX, VOLUME_MIN, VOLUME_STEP, "", "volume set", "音量等级" },
    { DEFVOLUME_VALUE, DEFVOLUME_MAX, DEFVOLUME_MIN, DEFVOLUME_STEP, "", "vol default", "默认音量" },
    { RECGAIN_VALUE, RECGAIN_MAX, RECGAIN_MIN, RECGAIN_STEP, "", "Record gain", "录音增益" },
    { RECTIME_VALUE, RECTIME_MAX, RECTIME_MIN, RECTIME_STEP, "", "Record time", "录音时间(小时)" },
    { NOR_RECTIME_VALUE, NOR_RECTIME_MAX, NOR_RECTIME_MIN, NOR_RECTIME_STEP, "", "Nor Record time", "Nor录音时间(秒)" }

};

const string_t StringSet[PARA_STRING_TOTAL] =
{
    { "AS260B_V0001", "Device Version", "设备版本" },
    { "2013/11/18", "Firmware Date", "固件日期" }
};

#pragma userclass(nconst = CFG_KEY)

/********************按键设置**********************************************/
//const uint8 KeyTotal = 5;
//const uint8 TotalItem = 11;
//const char  KeyReserv[14];
//const uint8 keyValue[16]

#if 0
const key_cfg_t KeyCfg =
{
    16, 16, "",
    {
        Msg_KeyPlayPause, Msg_KeyVolSub, Msg_KeyVolAdd, Msg_KeyNext,
        Msg_KeyLast, Msg_KeyNum3, Msg_KeyNum6, Msg_KeyNum9,
        Msg_KeyLast, Msg_KeyVolAdd, Msg_KeyVolSub, Msg_KeyNum0,
        Msg_KeyNum1, Msg_KeyNum4, Msg_KeyNum7, Msg_KeyNext
    }
};
#else
const key_cfg_t KeyCfg =
{
    3, 3, "",
    {
        Msg_KeyPlayPause, Msg_KeyVolSub, Msg_KeyVolAdd
    }
};
#endif

const key_infor_t KeyInfor[16] =
{
    { Msg_KeyFolderAdd, 0, "FOLDERADD" },
    { Msg_KeyFolderSub, 0, "FOLDERSUB" },
    { Msg_KeyClock, 0, "CLOCK" },
    { Msg_KeyCaraOk, 0, "CARAOK" },
    { Msg_KeyPlayPause, 1, "PLAY/PAUSE" }, //PLAY键不可配置
    { Msg_KeyLast, 0, "PREV" },
    { Msg_KeyREC, 0, "REC/AB" },
    { Msg_Keyautosleep, 0, "AUTOSLEEP" },
    { Msg_KeyMenu, 0, "MENU" },
    { Msg_KeyNext, 0, "NEXT" },
    { Msg_KeyVolAdd, 0, "VOL+" },
    { Msg_KeyVolSub, 0, "VOL-" },
    { Msg_KeyMute, 0, "MUTE" },
    { Msg_KeyVol, 0, "VOL" },
    { Msg_KeyEQ, 0, "EQ" },
    { Msg_KeyLoop, 0, "LOOP" }
};

#pragma userclass(nconst = CFG_AP)

/********************AP 配置*****************************/
//const uint8 TotalAP = 7;
//const uint8 AvtiveAP = 1;
//const char  Reserv[14];

const ap_cfg_head_t APCfg =
{ MAX_AP_NUM, 1, "" };

const ap_infor_t APTable[MAX_AP_NUM] =
{
    { RESULT_MUSIC, 1, "Music", "音乐" },
    { RESULT_RADIO, 1, "Radio", "收音机" },
    { RESULT_LINEIN, 1, "Line-in", "音频输入" },
    { RESULT_CLOCK, 1, "Clock", "时钟" }
};

#pragma userclass(nconst = CFG_LAN)
/******************语言设置*****************************************/
const lang_infor_t LanguageCfg =
{
    0, 0,
    {
        LAN_ID_SCHINESE, LAN_ID_ENGLISH, LAN_ID_TCHINESE, LAN_ID_JAPANESE, LAN_ID_KOREAN, LAN_ID_FRENCH, LAN_ID_GERMANY,
        LAN_ID_ITALIAN, LAN_ID_DUTCH, LAN_ID_PORTUGUESE, LAN_ID_SPANISH, LAN_ID_SWEDISH, LAN_ID_CZECHISH,
        LAN_ID_DENISH, LAN_ID_POLAND, LAN_ID_RUSSIAN, LAN_ID_TURKISH, LAN_ID_HEBREW, LAN_ID_THAI, LAN_ID_HUNGARIAN,
        LAN_ID_ROMANIAN, LAN_ID_RABBINIC, LAN_ID_INDONESIAN, LAN_ID_GREEK, LAN_ID_SLOVAKIAN, LAN_ID_FINNISH,
        LAN_ID_CROATIA, LANGUAGETOTAL
    }
};

/*************文件替换******************/
#pragma userclass(nconst = CFG_FILE_EX)
//const uint8 FileTotal = 10;
//const char  FileExReserv[15];
#define CFG_AP_MAX  14
const file_ex_t FileCfg =
{ CFG_AP_MAX, "" };

const char FileName[CFG_AP_MAX][16] =
{
    "music.ap", "record.ap", "radio.ap", "main.ap", "ap_rec_p.ap", "udisk.ap", "usbsound.ap", "standby.ap", "linein.ap", "alarm.ap",  "ky_hard.drv",
    "drv_ui.drv", "alarm.mp3", "welcome.bin"
};

/*功能配置项*/
#pragma userclass(nconst = CFG_FUNC)
#define FUNC_ITEM_MAX   4

const func_cfg_t FuncCfg =
{ FUNC_ITEM_MAX, "" };

const func_item_t FuncItem[FUNC_ITEM_MAX] =
{
    { 0, "", "Support KeyTone", "支持按键音" },
    { 0, "", "Support WAV", "支持 WAV 录音" },
    { 1, "", "Support TTS", "支持语音播报" },
    { 0, "", "Support NorRec", "支持Nor录音" }
};

const char FW_TYPE = 1;

#pragma userclass(nconst = default)
