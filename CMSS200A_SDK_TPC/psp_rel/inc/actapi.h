/*
*******************************************************************************
*                        ACTOS
*                        API HEAD FILE
*
*                (c) Copyright 2002, Actions Co,Ld.
*                        All Right Reserved
*
* File: API.h
* By  : Gongee
*******************************************************************************
*/
#ifndef _ACTAPI_H_
#define _ACTAPI_H_
#include "as260x.h"
//BankA/BankB Define
//**************************************************************************
//
#define 	BANKBHEAD_SIZE			    19

//Software Interrupt entrance
//**************************************************************************
//api entry
#define RSTBASE				    0xFFC090
#define FARRSTBASE				0xff00c0

#define ACTOS_ROM_BASE         0xFFC0E0

//#define RSTFastAPI              RSTBASE + 0x08    //快速api
//#define FFSDCardAPI             RSTBASE + 0x0c    //快速SD卡API
//#define RSTBankAPI              RSTBASE + 0x10    //bank api(相对慢速)
#define RSTSYSAPI               RSTBASE + 0x10	  //系统固化bank函数表入口
//#define I2CDrvAPI               RSTBASE + 0x14    //I2C Driver 入口
//#define RSTMIDAPI              	RSTBASE + 0x14    //MID 入口
//#define RSTStgAPI               RSTBASE + 0x18    //存储类驱动专用api入口
#define RSTSTGAPI               RSTBASE + 0x18    //存储类驱动专用api入口
//#define SDRAMAPI                RSTBASE + 0x1c    //SDRAM Driver 入口
//#define RSTFARAPI				FARRSTBASE	+ 0x1C	   //Huge模式调用API
#define RSTKEYAPI				RSTBASE + 0x1C 	  //按键驱动api入口
#define RSTUIAPI           		RSTBASE + 0x20    //显示类驱动专用api入口
#define RSTMFSAPI               RSTBASE + 0x24    //mini文件系统驱动专用api入口
#define RSTFSAPI                RSTBASE + 0x28    //文件系统驱动专用api入口
#define RSTFMAPI				RSTBASE + 0x2C	  //FM驱动专用api入口
//#define RSTSysErrAPI            RSTBASE + 0x30    //系统出错处理专用api入口
#define RSTSYSAPI1              RSTBASE + 0x30    //系统固化表入口
#define RSTSYSAPI2              RSTBASE + 0x34    //系统常驻表入口
#define RSTSDFFAPI              RSTBASE + 0x38    //SD_Read快速入口
#define RSTRESAPI               RSTBASE + 0x3C    //保留驱动入口
#define WDAPI					ACTOS_ROM_BASE + 0x0c
#define ROMDIAPI                ACTOS_ROM_BASE + 0x0f
#define ROMEIAPI                ACTOS_ROM_BASE + 0x12
//**************************************************************************

//-------------------------------------
//系统bank函数api
//-------------------------------------
#define API_IRQ_Base         0x0000        //irq manager in bank a
#define API_ADFU_Base        0x0002        //adfu
#define API_TM_Base          0x0003        //time manager in bank a
#define API_SDFS_Base        0x000c        //sd file system in bank a
#define API_DRV_Base       	 0x000e  //驱动管理        
#define API_APM_Base       	 0x0016  //ap管理  
#define API_VM_Base          0x0017
#define API_DEVICE_Base      0x0019        //device in bank a
//-------------------------------------
//系统固化函数api
//-------------------------------------
#define APINO_MSG_GetSysMsg       0x00
#define APINO_MSG_PutSysMsg       0x01
#define APINO_SD_Read             0x02
#define APINO_SD_FRead            0x03
#define APINO_SD_FSeek            0x04
#define APINO_SD_FTell            0x05
#define APINO_TM_DelayMS		  0x06
#define APINO_CardInOut_MSG		  0x07
#define APINO_BankSwitch      	  0x08
#define APINO_DATA_SAVE			  0x09
#define APINO_DATA_RESTORE		  0x0a
#define APINO_TM_DelayuS		  0x0b

//***********************************************************************

//-------------------------------------
//系统新增函数api
//-------------------------------------
#define  API_NEW_BASE 0x00

#define RAMAPI_DEVICE   0x0000+API_NEW_BASE
#define RAMAPI_DRV_UD   0x0003+API_NEW_BASE
#define RAMAPI_SD_BASE   0x0004+API_NEW_BASE

#endif /*_ACTAPI_H */