/*
*********************************************************************************************************
*                                       ACTOS GL5101
*
*
*                           (c) Copyright 2002-2008, Actions Co.,Ltd.
*                                   All Right Reserved
*
* File   : otg.h
* By     : zhouxl
* Version: v1.00     first version     2008-1-24 10:57
*********************************************************************************************************
*/

#ifndef _OTG_H
#define _OTG_H

#include "actos.inc"
#include "time.h"
#include "message.h"
#include "irq.h"
#include "string.h"
#include "device.h"

//是否支持USB Host功能
#define         USB_HOST_SUPPORT
#define FULL_SPEED_ENABLE              0x01

//注册的Timer中断号
#define         USB_DETECT_TIMER_INDEX_ADDR             0xff43c0
//USB检测函数Timer中断入口地址
#define         USB_DETECT_ENTRY_ADDR                   0xff43c1
//Timer中断入口地址保存的Memory地址
#define         TIMER_ENTRY_MEM_ADDR                  	0x1802


#define UIV_OTGIRQ                     0xd8

/*  bit map flags for g_uhost_state  */
#define GL_DEVICE_IN                   0x01
#define GL_CONTROLLER_ENABLED          0x02
#define GL_ENUMERATED                  0x04
#define GL_DP_PULLHIGH                 0x10

#ifdef __C251__
extern uint8 sHOST_Glbal;
extern uint8 UH_MAX_LUN;

/*
*********************************************************************************************************
*                                           usb_detectfnc_reg
*
* Description: 挂载USB检测程序到Timer中断.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : USB检测程序通过400ms Timer中断定时查询方式进入,启动过程中导入到Zram3中.
*********************************************************************************************************
*/
/*
void usb_detectfnc_reg(void)
{
	uint8 i,timeindex;
	uint16* data_p;
	uint32 vector;

	data_p = (uint16*)TIMER_ENTRY_MEM_ADDR;

	for(i=1;i<7;i++)
	{
		timeindex = i;
		if((*data_p) == USB_DETECT_ENTRY_ADDR)
		{
			break;
		}
		data_p += 2;
	}

	if(i > 0x06)
	{
		vector = USB_DETECT_ENTRY_ADDR;
		timeindex = TM_SetTimer((int16)vector, 0x08);
	}

	*((uint8*)USB_DETECT_TIMER_INDEX_ADDR) = timeindex;
}
*/

/*
*********************************************************************************************************
*                                           usb_detectfnc_dereg
*
* Description: 注销挂载到Timer中断上的USB检测程序.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
/*
void usb_detectfnc_dereg(void)
{
	 uint8 index,*data_p;
	 index = *((uint8*)USB_DETECT_TIMER_INDEX_ADDR);
	 TM_KillTimer(index);
	 data_p = USB_DETECT_TIMER_INDEX_ADDR;
	 *data_p = 0x00;
}
*/

/*
*********************************************************************************************************
*                                           usb_plugin_detect
*
* Description: USB线插入检测,D+,D-上信号变化来检测插入PC或外接U盘.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : 此段程序必须保证地址可重定位,不能使用jp, call指令.
*********************************************************************************************************
*/
/*
void usb_plugin_detect(void)
{
	uint8 sfrbak;

	sfrbak = SFR_BANK;
	SFR_BANK = BANK_USB;

	//check if D+ pull high, indicate Udisk plug-in
	if ((USBSTATUS&0x08)!=0x00)
	{
		//last DP status,check if USB DP status changed
		if((HOST_Glbal&GL_DP_PULLHIGH) == 0x00)
		{
			HOST_Glbal = HOST_Glbal | GL_DP_PULLHIGH;
			USB_CON_ENABLE();
		}
	}
	else
	{
		HOST_Glbal = HOST_Glbal & (~GL_DP_PULLHIGH);
	}

	SFR_BANK = sfrbak;
}
*/
#endif

#ifdef __A251__
/*
*********************************************************************************************************
*                                           mUSB_DETECTFNC_REG
*
* Description: 挂载USB检测程序到Timer中断.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : USB检测程序通过400ms Timer中断定时查询方式进入,启动过程中导入到Zram3中.
*********************************************************************************************************
*/
mUSB_DETECTFNC_REG macro
mov		r11, #0x00
mov		wr2, #TIMER_ENTRY_MEM_ADDR
mov     wr4, #WORD0 USB_DETECT_ENTRY_ADDR
check_reg_usb_timer:
add		r11, #0x01
mov		wr6, @wr2
cmp		wr6, wr4
je		had_reg_usb_timer
add		wr2, #0x0004
cmp		r11, #0x06
jne		check_reg_usb_timer

mov     wr6, #WORD0 USB_DETECT_ENTRY_ADDR //注册80ms查询中断
mov		r5, #0x08
mTM_SetTimer

had_reg_usb_timer:
mov		wr2, #WORD0 USB_DETECT_TIMER_INDEX_ADDR
mov		wr0, #WORD2 USB_DETECT_TIMER_INDEX_ADDR
mov		@dr0, r11
endm


/*
*********************************************************************************************************
*                                           mUSB_DETECTFNC_DEREG
*
* Description: 注销挂载到Timer中断上的USB检测程序.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
mUSB_DETECTFNC_DEREG macro
mov		wr6, #WORD0 USB_DETECT_TIMER_INDEX_ADDR
mov		wr4, #WORD2 USB_DETECT_TIMER_INDEX_ADDR
mov		r11, @dr4
mTM_KillTimer
mov		r11, #0x00
mov		wr6, #WORD0 USB_DETECT_TIMER_INDEX_ADDR
mov		wr4, #WORD2 USB_DETECT_TIMER_INDEX_ADDR
mov		@dr4, r11
endm


/*
*********************************************************************************************************
*                                           mUSB_PLUGIN_DETECT
*
* Description: USB线插入检测,D+,D-上信号变化来检测插入PC或外接U盘.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : 此段程序必须保证地址可重定位,不能使用jp, call指令.
*********************************************************************************************************
*/
/*
mUSB_PLUGIN_DETECT  macro
		push	SFR_BANK
		mov		SFR_BANK,#BANK_USB

		mov		r0,USBSTATUS			//check if D+ pull high, indicate Udisk plug-in
		anl		r0,#0x08
		cjne	r0,#0x08,usb_not_plugin

        mov		r0,edata(sHOST_Glbal)
		anl		r0,#GL_DP_PULLHIGH		//last DP status
		cjne	r0,#0x00,usb_detect_ret			//USB DP status not changed

		mov		r11,edata(sHOST_Glbal)
		orl		r11,#GL_DP_PULLHIGH
		mov		edata(sHOST_Glbal),r11

		// mUSB_CON_ENABLE
		ecall   sUSB_CON_ENABLE{RPrefix}
		jmp		usb_detect_ret

usb_not_plugin:
		mov		r11,edata(sHOST_Glbal)
		anl     r11,#(~GL_DP_PULLHIGH)
		mov		edata(sHOST_Glbal),r11
usb_detect_ret:
		pop		SFR_BANK
		eret
		endm  */
#endif /* __A251__ */


#endif  //#ifndef _OTG_H