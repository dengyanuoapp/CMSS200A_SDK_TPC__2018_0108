#ifndef _ACTOS_INC
#define _ACTOS_INC


#include "config.h"
#include "as260x.h"
#include "sysdef.h"
#include "TypeExt.h"
#include "port.h"
#include "actapi.h"
#include "device.h"
#include "adfu.h"
#include "actos_lin.h"
#include "addr_bank_map.h"



//sysconfig 模块的起始地址
#define     SYSCFG_ADDR                 0x0600
//MainAP 模块的起始地址
#define     MAINAP_ADDR                 0x0800
//brec 模块的起始地址
#define     BinAddr_BRECLauncher        0x8100

#define     Code0StartPage              0x0000
#define     CodeLength                  SD_CAP>>10
#define     Code1StartPage              Code0StartPage+CodeLength

#define BinAddr_ADFULauncher        0x9800
/***************************************************
 **        message                                **
 ***************************************************/
//user message depth
#define     UserMsgPoolDepth            0x10
#define     RTCMsgPoolDepth             0x04
#define     KeyMsgPoolDepth             0x04

//#define     BNKA_SRAM_ADDR              0xff0600
//#define 	BNKA_DATA_SRAM_ADDR		      0x000400
//#define     BNKB_SRAM_ADDR              0xff0800
//#define     BNKB_DATA_SRAM_ADDR			    0x000480


#define     BNKA_HEAD_LEN				19
#define     BNKB_HEAD_LEN				19
#define     DRV_INITFUNC_IN_BNKA        0
#define     DRV_INITFUNC_IN_BNKB        1

/***************************************************
 **        cardsuppflag                           **
 ***************************************************/
//#define     IfSuppCard          0x9a    //根据Actos.xcl分配的地址


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  AP类型
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define     AP_MAIN             0x00


#endif