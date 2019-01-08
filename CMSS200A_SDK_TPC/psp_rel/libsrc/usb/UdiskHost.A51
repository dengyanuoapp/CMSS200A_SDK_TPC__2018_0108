 
$include (actos.inc)
 
#include "uddrv.h"
#include "actapi.h"
#include "driver.h"
#include "otg.h"

 		UDISKHOST_C	    SEGMENT     CODE
		?ED?AT_FA_?VAL  SEGMENT  EDATA AT 0x58


        public  usb_detectfunc_dereg{RPrefix}
		public	USB_DetectFnc_Initial{RPrefix}
		
		public HOST_Glbal
		public UH_MAX_LUN

        rseg    UDISKHOST_C
usb_detectfunc_dereg{RPrefix}:
		mUSB_DETECTFNC_DEREG
		eret
usb_detectfunc_dereg_end:
/*
*********************************************************************************************************
*                                           USB_Plugin_Detect
*
* Description: USB线插入检测,D+,D-上信号变化来检测插入PC或外接U盘.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
USB_Plugin_Detect:
        mUSB_PLUGIN_DETECT
USB_Plugin_Detect_End:

		
/*
*********************************************************************************************************
*                                           USB_DetectFnc_Initial
*
* Description: USB检测程序初始化,挂载Timer中断等.
*
* Arguments  : None.
*
* Returns    : None.
*
* Note(s)    : USB检测程序通过100ms Timer中断定时查询方式进入,启动过程中导入到Zram3中.
*********************************************************************************************************
*/
USB_DetectFnc_Initial{RPrefix}:
		mov		DPDMCTRL,#0x00
		
		mUSB_DETECTFNC_REG		

        mov      wr2,#WORD0 USB_Plugin_Detect
        mov      wr0,#WORD2 USB_Plugin_Detect		
		mov      wr6,#WORD0 USB_DETECT_ENTRY_ADDR
        mov      wr4,#WORD2 USB_DETECT_ENTRY_ADDR
		mov		 wr8,#WORD0 (USB_Plugin_Detect_End - USB_Plugin_Detect)					
CopyTmpramToCoderam:		
		mov		r11,@dr0
		mov		@dr4,r11
		add		dr0,#0x01
		add		dr4,#0x01
		dec		wr8,#0x01
		CMP		wr8,#0x00
		JNE		CopyTmpramToCoderam
		ERET	
		
		
RSEG       			 ?ED?AT_FA_?VAL
HOST_Glbal:  			DS 1  
UH_MAX_LUN:				DS 1								        
end