/*
* Automatically generated register definition: don't edit
* GL5115 Spec Version_V1.00
* Sat 07-24-2012  12:30:06
*/

////////////////////////////////////////////////////////////////////////////////////////////////
// BANK NUMBER OF EACH MODULE
#define BANK_BASIC           0
#define BANK_CMU_RMU         0x1
#define BANK_DMA0            0xe
#define BANK_DMA1            0xf
#define BANK_DMA2            0x10
#define BANK_DMA3            0x11
#define BANK_DMA4            0x12
#define BANK_AUIP            0x4
#define BANK_PMU             0x5
#define BANK_GPIO            0x6
#define BANK_USB             0x7
#define BANK_CARD            0x9
#define BANK_I2C             0xa
#define BANK_SPI             0xa
#define BANK_UART            0xa
#define BANK_IR              0xa
#define BANK_RTC             0xc
#define BANK_LCD             0x6
#define BANK_EMIF            0x6
#define BANK_DAC_ADC         0x13
#define BANK_SPDIF           0x13
#define BANK_TK           0x14
#define BANK_ACC             0x8
#define BANK_FM              0xd
#define BANK_MJPEG           0x0b

#define BANK_DMA_USB1        BANK_DMA0
#define BANK_DMA_USB2        BANK_DMA1
#define BANK_DMA_AUIP        BANK_DMA2
#define BANK_DMA_FS        	 BANK_DMA2
#define BANK_DMA_CARD        BANK_DMA3
#define BANK_DMA_UHOST       BANK_DMA3
#define BANK_DMA_BASL        BANK_DMA3
#define BANK_DMA_NOR         BANK_DMA4

#ifndef __as260x_H__
#define __as260x_H__

////////////////////////////////////////////////////////////////////////////////////////////////
// SFR Bank control
// mcu core register
sfr SPH                   =  0xbe;      // all bank     SPH Stack Pointer high byte
sfr SPL                   =  0x81;      // all bank     SPL Stack Pointer low byte
sfr SP                    =  0x81;      // all bank     SPL Stack Pointer low byte  另外一种写法
sfr DPXL                  =  0x84;      // all bank     DPXL    Data Pointer extended byte
sfr DPH                   =  0x83;      // all bank     DPH Data Pointer high byte
sfr DPL                   =  0x82;      // all bank     DPL Data Pointer low byte
sfr PSW                   =  0xd0;      // all bank     PSW Program Status Word
sfr PSW1                  =  0xd1;      // all bank     PSW1    Program Status Word1
sfr ACC                   =  0xe0;      // all bank     ACC Accumulator
sfr B                     =  0xf0;      // all bank     B   B Register
// psw & psw1 bit register
sbit CY                   =  0xD7;
sbit AC                   =  0xD6;
sbit F0                   =  0xD5;
sbit RS1                  =  0xD4;
sbit RS0                  =  0xD3;
sbit OV                   =  0xD2;
sbit UD                   =  0xD1;
sbit P                    =  0xD0;
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// debug register
sfr CCMCON                =  0x8e;      // all bank     CCMCON  Communication Control Register
sfr CCMVAL                =  0x8f;      // all bank     CCMVAL  Communication Value Register
sfr BIRDINFO              =  0x85;      // all bank     BIRDINFO    Bird information
////////////////////////////////////////////////////////////////////////////////////////////////
// SFR Bank control
sfr SFR_BANK              =  0x80;      // all bank     SFR_BANK    SFR Banking control Register
////////////////////////////////////////////////////////////////////////////////////////////////
// Indirect Addressing
sfr P2                    =  0xa0;      // all bank     P2  Port2 Register which is not used of IO access
sfr MPAGE                 =  0xa1;      // all bank     MPAGE   same as P2 register
////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt control
sfr IE0                   =  0xa8;      // all bank     IE0 Interrupt Enable register 0
#ifndef PC
sbit EA     			  = IE0 ^ 7;
#else
sbit EA     			  = 0xAF;
#endif
sfr AIE                   =  0xe8;      // all bank     AIE Additional Interrupt Enable register
sfr IPH0                  =  0xb7;      // 0x00         Interrupt Priority High register 0
sfr IPL0                  =  0xb8;      // 0x00         Interrupt Priority Low register 0
sfr AIPH                  =  0xf7;      // 0x00         Additional Interrupt Priority High register
sfr AIPL                  =  0xf8;      // 0x00         Additional Interrupt Priority Low register
sfr IF0                   =  0x88;      // all bank     Interrupt Flag register 0
sfr AIF                   =  0xc0;      // all bank     Additional interrupt flag register
sfr EXTINT                =  0xd8;      // 0x06         External Interrupt Control
sfr IFDebug0              =  0xf5;      // 0x01         Interrupt Flag Debug register 0
sfr IFDebug1              =  0xf6;      // 0x01         Interrupt Flag Debug register 0
#ifndef __GL5115_MEMORY_CONTROLLER_H__
#define __GL5115_MEMORY_CONTROLLER_H__
////////////////////////////////////////////////////////////////////////////////////////////////
// bank switch register////////////////////////////////////////////////////////////////////////////////////////////////
sfr MemBankCtl                                  =  0xf9;      // all bank 	Memory banking control register
sfr PageAddr0H                                  =  0x90;      // 0x00 	High byte of Page Address register 0
sfr PageAddr0L                                  =  0x91;      // 0x00 	Low byte of Page Address register 0
sfr PageAddr1H                                  =  0x92;      // 0x00 	High byte of Page Address register 1
sfr PageAddr1L                                  =  0x93;      // 0x00 	Low byte of Page Address register 1
sfr PageAddr2H                                  =  0x94;      // 0x00 	High byte of Page Address register 2
sfr PageAddr2L                                  =  0x95;      // 0x00 	Low byte of Page Address register 2
sfr PageAddr3H                                  =  0x98;      // 0x00 	High byte of Page Address register 3
sfr PageAddr3L                                  =  0x99;      // 0x00 	Low byte of Page Address register 3
sfr PageAddr4H                                  =  0x9a;      // 0x00 	High byte of Page Address register 4
sfr PageAddr4L                                  =  0x9b;      // 0x00 	Low byte of Page Address register 4
sfr PageAddr5H                                  =  0x9c;      // 0x00 	High byte of Page Address register 5
sfr PageAddr5L                                  =  0x9d;      // 0x00 	Low byte of Page Address register 5
sfr PageAddr6H                                  =  0x9e;      // 0x00 	High byte of Page Address register 6
sfr PageAddr6L                                  =  0x9f;      // 0x00 	Low byte of Page Address register 6
sfr PageAddr7H                                  =  0xa2;      // 0x00 	High byte of Page Address register 7
sfr PageAddr7L                                  =  0xa3;      // 0x00 	Low byte of Page Address register 7
sfr PageAddr8H                                  =  0xd8;      // 0x00 	High byte of Page Address register 8
sfr PageAddr8L                                  =  0xd9;      // 0x00 	Low byte of Page Address register 8
sfr PageAddr9H                                  =  0xda;      // 0x00 	High byte of Page Address register 9
sfr PageAddr9L                                  =  0xdb;      // 0x00 	Low byte of Page Address register 9
sfr PageAddrMask0                               =  0xb0;      // 0x00 	Page 0 Address Mask register
sfr PageAddrMask1                               =  0xb1;      // 0x00 	Page 1 Address Mask register
sfr PageAddrMask2                               =  0xb2;      // 0x00 	Page 2 Address Mask register
sfr PageAddrMask3                               =  0xb3;      // 0x00 	Page 3 Address Mask register
sfr PageAddrMask4                               =  0xb4;      // 0x00 	Page 4 Address Mask register
sfr PageAddrMask5                               =  0xb5;      // 0x00 	Page 5 Address Mask register
sfr PageAddrMask6                               =  0xb6;      // 0x00 	Page 6 Address Mask register
sfr PageAddrMask7                               =  0xd2;      // 0x00 	Page 7 Address Mask register
sfr PageAddrMask8                               =  0xd4;      // 0x00 	Page 8 Address Mask register
sfr PageAddrMask9                               =  0xd5;      // 0x00 	Page 9 Address Mask register
sfr RedirectAddr0                               =  0xc8;      // 0x00 	Redirect Address 0
sfr RedirectAddr1                               =  0xc9;      // 0x00 	Redirect Address 1
sfr RedirectAddr2                               =  0xca;      // 0x00 	Redirect Address 2
sfr RedirectAddr3                               =  0xcb;      // 0x00 	Redirect Address 3
sfr RedirectAddr4                               =  0xcc;      // 0x00 	Redirect Address 4
sfr RedirectAddr5                               =  0xcd;      // 0x00 	Redirect Address 5
sfr RedirectAddr6                               =  0xce;      // 0x00 	Redirect Address 6
sfr RedirectAddr7                               =  0xcf;      // 0x00 	Redirect Address 7
sfr RedirectAddr8                               =  0xd6;      // 0x00 	Redirect Address 8
sfr RedirectAddr9                               =  0xd7;      // 0x00 	Redirect Address 9
sfr FixAddr0H                                   =  0xd3;      // 0x00 	Hight byte of Fix Address register 0
sfr FixAddr0M                                   =  0xb9;      // 0x00 	Middle byte of Fix Address register 0
sfr FixAddr0L                                   =  0xba;      // 0x00 	Low byte of Fix Address register 0
sfr FixAddr1H                                   =  0xbb;      // 0x00 	Hight byte of Fix Address register 1
sfr FixAddr1M                                   =  0xbc;      // 0x00 	Middle byte of Fix Address register 1
sfr FixAddr1L                                   =  0xbd;      // 0x00 	Low byte of Fix Address register 1
sfr FixAddr2H                                   =  0xc2;      // 0x00 	Hight byte of Fix Address register 2
sfr FixAddr2M                                   =  0xc3;      // 0x00 	Middle byte of Fix Address register 2
sfr FixAddr2L                                   =  0xc4;      // 0x00 	Low byte of Fix Address register 2
sfr FixAddr3H                                   =  0xc5;      // 0x00 	Hight byte of Fix Address register 3
sfr FixAddr3M                                   =  0xc6;      // 0x00 	Middle byte of Fix Address register 3
sfr FixAddr3L                                   =  0xc7;      // 0x00 	Low byte of Fix Address register 3
sfr PageMissEntryEx                             =  0xf4;      // 0x00 	Page Miss Entry Address Extension byte
sfr PageMissEntryH                              =  0xf5;      // 0x00 	Page Miss Entry Address High byte
sfr PageMissEntryL                              =  0xf6;      // 0x00 	Page Miss Entry Address Low byte
sfr CodeReplaceEntryEx                          =  0xa5;      // 0x00 	Code Replace Entry Address Extension byte
sfr CodeReplaceEntryH                           =  0xa6;      // 0x00 	Code Replace Entry Address High byte
sfr CodeReplaceEntryL                           =  0xa7;      // 0x00 	Code Replace Entry Address Low byte
sfr TestCounterByte0                            =  0xfc;      // 0x00 	Test Counter byte register 0
sfr TestCounterByte1                            =  0xfd;      // 0x00 	Test Counter byte register 1
sfr TestCounterByte2                            =  0xfe;      // 0x00 	Test Counter byte register 2
sfr TestCounterByte3                            =  0xff;      // 0x00 	Test Counter byte register 3
sfr TestCounterByte4                            =  0xf1;      // 0x00 	Test Counter byte register 4
sfr bist_en1                                    =  0xe1;      // 0x01 	Bist enable register1
sfr bist_en2                                    =  0xe2;      // 0x01 	Bist enable register2
sfr bist_fin1                                   =  0xe4;      // 0x01 	Bist finish flag register1
sfr bist_fin2                                   =  0xe5;      // 0x01 	Bist finish flag register2
sfr bist_info1                                  =  0xe7;      // 0x01 	Bist information register1
sfr bist_info2                                  =  0xe9;      // 0x01 	Bist information register2
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_PMU_H__
#define __GL5115_PMU_H__

sfr VOUT_CTL                                    =  0x89;      // 0x05 	VCC/VDD voltage set Register
sfr LDOPD_CTL                                   =  0x8a;      // 0x05 	Capless LDO pulldown control
sfr BDG_CTL                                     =  0x8b;      // 0x05 	Bandgap enable Register
sfr BDG_VOL                                     =  0x8c;      // 0x05 	Bandgap voltage Register
sfr MULTI_USED                                  =  0x8d;      // 0x05 	GPIO multi-used set Register
sfr PMUADC_CTL                                  =  0x90;      // 0x05 	PMU ADC frequency and enable Register
sfr BATADC_DATA                                 =  0x91;      // 0x05 	BATADC data Register
sfr LRADC1_DATA                                 =  0x92;      // 0x05 	LRADC1 data Register
sfr LRADC3_DATA                                 =  0x93;      // 0x05 	LRADC3 data Register
sfr LRADC4_DATA                                 =  0x94;      // 0x05 	LRADC4 data Register
sfr LRADC5_DATA                                 =  0x95;      // 0x05 	LRADC5 data Register
sfr CP_CTL0                                     =  0x97;      // 0x05 	Charge pump control Register
sfr CP_CTL1                                     =  0x98;      // 0x05 	Charge pump control Register
sfr VDD_reserved                                =  0x99;      // 0x05 	reserved
sfr TEST_CTL                                    =  0x9a;      // 0x05 	Standby test control Register
sfr SYSTEM_CTL                                  =  0x9b;      // 0x05 	System on/off time set Register
sfr SYSTEM_ONOFF                                =  0x9c;      // 0x05 	on/off statue & RESET time set Register
sfr RTCVDD_reserved                             =  0x9d;      // 0x05 	reserved
sfr FS_CTL                                      =  0xa2;      // 0x05 	Fsource control and EFUSE select Register
sfr EFUSE_CTL                                   =  0xa3;      // 0x05 	EFSUE control Register
sfr EFUSE0                                      =  0xa4;      // 0x05 	EFUSE0 data Register
sfr EFUSE1                                      =  0xa5;      // 0x05 	EFUSE1 data Register
sfr EFUSE2                                      =  0xa6;      // 0x05 	EFUSE2 data Register
sfr EFUSE3                                      =  0xa7;      // 0x05 	EFUSE3 data Register
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_RMU_DIGITAL_PART_H__
#define __GL5115_RMU_DIGITAL_PART_H__


sfr MRCR1                                       =  0x86;      // 0x01 	Module Reset Control Register 1
sfr PCON                                        =  0x87;      // all bank 	Power Control Register
sfr MRCR2                                       =  0x89;      // 0x01 	Module Reset Control Register 2
sfr MRCR3                                       =  0x9f;      // 0x01 	Module Reset Control Register 3
sfr ExWait                                      =  0x96;      // all bank 	External bus access wait cycle
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_CMU_ANALOG_H__
#define __GL5115_CMU_ANALOG_H__


sfr MCU_PLL_CTL                                 =  0xb3;      // 0x01 	MCUPLL control register
sfr HOSC_CTL                                    =  0xb6;      // 0x01 	HOSC control register
sfr LOSC_CTL                                    =  0xb7;      // 0x01 	LOSC control register
sfr HCL_CLK_CTL                                 =  0xb8;      // 0x01 	CALENDAR clock control register
sfr HCL_INTERVAL_CTL                            =  0xb5;      // 0x01 	HCL calibration interval control register
sfr AUDIO_PLL_CTL                               =  0xb9;      // 0x01 	AUDIO PLL control register
sfr MCU_PLL_SSC_CTL                             =  0xba;      // 0x01 	MCU PLL spread spectrum control register
sfr MCU_PLL_SSC_FSTEP                           =  0xbb;      // 0x01 	MCU PLL spread spectrum frequency step
sfr MCU_PLL_DEBUG_CTL                           =  0xbc;      // 0x01 	MCU PLL debug control register
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_CMU_DIGITAL_PART_H__
#define __GL5115_CMU_DIGITAL_PART_H__

sfr CLKENCTL0                                   =  0xa9;      // 0x01 	Clock Enable Control Register 0
sfr CLKENCTL1                                   =  0xaa;      // 0x01 	Clock Enable Control Register 1
sfr CLKENCTL2                                   =  0xab;      // 0x01 	Clock Enable Control Register 2
sfr SDCLKCTL                                    =  0xac;      // 0x01 	SD Card Control Register
sfr CPUCLKCTL                                   =  0xc2;      // 0x01 	CPU Clock Control Register
sfr PWMCLKCTL                                   =  0xc3;      // 0x01 	PWM Clock Control Register
sfr PWMDUTY                                     =  0xaf;      // 0x01 	PWM Duty Control Register
sfr AECLKCTL                                    =  0xc4;      // 0x01 	Audio Codec Clock Control Register
//sfr Reserved                                    =  0xc7;      // 0x01 	Reserved Register
sfr FMCLKCTL                                    =  0xc8;      // 0x01 	FM Clock Control Register
sfr LED_SEGLCDCLKCTL                            =  0xc9;      // 0x01 	LED & SEG LCD clock Control Register
sfr SPICLKCTL                                   =  0xca;      // 0x01 	SPI controller Clock Control Register
sfr ADC_DAC_CLK_CTL                             =  0xbd;      // 0x01 	ADC & DAC Clock Control Register
sfr MEMCLKSELCTL0                               =  0xad;      // 0x01 	Memory Clock Selection Control Register 0
sfr MEMCLKSELCTL1                               =  0xae;      // 0x01 	Memory Clock Selection Control Register 1
sfr MEMCLKENCTL0                                =  0xb4;      // 0x01 	Memory Clock Enable Control Register 0
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_RTC_H__
#define __GL5115_RTC_H__

sfr RTC_CTL0                                    =  0xa9;      // 0x0c 	RTC Control 0 Register
sfr RTC_CTL1                                    =  0xaa;      // 0x0c 	RTC Control 1 register
sfr RTCTimeS                                    =  0xab;      // 0x0c 	RTC Time Second Register
sfr RTCTimeMint                                 =  0xac;      // 0x0c 	RTC Time Minute Register
sfr RTCTimeH                                    =  0xad;      // 0x0c 	RTC Time Hour Register
sfr RTCTimeD                                    =  0xae;      // 0x0c 	RTC Time Day Register
sfr RTCTimeMon                                  =  0xaf;      // 0x0c 	RTC Time Month Register
sfr RTCTimeY                                    =  0xb0;      // 0x0c 	RTC Time Year Register
sfr RTCALMS                                     =  0xb1;      // 0x0c 	RTC Alarm Second Register
sfr RTCALMM                                     =  0xb2;      // 0x0c 	RTC Alarm Minute Register
sfr RTCALMH                                     =  0xb3;      // 0x0c 	RTC Alarm Hour Register
sfr RTCRUPD                                     =  0xb4;      // 0x0c 	RTC Register update Register
sfr TimerLB                                     =  0xb5;      // 0x0c 	Timer low Byte
sfr TimerMB                                     =  0xb6;      // 0x0c 	Timer middle Byte
sfr TimerHB                                     =  0xb7;      // 0x0c 	Timer high Byte
sfr WDCTL                                       =  0xb8;      // 0x0c 	watch dog control register
sfr CTCCTL                                      =  0xb9;      // 0x0c 	CTC control register
sfr CTCCNTL                                     =  0xba;      // 0x0c 	CTC counter low register
sfr CTCCNTH                                     =  0xbb;      // 0x0c 	CTC counter high register
sfr RTCRDM0                                     =  0xbc;      // 0x0c 	RTC Random access Register
sfr RTCRDM1                                     =  0xbd;      // 0x0c 	RTC Random access Register
sfr RTCRDM2                                     =  0xbf;      // 0x0c 	RTC Random access Register
sfr RTCRDM3                                     =  0xc1;      // 0x0c 	RTC Random access Register
sfr RTCRDM4                                     =  0xc2;      // 0x0c 	RTC Random access Register
sfr RTCRDM5                                     =  0xc3;      // 0x0c 	RTC Random access Register
sfr RTCRDM6                                     =  0xc4;      // 0x0c 	RTC Random access Register
sfr RTCRDM7                                     =  0xc5;      // 0x0c 	RTC Random access Register
sfr RTCRDM8                                     =  0xc6;      // 0x0c 	RTC Random access Register
sfr RTCRDM9                                     =  0xc7;      // 0x0c 	RTC Random access Register
sfr RTCRDM10                                    =  0xc8;      // 0x0c 	RTC Random access Register
sfr RTCRDM11                                    =  0xc9;      // 0x0c 	RTC Random access Register
sfr RTCRDM12                                    =  0xca;      // 0x0c 	RTC Random access Register
sfr RTCRDM13                                    =  0xcb;      // 0x0c 	RTC Random access Register
sfr RTCRDM14                                    =  0xcc;      // 0x0c 	RTC Random access Register
sfr RTCRDM15                                    =  0xcd;      // 0x0c 	RTC Random access Register
sfr RTCRDM16                                    =  0xce;      // 0x0c 	RTC Random access Register
sfr RTCRDM17                                    =  0xcf;      // 0x0c 	RTC Random access Register
sfr RTCRDM18                                    =  0xd2;      // 0x0c 	RTC Random access Register
sfr RTCRDM19                                    =  0xd3;      // 0x0c 	RTC Random access Register
sfr RTCRDM20                                    =  0xd4;      // 0x0c 	RTC Random access Register
sfr RTCRDM21                                    =  0xd5;      // 0x0c 	RTC Random access Register
sfr RTCRDM22                                    =  0xd6;      // 0x0c 	RTC Random access Register
sfr CTCCTL2                                     =  0xd7;      // 0x0c 	CTC2 control register
sfr CTCCNTL2                                    =  0xd8;      // 0x0c 	CTC2 counter low register
sfr CTCCNTH2                                    =  0xd9;      // 0x0c 	CTC3 counter high register
//---------------------------------------------------------------------//
#endif



#ifndef __GL5115_DMA_H__
#define __GL5115_DMA_H__

sfr DMA0IP                                      =  0x91;      // 0x0e 	DMA0 interrupt pending register
sfr DMA0IE                                      =  0x92;      // 0x0e 	DMA0 interrupt enable register
sfr DMA01234DBG                                 =  0x93;      // 0x0e/0x0f/0x10/0x11/0x12 	DMA0/1/2/3/4 debug register
sfr DMA0CTL0                                    =  0x94;      // 0x0e 	DMA0 control register 0
sfr DMA0CTL1                                    =  0x95;      // 0x0e 	DMA0 control register 1
sfr DMA0SADDR0H                                 =  0x97;      // 0x0e 	DMA0 source address register 0 high byte
sfr DMA0SADDR0L                                 =  0x98;      // 0x0e 	DMA0 source address register 0 low byte
sfr DMA0SADDR1H                                 =  0x99;      // 0x0e 	DMA0 source address register 1 high byte
sfr DMA0SADDR1L                                 =  0x9a;      // 0x0e 	DMA0 source address register 1 low byte
sfr DMA0SADDR0M                                 =  0x9b;      // 0x0e 	DMA0 source address register 0 middle byte
sfr DMA0SADDR1M                                 =  0x9c;      // 0x0e 	DMA0 source address register 1 middle byte
sfr DMA0DADDR0H                                 =  0x9d;      // 0x0e 	DMA0 destination address register 0 high byte
sfr DMA0DADDR0L                                 =  0x9e;      // 0x0e 	DMA0 destination address register 0 low byte
sfr DMA0DADDR0M                                 =  0x9f;      // 0x0e 	DMA0 destination address register 0 middle byte
sfr DMA0DADDR1H                                 =  0xa2;      // 0x0e 	DMA0 destination address register 1 high byte
sfr DMA0DADDR1L                                 =  0xa3;      // 0x0e 	DMA0 destination address register 1 low byte
sfr DMA0FrameLenH                               =  0xa4;      // 0x0e 	DMA0 frame length register high byte
sfr DMA0FrameLenL                               =  0xa5;      // 0x0e 	DMA0 frame length register low byte
sfr DMA0DADDR1M                                 =  0xa6;      // 0x0e 	DMA0 destination address register 1 middle byte
sfr DMA1IP                                      =  0x91;      // 0x0f 	DMA1 interrupt pending register
sfr DMA1IE                                      =  0x92;      // 0x0f 	DMA1 interrupt enable register
sfr DMA1CTL0                                    =  0x94;      // 0x0f 	DMA1 control register 0
sfr DMA1CTL1                                    =  0x95;      // 0x0f 	DMA1 control register 1
sfr DMA1SADDR0H                                 =  0x97;      // 0x0f 	DMA1 source address register 0 high byte
sfr DMA1SADDR0L                                 =  0x98;      // 0x0f 	DMA1 source address register 0 low byte
sfr DMA1SADDR1H                                 =  0x99;      // 0x0f 	DMA1 source address register 1 high byte
sfr DMA1SADDR1L                                 =  0x9a;      // 0x0f 	DMA1 source address register 1 low byte
sfr DMA1SADDR0M                                 =  0x9b;      // 0x0f 	DMA1 source address register 0 middle byte
sfr DMA1SADDR1M                                 =  0x9c;      // 0x0f 	DMA1 source address register 1 middle byte
sfr DMA1DADDR0H                                 =  0x9d;      // 0x0f 	DMA1 destination address register 0 high byte
sfr DMA1DADDR0L                                 =  0x9e;      // 0x0f 	DMA1 destination address register 0 low byte
sfr DMA1DADDR0M                                 =  0x9f;      // 0x0f 	DMA1 destination address register 0 middle byte
sfr DMA1DADDR1H                                 =  0xa2;      // 0x0f 	DMA1 destination address register 1 high byte
sfr DMA1DADDR1L                                 =  0xa3;      // 0x0f 	DMA1 destination address register 1 low byte
sfr DMA1FrameLenH                               =  0xa4;      // 0x0f 	DMA1 frame length register high byte
sfr DMA1FrameLenL                               =  0xa5;      // 0x0f 	DMA1 frame length register low byte
sfr DMA1DADDR1M                                 =  0xa6;      // 0x0f 	DMA1 destination address register 1 middle byte
sfr DMA2IP                                      =  0x91;      // 0x10 	DMA2 interrupt pending register
sfr DMA2IE                                      =  0x92;      // 0x10 	DMA2 interrupt enable register
sfr DMA2CTL0                                    =  0x94;      // 0x10 	DMA2 control register 0
sfr DMA2CTL1                                    =  0x95;      // 0x10 	DMA2 control register 1
sfr DMA2SADDR0H                                 =  0x97;      // 0x10 	DMA2 source address register 0 high byte
sfr DMA2SADDR0L                                 =  0x98;      // 0x10 	DMA2 source address register 0 low byte
sfr DMA2SADDR1H                                 =  0x99;      // 0x10 	DMA2 source address register 1 high byte
sfr DMA2SADDR1L                                 =  0x9a;      // 0x10 	DMA2 source address register 1 low byte
sfr DMA2SADDR0M                                 =  0x9b;      // 0x10 	DMA2 source address register 0 middle byte
sfr DMA2SADDR1M                                 =  0x9c;      // 0x10 	DMA2 source address register 1 middle byte
sfr DMA2DADDR0H                                 =  0x9d;      // 0x10 	DMA2 destination address register 0 high byte
sfr DMA2DADDR0L                                 =  0x9e;      // 0x10 	DMA2 destination address register 0 low byte
sfr DMA2DADDR0M                                 =  0x9f;      // 0x10 	DMA2 destination address register 0 middle byte
sfr DMA2DADDR1H                                 =  0xa2;      // 0x10 	DMA2 destination address register 1 high byte
sfr DMA2DADDR1L                                 =  0xa3;      // 0x10 	DMA2 destination address register 1 low byte
sfr DMA2FrameLenH                               =  0xa4;      // 0x10 	DMA2 frame length register high byte
sfr DMA2FrameLenL                               =  0xa5;      // 0x10 	DMA2 frame length register low byte
sfr DMA2DADDR1M                                 =  0xa6;      // 0x10 	DMA2 destination address register 1 middle byte
sfr DMA3IP                                      =  0x91;      // 0x11 	DMA3 interrupt pending register
sfr DMA3IE                                      =  0x92;      // 0x11 	DMA3 interrupt enable register
sfr DMA3CTL0                                    =  0x94;      // 0x11 	DMA3 control register 0
sfr DMA3CTL1                                    =  0x95;      // 0x11 	DMA3 control register 1
sfr DMA3SADDR0H                                 =  0x97;      // 0x11 	DMA3 source address register 0 high byte
sfr DMA3SADDR0L                                 =  0x98;      // 0x11 	DMA3 source address register 0 low byte
sfr DMA3SADDR1H                                 =  0x99;      // 0x11 	DMA3 source address register 1 high byte
sfr DMA3SADDR1L                                 =  0x9a;      // 0x11 	DMA3 source address register 1 low byte
sfr DMA3SADDR0M                                 =  0x9b;      // 0x11 	DMA3 source address register 0 middle byte
sfr DMA3SADDR1M                                 =  0x9c;      // 0x11 	DMA3 source address register 1 middle byte
sfr DMA3DADDR0H                                 =  0x9d;      // 0x11 	DMA3 destination address register 0 high byte
sfr DMA3DADDR0L                                 =  0x9e;      // 0x11 	DMA3 destination address register 0 low byte
sfr DMA3DADDR0M                                 =  0x9f;      // 0x11 	DMA0 destination address register 0 middle byte
sfr DMA3DADDR1H                                 =  0xa2;      // 0x11 	DMA3 destination address register 1 high byte
sfr DMA3DADDR1L                                 =  0xa3;      // 0x11 	DMA3 destination address register 1 low byte
sfr DMA3FrameLenH                               =  0xa4;      // 0x11 	DMA3 frame length register high byte
sfr DMA3FrameLenL                               =  0xa5;      // 0x11 	DMA3 frame length register low byte
sfr DMA3DADDR1M                                 =  0xa6;      // 0x11 	DMA3 destination address register 1 middle byte
sfr DMA4IP                                      =  0x91;      // 0x12 	DMA4 interrupt pending register
sfr DMA4IE                                      =  0x92;      // 0x12 	DMA4 interrupt enable register
sfr DMA4CTL0                                    =  0x94;      // 0x12 	DMA4 control register 0
sfr DMA4CTL1                                    =  0x95;      // 0x12 	DMA4control register 1
sfr DMA4SADDR0H                                 =  0x97;      // 0x12 	DMA4 source address register 0 high byte
sfr DMA4SADDR0L                                 =  0x98;      // 0x12 	DMA4 source address register 0 low byte
sfr DMA4SADDR1H                                 =  0x99;      // 0x12 	DMA4 source address register 1 high byte
sfr DMA4SADDR1L                                 =  0x9a;      // 0x12 	DMA4 source address register 1 low byte
sfr DMA4SADDR0M                                 =  0x9b;      // 0x12 	DMA4 source address register 0 middle byte
sfr DMA4SADDR1M                                 =  0x9c;      // 0x12 	DMA4 source address register 1 middle byte
sfr DMA4DADDR0H                                 =  0x9d;      // 0x12 	DMA4 destination address register 0 high byte
sfr DMA4DADDR0L                                 =  0x9e;      // 0x12 	DMA4 destination address register 0 low byte
sfr DMA4DADDR0M                                 =  0x9f;      // 0x12 	DMA4 destination address register 0 middle byte
sfr DMA4DADDR1H                                 =  0xa2;      // 0x12 	DMA4 destination address register 1 high byte
sfr DMA4DADDR1L                                 =  0xa3;      // 0x12 	DMA4 destination address register 1 low byte
sfr DMA4FrameLenH                               =  0xa4;      // 0x12 	DMA4 frame length register high byte
sfr DMA4FrameLenL                               =  0xa5;      // 0x12 	DMA4 frame length register low byte
sfr DMA4DADDR1M                                 =  0xa6;      // 0x12 	DMA4 destination address register 1 middle byte
//---------------------------------------------------------------------//
sfr DMAnIP                			=  0x91;      // 0x0e/0f10/11/12	DMA1 interrupt pending register
sfr DMAnIE                			=  0x92;      // 0x0e/0f10/11/12	DMA1 interrupt enable register
sfr DMAnCTL0              			=  0x94;      // 0x0e/0f10/11/12	DMA1 control register 0
sfr DMAnCTL1              			=  0x95;      // 0x0e/0f10/11/12	DMA1 control register 1
sfr DMAnSADDR0H           			=  0x97;      // 0x0e/0f10/11/12	DMA1 source address register 0 high byte
sfr DMAnSADDR0M           			=  0x9b;      // 0x0e/0f10/11/12	DMA1 source address register 0 midlle byte
sfr DMAnSADDR0L           			=  0x98;      // 0x0e/0f10/11/12	DMA1 source address register 0 low byte
sfr DMAnSADDR1H           			=  0x99;      // 0x0e/0f10/11/12	DMA1 source address register 1 high byte
sfr DMAnSADDR1M           			=  0x9c;      // 0x0e/0f10/11/12	DMA1 source address register 1 midlle byte
sfr DMAnSADDR1L           			=  0x9a;      // 0x0e/0f10/11/12	DMA1 source address register 1 low byte
sfr DMAnDADDR0H           			=  0x9d;      // 0x0e/0f10/11/12	DMA1 destination address register 0 high byte
sfr DMAnDADDR0M           			=  0x9f;      // 0x0e/0f10/11/12	DMA1 destination address register 0 middle byte
sfr DMAnDADDR0L           			=  0x9e;      // 0x0e/0f10/11/12	DMA1 destination address register 0 low byte
sfr DMAnDADDR1H           			=  0xa2;      // 0x0e/0f10/11/12	DMA1 destination address register 1 high byte
sfr DMAnDADDR1M           			=  0xa6;      // 0x0e/0f10/11/12	DMA1 destination address register 1 middle byte
sfr DMAnDADDR1L           			=  0xa3;      // 0x0e/0f10/11/12	DMA1 destination address register 1 low byte
sfr DMAnFrameLenH         			=  0xa4;      // 0x0e/0f10/11/12	DMA1 frame length register high byte
sfr DMAnFrameLenL         			=  0xa5;      // 0x0e/0f10/11/12   DMA0 frame length register low byte
#endif

#ifndef __GL5115_AUIP_H__
#define __GL5115_AUIP_H__

sfr AuCodecCtl                                  =  0x90;      // 0x04 	Audio Codec Control Register
sfr AuDebugLength                               =  0x91;      // 0x04 	Audio Codec Debug Length Register
sfr AuCodecDebug                                =  0x92;      // 0x04 	Audio Codec Debug Register
sfr AuCodecFIFOCtl                              =  0x93;      // 0x04 	Audio Codec FIFO control Register
sfr AuCodecFIFOData                             =  0x94;      // 0x04 	Audio Codec FIFO Data Register
sfr AuCodecDebug2                               =  0x95;      // 0x04 	Audio Codec Debug Register 2
sfr AuCodecDecStateH                            =  0x97;      // 0x04 	Decoding State Register High byte
sfr AuCodecDecStateL                            =  0x98;      // 0x04 	Decoding State Register Low byte
sfr MP3IE                                       =  0xa9;      // 0x04 	MP3 Decoder Interrupt Enable Register
sfr MP3IP                                       =  0xaa;      // 0x04 	MP3 Decoder Interrupt Pending Register
sfr MP3Ctl                                      =  0xab;      // 0x04 	MP3 Decoder Control Register
sfr MP3HeaderInfo                               =  0xac;      // 0x04 	Header information register
sfr MP3BitRateH                                 =  0xad;      // 0x04 	High byte of bit rate index register
sfr MP3BitRateL                                 =  0xae;      // 0x04 	Low byte of bit rate index register
sfr MP3TTimeH                                   =  0xaf;      // 0x04 	Total time hours register
sfr MP3TTimeM                                   =  0xb0;      // 0x04 	Total time minutes register
sfr MP3TTimeS                                   =  0xb1;      // 0x04 	Total time seconds register
sfr MP3CTimeH                                   =  0xb2;      // 0x04 	Current time hours register
sfr MP3CTimeM                                   =  0xb3;      // 0x04 	Current time minutes register
sfr MP3CTimeS                                   =  0xb4;      // 0x04 	Current time seconds register
sfr MP3FrameNumH                                =  0xb5;      // 0x04 	High byte of Frame Number register
sfr MP3FrameNumM                                =  0xb6;      // 0x04 	Middle byte of Frame Number register
sfr MP3FrameNumL                                =  0xb7;      // 0x04 	Low byte of Frame Number register
sfr MP3FileLen3                                 =  0xb8;      // 0x04 	Byte 3 of File Length register
sfr MP3FileLen2                                 =  0xb9;      // 0x04 	Byte 2 of File Length register
sfr MP3FileLen1                                 =  0xba;      // 0x04 	Byte 1 of File Length register
sfr MP3HeaderSynCtl                             =  0xbb;      // 0x04 	MP3 header synchronization control register
sfr SynHeaderData1                              =  0xbc;      // 0x04 	MP3 header synchronization Data register 1
sfr SynHeaderData2                              =  0xbd;      // 0x04 	MP3 header synchronization Data register 2
sfr WMAIE                                       =  0xa9;      // 0x04 	WMA Decoder Interrupt Enable Register
sfr WMAIP                                       =  0xaa;      // 0x04 	WMA Decoder Interrupt Pending Register
sfr WMACtl                                      =  0xab;      // 0x04 	WMA Decoder Control Register
sfr WMAHeaderInfo                               =  0xac;      // 0x04 	Header information register
sfr WMABitRateH                                 =  0xad;      // 0x04 	High byte of bit rate index register
sfr WMABitRateL                                 =  0xae;      // 0x04 	Low byte of bit rate index register
sfr WMATTimeH                                   =  0xaf;      // 0x04 	Total time hours register
sfr WMATTimeM                                   =  0xb0;      // 0x04 	Total time minutes register
sfr WMATTimeS                                   =  0xb1;      // 0x04 	Total time seconds register
sfr WMACTimeH                                   =  0xb2;      // 0x04 	Current time hours register
sfr WMACTimeM                                   =  0xb3;      // 0x04 	Current time minutes register
sfr WMACTimeS                                   =  0xb4;      // 0x04 	Current time seconds register
sfr WMAPackNumH                                 =  0xb5;      // 0x04 	High byte of Packet Number register
sfr WMAPackNumM                                 =  0xb6;      // 0x04 	Middle byte of Packet Number register
sfr WMAPackNumL                                 =  0xb7;      // 0x04 	Low byte of Packet Number register
sfr WMAPackSizeH                                =  0xb8;      // 0x04 	High byte of Packet Size register
sfr WMAPackSizeM                                =  0xb9;      // 0x04 	Middle byte of Packet Size register
sfr WMAPackSizeL                                =  0xba;      // 0x04 	Low byte of Packet Size register
sfr VirtualAddrH                                =  0xbb;      // 0x04 	High byte of virtual address of current frame
sfr VirtualAddrL                                =  0xbc;      // 0x04 	Low byte of virtual address of current frame
sfr PhysicalAddrH                               =  0xbd;      // 0x04 	High byte of physical address of current frame
sfr PhysicalAddrL                               =  0xbf;      // 0x04 	Low byte of physical address of current frame
sfr DiscardLen                                  =  0xc1;      // 0x04 	Discard length of current subframe
sfr WAVIE                                       =  0xa9;      // 0x04 	WAV Decoder Interrupt Enable Register
sfr WAVIP                                       =  0xaa;      // 0x04 	WAV Decoder Interrupt Pending Register
sfr WAVCtl                                      =  0xab;      // 0x04 	WAV Decoder Control Register
sfr WAVHeaderInfo1                              =  0xac;      // 0x04 	Header information register 1
sfr WAVHeaderInfo2                              =  0xad;      // 0x04 	Header information register 2
sfr WAVTTimeH                                   =  0xaf;      // 0x04 	Total time hours register
sfr WAVTTimeM                                   =  0xb0;      // 0x04 	Total time minutes register
sfr WAVTTimeS                                   =  0xb1;      // 0x04 	Total time seconds register
sfr WAVCTimeH                                   =  0xb2;      // 0x04 	Current time hours register
sfr WAVCTimeM                                   =  0xb3;      // 0x04 	Current time minutes register
sfr WAVCTimeS                                   =  0xb4;      // 0x04 	Current time seconds register
sfr WAVBlockNumH                                =  0xb5;      // 0x04 	High byte of Block Number register
sfr WAVBlockNumM                                =  0xb6;      // 0x04 	Middle byte of Block Number register
sfr WAVBlockNumL                                =  0xb7;      // 0x04 	Low byte of Block Number register
sfr WAVEncCtl                                   =  0xab;      // 0x04 	WAV Encoder Control Register
sfr WAVEncInfo                                  =  0xac;      // 0x04 	WAV Encoder Information Register
sfr MP3EncCtl                                   =  0xab;      // 0x04 	MP3 Encoder Control Register
sfr MP3EncInfo                                  =  0xac;      // 0x04 	MP3 Encoder Information Register
sfr SoftIE                                      =  0xa9;      // 0x04 	Software Decoder Interrupt Enable Register
sfr SoftIP                                      =  0xaa;      // 0x04 	Software Decoder Interrupt Pending Register
sfr SoftCtl                                     =  0xab;      // 0x04 	Software Decoder Control Register
sfr FrameLen1                                   =  0xb8;      // 0x04 	Frame Length Register 1
sfr FrameLen0                                   =  0xb9;      // 0x04 	Frame Length Register 0
sfr BEPCtl1                                     =  0xc2;      // 0x04 	Post Processor Control Register 1
sfr BEPCtl2                                     =  0xc3;      // 0x04 	Post Processor Control Register 2
sfr BEPCtl3                                     =  0xc4;      // 0x04 	Post Processor Control Register 3
sfr BEPCtl4                                     =  0xc5;      // 0x04 	Post Processor Control Register 4
sfr BEPCtl5                                     =  0xc6;      // 0x04 	Post Processor Control Register 5
sfr GlobalGainH                                 =  0xc7;      // 0x04 	Global Gain High Byte Register
sfr GlobalGainM                                 =  0xc8;      // 0x04 	Global Gain Middle Byte Register
sfr GlobalGainL                                 =  0xc9;      // 0x04 	Global Gain Low Byte Register
sfr CurrentEnergy                               =  0xca;      // 0x04 	Current Energy Register
sfr KaraokeCtl                                  =  0xcb;      // 0x04 	Karaoke Control Register
sfr LCHGain                                     =  0xcc;      // 0x04 	Left channel gain
sfr RCHGain                                     =  0xcd;      // 0x04 	Right channel gain
sfr LPFGain                                     =  0xce;      // 0x04 	Low pass filter gain
sfr HPFGain                                     =  0xcf;      // 0x04 	High pass filter gain
sfr SRSCtl0                                     =  0xcc;      // 0x04 	SRS Control Register 0
sfr SRSCtl1                                     =  0xcd;      // 0x04 	SRS Control Register 1
sfr SRSCtl2                                     =  0xce;      // 0x04 	SRS Control Register 2
// audio ip common registers
sfr AuCodecIE             =  0xa9;      // bank 04h     Audio Codec Interrupt Enable Register
sfr AuCodecIP             =  0xaa;      // bank 04h     Audio Codec Interrupt Pending Register
sfr AuDecEncCtl           =  0xab;      // bank 04h     Audio decoding or encoding Control Register
sfr AuCodecHeaderInfo     =  0xac;      // bank 04h     Header information register
sfr AuCodecBirRateH       =  0xad;      // bank 04h     High byte of bit rate index register
sfr AuCodecBirRateL       =  0xae;      // bank 04h     Low byte of bit rate index register
sfr AuCodecTTimeH         =  0xaf;      // bank 04h     Total time hours register
sfr AuCodecTTimeM         =  0xb0;      // bank 04h     Total time minutes register
sfr AuCodecTTimeS         =  0xb1;      // bank 04h     Total time seconds register
sfr AuCodecCTimeH         =  0xb2;      // bank 04h     Current time hours register
sfr AuCodecCTimeM         =  0xb3;      // bank 04h     Current time minutes register
sfr AuCodecCTimeS         =  0xb4;      // bank 04h     Current time seconds register
////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_ACC_H__
#define __GL5115_ACC_H__

sfr ACC_CTRL                                    =  0x89;      // Bank 	Control Register
sfr ACC_BLKLEN_LENHI                            =  0x8a;      // 0x08 	Search block length or the low byte of data length of unsigned accumulation unit number
sfr ACC_BLKNUM_LENLO                            =  0x8b;      // 0x08 	Search block number or the high byte of data length of unsigned accumulation unit number
sfr ACC_CMPDATA0_SUMHIE                         =  0xf1;      // 0x08 	The first byte of data searched or the higher byte of unsigned accelerator’s result
sfr ACC_CMPDATA1_SUMHI                          =  0xf2;      // 0x08 	The second byte of data searched or the high byte of unsigned accelerator’s result
sfr ACC_CMPDATA2_SUMLO                          =  0xf3;      // 0x08 	The third byte of data searched or the low byte of unsigned accelerator’s result
sfr ACC_CMPDATA3_SUMLOE                         =  0xf4;      // 0x08 	The fourth byte of data searched or the lower byte of unsigned accelerator’s result
sfr ACC_MATCNTHI                                =  0xf5;      // 0x08 	The high byte of bitmap
sfr ACC_MATCNTLO                                =  0xf6;      // 0x08 	The low byte of match number
sfr ACC_BITMAPHI                                =  0xf7;      // 0x08 	The high byte of bitmap
sfr ACC_BITMAPLO                                =  0xf8;      // 0x08 	The low byte of bitmap
sfr ACC_FIFO                                    =  0x8c;      // 0x08 	ACC FIFO address
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_TKRX_H__
#define __GL5115_TKRX_H__


sfr TK_EN_CTL                                   =  0x90;      // 0x14 	Touch Key Control Register.
sfr TK_FUN_CTL                                  =  0x91;      // 0x14 	Touch Key Function Control Register.
sfr TK_IRQ_PD                                   =  0x92;      // 0x14 	Touch Key IRQ Pending Status Register.
sfr TK_PRESS_STA                                =  0x93;      // 0x14 	Touch Key Press Status Register.
sfr TK_DB_CTL                                   =  0x94;      // 0x14 	Touch Key Debounce Control Register.
sfr TK_PWM_CNTH                                 =  0x95;      // 0x14 	Touch Key PWM Counter High Register.
sfr TK_PWM_CNTL                                 =  0x97;      // 0x14 	Touch Key PWM Counter Low Register.
sfr TK_PRESS_TH                                 =  0x99;      // 0x14 	Touch Key Press Threshold Register.
sfr TK_BL_CAL_TH                                =  0x9A;      // 0x14 	Touch Key Baseline Calibration Threshold Register.
sfr TK_NOISE_TH                                 =  0x9B;      // 0x14 	Touch Key Noise Threshold Register.
sfr TK_BUCKET_TH                                =  0x9C;      // 0x14 	Touch Key Bucket Threshold And Baseline Step Adjust Register.
sfr TK_AUTO_LOWTH_H                             =  0x9D;      //  	Touch Key idac auto adjust low threshold high bits Register.
sfr TK_AUTO_LOWTH_L                             =  0x9E;      //  	Touch Key idac auto adjust low threshold low bits Register.
sfr TK_AUTO_HIGHTH_H                            =  0xA2;      //  	Touch Key idac auto adjust high threshold high bits Register.
sfr TK_AUTO_HIGHTH_L                            =  0xA3;      //  	Touch Key idac auto adjust high threshold low bits Register.
sfr TK_KEY0_IDA_CTL                             =  0xA4;      // 0x14 	Touch Key0 charging current Control Register.
sfr TK_KEY1_IDA_CTL                             =  0xA5;      // 0x14 	Touch Key1 charging current Control Register.
sfr TK_KEY2_IDA_CTL                             =  0xA6;      // 0x14 	Touch Key2 charging current Control Register.
sfr TK_KEY3_IDA_CTL                             =  0xA9;      // 0x14 	Touch Key3 charging current Control Register.
sfr TK_KEY4_IDA_CTL                             =  0xAA;      // 0x14 	Touch Key4 charging current Control Register.
sfr TK_KEY5_IDA_CTL                             =  0xAB;      // 0x14 	Touch Key5 charging current Control Register.
sfr TK_UPDAT_PD                                 =  0xAD;      // 0x14 	Touch Key New Data Update Pending Register.
sfr TK_KEY0_DATH                                =  0xAE;      // 0x14 	Touch Key0 current data high bits.
sfr TK_KEY0_DATL                                =  0xAF;      // 0x14 	Touch Key0 current data low bits.
sfr TK_KEY1_DATH                                =  0xB0;      // 0x14 	Touch Key1 current data high bits.
sfr TK_KEY1_DATL                                =  0xB1;      // 0x14 	Touch Key1 current data low bits.
sfr TK_KEY2_DATH                                =  0xB2;      // 0x14 	Touch Key2 current data high bits.
sfr TK_KEY2_DATL                                =  0xB3;      // 0x14 	Touch Key2 current data low bits.
sfr TK_KEY3_DATH                                =  0xB4;      // 0x14 	Touch Key3 current data high bits.
sfr TK_KEY3_DATL                                =  0xB5;      // 0x14 	Touch Key3 current data low bits.
sfr TK_KEY4_DATH                                =  0xB6;      // 0x14 	Touch Key4 current data high bits.
sfr TK_KEY4_DATL                                =  0xB7;      // 0x14 	Touch Key40 current data low bits.
sfr TK_KEY5_DATH                                =  0xB8;      // 0x14 	Touch Key5 current data high bits.
sfr TK_KEY5_DATL                                =  0xB9;      // 0x14 	Touch Key5 current data low bits.
sfr TK_RAW_PD                                   =  0xBA;      // 0x14 	Touch Key New RAW Data Update Pending Register.
sfr TK_KEY0_RAWH                                =  0xBC;      // 0x14 	Touch Key0 current RAW high bits.
sfr TK_KEY0_RAWL                                =  0xBD;      // 0x14 	Touch Key0 current RAW low bits.
sfr TK_KEY1_RAWH                                =  0xBF;      // 0x14 	Touch Key1 current RAW high bits.
sfr TK_KEY1_RAWL                                =  0xC1;      // 0x14 	Touch Key1 current RAW low bits.
sfr TK_KEY2_RAWH                                =  0xC2;      // 0x14 	Touch Key2 current RAW high bits.
sfr TK_KEY2_RAWL                                =  0xC3;      // 0x14 	Touch Key2 current RAW low bits.
sfr TK_KEY3_RAWH                                =  0xC4;      // 0x14 	Touch Key3 current RAW high bits.
sfr TK_KEY3_RAWL                                =  0xC5;      // 0x14 	Touch Key3 current RAW low bits.
sfr TK_KEY4_RAWH                                =  0xC6;      // 0x14 	Touch Key4 current RAW high bits.
sfr TK_KEY4_RAWL                                =  0xC7;      // 0x14 	Touch Key4 current RAW low bits.
sfr TK_KEY5_RAWH                                =  0xC8;      // 0x14 	Touch Key5 current RAW high bits.
sfr TK_KEY5_RAWL                                =  0xC9;      // 0x14 	Touch Key5 current RAW low bits.
sfr TK_KEY0_BL_H                                =  0xCA;      // 0x14 	Touch Key0 current baseline high bits.
sfr TK_KEY0_BL_L                                =  0xCC;      // 0x14 	Touch Key0 current baseline low bits.
sfr TK_KEY1_BL_H                                =  0xCD;      // 0x14 	Touch Key1 current baseline high bits.
sfr TK_KEY1_BL_L                                =  0xCE;      // 0x14 	Touch Key1 current baseline low bits.
sfr TK_KEY2_BL_H                                =  0xD2;      // 0x14 	Touch Key2 current baseline high bits.
sfr TK_KEY2_BL_L                                =  0xD3;      // 0x14 	Touch Key2 current baseline low bits.
sfr TK_KEY3_BL_H                                =  0xD4;      // 0x14 	Touch Key3 current baseline high bits.
sfr TK_KEY3_BL_L                                =  0xD5;      // 0x14 	Touch Key3 current baseline low bits.
sfr TK_KEY4_BL_H                                =  0xD6;      // 0x14 	Touch Key4 current baseline high bits.
sfr TK_KEY4_BL_L                                =  0xD7;      // 0x14 	Touch Key4 current baseline low bits.
sfr TK_KEY5_BL_H                                =  0xD8;      // 0x14 	Touch Key5 current baseline high bits.
sfr TK_KEY5_BL_L                                =  0xD9;      // 0x14 	Touch Key5 current baseline low bits.
sfr TK_DEBUG                                    =  0xDA;      // 0x14 	Touch Key Debug Register.
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_SDC_H__
#define __GL5115_SDC_H__


sfr SD_CMD                                      =  0x90;      // 0x09 	SD/MMC CMD Register
sfr SD_CMD_ARGRSP                               =  0x91;      // 0x09 	SD/MMC Argument or RSP Register
sfr SD_RSP_POIN                                 =  0x92;      // 0x09 	SD/MMC RSP Point Register
sfr SD_TF_CTL                                   =  0x93;      // 0x09 	SD/MMC control register
sfr SD_STATE                                    =  0x94;      // 0x09 	MMC/SD status Register
sfr SD_BLK_SIZE_H                               =  0x97;      // 0x09 	SD/MMC High Block size Register
sfr SD_BLK_SIZE_L                               =  0x98;      // 0x09 	SD/MMC Low Block size Register
sfr SD_BLK_NUM                                  =  0x99;      // 0x09 	SD/MMC BLOCK number Register
sfr SD_CLK_CTL                                  =  0x9a;      // 0x09 	SD/MMC Clock Control Register
sfr SD_PAD_CTL                                  =  0x9b;      // 0x09 	SD/MMC PAD CONTROL Register
sfr SD_INT_CTL                                  =  0x9c;      // 0x09 	SD/MMC INTERRUPT Control Register
sfr SD_DATA_FIFO                                =  0x9d;      // 0x09 	SD/MMC Data FIFO Register
sfr SD_TIMEOUT_CTL                              =  0x9e;      // 0x09 	Data Timeout Counter Register
sfr SD_TIMING_CTL                               =  0xad;      // 0x09 	CARD TIMING CONTROL
sfr SD_DBG_CTL                                  =  0xae;      // 0x09 	CARD Debug Signal Output Register
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_EM_H__
#define __GL5115_EM_H__


sfr EXTMEM_CTL                                  =  0x9e;      // 0x06 	Extended Memory Interface Control Register
sfr EXTMEM_WT                                   =  0x9f;      // 0x06 	Extended Memory Interface Wait State Register
sfr EXTMEM_DH                                   =  0xfe;      // 0x06 	Extended Memory Interface high Byte Register
sfr EXTMEM_DL                                   =  0xff;      // 0x06 	Extended Memory Interface Low Byte Register
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_USB_H__
#define __GL5115_USB_H__


sfr UDMAM                                       =  0x86;      // 0x07 	USB DMA MODE
sfr AUTOINTIMER                                 =  0x89;      // 0x07 	Auto in mode in token timer
sfr EP1STADDRH                                  =  0x8a;      // 0x07 	EP1 FIFO start address high register
sfr EP1STADDRL                                  =  0x8b;      // 0x07 	EP1 FIFO start address low register
sfr EP1DMALENH                                  =  0x8c;      // 0x07 	EP1 DMA transfer length high in normal mode
sfr EP1DMALENL                                  =  0x8d;      // 0x07 	EP1 DMA transfer length low in normal mode
sfr OUTPCKCNTH                                  =  0x90;      // 0x07 	Out transaction packet counter high
sfr OUTPCKCNTL                                  =  0x91;      // 0x07 	Out transaction packet counter low
sfr IDVBUSCTRL                                  =  0x92;      // 0x07 	ID&VBUS control
sfr USBSTATUS                                   =  0x93;      // 0x07 	USB status
sfr DPDMCTRL                                    =  0x94;      // 0x07 	DP DM control register
sfr USB_PHYCTRL                                 =  0x95;      // 0x07 	PHY control register
sfr Out0bc_hcin0bc                              =  0x97;      // 0x07 	Endpoint 0 OUT Byte Count
sfr In0bc_hcout0bc                              =  0x98;      // 0x07 	Endpoint 0 IN Byte Count
sfr Ep0cs_hcep0cs                               =  0x99;      // 0x07 	Endpoint 0 Control and Status
sfr In1bch_hcout1bch                            =  0x9a;      // 0x07 	Endpoint 1 IN Byte Count High
sfr In1bcl_hcout1bcl                            =  0x9b;      // 0x07 	Endpoint 1 IN Byte Count Low
sfr In1ctrl_hcout1ctrl                          =  0x9c;      // 0x07 	Endpoint 1 IN Control
sfr In1cs_hcout1cs                              =  0x9d;      // 0x07 	Endpoint 1 IN Control And Status
sfr Out2bch_hcin2bch                            =  0x9e;      // 0x07 	Endpoint 2 OUT Byte Count High
sfr Out2bcl_hcin2bcl                            =  0x9f;      // 0x07 	Endpoint 2 OUT Byte Count Low
sfr Out2ctrl_hcin2ctrl                          =  0xa2;      // 0x07 	Endpoint 2 OUT Control
sfr Out2cs_hcin2cs                              =  0xa3;      // 0x07 	Endpoint 2 OUT Control And Status
sfr In3bc_hcout3bc                              =  0xa4;      // 0x07 	Endpoint 3 IN Byte Count
sfr In3ctrl_hcout3ctrl                          =  0xa5;      // 0x07 	Endpoint 3 IN Control
sfr In3cs_hcout3cs                              =  0xa6;      // 0x07 	Endpoint 3 IN Control And Status
sfr Fifo1dat                                    =  0xa7;      // 0x07 	FIFO 1 Data
sfr Fifo2dat                                    =  0xa9;      // 0x07 	FIFO 2 Data
sfr Fifo3dat                                    =  0xaa;      // 0x07 	FIFO 3 Data
sfr Ep0indata                                   =  0xab;      // 0x07 	EP0 IN DATA
sfr Ep0outdata                                  =  0xac;      // 0x07 	EP0 OUT DATA
sfr Usbirq_hcusbirq                             =  0xad;      // 0x07 	USB Interrupt
sfr Usbien_hcusbien                             =  0xae;      // 0x07 	USB interrupt enable
sfr SHORTPCKIRQ                                 =  0xaf;      // 0x07 	Short packets Interrupt request and enable
sfr Hcep0ctrl                                   =  0xb0;      // 0x07 	Endpoint 0 Control
sfr Hcout0err                                   =  0xb1;      // 0x07 	Endpoint 0 HC OUT Error
sfr Hcin0err                                    =  0xb2;      // 0x07 	Endpoint 0 HC IN Error
sfr Hcout1ctrl                                  =  0xb3;      // 0x07 	Endpoint 1 HC OUT Control
sfr Hcout1err                                   =  0xb4;      // 0x07 	Endpoint 1 HC OUT Error
sfr Hcin2ctrl                                   =  0xb5;      // 0x07 	Endpoint 2 HC IN Control
sfr Hcin2err                                    =  0xb6;      // 0x07 	Endpoint 2 HC OUT Error
sfr EP2STADDRH                                  =  0xb7;      // 0x07 	EP2 FIFO start address high register
sfr EP2STADDRL                                  =  0xb8;      // 0x07 	EP2 FIFO start address low register
sfr Hcout3ctrl                                  =  0xb9;      // 0x07 	Endpoint 3 HC OUT Control
sfr Hcout3err                                   =  0xba;      // 0x07 	Endpoint 3 HC OUT Error
sfr Setupdat0                                   =  0xbb;      // 0x07 	SETUP DATA0
sfr Setupdat1                                   =  0xbc;      // 0x07 	SETUP DATA1
sfr Setupdat2                                   =  0xbd;      // 0x07 	SETUP DATA2
sfr Setupdat3                                   =  0xbf;      // 0x07 	SETUP DATA3
sfr Setupdat4                                   =  0xc1;      // 0x07 	SETUP DATA4
sfr Setupdat5                                   =  0xc2;      // 0x07 	SETUP DATA5
sfr Setupdat6                                   =  0xc3;      // 0x07 	SETUP DATA6
sfr Setupdat7                                   =  0xc4;      // 0x07 	SETUP DATA7
sfr Ep03irq                                     =  0xc5;      // 0x07 	Endpoint 0 to 3 Interrupt Request
sfr Ep03ien                                     =  0xc6;      // 0x07 	Endpoint 0 to 3 Interrupt Enables
sfr Ep03tokirq                                  =  0xc7;      // 0x07 	Endpoint 0 to 3 Token Interrupt Request
sfr Ep03tokien                                  =  0xc8;      // 0x07 	Endpoint 0 to 3 Token Interrupt Request Enable
sfr IVECT                                       =  0xc9;      // 0x07 	Interrupt Vector
sfr EPRST                                       =  0xca;      // 0x07 	Endpoint Reset
sfr UsbCTRL_STUS                                =  0xcb;      // 0x07 	USB Control And Status
sfr FrmCNTH                                     =  0xcc;      // 0x07 	USB Frame Counter HIGH
sfr FrmCNTL                                     =  0xcd;      // 0x07 	USB Frame Counter Low
sfr Fnaddr                                      =  0xce;      // 0x07 	Function Address
sfr Clkgate                                     =  0xcf;      // 0x07 	Clock Gate
sfr Fifoctrl                                    =  0xd2;      // 0x07 	FIFO Control
sfr Hcportctrl                                  =  0xd3;      // 0x07 	HC Port Control
sfr Hcfrmnh                                     =  0xd4;      // 0x07 	HC Frame Number high
sfr Hcfrmnl                                     =  0xd5;      // 0x07 	HC Frame Number low
sfr Hcfrmremainh                                =  0xd6;      // 0x07 	HC Frame Remain high
sfr Hcfrmremainl                                =  0xd7;      // 0x07 	HC Frame Remain Low
sfr Hcep03errirq                                =  0xd8;      // 0x07 	HC 0 to 3 Error Interrupt Request
sfr Hcep03errien                                =  0xd9;      // 0x07 	HC 0 to 3 Error Interrupt Enable
sfr Otgirq                                      =  0xda;      // 0x07 	OTG Interrupt Request
sfr Otgstate                                    =  0xdb;      // 0x07 	The OTG FSM State
sfr Otgctrl                                     =  0xdc;      // 0x07 	OTG Control
sfr Otgstatus                                   =  0xdd;      // 0x07 	OTG Status
sfr Otgien                                      =  0xde;      // 0x07 	OTG Interrupt Enable
sfr EP2DMALENH                                  =  0xe2;      // 0x07 	EP2 DMA transfer length high in normal mode
sfr EP2DMALENL                                  =  0xe3;      // 0x07 	EP2 DMA transfer length low in normal mode
sfr Hcin0maxpck                                 =  0xe4;      // 0x07 	HC IN 0 Max Packet Size
sfr Hcin2maxpckh                                =  0xe5;      // 0x07 	HC IN 2 max packet high
sfr Hcin2maxpckl                                =  0xe6;      // 0x07 	HC IN 2 max packet low
sfr Hcout3maxpck                                =  0xe7;      // 0x07 	HC OUT 3 max packet
sfr Hcout1maxpckh                               =  0xe9;      // 0x07 	HC OUT 1 max packet low
sfr Hcout1maxpckl                               =  0xea;      // 0x07 	HC OUT 1 max packet high
sfr USBEIRQ                                     =  0xeb;      // 0x07 	USB external Interrupt request
sfr AUTONAKCTRL                                 =  0xec;      // 0x07 	auto nak control
sfr HCINCTRL                                    =  0xed;      // 0x07 	host in control
sfr DBGMODE                                     =  0xee;      // 0x07 	debug mode
sfr VDCTRL                                      =  0xef;      // 0x07 	USB PHY vendor control
sfr VDSTAT                                      =  0xf1;      // 0x07 	USB PHY vendor status
sfr BKDOOR                                      =  0xf3;      // 0x07 	Test back door
sfr OTGTRIEN                                    =  0xf5;      // 0x07 	OTR status machine interrupt enable
sfr OTGTRIRQ                                    =  0xf6;      // 0x07 	OTR status machine interrupt request
sfr USB_Efuse_Ref                               =  0xf7;      // 0x07 	Usb Access Efuse_Ref register
sfr FSMPRESTATE                                 =  0xfc;      // 0x07 	FSM pre-state register
sfr HCIN2CNTH                                   =  0xfd;      // 0x07 	hcin2 packet counter high
sfr HCIN2CNTL                                   =  0xfe;      // 0x07 	hcin2 packet counter low
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_SPI_H__
#define __GL5115_SPI_H__


sfr SPI_CTL                                     =  0x99;      // 0x0a 	SPI Control Register
sfr SPI_DRQ                                     =  0x9a;      // 0x0a 	SPI DMA/IRQ control Register.
sfr SPI_STA                                     =  0x9b;      // 0x0a 	SPI Status Register
sfr SPI_CLKDIV                                  =  0x9c;      // 0x0a 	SPI Clock Divide Control Register
sfr SPI_TXDAT                                   =  0x9d;      // 0x0a 	SPI tx fifo register
sfr SPI_RXDAT                                   =  0x9e;      // 0x0a 	SPI rx fifo register
sfr SPI_BCL                                     =  0x9f;      // 0x0a 	SPI Bytes Count Low Register
sfr SPI_BCH                                     =  0xa2;      // 0x0a 	SPI Bytes Count high Register
sfr SPI_DEBUG                                   =  0xa3;      // 0x0a 	SPI debug register
sfr SPI_RAND                                    =  0xc8;      // 0x0a 	SPI Randomizer Control Register
sfr SPI_SEED0                                   =  0xc9;      // 0x0a 	SPI Randomizer test seed Register.
sfr SPI_SEED1                                   =  0xca;      // 0x0a 	SPI Randomizer test seed Register.
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_UART_H__
#define __GL5115_UART_H__


sfr UART_BR                                     =  0x90;      // 0x0a 	UART BAUDRATE Register.
sfr UART_MODE                                   =  0x91;      // 0x0a 	UART mode setup Register.
sfr UART_CTL                                    =  0x92;      // 0x0a 	UART Control Register.
sfr UART_DRQ                                    =  0x93;      // 0x0a 	UART DRQ/IRQ register
sfr UART_STA                                    =  0x94;      // 0x0a 	UART Status Register
sfr UART_TXDAT                                  =  0x95;      // 0x0a 	UART TX FIFO register
sfr UART_RXDAT                                  =  0x97;      // 0x0a 	UART RX FIFO register
sfr UART_DEBUG                                  =  0x98;      // 0x0a 	UART debug register.
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_IRC_H__
#define __GL5115_IRC_H__


sfr IR_CTL                                      =  0xa5;      // 0x0a 	IR Control Register
sfr IR_STA                                      =  0xa6;      // 0x0a 	IR Status Register
sfr IR_LUC                                      =  0xa7;      // 0x0a 	IR low user code register.
sfr IR_HUC                                      =  0xa9;      // 0x0a 	IR high user code register.
sfr IR_KDC                                      =  0xaa;      // 0x0a 	IR key data code register.
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_DAC_H__
#define __GL5115_DAC_H__

sfr DAC_CTL                                     =  0xd5;      // 0x13/0x04 	DAC Control Register
sfr DAC_VOLUME0                                 =  0xd6;      // 0x13/0x04 	DAC Volume Control register 0
sfr DAC_VOLUME1                                 =  0xd7;      // 0x13/0x04 	DAC Volume Control register 1
sfr DAC_CH0_FIFO_CTL0                           =  0xd8;      // 0x13/0x04 	DAC Channel 0 FIFO Control Register 0
sfr DAC_CH0_FIFO_CTL1                           =  0xd9;      // 0x13/0x04 	DAC Channel 0 FIFO Control Register 1
sfr DAC_CH0_PCML                                =  0xda;      // 0x13/0x04 	DAC Channel 0 PCM DATA Low byte
sfr DAC_CH0_PCMM                                =  0xdb;      // 0x13/0x04 	DAC Channel 0 PCM DATA Middle byte
sfr DAC_CH0_PCMH                                =  0xdc;      // 0x13/0x04 	DAC Channel 0 PCM DATA High byte
sfr DAC_CH1_FIFO_CTL0                           =  0xdd;      // 0x13/0x04 	DAC Channel 1 FIFO Control Register 0
sfr DAC_CH1_FIFO_CTL1                           =  0xde;      // 0x13/0x04 	DAC Channel 1 FIFO Control Register 1
sfr DAC_CH1_PCML                                =  0xdf;      // 0x13/0x04 	DAC Channel 1 PCM DATA Low byte
sfr DAC_CH1_PCMH                                =  0xe1;      // 0x13/0x04 	DAC Channel 1 PCM DATA High byte
sfr I2S_CTL0                                    =  0xe2;      // 0x13/0x04 	I2S Control Register 0
sfr I2S_CTL1                                    =  0xe3;      // 0x13/0x04 	I2S Control Register 1
sfr DAC_ANALOG0                                 =  0xe4;      // 0x13/0x04 	DAC Analog Register 0
sfr DAC_ANALOG1                                 =  0xe5;      // 0x13/0x04 	DAC Analog Register 1
sfr DAC_TUNE0                                   =  0xe6;      // 0x13/0x04 	DAC tuning Control Register 0
sfr DAC_TUNE1                                   =  0xe7;      // 0x13/0x04 	DAC tuning Control Register 1
sfr PA_VOLUME                                   =  0xe9;      // 0x13/0x04 	PA VOLUME Control Register
sfr PA_CTL                                      =  0xea;      // 0x13/0x04 	PA Control Register
sfr PA_APCTL                                    =  0xeb;      // 0x13/0x04 	PA anti-pop Control Register
sfr DDV_CTL0                                    =  0xec;      // 0x13/0x04 	Direct drive Control Register 0
sfr DAC_CH0_SR_CTL                              =  0x90;      // 0x04 	DAC Channel 0 Sample Rate Control Register
sfr DAC_CH0_SR_GAIN                             =  0x91;      // 0x04 	DAC Channel 0 Sample Rate Gain Control Register
sfr DAC_CH0_SRFT_CTL0                           =  0x92;      // 0x04 	DAC Channel 0 Sample Rate Fine Tune Control Register 0
sfr DAC_CH0_SRFT_CTL1                           =  0x93;      // 0x04 	DAC Channel 0 Sample Rate Fine Tune Control Register 1
sfr DAC_CH0_SRFT_CTL2                           =  0x94;      // 0x04 	DAC Channel 0 Sample Rate Fine Tune Control Register 2
sfr DAC_CH0_SRFT_CTL3                           =  0x95;      // 0x04 	DAC Channel 0 Sample Rate Fine Tune Control Register 3
sfr DAC_CH0_SRFT_CTL4                           =  0x97;      // 0x04 	DAC Channel 0 Sample Rate Fine Tune Control Register 4
sfr DAC_CH1_SR_CTL                              =  0x98;      // 0x04 	DAC Channel 1 Sample Rate Control Register
sfr DAC_CH1_SR_GAIN                             =  0x99;      // 0x04 	DAC Channel 1 Sample Rate Gain Control Register
sfr DAC_CH1_SRFT_CTL0                           =  0x9a;      // 0x04 	DAC Channel 1 Sample Rate Fine Tune Control Register 0
sfr DAC_CH1_SRFT_CTL1                           =  0x9b;      // 0x04 	DAC Channel 1 Sample Rate Fine Tune Control Register 1
sfr DAC_CH1_SRFT_CTL2                           =  0x9c;      // 0x04 	DAC Channel 1 Sample Rate Fine Tune Control Register 2
sfr DAC_CH1_SRFT_CTL3                           =  0x9d;      // 0x04 	DAC Channel 1 Sample Rate Fine Tune Control Register 3
sfr DAC_CH1_SRFT_CTL4                           =  0x9e;      // 0x04 	DAC Channel 1 Sample Rate Fine Tune Control Register 4
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_ADC_H__
#define __GL5115_ADC_H__


sfr ADC_CTL0                                    =  0xf1;      // 0x13/0x04 	ADC Control Register 0
sfr AINOP_CTL                                   =  0xf2;      // 0x13/0x04 	AnalogIN OP Control Register
sfr ADC_GAIN0                                   =  0xf3;      // 0x13/0x04 	ADC gain Control Register0
sfr ADC_GAIN1                                   =  0xf4;      // 0x13/0x04 	ADC gain Control Register1
sfr ADC_TUNE0                                   =  0xf5;      // 0x13/0x04 	ADC tuning control Register 0
sfr ADC_TUNE1                                   =  0xf6;      // 0x13/0x04 	ADC tuning control Register 1
sfr ADC_FIFO_DAT                                =  0xf7;      // 0x13/0x04 	ADC FIFO data register
sfr ADC_FIFOCTL0                                =  0xf8;      // 0x13/0x04 	ADC FIFO control register 0
sfr ADC_FIFOCTL1                                =  0xfc;      // 0x13/0x04 	ADC FIFO control register 1
sfr ADC_CTL1                                    =  0xfd;      // 0x13/0x04 	ADC Control Register 1
sfr ADC_SRFT_CTL0                               =  0xa2;      // 0x04 	ADC Sample Rate Fine Tune Control Register 0
sfr ADC_SRFT_CTL1                               =  0xa3;      // 0x04 	ADC Sample Rate Fine Tune Control Register 1
sfr ADC_SRFT_CTL2                               =  0xa4;      // 0x04 	ADC Sample Rate Fine Tune Control Register 2
sfr ADC_SRFT_CTL3                               =  0xa5;      // 0x04 	ADC Sample Rate Fine Tune Control Register 3
sfr ADC_SRFT_CTL4                               =  0xa6;      // 0x04 	ADC Sample Rate Fine Tune Control Register 4
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_SPDIFRX_H__
#define __GL5115_SPDIFRX_H__


sfr SPDIFRX_CTL                                 =  0xb0;      // 0x13 	SPDIFRX Control Register
sfr SPDIFRX_STAT                                =  0xb1;      // 0x13 	SPDIFRX Statue Register
sfr SPDIFRX_CSTAT                               =  0xb2;      // 0x13 	SPDIFRX Channel Statue Register
sfr SPDIFRX_DEBUG                               =  0xb3;      // 0x13 	SPDIFRX Debug Register
sfr SPDIFRX_DAT                                 =  0xb4;      // 0x13 	SPDIFRX Data Register
sfr SPDIFRX_CNT_WR0                             =  0xb5;      // 0x13 	SPDIFRX Counter for Write Register 0
sfr SPDIFRX_CNT_WR1                             =  0xb6;      // 0x13 	SPDIFRX Counter for Write Register 1
sfr SPDIFRX_CNT_WR2                             =  0xb7;      // 0x13 	SPDIFRX Counter for Write Register 2
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_LCD_H__
#define __GL5115_LCD_H__


sfr LCD_MODE                                    =  0x8a;      // 0x06 	LCD Mode Control Register
sfr LCD_DATA0                                   =  0x8b;      // 0x06 	COM[3:0] of SEG0 and SEG1; SEG[7:0] of COM0
sfr LCD_DATA1                                   =  0x8c;      // 0x06 	COM[3:0] of SEG2 and SEG3; SEG[7:0] of COM1
sfr LCD_DATA2                                   =  0x8d;      // 0x06 	COM[3:0] of SEG4 and SEG5; SEG[7:0] of COM2
sfr LCD_DATA3                                   =  0x90;      // 0x06 	COM[3:0] of SEG6 and SEG7; SEG[7:0] of COM3
sfr LCD_DATA4                                   =  0x91;      // 0x06 	COM[3:0] of SEG8 and SEG9; SEG[7:0] of COM4
sfr LCD_DATA5                                   =  0x92;      // 0x06 	COM[3:0] of SEG10 and SEG11; SEG[7:0] of COM5
sfr LCD_DATA6                                   =  0x93;      // 0x06 	COM[3:0] of SEG12 and SEG13; SEG[7:0] of COM6
sfr LCD_DATA7                                   =  0x94;      // 0x06 	COM[3:0] of SEG14 and SEG15; SEG[7:0] of COM7
sfr LCD_DATA8                                   =  0x95;      // 0x06 	COM[3:0] of SEG16 and SEG17
sfr LCD_DATA9                                   =  0x97;      // 0x06 	COM[3:0] of SEG18 and SEG19
sfr LCD_DATA10                                  =  0x98;      // 0x06 	COM[3:0] of SEG20 and SEG21
sfr LCD_DATA11                                  =  0x99;      // 0x06 	COM[3:0] of SEG22 and SEG23
sfr LCD_DATA12                                  =  0x9a;      // 0x06 	COM[3:0] of SEG24 and SEG25
sfr LCD_DATA13                                  =  0x9b;      // 0x06 	COM[3:0] of SEG26 and SEG27
sfr LCD_DATA14                                  =  0x9c;      // 0x06 	COM[3:0] of SEG28 and SEG29
sfr LCD_DATA15                                  =  0x9d;      // 0x06 	COM[3:0] of SEG30 and SEG31
//---------------------------------------------------------------------//
#endif

#ifndef __GL5115_GPIO_H__
#define __GL5115_GPIO_H__


sfr GPIOAOUTEN                                  =  0xa2;      // 0x06 	General Purpose Input Output Group A Output Enable
sfr GPIOAINEN                                   =  0xa3;      // 0x06 	General Purpose Input Output Group A Input Enable
sfr GPIOADAT                                    =  0xa4;      // 0x06 	General Purpose Input Output Group A Data
sfr GPIOAPUEN                                   =  0xa5;      // 0x06 	General Purpose Input Output Group A PU Enable
sfr GPIOAPDEN                                   =  0xa6;      // 0x06 	General Purpose Input Output Group A PD Enable
sfr GPIOBOUTEN                                  =  0xa7;      // 0x06 	General Purpose Input Output Group B Output Enable
sfr GPIOBINEN                                   =  0xa9;      // 0x06 	General Purpose Input Output Group B Input Enable
sfr GPIOBDAT                                    =  0xaa;      // 0x06 	General Purpose Input Output Group B Data
sfr GPIOBPUEN                                   =  0xab;      // 0x06 	General Purpose Input Output Group B PU Enable
sfr GPIOBPDEN                                   =  0xac;      // 0x06 	General Purpose Input Output Group B PD Enable
sfr GPIOCOUTEN                                  =  0xad;      // 0x06 	General Purpose Input Output Group C Output Enable
sfr GPIOCINEN                                   =  0xae;      // 0x06 	General Purpose Input Output Group C Input Enable
sfr GPIOCDAT                                    =  0xaf;      // 0x06 	General Purpose Input Output Group C Data
sfr GPIOCPUEN                                   =  0xb0;      // 0x06 	General Purpose Input Output Group C PU Enable
sfr GPIOCPDEN                                   =  0xb1;      // 0x06 	General Purpose Input Output Group C PD Enable
sfr GPIODOUTEN                                  =  0xb2;      // 0x06 	General Purpose Input Output Group D Output Enable
sfr GPIODINEN                                   =  0xb3;      // 0x06 	General Purpose Input Output Group D Input Enable
sfr GPIODDAT                                    =  0xb4;      // 0x06 	General Purpose Input Output Group D Data
sfr GPIODPUEN                                   =  0xb5;      // 0x06 	General Purpose Input Output Group D PU Enable
sfr GPIODPDEN                                   =  0xb6;      // 0x06 	General Purpose Input Output Group D PD Enable
sfr GPIOEOUTEN                                  =  0xb7;      // 0x06 	General Purpose Input Output Group E Output Enable
sfr GPIOEINEN                                   =  0xb8;      // 0x06 	General Purpose Input Output Group E Input Enable
sfr GPIOEDAT                                    =  0xb9;      // 0x06 	General Purpose Input Output Group E Data
sfr GPIOEPUEN                                   =  0xba;      // 0x06 	General Purpose Input Output Group E PU Enable
sfr GPIOEPDEN                                   =  0xbb;      // 0x06 	General Purpose Input Output Group E PD Enable
sfr GPIOFOUTEN                                  =  0xbc;      // 0x06 	General Purpose Input Output Group F Output Enable
sfr GPIOFINEN                                   =  0xbd;      // 0x06 	General Purpose Input Output Group F Input Enable
sfr GPIOFDAT                                    =  0xbf;      // 0x06 	General Purpose Input Output Group F Data
sfr GPIOFPUEN                                   =  0xc1;      // 0x06 	General Purpose Input Output Group F PU Enable
sfr GPIOFPDEN                                   =  0xc2;      // 0x06 	General Purpose Input Output Group F PD Enable
sfr GPIOGOUTEN                                  =  0xc3;      // 0x06 	General Purpose Input Output Group G Output Enable
sfr GPIOGINEN                                   =  0xc4;      // 0x06 	General Purpose Input Output Group G Input Enable
sfr GPIOGDAT                                    =  0xc5;      // 0x06 	General Purpose Input Output Group G Data
sfr GPIOGPUEN                                   =  0xc6;      // 0x06 	General Purpose Input Output Group G PU Enable
sfr GPIOGPDEN                                   =  0xc7;      // 0x06 	General Purpose Input Output Group G PD Enable
sfr MFP_CTL0                                    =  0xc8;      // 0x06 	Multi-Function PAD Control Register 0
sfr MFP_CTL1                                    =  0xc9;      // 0x06 	Multi-Function PAD Control Register 1
sfr MFP_CTL2                                    =  0xca;      // 0x06 	Multi-Function PAD Control Register 2
sfr MFP_CTL3                                    =  0xcb;      // 0x06 	Multi-Function PAD Control Register3
sfr MFP_CTL4                                    =  0xcc;      // 0x06 	Multi-Function PAD Control Register4
sfr MFP_CTL5                                    =  0xcd;      // 0x06 	Multi-Function PAD Control Register5
sfr MFP_CTL6                                    =  0xce;      // 0x06 	Multi-Function PAD Control Register6
sfr MFP_CTL7                                    =  0xcf;      // 0x06 	Multi-Function PAD Control Register7
sfr MFP_CTL8                                    =  0xd2;      // 0x06 	Multi-Function PAD Control Register8
sfr AD_Select                                   =  0xd3;      // 0x06 	ANALOG/DIGTAL Select
sfr AD_Select1                                  =  0xdc;      // 0x06 	ANALOG/DIGTAL Select1
sfr PADPUPD                                     =  0xd4;      // 0x06 	PADPUPD
sfr PADDRV0                                     =  0xd5;      // 0x06 	Pad Drive CTL0
sfr PADDRV1                                     =  0xd6;      // 0x06 	Pad Drive CTL1
sfr PADDRV2                                     =  0xd7;      // 0x06 	Pad Drive CTL2
sfr PADDRV3                                     =  0xd9;      // 0x06 	Pad Drive CTL3
sfr PADDRV4                                     =  0xda;      // 0x06 	Pad Drive CTL4
sfr DBGSEL                                      =  0xdb;      // 0x06 	Debug Select Register
sfr DBGAOE                                      =  0xe2;      // 0x06 	DEBUGAOUTEN
sfr DBGAIE                                      =  0xe3;      // 0x06 	DEBUGAINEN
sfr DBGBOE                                      =  0xe4;      // 0x06 	DEBUGBOUTEN
sfr DBGBIE                                      =  0xe5;      // 0x06 	DEBUGBINEN
sfr DBGCOE                                      =  0xe6;      // 0x06 	DEBUGCOUTEN
sfr DBGDOE                                      =  0xe9;      // 0x06 	DEBUGDOUTEN
sfr LED_SEG_RC_EN                               =  0xeb;      // 0x06 	LED SEG Restrict Current
sfr LED_SEG_BIAS_EN                             =  0xec;      // 0x06 	LED SEG Restrict Current
sfr ChipVersion               					=  0xf4;      // 0x06  Chip Version Register
//---------------------------------------------------------------------//
#endif
#endif
