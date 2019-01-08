/*
********************************************************************************
*                        ACTOS
*                  audio decoder codec
*
*             (c) Copyright 2002-2009, Actions Co,Ld.
*                        All Right Reserved
*
* File   : audio_enc.a51
* By     : jpliao
* Version: 1> v1.00     first version     05/9/2009 11:20AM
********************************************************************************
*/
;$include (ak211x.h)
#include "ak211x.h"
#include <irq.h>
;$include (action251.inc)
;$include (audioip.inc)
;$include (adda_reg.inc)

;****************************************************************************
; 					Module name (optional)
;****************************************************************************
//NAME        AUDIO_ENCODER
;****************************************************************************
; 					Import symbols form other modules.
;****************************************************************************
//EXTRN   CODE   (BANK_CALL_DIRECT_L08)
;****************************************************************************
;						PUBLIC symbols in one module.
;****************************************************************************

PUBLIC  encInit??
PUBLIC  encStart??              
PUBLIC  encExit??
PUBLIC	encStop??
PUBLIC	encPause??
PUBLIC	encContinue??
PUBLIC	encGetBitstream??            
PUBLIC  encTimerIntSub?_
PUBLIC  encAudioIntSub?_

PUBLIC  g_encControlInfor
PUBLIC  g_encStatus
PUBLIC  g_encWriteFilePara

/***ADC setting define****/
SamplRate              equ     32           ; ADC Frequency Sampling:8/16/24/32/48
Adc_Channel            equ     0ch          ; 0ch: Stereo
                                            ; 08h: only left
                                            ; 04h: only Right
                                            ; 00h: disable       ADC_CTL bit[3:2]
Fifo_level             equ   01h            ; 00h: 14 of 16
                                            ; 01h: 10 of 16
                                            ; 02h:  6 of 16
                                            ; 03h:  2 of 16      ADC_FIFOCTL0 bit[1:0]
InputSel               equ   01h            ; 00h: mic
                                            ; 01h: FM(line in)
                                            ; 02h: reserved
                                            ; 03h: mix
#define FMGAIN     07    //00~01~...07
#define MICGAIN    07    //00~01~...07
                                            
//#define FPGA
#ifdef FPGA
#define        ADC_D_DEBUG
#endif


#define	AUENC_USE_DMA1 0
#define	AUENC_USE_DMA2 1
#define	AUENC_DMA_MODE AUENC_USE_DMA1

ENC_BUFFER_START EQU	05000h				//muram1 low
ENC_BUFFER_END	 EQU	09000h				//muram1 low
ENC_BUFFER_LENGTH EQU	04000h   		//16k : muram low + muram mid
ENC_BUFFER_PAGES EQU	32						//16k = 32 pages
ENC_BLOCK1_START EQU	09000h				//pcm ram low, need to merge
ENC_BLOCK2_START EQU	0b000h				//muram2
ENC_WRITE_FILE_SIZE EQU	512


;****************************************************************************


;***********************************************************
;						Segment location.
;***********************************************************
?ED?MW_CC_AD_ENC_R	SEGMENT  EDATA
?BT?ENC_BIT_VAR	SEGMENT  BIT 
MW_CC_AD_ENC_R	SEGMENT  ECODE				// RCODE
MW_CC_AD_ENC_B1	SEGMENT  ECODE				//BANK CODE
MW_CC_AD_ENC_B2	SEGMENT  ECODE				//BANK CODE
 
;********************************************************************************* 
                RSEG    ?ED?MW_CC_AD_ENC_R
;********************************************************************************* 

g_encControlInfor:									;
c_encSongType:			DS		1			;0:mp3,1:wma,2:wav, reserve
c_encFsIndex: 			DS      1 			;INDEX FOR SAMPLERATE
c_encBitDepth: 			DS      1 			;bits per sample,only support 16 bps here
c_encBTIndex: 			DS      1 			;INDEX FOR BITRATE,	RESERVE HERE
c_encEncodeMode: 		DS      1 			;wav type record, 0:adpcm,1:pcm
c_encChannelNum: 		DS      1 			;CHANNEL NUMBER
c_encInputSel:	 		DS      1 			;INPUT SOURCE SELECT
c_encInputGain: 		DS      1 			;INPUT GAIN PARAMETERS
c_encADCGain: 			DS      1 			;ADC GAIN PARAMETERS
c_encDSPGain: 			DS      1 			;DSP GAIN PARAMETERS, Recommend 16 8 20 = 320 ms
c_encDelayTime:			DS      1 			;Delay time for avoidding noise, unit: 20ms
c_encEnergyOut: 		DS      1 			;0:not out 非零:out energy
c_encAVRFlag: 			DS      1 			;0:not SILENCE DETECTION 非零:SILENCE DETECTION
c_encAVRDelay:			DS      1 			;SILENCE DETECTION PERIOD
c_encAVRLevel: 			DSW     1 			;SILENCE DETECTION LEVEL
c_encLeftOffset:		DSW			1				;left channel dc offset
c_encRightOffset:		DSW			1				;right channel dc offset

g_encWriteFilePara:		
c_encWriteFileHandle:		DSD		1			//文件读写接口的指针（或句柄），位于Basal层，每次写特定的n*512byte			
c_encFileLen:						DSD		1 			//文件总长度，单位：字节
c_encWriteFileBuffAddr:	DSW		1  			//文件数据缓冲区的起始地址，缓冲区大小为n*512byte，暂定为单缓冲区，暂定放在DM
c_encWriteFileCurSize:	DSW   1 			//当前写的编码数据量，单位：字节		

c_encInitToZeroStart:		
g_encStatus:
c_encStatus: 						DS      1				;ENCODER STATUS
c_encAVRStatus:					DS      1				;0: not silence, 1: 声控停止，2：分曲
c_encEnergyLevel:				DSW     1				;Energy Level			
c_encCurTime:						DSD			1				;对应时，分，秒，最高的byte为0 

c_encSamplerate:				DSW			1			;SAMPLE RATE: SAMPLES PER SECNOD
c_encBitrate:					DSW			1			;birate: kbps

c_encWriteTempBufferAddr:DSW		1  			;16k数据缓冲区的写地址
c_encReadTempBufferAddr: DSW		1  			;16k数据缓冲区的读地址
c_encByteshaveEncode: 	DSD     1 			;已编码字节数
c_encTempBuffByteRemain: DSW    1			;BYTES IN BITSTREAM BUFFER FOR WRITE OUT To FILE
c_encWriteFileMAXSize:	 DSW	1			//当前BASAL BUFFER 长度，

c_encBytesPerBlock: 		DSW     1 		;
c_encBytesPerSec: 			DSD     1 			;
c_encByteNeedForNextSec:DSD     1 			;

c_encGetBSResult: 			DS      1 			;0: no data to wirte out, 1: write data chunk, 2: write header
c_encPCMLostNum: 				DSW     1 		;
c_encB1B2AllFullNum:			DSW     1 		;

c_encCurBlockPCmVal:			DSW			1			;FIRST SAMPLE OF CURRENT BLOCK,BIG ENDIAN
c_encSilenceCnt:				DSW			1			;BLOCK COUNT BELOW 	c_encAVRLevel
c_encSilenceCntMAX:				DSW			1			;MAXIMUM BLOCK COUNT BELOW 	c_encAVRLevel

c_encDemolishKeyVoiceTming: DS  1 			;AVOID KEY NOISE			;

c_encADCInitPara:
c_encADCInputSel:       DS      1 			; 00h: mic ; 01h: FM(line in) ; 02h: reserved ; 03h: mix
c_encADCChannel:    	DS      1			; 0ch: Stereo ; 08h: only left ; 04h: only Right
c_encADCSampleRate:     DS      1			; ADC Frequency Sampling:8/16/24/32/48 etc
c_encADCInputGain:      DS      1			; mic: 0-7 for 20-40db, fm: 0-7 for -3.0 - +7.5db, input source gain,mic录音推荐值5即放大50倍，fm录音推荐值2即不放大也不缩小
c_encADCLPFGain:	    DS      1			; 0: -6 db, 20h: -3db, 40h: 0db, 60h: +3db, adc analog gain,推荐值40h即不放大也不缩小
c_encADCFifoAccess:   	DS      1			; 0: MCU, 10h: DMA, 20h: AHC, 30h: AHC
c_encADCTrimmingFlag:	DS		1			; 0: no trimming, 01h: trimming
c_encADCFSBit:       	DS      1		  	; 0: 16-bit A/D	, 80h: 21-bit A/D
c_encADCFifoLevel:		DS		1			; 0: 14 of 16,01h: 10 of 16, 02h: 6 of 16, 03h: 2 of 16
c_encADCDigitalGain:    DS      1			; 0: 0 db, 40h: +6db, 60h: +12db, c0h: reserve, adc digital gain
c_encInitToZeroEnd:              
;------------------------------------------------------------------------------
; Add constant (typeless) numbers here.
;------------------------------------------------------------------------------
C_ENCINITPARALEN		EQU 	c_encInitToZeroEnd - c_encInitToZeroStart

NOSOURINPUTERR	EQU		0F7H
PENDINGERR		EQU		0F5H
;------------------------------------------------------------------------------
; BIT CONTROL VARS
;------------------------------------------------------------------------------
				RSEG	?BT?ENC_BIT_VAR  ;
c_encStatusFlag:			DBIT  1 			//0x00:正常,0x01:异常.
c_encStartRecordFlag:	DBIT  1 			//0x00:未开始,0x01:开始.
c_encPauseFlag:				DBIT	1			//0X00:正常,0x01:暂停.
c_encStopFlag:				DBIT	1			//0X00:正常,0x01:退出.
c_encWriteFHDRFlag:		DBIT	1			//0X00:缺文件头,0x01:已写文件头.
c_encReadBlockFlag:		DBIT	1			//0x00: B1, 0x01: B2
c_encIsBufferFullFlag:DBIT	1			//0X00:未满,0x01:已满.
c_encInputSrcDAC:			DBIT	1			//input source is dac
c_encInputSrcFM:			DBIT	1			//input source is FM
c_encInputSrcLine:		DBIT	1			//input source is Line
c_encInputSrcMic:			DBIT	1			//input source is Mic
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------

;******************************************************************************
												RSEG    MW_CC_AD_ENC_B2 
;******************************************************************************
; Input parameters: none.
; Retrurn value: 0: false, 1: true
; Fuction description: 初始化codec，audio编码，并提前做好adc初始化的工作											
;******************************************************************************
encInit??:        
								
				mDI		 ;有跟中断程序同时访问的变量吗，若没有无须关中断。

//only for test
//        ld       a,MICIN
//        ld       (lwrd c_encInputSel),a
//        xor      a
//        ld       (lwrd  c_encFsIndex),a
//        ld       a,01
//        ld       (lwrd c_encChannelNum),a
//only for test

				PUSH    SFR_BANK

				//Store Regs if it is needed

				//initialization
				MOV		R11,#00H
				MOV		WR0,#C_ENCINITPARALEN			;initialize codec vars
				MOV		WR2,#c_encInitToZeroStart
InitVarToZero:
				MOV		@WR2,R11
				INC		WR2,#01H
				DEC		WR0,#01H
				CMP		WR0,#00H
				JNE		InitVarToZero
InitDataBuffVar:

				//INIT	BIT VARS
				CLR		c_encStatusFlag
				CLR		c_encStartRecordFlag
				CLR		c_encPauseFlag
				CLR		c_encStopFlag
				CLR		c_encWriteFHDRFlag
				CLR		c_encReadBlockFlag
				CLR		c_encIsBufferFullFlag
				CLR		c_encInputSrcDAC
				CLR		c_encInputSrcFM
				CLR		c_encInputSrcLine
				CLR		c_encInputSrcMic
				

StoreKeyInfo:
				//store memory mapping mode and interrupt configure
				
				LCALL	InitVars

    			mov     SFR_BANK,#BANK_CMU_RMU
				setb    MemBankCtl.0            ; Memory Banking Enable
				setb    MemBankCtl.3            ; pcmram and muram2 is aligned to 009000h in data space				

//    			clr		MRCR1.7
//				clr		MRCR2.5
//				clr		MRCR2.4
//				clr		MCSR1.7
//				clr     MCSR1.6
//				clr    	MCSR3.2

				MOV     R0,#32
				DJNZ    R0,$
 
				setb    MRCR1.7                 ; wakeup auip
				setb    MRCR2.5                 ; enable adc
				setb    MRCR2.4                 ; enable dachh
				setb    MCSR1.7                 ; set audio ip clock
				clr     MCSR1.6
				setb    MCSR3.2                 ; set dac clock

			    clr     MCSR5.2                 ; set pcmram clock
			    setb    MCSR5.1
			    clr     MCSR6.3                 ; set muram2 clock
			    setb    MCSR6.2

				LCALL	InitADCPara
				MOV		WR10,#c_encADCInitPara
				//mADC_Enaable

				LCALL	Init_ADC				


				LCALL InitDSPPara

			  MOV		R11,#1
encInitExit:

				//Restore Regs if it is needed
				POP    SFR_BANK

				mEI

       ERET
InitVars:

				//CHECK PARAMETRE IS RGIHT
				MOV		WR0,c_encWriteFileCurSize
				MOV		c_encWriteFileMAXSize,WR0				


				SETB	c_encReadBlockFlag	  				;BLOCK2 IS THE FIRST BLOCK
				
				MOV		WR0,#ENC_BUFFER_START
				MOV		c_encWriteTempBufferAddr,WR0
				MOV		c_encReadTempBufferAddr,WR0
				MOV		WR0,#0
				MOV		c_encTempBuffByteRemain,WR0

				MOV		DR0,#WORD0(SampleRateTbl)
				MOVH	DR0,#WORD2(SampleRateTbl)
				MOV		WR4,#0
				MOV		R6,#0
				MOV		R7,c_encFsIndex
				ADD		DR0,DR4
				ADD		DR0,DR4
				MOV		WR6,@DR0
				MOV		WR4,#0
				MOV		c_encSamplerate,WR6
				SRL		WR6
				
				MOV		WR2,#1024
				MOV		R0,c_encChannelNum
				CMP		R0,#02
				JNE		IsSingleChannel
				MOV		WR2,#2048
				ADD		WR6,WR6
IsSingleChannel:				
				MOV		c_encBytesPerBlock,WR2				
				MOV		c_encBytesPerSec,DR4
				MOV		c_encByteNeedForNextSec,DR4
								
				MOV		R0,c_encEncodeMode
				ANL		R0,R0
				JE		IsADPCMMode
				//PCM ENCODING?
				MOV		DR4,c_encBytesPerSec									;BITRATE * 4
				ADD		DR4,DR4
				ADD		DR4,DR4
				MOV		c_encBytesPerSec,DR4
				MOV		c_encByteNeedForNextSec,DR4

IsADPCMMode:
 				MOV		R0,c_encAVRFlag
				ANL		R0,R0
				JE		InitVarsExit
				MOV		R0,c_encAVRDelay
				MOV		DR4,#0
				MOVH	DR4,#0
				MOV		DR8,c_encBytesPerSec
GetTotalBytesInPeriod:
				ADD		DR4,DR8
				DEC		R0,#01H
				JNE		GetTotalBytesInPeriod
				MOV		WR0,#0
				MOV		WR10,c_encBytesPerBlock
				MOV		WR8,#0
GetSilenceCntMAX:
				INC		WR0,#01H
				SUB		DR4,DR8
				JSG		GetSilenceCntMAX
				MOV		c_encSilenceCntMAX,WR0

InitVarsExit:		
			
				RET
InitADCPara:
				MOV		R0,c_encInputSel
				ANL		R0,#03H
				MOV		c_encADCInputSel,R0

				MOV		R0,c_encChannelNum
				MOV		R1,#0CH
				CMP		R0,#01H
				JNE		SetADCChannelNum
				MOV		R1,#08H
SetADCChannelNum:				
				MOV		c_encADCChannel,R1

				MOV		DR0,#WORD0(SampleRateTable)
				MOVH	DR0,#WORD2(SampleRateTable)
				MOV		WR4,#0
				MOV		R6,#0
				MOV		R7,c_encFsIndex
				ADD		DR0,DR4
				MOV		R4,@DR0
				MOV		c_encADCSampleRate,R4			

				MOV		R0,c_encInputGain
				ANL		R0,#07H
				MOV		c_encADCInputGain,R0								

				MOV		R0,#01
				MOV		c_encADCTrimmingFlag,R0		 //需要校正.

				MOV		R0,#0
				ANL		R0,#080H
				MOV		c_encADCFSBit,R0		 	//仅支持16bit.

				MOV		R0,c_encADCGain				//0,1,2,3
				ANL		R0,#03H
				SRL		R0
				SRL		R0
				SRL		R0
				SRL		R0
				MOV		c_encADCLPFGain,R0

				MOV		R0,#0
				MOV		c_encADCFifoLevel,R0		//14 级 level.

				MOV		R0,c_encDSPGain
				ANL		R0,#03H
				MOV		c_encADCDigitalGain,R0		//0,1,2

				MOV		R0,#20H
				MOV		c_encADCFifoAccess,R0		//AHC ACCESS THE FIFO

				RET

InitDSPPara:
		  //we need to make a dsp bin for this driver code

//        ld      de,lwrd c_decDSPProgName
//        ld      a,(lwrd c_decSongType)
//        cp      MP3TYPE
//        jr      nz,LoadDSPName
//        ld      de,lwrd c_decMP3DSPName
//LoadDSPName:
//        ld      c,a                            //songtype
//
//        ld      hl,00h                         //decode mode
//        push    hl
//        mInitDSPCodeExp
//        pop     hl
			   
			   
			   //Reset DSP module
; initial reset and clock of audio ip    
//AudioIpInit:
	//not need in the whole system verification			   

    mov     SFR_BANK,#BANK_CMU_RMU
//    clr    	MRCR1.7                 ; reset auip
//    mov     MRCR1,#0
//    mov     MRCR2,#0
//    mov     MRCR3,#0
//    mov     r0,#32
//    djnz    r0,$
//    setb    MRCR1.7                 ; wakeup auip
//    setb    MRCR2.5                 ; enable adc
//    setb    MRCR2.4                 ; enable dachh
//    setb    MCSR1.7                 ; set audio ip clock
//    clr     MCSR1.6
//    setb    MCSR3.2                 ; set dac clock
//    clr     MCSR5.2                 ; set pcmram clock
//    setb    MCSR5.1
//    clr     MCSR6.3                 ; set muram2 clock
//    setb    MCSR6.2

	clr		MCSR8.7
	setb	MCSR8.6
	clr		MCSR8.5	
 	clr		MCSR8.4
	setb	MCSR8.3
	clr		MCSR8.2					;set muram1 low and mid clock to mcu
		
	;clr     MRCR2.0                 ; reset DMA0/1/2 
    setb    MRCR2.0                 ; enable DMA0/1/2
	setb    MCSR4.5                 ; enable DMA0/1/2 clock


//    acall   Init_ADC
//    acall   AHC_init                ; audio hardcore init			   

//open and reset audio ip
//		    mov     SFR_BANK,#BANK_CMU_RMU
//			clr	    MRCR1.7					;reset auip module
//    		setb    MRCR1.7                 ; wakeup auip module
//    		setb    MCSR1.7                 ; set audio ip clock

;**************************************************************  	
; set mode and start encoding or decoding
//AHC_init:

    mov     SFR_BANK,#BANK_AUIP 
    mov     AuCodecCtl,#0b1h        ; asynchronous reset
    mov     AuCodecCtl,#31h         ; adpcm encoding mode
    mov     AuCodecCtl,#0b1h
    clr     a
    mov     AuCodecIE,a             ; clear audio interrupt enable register
//    mov     BEPCtl3,a               ; disable mute
    mov     AuCodecIP,#0ffh         ; clear audio interrupt pending register
    mov     AuCodecFIFOCtl,#25h     ; set drq and fiforeset 
    mov     AuCodecFIFOCtl,#24h 
    mov     AuCodecFIFOCtl,#25h
    
    
//#ifdef AUIP_DEBUG
//    mov     AuCodecDebug,#0c0h      ; enable debug mode
//    mov     AuDecEncCtl,#0c0h       ; generate newfile signal and enable audio decoding or encoding
                                    ; continue -- adpcm encoding, dual channels
//#else
//    mov     AuDecEncCtl,#0d8h       ; generate newfile signal and enable audio decoding or encoding
                                    ; continue -- pcm encoding, dual channels
//#endif

			   

			RET

;************************************************************** 
//tables
;wavhead:		 DB 		52h,49h,46h,46h,00h,00h,00h,00h,57h,41h,56h,45h,66h,6dh,74h,20h;
;                DB 		0e4h,01h,00h,00h,11h,00h,02h,00h,80h,0bbh,00h,00h,80h,0bbh,00h,00h;
;                DB 		00h,08h,04h,00h,02h,00h,0f9h,07h;
;                DB 		64h,61h,74h,61h,00h,00h,00h,00h;
SampleRateTable:	DB		8,11,12,16,22,24,32,44,48
SampleRateTbl:		DW		8000,11025,12000,16000,22050,24000,32000,44100,48000 
//c_encDSPProgName:	DB      'AUDIO.DSP' 
;************************************************************** 
; set ADC
Init_ADC:
	push    SFR_BANK
	mov     SFR_BANK,#04h	        ; DAC bank:04h	
#ifdef ADC_D_DEBUG
	setb    DAC_CTL3.4              ; enable ADC_D_DEBUG mode
#endif
	acall   set_adc_sr	
	acall   analog_input_sel
#ifdef FPGA		
	orl     ADC_CTL,#Adc_Channel    ; set adc channel
#else
	mov		r0,ADC_CTL
	ANL		r0,#0f3h
	mov		r1,c_encADCChannel    ; set adc channel
	orl		r0,r1
	mov		ADC_CTL,r0
#endif	
	;orl     ADC_GCTL,#ADC_bits      ; set bus width fpga 验证暂时不用
	mov     ADC_FIFOCTL1,#20h       ; ahc access adc fifo, adc fifo source adc	
	mov     ADC_FIFOCTL0,#(0c8h+Fifo_level)
	                                ; clr irq/drq, enable irq/drq, set fifo_level	
	setb    ADC_FIFOCTL0.2          ; enable ADC FIFO
	pop     SFR_BANK	
	ret 
;**************************************************************   
analog_input_sel:
#ifdef FPGA	
	mov     r0,#InputSel
#else
	mov     r0,c_encADCInputSel
#endif	
	
FM_s:
	cmp     r0,#01h
	jne     MIC_s
	orl     FMOP_CTL,#18h           ; enable fm lr	
//#ifdef  vmic
	setb    MICOP_CTL.7
//#endif	
#ifdef  mic_trim
	setb    MICOP_CTL.6             ; mic offset trimming enable
#endif
	jmp     inp_sel_ret	
MIC_s:	
	cmp     r0,#00h
	jne     inp_sel_ret
	orl     MICOP_CTL,#18h          ; enable mic lr
	//jmp     inp_sel_ret
inp_sel_ret:
	mov 	a,ADC_CTL
	anl		a,#0fch
	orl		a,r0
	mov		ADC_CTL,a       ; select ADC source
	ret
;**************************************************************
set_adc_sr:
	push    SFR_BANK
	mov     SFR_BANK,  #04h	        ; DAC bank:04h
#ifdef FPGA
	mov     r0,#SamplRate
#else
	mov     r0,c_encADCSampleRate
#endif	
cmp_48:
	cjne    r0,#48,cmp_44
	mov     r0,#05h
	mov     r1,#00h
	jmp     set_sr
cmp_44:
	cjne    r0,#44,cmp_32
	mov     r0,#05h
	mov     r1,#08h
	jmp     set_sr
cmp_32:
	cjne    r0,#32,cmp_24
	mov     r0,#04h
	mov     r1,#00h
	jmp     set_sr
cmp_24:
	cjne    r0,#24,cmp_22
	mov     r0,#03h
	mov     r1,#00h
	jmp     set_sr
cmp_22:
	cjne    r0,#22,cmp_16
	mov     r0,#02h
	mov     r1,#08h
	jmp     set_sr
cmp_16:
	cjne    r0,#16,cmp_12
	mov     r0,#02h
	mov     r1,#00h
	jmp     set_sr
cmp_12:
	cjne    r0,#12,cmp_11
	mov     r0,#01h
	mov     r1,#00h
	jmp     set_sr
cmp_11:
	cjne    r0,#11,cmp_8
	mov     r0,#01h
	mov     r1,#08h
	jmp     set_sr
cmp_8:
	cjne    r0,#8,sr_err
	mov     r0,#00h
	mov     r1,#00h
	jmp     set_sr	
sr_err:
	jmp     $
set_sr:
    sll     r0
    sll     r0
    sll     r0
    sll     r0
	mov 	a,ADC_CTL
	anl		a,#08fh
	orl		a,r0
	mov     ADC_CTL,a                ; set ADC sample rate
	;set PLL series
set_audiopll:
	mov     SFR_BANK,  #01h	         ; OSC&PLL bank:01h
	
	mov		a,AUDIOPLL_CTL
	anl		a,#0f7h
	orl		a,r1
	orl		a,#01h					; enable AUDIOPLL, r1: sr series
	mov     AUDIOPLL_CTL,a
	
	pop     SFR_BANK

	ret
;************************************************************* 

;******************************************************************************
												RSEG    MW_CC_AD_ENC_B1 
;******************************************************************************

encStart??:       
				PUSH	SFR_BANK

				LCALL 	StartDSP

				POP		SFR_BANK
				
				MOV		R11,#01
				
        ERET

;******************************************************************************
encContinue??:	
			  	CLR		c_encPauseFlag
			  	SETB	AIE.5
				ERET	
;******************************************************************************
encPause??:
				CLR		AIE.5
				SETB	c_encPauseFlag
				ERET
;******************************************************************************
encStop??:					
				CLR		AIE.5
				SETB	c_encStopFlag
				ERET	
;******************************************************************************										
encExit??:

				PUSH	SFR_BANK

				//DISABLE RTC TIMER INTERRUPT
				//CLR		IE0.3
					
				LCALL		StopDSP
								
//			mADC_Disable
//			CLR		ADC_FIFOCTL1.5			//5:4 ADCFSS 1X: AHC

				POP		SFR_BANK

				//restore memory mapping and interrupt configure

				MOV		R11,#1

				ERET
;******************************************************************************
StartDSP:
				
				mov     SFR_BANK,#BANK_AUIP
				
				mov		AuDecEncCtl,#0
				
				mov		r11,c_encChannelNum
				cmp		r11,#02h
				je		IsDualChEnc
				setb	AuDecEncCtl.5						//momo channel		
IsDualChEnc:
				
				mov		r11,c_encEncodeMode
				anl		r11,r11
				je		IsAdpcmEnc
				setb	AuDecEncCtl.4						//pcm encode mode	
IsAdpcmEnc:
				
				setb	AuDecEncCtl.3						//select adc fifo for input
				
				mov		r0,c_encDelayTime
				anl		r0,r0
				jne		NeedToAvoidNoise				
				
				SETB		c_encStartRecordFlag

				mov			a,AuDecEncCtl
				orl			a,#0c0h
				mov			AuDecEncCtl,a

NeedToAvoidNoise:
								
				mov		AuCodecIE,#03h						;ena enable B1Rdy and B2Rdy interrupt
	
				clr     AIPH.5                  ; set audioip interrupt to lowest level
				clr     AIPL.5   
				setb    AIE.5                   ; enable audioip interrupt

				RET				
;******************************************************************************
StopDSP:

				MOV     SFR_BANK,#BANK_AUIP	
								
				MOV   AuCodecIP,#0ffH				 ;clear all codec pending bit
				MOV		AuCodecIE,#00H				;disable all codec interrupt
				
				CLR		AIE.5						 ;DISABLE Audio/video codec Interrupt		

				//stop and close audio ip
			  MOV     SFR_BANK,#BANK_CMU_RMU
	 	   	CLR    MCSR1.7                 ; disable audio ip clock
				CLR	   MRCR1.7								;reset auip module
				
				RET
;******************************************************************************
encGetBitstream??:


				mDI
				MOV		R11,#0
				MOV		c_encGetBSResult,R11
					
				JNB		c_encStopFlag,NotFileEnd
					
				MOV		WR0,c_encTempBuffByteRemain
				ANL		WR0,WR0
				JNE		BufferNotEmpty

				JB		c_encWriteFHDRFlag,encGetBitstream_exit
					
				LCALL	WriteWaveFileHeader
				
				MOV		R11,#02H
				MOV		c_encGetBSResult,R11
				
				MOV		WR4,#512
					
				LJMP	WriteOutToFile
					
NotFileEnd:
BufferNotEmpty:
						
				MOV		WR0,c_encTempBuffByteRemain
				ANL		WR0,WR0
				JE		encGetBitstream_exit	
				
				LCALL	OutputEnergy				

				MOV		R11,#01H
				MOV		c_encGetBSResult,R11
				
//				MOV		WR4,#ENC_WRITE_FILE_SIZE
				MOV		WR4,c_encWriteFileMAXSize
					
WriteOutToFile://NEED WR4 FOR SIZE

				MOV		c_encWriteFileCurSize,WR4
//				MOV		WR0,c_encReadTempBufferAddr
//				MOV		c_encWriteFileBuffAddr,WR0

				MOV		WR10,c_encWriteFileBuffAddr
				MOV		WR8, c_encReadTempBufferAddr
				MOV		WR6,c_encWriteFileCurSize
				DEC		WR6,#01h
				LCALL   mDMATransfer_1
				
				MOV		WR0,c_encReadTempBufferAddr
				MOV		WR4,c_encWriteFileCurSize
				ADD		WR0,WR4
				MOV		WR6,#ENC_BUFFER_END
				SUB		WR6,WR0
				JNE		NotEncBufferEnd_1
				MOV		WR0,#ENC_BUFFER_START
NotEncBufferEnd_1:
				MOV		c_encReadTempBufferAddr,WR0
								
				MOV		WR2,c_encTempBuffByteRemain			
				SUB		WR2,WR4
				MOV		c_encTempBuffByteRemain,WR2


					

encGetBitstream_exit:
				mEI

				MOV		R11,	c_encGetBSResult

				ERET
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
////必须确保该缓冲区是mcu或dma012能访问的
//Input:
//WR10:	DST	ADDR
//WR8:  SRC ADDR
//WR6: SIZE
//Output:
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
mDMATransfer_1:

				PUSH    SFR_BANK

#if AUENC_DMA_MODE == AUENC_USE_DMA1
				MOV     SFR_BANK,#BANK_DMA1     ; access dma x register group
#else
				MOV     SFR_BANK,#BANK_DMA2     ; access dma x register group
#endif

				MOV		DMAxCTL0,#054H						;abort DMA transfer, SET UP COUNT, MEMORY SELECT

				MOV		DMAxDADDR,WR10
				MOV		DMAxSADDR,WR8
				MOV		DMAxSZ,WR6
				
				CLR		DMAxIP.4							;src and dst is data memory
				CLR		DMAxIP.3
				
				MOV		DMAxCTL1,#01H						;ENA DMA TRANSFER AND DIS ALL INTERRUPT

WaitDMATSFin_1:
				JB		DMAxCTL1.0,WaitDMATSFin_1

				POP		SFR_BANK
mDMATransfer_1_Exit:
				RET

;******************************************************************************
WriteWaveFileHeader:

				MOV		R11,#00H
				MOV		WR0,#512			
				MOV		WR2,c_encWriteTempBufferAddr
ClearFileHeader:
				MOV		@WR2,R11
				INC		WR2,#01H
				DEC		WR0,#01H
				ANL		WR0,WR0
				JNE		ClearFileHeader

      	//modify wav file header
				MOV		WR2,c_encWriteTempBufferAddr
				MOV		WR4,#5249h
				MOV		WR6,#4646h
				MOV		@WR2,WR4
				MOV		@WR2+2,WR6												//"RIFF"
				
				MOV		DR4,c_encByteshaveEncode
				MOV		WR10,#(512-8)
				MOV		WR8,#0
				ADD		DR4,DR8															
				MOV		@WR2+4,R7
				MOV		@WR2+5,R6
				MOV		@WR2+6,R5
				MOV		@WR2+7,R4											   //c_encFileChunkSize	LITTLE ENDIAN	
				
				MOV		WR4,#5741h
				MOV		WR6,#5645h
				MOV		@WR2+8,WR4
				MOV		@WR2+10,WR6												//"WAVE"
				
				MOV		WR4,#666dh
				MOV		WR6,#7420h
				MOV		@WR2+12,WR4
				MOV		@WR2+14,WR6												//"fmt "				
				
				MOV		WR4,#0e401h
				MOV		@WR2+16,WR4
																								//fomat chunk size		
				MOV		R4,#11h	
				MOV		R5,c_encEncodeMode
				ANL		R5,R5
				JE		IsADPCMEncode_1
				MOV		R4,#01h														//how about pcm wav file? 0
IsADPCMEncode_1:					
				MOV		@WR2+20,R4												//WAV TYPE: ADPCM
				
				MOV		R4,c_encChannelNum														
				MOV		@WR2+22,R4												//nChannels, LITTLE ENDIAN
				
				MOV		WR4,c_encSamplerate
				MOV		@WR2+24,R5
				MOV		@WR2+25,R4											//Samplerate, LITTLE ENDIAN		
				
				MOV		DR4,c_encBytesPerSec
				MOV		@WR2+28,R7
				MOV		@WR2+29,R6
				MOV		@WR2+30,R5
				MOV		@WR2+31,R4											//nAvgBytesPerSec, LITTLE ENDIAN
				
				MOV		WR4,c_encBytesPerBlock
				MOV		R6,#04h	
				MOV		R7,c_encEncodeMode
				ANL		R7,R7
				JE		IsADPCMEncode_2
				MOV		WR4,#0004h
				MOV		R6,#10h												//how about pcm wav file? 4
IsADPCMEncode_2:															//how about pcm wav file? 16
				MOV		@WR2+32,R5
				MOV		@WR2+33,R4											//nBlockAlign, LITTLE ENDIAN								
				MOV		@WR2+34,R6											//wBitsPerSample
				
				MOV		R4,#02h
				MOV		@WR2+36,R4											 //cbSize		
				
				MOV		WR4,#0f907h
				MOV		@WR2+38,WR4											//wSamplesPerBlock, LITTLE ENDIAN																
				
				;fact chunck is not nessary
				;dwSamplesLength = c_encByteshaveEncode / channels not neccesary	
				
				MOV		WR4,#6461h
				MOV		WR6,#7461h
				MOV		@WR2+504,WR4
				MOV		@WR2+506,WR6												//"data"				

				MOV		DR4,c_encByteshaveEncode														
				MOV		@WR2+508,R7
				MOV		@WR2+509,R6
				MOV		@WR2+510,R5
				MOV		@WR2+511,R4											   //c_encDataChunkSize	LITTLE ENDIAN	
			
				MOV		WR4,c_encWriteTempBufferAddr
				MOV		WR6,#512
				ADD		WR4,WR6
				MOV		c_encWriteTempBufferAddr,WR4				
				MOV		c_encTempBuffByteRemain,WR6

				SETB	c_encWriteFHDRFlag


				RET
;******************************************************************************
OutputEnergy:
				MOV		R0,c_encEnergyOut
				ANL		R0,R0
				JE		OutputEnergyExit

				MOV		WR8,#ENC_BUFFER_START
				MOV		WR4,#0
				MOV		WR6,#0
OutputEnergyLoop:				
				MOV		WR0,#0
				MOV		R4,@WR8
				MOV		R3,@WR8+1
				ADD		DR4,DR0
				MOV		WR10,c_encBytesPerBlock
				ADD		WR8,WR10
				MOV		WR10,#ENC_BUFFER_END
				SUB		WR10,WR8
				JSG		OutputEnergyLoop
				
				MOV		R1,R6
				MOV		R0,R5
				SRA		WR0
				SRA		WR0
				SRA		WR0					
				MOV		WR2,c_encBytesPerBlock
				CMP		WR2,#2048
				JE		GetEnergyLevel
				SRA		WR0					// SUM / 8(16) / 256
GetEnergyLevel:
				MOV		c_encEnergyLevel,WR0	;8(16) block 移动平均值

OutputEnergyExit:

				RET			
;******************************************************************************


;******************************************************************************
												RSEG    MW_CC_AD_ENC_R 
;******************************************************************************
encTimerIntSub?_: 
			  PUSH	DR0
			  
				JB			c_encStartRecordFlag,NowCanRecord		   
				MOV			R0,c_encDemolishKeyVoiceTming					;消去按键的啪啪声，延迟320ms
				INC			R0,#01H
				MOV			c_encDemolishKeyVoiceTming,R0
				MOV			R1,c_encDelayTime
				CMP			R0,R1
//				CMP			R0,#010H
				JSL			encTimerIntSubExit

				SETB		c_encStartRecordFlag							;开始录音标志置位, 保留在这里，将来有需要再开启.
				
				PUSH		SFR_BANK
				MOV     	SFR_BANK,#BANK_AUIP
				MOV			a,AuDecEncCtl
				ORL			a,#0c0h
				MOV			AuDecEncCtl,a
				POP			SFR_BANK

				LJMP		encTimerIntSubExit
NowCanRecord:
					
				JB			c_encStopFlag,encTimerIntSubExit
				JB			c_encPauseFlag,encTimerIntSubExit
				JNB			c_encIsBufferFullFlag,encTimerIntSubExit

				MOV			WR0,c_encBytesPerBlock
				MOV			WR2,c_encTempBuffByteRemain
				ADD			WR0,WR2
				CMP			WR0,#ENC_BUFFER_LENGTH
				JSG			encTimerIntSubExit				
	      		CLR			c_encIsBufferFullFlag
				SETB		AIE.5
	      		//OPEN AUDIO PENDING BIT

encTimerIntSubExit:
				
				POP		DR0
				
        		RET
;******************************************************************************
encAudioIntSub?_: 
				PUSH	DR0
				PUSH	DR4
				PUSH	DR8
				
				PUSH	SFR_BANK
				MOV     SFR_BANK,#BANK_AUIP

				MOV		R0,WAVEncIP
				ANL		R0,R0
				JNE		GetPendingBit
				LJMP	EncoderIntSubexit
GetPendingBit:
				CMP		R0,#03H
				JNE		OnlyOneBlockIsFull
				MOV		WR0,c_encB1B2AllFullNum
				INC		WR0,#01H
				MOV		c_encB1B2AllFullNum,WR0
OnlyOneBlockIsFull:

				JNB		c_encStopFlag,NotStopFlag
				//mask  auip pending
				clr		AIE.5
				LJMP	EncoderIntSubexit
NotStopFlag:				
				JNB		c_encPauseFlag,NotPauseFlag
				//mask  auip pending
				clr		AIE.5
				LJMP	EncoderIntSubexit
NotPauseFlag:
				MOV		WR0,c_encBytesPerBlock
				MOV		WR2,c_encTempBuffByteRemain
				ADD		WR0,WR2
				CMP		WR0,#ENC_BUFFER_LENGTH
				JSG		BufferIsFull

				MOV		WR10,c_encWriteTempBufferAddr						 //dst
				MOV		WR6,c_encBytesPerBlock									//size
				DEC		WR6,#01H				
								
				JB		c_encReadBlockFlag,CheckIsBlock2Full
CheckIsBlock1Full:
				JB      WAVEncIP.1,GetBlock1StartAddr
				LJMP	WrongPendingStatus
GetBlock1StartAddr: //at pcmram				
				
				MOV		WR8,#ENC_BLOCK1_START										//src
				
				MOV     SFR_BANK,#BANK_CMU_RMU
				SETB    MCSR5.2                 							; set pcmram clock to mcu
				CLR     MCSR5.1				
				
				LCALL MergeThisBlock
				
//				LCALL	mDMATransfer																//TRANSFER THIS BLOCK
				
				CLR     MCSR5.2                 							; set pcmram clock to audio
			  SETB    MCSR5.1				
			  
				MOV     SFR_BANK,#BANK_AUIP
				
				MOV		R0,#02H
				MOV		WAVEncIP,R0
				//SETB  WAVEncIP.1	
				
				SETB  c_encReadBlockFlag												//NEXT IS BLOCK 2 
				LJMP	TransferCurrentBlock
CheckIsBlock2Full://at muram2
				JB      WAVEncIP.0,GetBlock2StartAddr
				LJMP	WrongPendingStatus
GetBlock2StartAddr:
				
				MOV		WR8,#ENC_BLOCK2_START										//src
				
				MOV     SFR_BANK,#BANK_CMU_RMU
				SETB   MCSR6.3                 								; set muram2 clock to mcu
				CLR    MCSR6.2
				
				LCALL	mDMATransfer																//TRANSFER THIS BLOCK

				CLR   MCSR6.3                									; set muram2 clock to audio
				SETB  MCSR6.2 
			  
				MOV     SFR_BANK,#BANK_AUIP	
				
				MOV		R0,#01H
				MOV		WAVEncIP,R0		
				//SETB  WAVEncIP.0	
				CLR		c_encReadBlockFlag												//NEXT IS BLOCK 1 
TransferCurrentBlock:

				LCALL	EnergyDectection				

				LCALL	UpdateEncStatus
				
EncoderIntSubexit:

			 	POP		SFR_BANK
			 	POP		DR8
			 	POP		DR4
				POP		DR0
				
        RET
BufferIsFull:
	     		CLR		AIE.5 
				SETB  c_encIsBufferFullFlag
	      		//MASK AUDIO PENDING BIT

				MOV		WR0,c_encPCMLostNum
				INC		WR0,#01H
				MOV		c_encPCMLostNum,WR0					//PCM被丢弃的次数, 暂停的不算 .	      	      
	     		LJMP	EncoderIntSubexit		        
WrongPendingStatus:
				MOV		R11,#PENDINGERR
				MOV		c_encStatus,R11
				LJMP	EncoderIntSubexit	        
        
;******************************************************************************
EnergyDectection:
				MOV		WR0,c_encWriteTempBufferAddr
				MOV		R3,@WR0
				MOV		R2,@WR0+1
//				MOV		WR4,c_encLeftOffset		 		;修正直流电平偏直，注意大小端.
//				SUB		WR2,WR4
				MOV		R0,R2
				ANL		R0,#80
				JE		IsPositive
				MOV		WR4,#0000H
				SUB		WR4,WR2
				MOV		WR2,WR4
IsPositive:
				MOV		c_encCurBlockPCmVal,WR2	   		;ABS VALUE

				MOV		R0,c_encAVRFlag
				ANL		R0,R0
				JE		EnergyDectectionExit				;NO SHENGKONG OR FENQU
				
				MOV		WR4,c_encAVRLevel
				MOV		WR2,c_encCurBlockPCmVal
				SUB		WR4,WR2
				JSG		EnergyBelowThreshold
				MOV		WR0,#0
				MOV		c_encAVRStatus,R0				;CLEAR SHENGKONG FLAG
				//INIT SOME VARS
				MOV		c_encSilenceCnt,WR0
				LJMP	EnergyDectectionExit
EnergyBelowThreshold:
				MOV		WR0,c_encSilenceCnt
				INC		WR0,#01H
				MOV		c_encSilenceCnt,WR0
				MOV		WR2,c_encSilenceCntMAX
				SUB		WR2,WR0
				JNC		EnergyDectectionExit			;DETECT LONG PERIOD SILENCE		
				MOV		R0,c_encAVRFlag
				MOV		c_encAVRStatus,R0
				CMP		R0,#01H
				JE		IsShengKong
IsFenQu:
				CLR		AIE.5
				SETB	c_encPauseFlag					;WAIT TO EXIT
//				LJMP	EnergyDectectionExit
IsShengKong:				
EnergyDectectionExit:

				RET
 ;******************************************************************************
UpdateEncStatus:
				MOV		R0,c_encAVRStatus
				CMP		R0,#01H
				JNE		NotShengKongStatus
				LJMP	UpdateEncStatusExit					  ;drop the bitstream data

NotShengKongStatus:
				MOV		WR0,c_encBytesPerBlock
				MOV		WR2,c_encTempBuffByteRemain
				ADD		WR0,WR2
				MOV		c_encTempBuffByteRemain,WR0								

				MOV		WR0,c_encWriteTempBufferAddr						
				MOV		WR2,c_encBytesPerBlock									
				ADD		WR0,WR2
				MOV		WR2,#ENC_BUFFER_END
				SUB		WR2,WR0
				JNE		NotEncBufferEnd
				MOV		WR0,#ENC_BUFFER_START
NotEncBufferEnd: 
				MOV		c_encWriteTempBufferAddr,WR0
				
				MOV		DR0,c_encByteshaveEncode
				MOV		WR6,c_encBytesPerBlock
				MOV		WR4,#0
				ADD		DR0,DR4
				MOV		c_encByteshaveEncode,DR0
					
				MOV		DR0,c_encByteNeedForNextSec
				MOV		WR6,c_encBytesPerBlock
				MOV		WR4,#0
				SUB		DR0,DR4
				MOV		c_encByteNeedForNextSec,DR0
				JSG		UpdateDisplayTimeEnd
				MOV		DR4,c_encBytesPerSec
				ADD		DR0,DR4
				MOV		c_encByteNeedForNextSec,DR0
					
				MOV		R0,c_encCurTime+3											//SECOND
				INC		R0,#01H
				MOV		c_encCurTime+3,R0
				SUB		R0,#60
				JNE		UpdateDisplayTimeEnd
				MOV		c_encCurTime+3,R0
				MOV		R0,c_encCurTime+2											//MINITUE
				INC		R0,#01H
				MOV		c_encCurTime+2,R0
				SUB		R0,#60
				JNE		UpdateDisplayTimeEnd
				MOV		c_encCurTime+2,R0	
				MOV		R0,c_encCurTime+1											//HOUR
				INC		R0,#01H
				MOV		c_encCurTime+1,R0													
UpdateDisplayTimeEnd:

UpdateEncStatusExit:
				RET

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
//Input:
//WR10:	DST	ADDR
//WR8:  SRC ADDR
//WR6: SIZE
//Output:
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
MergeThisBlock:
				MOV		R0,c_encEncodeMode
				ANL		R0,R0
				JNE		MergeThisBlockForPCM
				
				INC		WR6,#01H		 //WR6: SIZE
				SRA		WR6				 //>>1						
MovNextSample:
				MOV		R0,@WR8			  //WR8: SRC
				MOV		R1,@WR8+2
				MOV		@WR10,WR0 			//WR10: DST
				INC		WR10,#02H
				INC		WR8,#04H
				DEC		WR6,#01H
				ANL		WR6,WR6
				JNE		MovNextSample

MergeThisBlockExit:
												
				RET		
						
MergeThisBlockForPCM:
				INC		WR6,#01H		 //WR6: SIZE
				SRA		WR6				 //>>1						
MoveNextPCMInB1:
				MOV		R1,@WR8			  //WR8: SRC
				MOV		R0,@WR8+2		  //LITTLE ENDIAN
				MOV		@WR10,WR0    	//WR10: DST
				INC		WR10,#02H
				INC		WR8,#04H
				DEC		WR6,#01H
				ANL		WR6,WR6
				JNE		MoveNextPCMInB1


				LJMP  MergeThisBlockExit	


				
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
//Input:
//WR10:	DST	ADDR
//WR8:  SRC ADDR
//WR6: SIZE
//Output:
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
mDMATransfer:
				MOV		R0,c_encEncodeMode
				ANL		R0,R0
				JNE		mDMATransferForPCM

				PUSH    SFR_BANK

#if AUENC_DMA_MODE == AUENC_USE_DMA1
				MOV     SFR_BANK,#BANK_DMA1     ; access dma x register group
#else
				MOV     SFR_BANK,#BANK_DMA2     ; access dma x register group
#endif

				MOV		DMAxCTL0,#054H						;abort DMA transfer, SET UP COUNT, MEMORY SELECT

				MOV		DMAxDADDR,WR10
				MOV		DMAxSADDR,WR8
				MOV		DMAxSZ,WR6
				
				CLR		DMAxIP.4							;src and dst is data memory
				CLR		DMAxIP.3
				
				MOV		DMAxCTL1,#01H						;ENA DMA TRANSFER AND DIS ALL INTERRUPT

WaitDMATSFin:
				JB		DMAxCTL1.0,WaitDMATSFin

				POP		SFR_BANK
mDMATransferExit:
				RET
mDMATransferForPCM:
				INC		WR6,#01H		 //WR6: SIZE
				SRA		WR6				 //>>1						
MoveNextPCMInB2:
				MOV		R1,@WR8			  //WR8: SRC
				MOV		R0,@WR8+1
				MOV		@WR10,WR0    	//WR10: DST
				INC		WR10,#02H
				INC		WR8,#02H
				DEC		WR6,#01H
				ANL		WR6,WR6
				JNE		MoveNextPCMInB2


				LJMP  mDMATransferExit


;******************************************************************************
			end

