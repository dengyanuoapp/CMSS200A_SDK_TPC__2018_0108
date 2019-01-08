/*
********************************************************************************
*                       ACTOS
*                  key board message define
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : key.msa
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 9:14
********************************************************************************
********************************************************************************
*/
#ifndef _KEY_H			//防止重定义
#define _KEY_H
#include "actapi.h"
#include "port.h"
#include "actapi.h"
#include "driver.h"
#include "sddrv.h"
#include "sdfs.h"
#include "sysdef.h"
#include "ap_cfg.h"

//#define EVB_KEY
//#define CMSS101		// Separated mode, lightning + mainboard. 
//#define CMSS102		// CMSS101 + 3.5 Jack.
//#define CMSS201       // Integreated mode. 
//#define CMSS202       // CMSS201 + 3.5 Jack.
//#define CMSS203		// CMSS202 + support APPLE headset wire control. 
//#define CMSS204		// CMSS203 + Charging.
//#define CMSS301		// Support Type-C

//#define EVB_KEY
#define CMSS205

//#define ONE_KEY		

// key value											PLAY	VOL-	VOL+
//{0x00, 0x04, 0x17};	//CMSS101 Beta  				ON/OFF	0V		1.0V
//{0x00, 0x04, 0x28};	//CMSS101 Standard				ON/OFF	0V		1.5V
//{0x22, 0x2E, 0x3A};	//CMSS201 Standard				1.5V	2.0V	2.5V
//{0x22, 0x3A, 0x32};	//CMSS202 Standard				1.5V	2.5V	2.3V
//{0x00, 0x25, 0x19};	//CMSS203 APPLE earphone		1.5V	1.5V	1.0V
//{0x21, 0x2C, 0x25};	//CMSS203 android earphone		1.5V	1.9V	1.7V

#ifdef EVB_KEY
#define		KEY_PLAY_TH			0x30
#define		KEY_VOLSUB_TH		0x30
#define		KEY_VOLADD_TH		0x30
#endif

#ifdef CMSS101
#define		KEY_PLAY_TH			0x00
#define		KEY_VOLSUB_TH		0x04
#define		KEY_VOLADD_TH		0x28
#endif

#ifdef CMSS102
#endif

#ifdef CMSS201
#define 	NONE_ONOFF
#define		KEY_PLAY_TH			0x22
#define		KEY_VOLSUB_TH		0x2E
#define		KEY_VOLADD_TH		0x3A
#endif

#ifdef CMSS202
#define 	NONE_ONOFF
#define		KEY_PLAY_TH			0x22
#define		KEY_VOLSUB_TH		0x3A
#define		KEY_VOLADD_TH		0x32
#endif

#ifdef CMSS203
#define 	NONE_ONOFF
#define 	SUPPORT_APPLE_KEY
#define		KEY_PLAY_TH			0x21
#define		KEY_VOLSUB_TH		0x2C	
#define		KEY_VOLADD_TH		0x25	
#endif

#ifdef CMSS204
#define 	NONE_ONOFF
#define 	SUPPORT_APPLE_KEY
#define		SUPPORT_CHARGE_2A
#define		KEY_PLAY_TH			0x21
#define		KEY_VOLSUB_TH		0x2C	
#define		KEY_VOLADD_TH		0x25
#endif

#ifdef CMSS205
#define 	NONE_ONOFF
#define		SUPPORT_CHARGE_2A
#define		KEY_PLAY_TH			0x26//0x2c //0x38 //0x22
#define		KEY_VOLSUB_TH		0x3A//0x2d //0x3A	
#define		KEY_VOLADD_TH		0x30//0x2f //0x32	
#endif

#ifdef SUPPORT_APPLE_KEY
#define		KEY_PLAY_TH_A		0x00	
#define		KEY_VOLSUB_TH_A		0x25	//0x27
#define		KEY_VOLADD_TH_A		0x19	//0x24
#endif

/*define key message of key driver*/
#define Msg_KeyNull		      	0x00
#define Msg_KeyPlayPause      	0x01
#define Msg_KeyLoop           	0x02
#define Msg_KeyVolSub         	0x03
#define Msg_KeyVolAdd         	0x04
#define Msg_KeyLast           	0x05
#define Msg_KeyNext           	0x06
#define Msg_KeyMenu           	0x07
#define Msg_KeyREC            	0x08
#define Msg_KeyEQ             	0x09
#define Msg_KeyVol	      		0x0a
#define Msg_KeyChanSub          0x0b
#define Msg_KeyChanAdd          0x0c
#define Msg_KeyNum0             0x0d
#define Msg_KeyNum1             0x0e
#define Msg_KeyNum2             0x0f
#define Msg_KeyNum3             0x10
#define Msg_KeyNum4             0x11
#define Msg_KeyNum5             0x12
#define Msg_KeyNum6             0x13
#define Msg_KeyNum7             0x14
#define Msg_KeyNum8             0x15
#define Msg_KeyNum9             0x16
#define Msg_KeyPower            0x17
#define Msg_KeyFolderAdd     0x18
#define Msg_KeyFolderSub     0x19
#define Msg_KeyCaraOk         0x1a
#define Msg_KeyClock         0x1b
#define Msg_KeyMute         0x1c
#define Msg_KeyMicIn         0x1d
#define Msg_Keyautosleep  0x1e
#define Msg_KeyUSBTest          0x1F

#define Msg_KeyShortUp        0xC2    //短按键抬起 
#define Msg_KeyLongUp         0xC3    //长按键抬起 
#define Msg_KeyHold           0xC4        //避免和Msg_KeyLoop|AP_KEY_UP冲突，Mars，2008-04-16
#define Msg_KeyUnHold         0xC5
#define Msg_HalfSecond        0xC6
#define Msg_DeviceMsg        0xC7          //有卡或u盘拔插情况
#define Msg_NOKEY              0xFF
/*define key message of ap  */
#define AP_KEY_LONG       0x80
#define AP_KEY_UP         0x40

#define AP_KEY_NULL       Msg_KeyNull
#define AP_KEY_MODE       Msg_KeyMenu
#define AP_KEY_PLAY       Msg_KeyPlayPause
#define AP_KEY_NEXT       Msg_KeyNext
#define AP_KEY_PREV       Msg_KeyLast
#define AP_KEY_VOLADD     Msg_KeyVolAdd
#define AP_KEY_VOLSUB     Msg_KeyVolSub
#define AP_KEY_REC         Msg_KeyREC
#define AP_KEY_EQ         Msg_KeyEQ
#define AP_KEY_LOOP       Msg_KeyLoop
#define AP_KEY_VOL 	      Msg_KeyVol
#define AP_KEY_CHANSUB    Msg_KeyChanSub
#define AP_KEY_CHANADD    Msg_KeyChanAdd
#define AP_KEY_NUM0       Msg_KeyNum0
#define AP_KEY_NUM1       Msg_KeyNum1
#define AP_KEY_NUM2       Msg_KeyNum2
#define AP_KEY_NUM3       Msg_KeyNum3
#define AP_KEY_NUM4       Msg_KeyNum4
#define AP_KEY_NUM5       Msg_KeyNum5
#define AP_KEY_NUM6       Msg_KeyNum6
#define AP_KEY_NUM7       Msg_KeyNum7
#define AP_KEY_NUM8       Msg_KeyNum8
#define AP_KEY_NUM9       Msg_KeyNum9
#define AP_KEY_FOLDERADD       Msg_KeyFolderAdd
#define AP_KEY_FOLDERSUB       Msg_KeyFolderSub
#define AP_KEY_CLOCK     Msg_KeyClock
#define           AP_KEY_MUTE          Msg_KeyMute
#define           AP_KEY_MICIN          Msg_KeyMicIn
#define           AP_KEY_KARAOKE          Msg_KeyCaraOk
#define           AP_KEY_FUNCTION          Msg_KeyPower
#define           AP_KEY_DEVICEMSG          Msg_DeviceMsg
#define           AP_KEY_LONGUP          Msg_KeyLongUp
#define  AP_KEY_AUTOSLEEP   Msg_Keyautosleep

//#endif         //#ifdef Use_RotateKey



#define AP_KEY_USBTEST Msg_KeyUSBTest
#ifdef __A251__       //汇编使用的定义 

////汇编调用通过宏调用
//mKY_HoldCheck  macro
//        ld      hl,API_HoldCheck
//        rst     RSTBankAPI
//        endm
//
//mKY_OpenBacklight macro
//		ld		hl,API_OpenBacklight
//		rst		RSTBankAPI
//		endm
//
//mKY_CloseBacklight macro
//		ld		hl,API_CloseBacklight
//		rst		RSTBankAPI
//		endm
//
//mKY_CardPowerOn macro                               //for 7color Blight add by ccm
//		ld		hl,API_CardPowerOn
//		rst		RSTBankAPI
//		endm
//
//mKY_ChargeSet macro
//        ld        hl,API_ChargeSet
//        rst        RSTBankAPI
//        endm
//
//mKY_Beep macro
//        ld        hl,API_Beep
//        rst        RSTBankAPI
//        endm
//msKY_ChargeGetandSet   macro
//        ld         hl,API_ChargeGetandSet
//        rst        RSTBankAPI
//        endm

//#ifdef __IAR_SYSTEMS_ICC            //C语言使用的定义
//
//non_banked bool sKY_HoldCheck(void);        //TRUE为HOLD，FALSE为UNHOLD
//non_banked bool sKY_SetBLightColor(char BLightColor);//设置背光颜色。for s2 by ccm
//non_banked bool    sKY_OpenBacklight(void);    //打开背光
//non_banked bool    sKY_CloseBacklight(void);    //关闭背光
//non_banked bool sKY_ChargeSet(char Setting);    //关闭背光
//non_banked bool sKY_Beep(char value);           //key tone
//non_banked char sKY_ChargeGetandSet(void);      //充电检测
//#endif /* __IAR_SYSTEMS_ICC */
//
//#ifdef __IAR_SYSTEMS_ASM        //汇编使用的定义
//
////BANKA，函数的地址
//#define API_HoldCheck       0x0200 + API_KY_BaseNoA		//02的取值根据ky_hard_bnkif.msa中的排列
//#define	API_OpenBacklight	0x0300 + API_KY_BaseNoA
//#define	API_CloseBacklight	0x0400 + API_KY_BaseNoA
//#define API_ChargeSet          0x0600 + API_KY_BaseNoA
//#define API_Beep                0x0700 + API_KY_BaseNoA
//#define API_ChargeGetandSet                0x0001 + API_KY_BaseNoA
////#define API_CardPowerOn      0x0101 + API_KY_BaseNoA        //在"actapi.h"中定义
//#define API_CtcIRQIntercept       0x0201 + API_KY_BaseNoA
//#define API_CtcIRQRelease        0x0301 + API_KY_BaseNoA
//#define API_GetKeyTabAddress  0x0401 + API_KY_BaseNoA
//
////汇编调用通过宏调用
//mKY_HoldCheck  macro
//        ld      hl,API_HoldCheck
//        rst     RSTBankAPI
//        endm
//
//mKY_SetBLightColor  macro
//        ld      hl,API_SetBLightColor
//        rst     RSTBankAPI
//        endm
//
//mKY_OpenBacklight macro
//        ld        hl,API_OpenBacklight
//        rst        RSTBankAPI
//        endm
//
//mKY_CloseBacklight macro
//        ld        hl,API_CloseBacklight
//        rst        RSTBankAPI
//        endm
//
//mKY_CardPowerOn macro                               //for 7color Blight add by ccm
//        ld        hl,API_CardPowerOn
//        rst        RSTBankAPI
//        endm
//
//mKY_ChargeSet macro
//        ld        hl,API_ChargeSet
//        rst        RSTBankAPI
//        endm
//
//mKY_Beep macro
//        ld        hl,API_Beep
//        rst        RSTBankAPI
//        endm
//msKY_ChargeGetandSet   macro
//        ld         hl,API_ChargeGetandSet
//        rst        RSTBankAPI
//        endm
#endif/**251_ASM***/
#endif/*_KEY_H*/
