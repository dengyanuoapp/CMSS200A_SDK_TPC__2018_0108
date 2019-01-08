

//******************************************************************************
//*     show logo api
//******************************************************************************   

public  sAdfuEnter??
//public  mcu_delay?_

;------------------------------------------------------------------------------
; Put segment and variable declarations here.
;------------------------------------------------------------------------------	
CODEDISPSTART	SEGMENT  ECODE 	 
//CODEDISPSTART	SEGMENT  CODE 	 
        rseg    CODEDISPSTART
		
sAdfuEnter??:
       	EJMP	0xFF9C5C		  // jmp to adfu entry
	    JMP $   
        ERET 
;----------------------------delay subroutine------------------------------
/*mcu_delay?_:							;delay subroutine
	    MOV   R2,#0ffh	
loop:	
        NOP
        NOP 
        NOP
        NOP						
    	NOP
    	NOP
    	DJNZ R2,loop
	    RET*/
end