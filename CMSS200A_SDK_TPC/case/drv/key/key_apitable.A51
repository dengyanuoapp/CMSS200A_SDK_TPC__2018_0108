
$include (actos.inc)
#include "driver.h"

/************************* Segment Define ********************/
KEY_APITABLE SEGMENT ECODE
	
EXTRN  			ECODE(key_init{RPrefix})                            
EXTRN  			ECODE(key_exit{RPrefix})                            
EXTRN  			ECODE(sKY_GetChargeStatus{RPrefix})                            
EXTRN  			ECODE(sKY_Beep{RPrefix})                            
EXTRN  			ECODE(KY_GetKeyTabAddress{RPrefix})             
EXTRN  			ECODE(drv_check_plug_device_msg{RPrefix})   
EXTRN  			ECODE(drv_switch_pilot_light{RPrefix})  
PUBLIC 	        KEY_API_TABLE
 
/************************* code begin ***********************/

rseg    KEY_APITABLE
KEY_API_TABLE:
DD		 key_init{RPrefix}
DD		 key_exit{RPrefix}
DD       sKY_GetChargeStatus{RPrefix}
DD       sKY_Beep{RPrefix}   
DD       KY_GetKeyTabAddress{RPrefix}
DD       drv_check_plug_device_msg{RPrefix}
DD       drv_switch_pilot_light{RPrefix}
end