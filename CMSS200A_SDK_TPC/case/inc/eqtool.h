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
#ifndef EQTOOL_H
#define EQTOOL_H
#include "actos.h"//系统头文件
#include "mmcmd.h"//模块层命令名定义
#include "basal.h"//basal module 头文件
#include "enhanced.h"//enhanced module 头文件
#include "ap_common.h"//控件头文件
#include "uhdrv.h"
#include "report_res.h"

#include "usbtest.h"
#include "decC.h"       //设置EQ时使用


#define  EQ_VRAM_MAGIC       0xaa55
#define  SRS_VRAM_MAGIC       0x55aa

#define  SUPPORT_EQ_CNT       8         // 1:normal 2:Rock 3:Pop 4:Classic 5:Soft 6:Jazz 7:Dbb 8:自定义
#define  ONE_EQPARA_BYTE         7          //一种音效参数表有7个字节



typedef enum
{
    Left_EQ = 0,
    Right_EQ
} EQ_Channel_e;

typedef struct
{
    uint16 magic;
    uint8   left_eq_para[ONE_EQPARA_BYTE *SUPPORT_EQ_CNT];
    uint8   right_eq_para[ONE_EQPARA_BYTE *SUPPORT_EQ_CNT];
} EQ_VM_t;  // 114 字节



extern int8 AuCodecRun;
extern int8 c_decEffectFlag;

extern void deal_pc_eqsrs(uint8 aptype);
extern uint8 deal_srssave_check(uint8 *srsbuf);
extern uint8 deal_eqsave_check(uint8 *eqbuf);
extern void Set_Onechannel_EQ(uint8 *buf, uint8 len, EQ_Channel_e left_right, uint8 aptype);
extern bool check_allbuf0(uint8 *buf, uint8 len);
extern void deal_pc_eqsrs_sub( uint8 aptype);
extern void callback_pc_eq(EQ_Channel_e left_right, uint8 *buf,  uint8 aptype);

#endif
