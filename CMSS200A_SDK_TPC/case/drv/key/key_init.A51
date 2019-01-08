$include (actos.inc)
#include "key.h"
#include "time.h"

public key_init{RPrefix}
EXTRN  CODE(key_scan{RPrefix})


?PR?BAC_IE0 SEGMENT CODE
 
rseg ?PR?BAC_IE0
key_init{RPrefix}:
    PUSH SFR_BANK
	mov     SFR_BANK,#BANK_PMU
	mov     R11,PMUADC_CTL
	ORL     R11,#0a0h  
	mov     PMUADC_CTL,	R11			 //enable LRADC1,ADC FREQUENCY 128Hz
	nop		
	LCALL	delay					//enable LRADC1后需要等待最长10ms才能读出正确的数据，否则可能是全0。
   
	MOV		SFR_BANK, #BANK_PMU
	MOV		R11,	SYSTEM_CTL_RTCVDD		
	ANL		R11,	#0x3f
	MOV		SYSTEM_CTL_RTCVDD,	R11			//设置play按键时间=2S

 	MOV		WR6,#WORD0(key_scan{RPrefix})	//设置RTC中断函数入口 
    MOV		R11,#02h
    mTM_SetTimer 
    MOV		R11,#01h

	POP 	SFR_BANK
	ret

delay:	
 	mov 	R5,#0ffh
delay1:
	mov 	R6,#0ffh
loop1:        
        nop
        nop                  
        DJNZ 	R6,loop1
        DJNZ 	R5,delay1
        ret
  
 

end