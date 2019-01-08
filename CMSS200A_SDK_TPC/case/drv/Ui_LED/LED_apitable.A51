
$include (actos.inc)
#include "driver.h"

/************************* Segment Define ********************/
LED_APITABLE SEGMENT ECODE

PUBLIC 	        LED_API_TABLE
//rcode api	
                
EXTRN ECODE(Sdrv_Init{RPrefix})
EXTRN ECODE(Sdrv_Exit{RPrefix})
EXTRN ECODE(Sdrv_ClearScreen{RPrefix})
EXTRN ECODE(Sdrv_LEDDisplay{RPrefix})
EXTRN ECODE(Sdrv_LEDPuts{RPrefix})
 
/************************* code begin ***********************/

rseg    LED_APITABLE

LED_API_TABLE: 
#if 0
DD		 Sdrv_Init{RPrefix}                 
DD		 Sdrv_Exit{RPrefix}                 	
DD       Sdrv_LEDDisplay{RPrefix}
DD       Sdrv_ClearScreen{RPrefix}                 
DD       Sdrv_LEDPuts{RPrefix}
#else
DD		 Sdrv_Init{RPrefix}                 
DD		 Sdrv_Init{RPrefix}                 	
DD       Sdrv_Init{RPrefix}
DD       Sdrv_Init{RPrefix}                 
DD       Sdrv_Init{RPrefix}
#endif
end   
   