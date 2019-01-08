/*
********************************************************************************
*                       ACTOS
*                       ACTOS
*                  time define head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : time.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 9:14
********************************************************************************
********************************************************************************
*/
#ifndef _TIME_H
#define _TIME_H

#include "actapi.h"
#include "typeext.h"

#define DOWNPOWERADDR   0x07
#define WORLD_DATE    0
#define CHINA_DATE    1

//定义api号
#define APINO_TM_SetTimer         0x0000+API_TM_Base
#define APINO_TM_KillTimer        0x0001+API_TM_Base
#define APINO_TM_SetClock         0x0002+API_TM_Base
#define APINO_TM_KillClock        0x0003+API_TM_Base
#define APINO_TM_SetAlarmTime     0x0004+API_TM_Base
#define APINO_TM_GetTime          0x0005+API_TM_Base
#define APINO_TM_GetDate          0x0006+API_TM_Base
#define APINO_TM_SetTime          0x0007+API_TM_Base
#define APINO_TM_SetDate          0x0008+API_TM_Base

//#define API_StdSectoDate        0x0011+API_TM_BaseNoA

//#define APINO_TM_DelayMS			0x0012+API_TM_Base


//#ifdef     USE_EXTERNAL_RTC
//    #define API_TM_GetGWTime_RTCIC  0x000c + API_UI_Base     //放在显示驱动
//    #define API_TM_GetGWDate_RTCIC  0x100c + API_UI_Base     //放在显示驱动
//    #define API_TM_getcurtime       0x200c + API_UI_Base     //放在显示驱动
//
//    #define API_TM_SetGWTime_RTCIC  0x000d + API_UI_Base     //放在显示驱动
//    #define API_TM_SetGWDate_RTCIC  0x100d + API_UI_Base    //放在显示驱动
//
//    #define API_Ertc_HT1380_AllGet  0x000e + API_UI_Base     //放在显示驱动
//    #define API_Ertc_HT1380_AllSet  0x100e + API_UI_Base     //放在显示驱动
//#endif


#ifdef __C251__



/* changed by Weng*/
typedef struct
{
    uint16 year;
    uint8 month;
    uint8 day;
} date_t;

typedef struct
{
    uint8 hour;
    uint8 minute;
    uint8 second;
} time_t;

#define TM_SetTimer(timersub,period)	API_TM_SetTimer(APINO_TM_SetTimer,timersub,period)
#define TM_KillTimer(timerno)   		API_TM_KillTimer(APINO_TM_KillTimer,timerno)
#define TM_SetClock(clocksub,period)	API_TM_SetClock(APINO_TM_SetClock,clocksub,period)
#define TM_KillClock(clockno)      		API_TM_KillClock(APINO_TM_KillClock,clockno)
#define TM_SetAlarmTime(time)   		API_TM_SetAlarmTime(APINO_TM_SetAlarmTime,time)
#define TM_SetTime(time)             	API_TM_SetTime(APINO_TM_SetTime,time)
#define TM_GetTime(time)             	API_TM_GetTime(APINO_TM_GetTime,time)
#define TM_SetDate(date)        		API_TM_SetDate(APINO_TM_SetDate,date)
#define TM_GetDate(date)        		API_TM_GetDate(APINO_TM_GetDate,date)
#define TM_DelayMS(mscnt)				API_TM_DelayMS(APINO_TM_DelayMS,mscnt)

#define TM_Delay100uS(uscnt)			API_TM_Delay100uS(APINO_TM_DelayuS,uscnt)


//timer的周期为1ms
// timersub是timer响应函数入口地址，此处定义为int是为了方便c的非bank调用
uint8 API_TM_SetTimer(uint8 cmd, uint32 timersub, int8 period);
// 杀掉timer必须提供timer号
bool API_TM_KillTimer(uint8 cmd, int8 timerno);

//clock的周期为0.5S,
// clocksub是clock响应函数入口地址，此处定义为int是为了方便c的非bank调用
uint8 API_TM_SetClock(uint8 cmd, uint32 clocksub, int8 period);
// 杀掉clock必须提供clock号
bool API_TM_KillClock(uint8 cmd, int8 clockno);

//设置Alarm 中断的时间
void API_TM_SetAlarmTime(uint8 cmd, time_t *time);

//时间为24小时制
bool API_TM_SetTime(uint8 cmd, time_t *time);
bool API_TM_GetTime(uint8 cmd, time_t *time);

//type must equ. 0 means 公历格式
bool API_TM_SetDate(uint8 cmd, date_t *date);
bool API_TM_GetDate(uint8 cmd, date_t *date);

//----------------------------------------------------------------------
void API_TM_DelayMS(uint8 cmd, uint8 mscnt);

void API_TM_Delay100uS(uint8 cmd, uint8 uscnt);
uint32 sTM_ChangetoStdSec(time_t *time, date_t *date);
// bool TM_ChangetoCalendar(uint32 cc_seconds,time_t *time,date_t *date);
#define RTCFlush() {if(RTCREGUPDATE != 0xA5) RTCREGUPDATE = 0xA5;while(RTCREGUPDATE != 0x5A);}
#endif /* __C251__ */

#ifdef __A251__
//*****************************************************
//此处宏的调用入口参数参考c原型



//定义宏
mTM_SetTimer       macro
MOV 				 R11, #APINO_TM_SetTimer
ECALL  		RSTSYSAPI
endm
mTM_KillTimer   macro
MOV     R11, #APINO_TM_KillTimer
ECALL     RSTSYSAPI
endm
mTM_SetClock    macro
MOV     R11, #APINO_TM_SetClock
ECALL     RSTSYSAPI
endm
mTM_KillClock   macro
MOV     R11, #APINO_TM_KillClock
ECALL     RSTSYSAPI
endm

mTM_SetAlarmTime   macro
MOV     R11, #APINO_TM_SetAlarmTime
ECALL     RSTSYSAPI
endm

mTM_SetTime     macro
MOV     R11, #APINO_TM_SetTime
ECALL     RSTBankAPI
endm
mTM_GetTime     macro
MOV     R11, #APINO_TM_GetTime
ECALL     RSTSYSAPI
endm
mTM_SetDate     macro
MOV     R11, #APINO_TM_SetDate
ECALL     RSTSYSAPI
endm
mTM_GetDate     macro
MOV     R11, #APINO_TM_GetDate
ECALL     RSTSYSAPI
endm


mTM_SetGWTime     macro
MOV     R11, #APINO_TM_SetGWTime
ECALL     RSTSYSAPI
endm
mTM_GetGWTime     macro
MOV     R11, #APINO_TM_GetGWTime
ECALL     RSTSYSAPI
endm
mTM_SetGWDate     macro
MOV     R11, #APINO_TM_SetGWDate
ECALL     RSTSYSAPI
endm
mTM_GetGWDate     macro
MOV     R11, #APINO_TM_GetGWDate
ECALL     RSTSYSAPI
endm


mTM_ChangetoStdSec     macro
MOV     R11, #APINO_TM_ChangetoStdSec
ECALL     RSTSYSAPI
endm
mTM_ChangetoCalendar     macro
MOV     R11, #APINO_TM_ChangetoCalendar
ECALL     RSTSYSAPI
endm
mTM_DelayMS macro
MOV     R11, #APINO_TM_DelayMS
ECALL     RSTSYSAPI1
endm
#endif /* __A251__ */
#endif/*_TIME_H*/
