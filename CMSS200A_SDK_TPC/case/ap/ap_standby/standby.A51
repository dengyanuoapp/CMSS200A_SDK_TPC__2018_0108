$include (actos.inc)
#if 0
//================================================================
#define K_STACK_START         3fe0h
#define K_VAR0_START          0030h
#define K_VAR1_START          0100h
#define K_CVAR1_START         0ff0200h
#define K_CODE0_START         0ff0000h
#define K_CODE1_START         0ff0100h
#define K_INT_VEC_START       0ff0003h

STACK  SEGMENT  EDATA  AT K_STACK_START
VAR0   SEGMENT  DATA   AT K_VAR0_START 
VAR1   SEGMENT  EDATA  AT K_VAR1_START
CVAR1  SEGMENT  HCONST AT K_CVAR1_START
CODE0  SEGMENT  ECODE  AT K_CODE0_START
CODE1  SEGMENT  ECODE  AT K_CODE1_START
INTVEC SEGMENT  ECODE  AT K_INT_VEC_START
//=================================================================
#endif
//PUBLIC Enter_Standby{RPrefix}
//PUBLIC RestoreGPIOReg{RPrefix}
//PUBLIC 	sRecover_Mcu{RPrefix}

GLOBALDATA SEGMENT EDATA
RSEG  GLOBALDATA
TempBuf			:DS	   20
INTValue		:DS	   2

CODE0	SEGMENT ECODE 

RSEG  CODE0
#if 0
Enter_Standby{RPrefix}:
    setb    MemBankCtl.2               	; reset address is changed to 0ff0000h
    setb    MemBankCtl.0               	; Memory Banking Enable
    setb    MemBankCtl.3            	; pcmram and muram2 is aligned to 009000h in data space

    mov     SFR_BANK,#01h
	
	setb    HOSC_CTL.0		; HOSC Enable 
	
	mov	r3,#07fh
wait_hosc_stable:
	lcall	delay
	djnz	r3,wait_hosc_stable
	nop				; wait HOSC steady
	nop

	mov     r1,MCSR4
	ANL     r1,#0Fch
	ORL     r1,#02h  
	mov     MCSR4,r1		; MCU_CLOCK=HOSC
	lcall	delay
	nop				; WAIT MCU CLK STEADY
	nop	

	
	MOV	0xA8,#0			;关闭中断
	MOV	0xE8,#0	    

;-----------------------------reset -----------------------------
  	mov     SFR_BANK,#01h
	
	MOV	MRCR1,#0x10		;nand normal
	MOV	MRCR3,#0x0c		;USB normal
	MOV	MRCR2,#0x08		;USB nornal
	nop
	nop
	nop
;-------------------------------USB----------------------------------------
	mov     SFR_BANK,#07h
	
	setb	USB_Efuse_Ref.6		;disable USB efuse
	
	SETB	USB_PHYCTRL.7
	SETB	USB_PHYCTRL.6
	nop
	nop
	
;--------------------------------RTC---------------------------------------
	mov     SFR_BANK,#0ch
	
	CLR	RTC_CTL0.7		;关闭calendar功能(RTCVDD域寄存器)
	
	SETB    WDCtl.3			;清watchdog计数器
        CLR     WDCtl.7			;关闭watchdog
        CLR     RTC_CTL1.6		;关闭定时器
        CLR     RTC_CTL1.7		;关闭2Hz中断
	nop
	nop
	
;--------------------------------NAND--------------------------------------	
	mov 	SFR_BANK,#06	
    	
    	mov 	PADDRV1,#00h
    	mov 	PADDRV0,#00h             ; 驱动增强设为最低档
    	clr 	PADPUPD0.2
	clr 	PADPUPD0.3               ; 取消RB1,RB2上拉电阻
	clr 	PADPUPD1.7
	nop
	nop
;--------------------------------PWM---------------------------------------		
;PWM进Standby时，有两种情况
;1.PWM并不是作背光控制，只需要将clock源切到LOSC，并且将模块reset写"0" 
;2.假如此时用PWM控制屏的背光，则进入Standby时，PWM选择LOSC，PWMDUTY寄存器写成0x00，
;  PWM便可以输出“0",背光一直被关闭，关闭的时机参考LCD进standby步骤
	
	mov     SFR_BANK,#06h
	mov     PWMDUTY,#0x00

;--------------------------------GPIO--------------------------------------
    lcall writeGPIOReg
	#if 0
	mov	SFR_BANK,#06h
   	
   	mov	r11,GPIOAOUTEN			; Enable GPIOA Output
   	orl	r11,#0xFF
   	mov	GPIOAOUTEN,r11
   	
   	mov	r11,GPIOADAT			; GPIOA Output high
   	orl	r11,#0xFF
   	mov	GPIOADAT,r11   
   	lcall	delay

	mov	r11,GPIOBOUTEN			; Enable GPIOB Output
   	orl	r11,#0xFF
   	mov	GPIOBOUTEN,r11
   	
   	mov	r11,GPIOBDAT			; GPIOB Output high
   	orl	r11,#0xFF
   	mov	GPIOBDAT,r11   	
   	lcall	delay

	mov	r11,GPIOCOUTEN			; Enable GPIOB Output
   	orl	r11,#0xFF
   	mov	GPIOCOUTEN,r11
   	
   	mov	r11,GPIOCDAT			; GPIOC Output high
   	orl	r11,#0xFF
   	mov	GPIOCDAT,r11   	
   	lcall	delay

	mov	r11,GPIODOUTEN			; Enable GPIOD Output
   	orl	r11,#0xFF
   	mov	GPIODOUTEN,r11
   	
   	mov	r11,GPIODDAT			; GPIOD Output high
   	orl	r11,#0xFF
   	mov	GPIODDAT,r11   	
   	lcall	delay


	mov	r11,GPIOEOUTEN			; Enable GPIOE Output
   	orl	r11,#0xFF
   	mov	GPIOEOUTEN,r11
   	
   	mov	r11,GPIOEDAT			; GPIOE Output high
   	orl	r11,#0xFF
   	mov	GPIOEDAT,r11  	
   	lcall	delay

	mov	r11,GPIOFOUTEN			; Enable GPIOF Output
   	orl	r11,#0xFF
   	mov	GPIOFOUTEN,r11
   	
   	mov	r11,GPIOFDAT			; GPIOF Output high
   	orl	r11,#0xFF
   	mov	GPIOFDAT,r11   	
   	lcall	delay

	
	clr	0xd2.0				; disable jtag
	mov	r11,GPIOGOUTEN			; Enable GPIOG[6:0] Output
   	orl	r11,#0xff
   	mov	GPIOGOUTEN,r11
   	  	
   	mov	r11,GPIOGDAT			; GPIOGPIOG[6:0] Output high
   	orl	r11,#0xff
   	mov	GPIOGDAT,r11
   	
   	lcall	delay
	#endif
;GPIOG[4:0]为JTAG口	
	
;--------------------------------PMU---------------------------------------
	mov     SFR_BANK,#05h

	MOV	R0,0X99
	MOV 	0X99,R0
	LCALL	delay
	
	SETB	0X99.6			;SELECT BANDGAP		
	LCALL	delay
	
	SETB	0X9A.6			;Disable EFUSE 
	
;--------------------------------切为低频-----------------------------------
	mov     SFR_BANK,#01h	
	CLR	0XB6.0			;DISABLE SINGLE PIN LOSC
;	CLR	0XB7.0			;DISABLE DUAL PIN LOSC

	CLR	MCSR4.0			;SELECT LOSC
	SETB	MCSR4.1
	NOP
	NOP
	NOP
	
;

;------------------------------disable all module clock----------------------	
	mov     SFR_BANK,#01h

	MOV	MCSR1,#0
	
	MOV	MCSR2,#0
	
	clr     AUDIOPLL_CTL.0          ;disable AudioPLL

	
	MOV	MCSR3,#0
	
	MOV	R0,MCSR4		;MCU CLOCK=HOSC
	ANL	R0,#03h
	MOV	MCSR4,R0

	MOV	R0,MCSR5
	ANL	R0,#0x20
	MOV	MCSR5,R0		;PRAM251是程序空间，不能关
	
	MOV	MCSR6,#0
	MOV	MCSR7,#0
	MOV	MCSR8,#0
	MOV	MCSR9,#0
	MOV	MCSR10,#0
	MOV	MCSR11,#0	
	MOV	MCSR12,#0
	nop
	nop
	nop	

;----------------------------------------------------------------------------
//	CLR     HOSC_CTL.0		 ;HOSC Disable
	NOP
	NOP
	NOP
	ret
;----------------------------------IDLE/PD------------------------------------

;	setb	PCON.0			;idle
;	setb	PCON.1			;Power down
;----------------------------------END-------------------------------------

//	SJMP	$	

;----------------------------delay subroutine------------------------------
delay:							;delay subroutine
	mov 		r2,#0ffh	
loop:	
        nop
        nop 
        nop
        nop						
    	nop
    	nop
    	djnz		r2,loop
	ret

sRecover_Mcu{RPrefix}: 
	setb    HOSC_CTL.0		; HOSC Enable 
	mov	r3,#07fh
wait_stable:
	lcall	delay
	djnz	r3,wait_stable
	nop				; wait HOSC steady
	nop

	mov     r1,MCSR4
	ANL     r1,#0Fch
	ORL     r1,#02h  
	mov     MCSR4,r1		; MCU_CLOCK=HOSC
	lcall	delay
	nop				; WAIT MCU CLK STEADY
	nop	
	ret

writeGPIOReg:
	mov	SFR_BANK,#06h
   	
	mov wr0,#TempBuf
   	mov	r11,GPIOAOUTEN			; Enable GPIOA Output
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOAOUTEN,r11
   	
   	mov	r11,GPIOADAT			; GPIOA Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOADAT,r11   
   	lcall	delay

	mov	r11,GPIOBOUTEN			; Enable GPIOB Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOBOUTEN,r11
   	
   	mov	r11,GPIOBDAT			; GPIOB Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOBDAT,r11   	
   	lcall	delay

	mov	r11,GPIOCOUTEN			; Enable GPIOB Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOCOUTEN,r11
   	
   	mov	r11,GPIOCDAT			; GPIOC Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOCDAT,r11   	
   	lcall	delay

	mov	r11,GPIODOUTEN			; Enable GPIOD Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIODOUTEN,r11
   	
   	mov	r11,GPIODDAT			; GPIOD Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIODDAT,r11   	
   	lcall	delay


	mov	r11,GPIOEOUTEN			; Enable GPIOE Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOEOUTEN,r11
   	
   	mov	r11,GPIOEDAT			; GPIOE Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOEDAT,r11  	
   	lcall	delay

	mov	r11,GPIOFOUTEN			; Enable GPIOF Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOFOUTEN,r11
   	
   	mov	r11,GPIOFDAT			; GPIOF Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xFF
   	mov	GPIOFDAT,r11   	
   	lcall	delay

	
	clr	0xd2.0				; disable jtag
	mov	r11,GPIOGOUTEN			; Enable GPIOG[6:0] Output
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xff
   	mov	GPIOGOUTEN,r11
   	  	
   	mov	r11,GPIOGDAT			; GPIOGPIOG[6:0] Output high
	inc wr0 ,#1
	mov @wr0,r11
   	orl	r11,#0xff
   	mov	GPIOGDAT,r11
   	
   	lcall	delay
	ret
RestoreGPIOReg{RPrefix}:
	mov	SFR_BANK,#06h  	
	mov wr0,#TempBuf

	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOAOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOADAT,r11   
   	lcall	delay

	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOBOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOBDAT,r11   	
   	lcall	delay

	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOCOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOCDAT,r11   	
   	lcall	delay

	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIODOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIODDAT,r11   	
   	lcall	delay


	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOEOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOEDAT,r11  	
   	lcall	delay

	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOFOUTEN,r11
   	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOFDAT,r11   	
   	lcall	delay

	
	setb 0xd2.0				; enable jtag
	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOGOUTEN,r11
   	  	
   	mov r11,@wr0
	inc wr0 ,#1
   	mov	GPIOGDAT,r11
   	
   	lcall	delay
	ret
	#endif
end