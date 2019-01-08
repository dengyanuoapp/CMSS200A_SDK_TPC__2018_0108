
$include (actos.inc)
#include "driver.h"

/************************* Segment Define ********************/
DRV_INFO SEGMENT ECODE
/************************* public info ***********************/
/************************* EXTRN symbol **********************/
EXTRN  			ECODE(key_init{RPrefix})
EXTRN  			ECODE(key_exit{RPrefix})
EXTRN			ECODE(KEY_API_TABLE)
/************************* code begin ***********************/

rseg    DRV_INFO
DRV_FileType:             	  DB    'D'
DRV_Type:                 	  DB    DRV_KY
DRV_RcodeStartAddr:       	  DW    0x5400
DRV_RcodeLength:          	  DW    0x400
DRV_Rcode2StartAddr:      	  DW    0
DRV_Rcode2Length:         	  DW    0
DRV_RdataStartAddr:       	  DW    0xb30
DRV_RdataLength:          	  DW    0x50
DRV_InitAPI:              	  DD    key_init{RPrefix}
DRV_ExitAPI:              	  DD    key_exit{RPrefix}
DRV_BnkAFileAddr:             DS    4
DRV_BnkBFileAddr:    	      DS    4
DRV_OpEntry:				  DD    KEY_API_TABLE


end