$include (actos.inc)
#include "driver.h"


		NAME   DRV_HEADER

		UI_INFO SEGMENT ECODE
		EXTRN CODE(UART_API_TABLE)
		EXTRN   ECODE(uart_init{RPrefix})
        EXTRN   ECODE(uart_exit{RPrefix})
		RSEG   UI_INFO
//定义驱动头信息 
DRV_FileType:             	  DB     'D'
DRV_Type:                 	  DB     DRV_UI
DRV_RcodeStartAddr:       	  DW     0x1200
DRV_RcodeLength:          	  DW     0x0400
DRV_Rcode2StartAddr:      	  DW     0x0000
DRV_Rcode2Length:         	  DW     0x0000
DRV_RdataStartAddr:       	  DW     0xa80
DRV_RdataLength:          	  DW     0x00b0
DRV_InitAPI:              	  DD     uart_init{RPrefix}
DRV_ExitAPI:              	  DD     uart_exit{RPrefix}
DRV_BnkAFileAddr:             DS     4
DRV_BnkBFileAddr:    	      DS     4
DRV_OpEntry:				  DD     UART_API_TABLE

END