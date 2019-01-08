/*
********************************************************************************
*                       ACTOS
*                  message api define
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : message.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 11:31
********************************************************************************
********************************************************************************
*/
#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "actapi.h"

#define MSG_USB_STICK            0x91
#define MSG_USB_UNSTICK          0x92
#define MSG_RTC2HZ               0x80
#define MSG_BAT_V_CHG            0x81
#define MSG_LOW_POWER            0x82
#define Msg_RtcAlarm             0x83

#define MSG_SD_IN                0xb0   /*²å¿¨×´Ì¬*/
#define MSG_SD_OUT               0xb1   /*²¦¿¨×´Ì¬*/

#define MSG_UH_IN				 0xc0	/*UÅÌ²åÈë*/
#define MSG_UH_OUT				 0xc1	/*UÅÌ°Î³ö*/

//#define APINO_MSG_GetSysMsg        0x0000+API_MSG_Base
//#define APINO_MSG_PutSysMsg        0x0001+API_MSG_Base

#ifdef __C251__
#include <TypeExt.h>

#define PutSysMsg(msg)  API_PutSysMsg(APINO_MSG_PutSysMsg,msg)
#define GetSysMsg() 	API_GetSysMsg(APINO_MSG_GetSysMsg)

void API_PutSysMsg(uint8 cmd, uint8 msg);
uint8 API_GetSysMsg(uint8 cmd);
#endif /* __C251__ */

#ifdef __A251__

mMSG_PutSysMsg  macro
MOV     R11, #APINO_MSG_PutSysMsg
ECALL   RSTSYSAPI1
endm
mMSG_GetSysMsg   macro
MOV    	R11, #APINO_MSG_GetSysMsg
ECALL  	RSTSYSAPI1
endm
#endif /* __A251__ */

#endif/*_MESSAGE_H*/