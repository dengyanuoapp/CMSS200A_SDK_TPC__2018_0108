#ifndef _IRQ_H
#define _IRQ_H

#include "actapi.h"

#define IRQ_INT0   0
#define IRQ_CTC    1
#define IRQ_INT1   2
#define IRQ_RTC	   3
#define IRQ_UART   4
#define IRQ_I2C	   5
#define IRQ_SPI	   6
#define IRQ_NMI	   7
#define IRQ_USB	   8
#define IRQ_DMA2   9
#define IRQ_DMA1   10
#define IRQ_DMA3   11
#define IRQ_ADC	   12
#define IRQ_DSP	   13
#define IRQ_DMA0   14
#define IRQ_MAX	   14

#define APINO_IRQ_Intercept  0x0000+API_IRQ_Base
#define APINO_IRQ_Release    0x0001+API_IRQ_Base

#ifdef __C251__

typedef struct
{
    uint8 isr_entry_h;
    uint8 isr_entry_m;
    uint8 isr_entry_l;
} isr_entry_t;

#define IRQ_Intercept(newisrptr,IRQType) 	API_IRQ_Intercept(APINO_IRQ_Intercept,newisrptr,IRQType)
#define IRQ_Release(oldisrptr,IRQType) 		API_IRQ_Release(APINO_IRQ_Release,oldisrptr,IRQType)

uint32 API_IRQ_Intercept(uint8 cmd, uint32 newisrptr, uint8 IRQType);
void API_IRQ_Release(uint8 cmd, uint32 oldisrptr, uint8 IRQType);
//#define terrupt()  EA=1;
//#define disable_interrupt() EA=0;


//设置MURAM地址和时钟
#define SetMemBankCtl_CODE(tmpval) 	{tmpval=SFR_BANK;SFR_BANK=BANK_CMU_RMU;MEMCLKSELCTL0&=0x3F;SFR_BANK=tmpval;tmpval=MemBankCtl;MemBankCtl_MURAM_ADDR = 0;}
#define SetMemBankCtl_DATA(tmpval) 	{tmpval=MemBankCtl;MemBankCtl_MURAM_ADDR = 1;}
#define RestoreMemBankCtl(tmpval)	{MemBankCtl=tmpval;}

//开关中断，清狗 宏定义
#define mEI()  			{(*((char data*)0x007f))--;if(!(*((char data*)0x007f))) EA=1;}
#define mDI() 			{EA=0; (*((char data*)0x007f))++;}
#define mCLRWD(tmpval) 	{tmpval=SFR_BANK;SFR_BANK=BANK_RTC;WDCTL |=0x08;SFR_BANK=tmpval;}
//开关中断，清狗 固化代码定义
void romDI(void);
void romEI(void);
void ClearWatchDog(void);

#define SYS_ClearWatchDog(tmpval) tmpval=SFR_BANK;SFR_BANK=BANK_RTC;WDCTL |=0x08;SFR_BANK=tmpval;

#endif /*__C251__ */


#ifdef __A251__
mIRQ_Intercept  macro
MOV      R11, #APINO_IRQ_Intercept
LCALL	 RSTSYSAPI
endm

mIRQ_Release    macro
MOV      R11, #APINO_IRQ_Release
LCALL	 RSTSYSAPI
endm

//开关中断，清狗 宏定义
mDI       macro
CLR	EA
INC	0x7F //IntFlag
endm

mEI       macro
local  _interrupt_disabled
DEC	0x7F //IntFlag
PUSH  R11
MOV   A, 0x7F
JNZ   _interrupt_disabled
SETB  EA
_interrupt_disabled:
POP R11
endm

mCLRWD	  macro
PUSH SFR_BANK
MOV  SFR_BANK, #BANK_RTC
SETB WDCtl.3
POP  SFR_BANK
endm


#endif /* __A251__ */

#endif /*_IRQ_H*/