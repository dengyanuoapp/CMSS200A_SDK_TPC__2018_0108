#ifndef _SIMVIDEODATA_H
#define _SIMVIDEODATA_H

#include "ak211x.h"
#include "typeext.h"
#include "codec_video.h"
#include "simulatorpc.h"

uint8 AMV_INIT(VideoDataParam_t *pVideoDataParam);
uint8 AMV_Play();
uint8 AMV_FrontStep(int8 step);
uint8 AMV_BackStep(int8 step);
uint8 AMV_Step(int8 step);
uint8 AMV_Stop();
void  AMV_EXIT();
uint8 AVI_INIT(VideoDataParam_t *pVideoDataParam);
uint8 AVI_Play();
uint8 AVI_FrontStep(int8 step);
uint8 AVI_BackStep(int8 step);
uint8 AVI_Step(int8 step);
uint8 AVI_Stop();
void  AVI_EXIT();

extern uint16 readint16le();
extern uint8 mygetchar();
extern void get512byte();
extern void get512bytefb();
extern void BreakPointProcess();
extern void CalBInofr();
extern void aviCalBInofr();
extern uint32 readint32le();
extern void seektodata(int16 pos);


uint8  mygetchar();
uint8  mygetcharfb();
void  fileinit();
void  seektodata(int16 pos);
//uint8 *mygetjram(uint16  *streamlen);
//uint8 *mygetjram1(uint16 *streamlen,uint16 *startpos);
void modifypsrc(int16 datalen);
void BreakPointProcess();



//video dec data
extern uint16 near imgHigh   ;
extern uint16 near imgWide   ;
extern uint32 near chunktype  ;
extern uint32 near chunklen  ;
extern int32  near bpblockcnt;
extern int16  near bpblockbyte;
extern int32  near bpframecnt;
extern uint8 near alltime_sec  ;
extern uint8 near alltime_min  ;
extern uint8 near alltime_hour  ;
extern uint8 near time_sec  ;
extern uint8 near time_min  ;
extern uint8 near time_hour  ;
extern uint8 near reserved  ;
extern uint32 near alltime;
extern uint32 near curtime;
extern uint32 near frmaepersec  ;
extern uint32 near samplespersec  ;
extern uint32 near allframes   ;
extern uint16 near decHigh ;
extern uint16 near decWide ;

// 变量类型冲突，在图片AP中不使用该申明
#ifndef AP_PHOTO_EXPORTS
extern uint8  near showlines;
#endif

extern uint16 near showlength;
extern int16  near  source_count  ;
extern uint8 near *near psrc ;
extern int32  near blockcnt ;
extern handle near  vidfp;
extern uint8 near downscale;
extern uint8 near RGBScale;//RGB 565
extern uint8 near FadeoutPara;// 0,无淡出; 1，1S淡出; 2,2S淡出
extern uint8 near ShowProgressBar;//进度条显示
extern uint8 near USetW_spare1;//软音量

extern uint8   near reg_adh_bak;//mcsr5
extern uint8   near reg_aeh_bak;//mcsr6
extern uint8   near reg_afh_bak;//mcsr7
extern uint8   near reg_b0h_bak;//mcsr8
extern uint8   near reg_b1h_bak;//mcsr9
extern uint8   near reg_b4h_bak;//mcsr11
extern uint8   near reg_bfh_bak;//mcsr12
extern VideoDataParam_t near *VideoDataParam;
extern uint8  near audendflag;
extern uint8  near showflag;
extern uint8  near fadeTime;
extern uint8  near readfileflag;

//reg declare
#define DAC_D_DEBUG

//jpg  reg
sbit AIE_Ecodec;
sbit AIE_Elcd;

sbit MemBankCtl_MenBankEn;
sbit MemBankCtl_IntVectPage;
sbit MemBankCtl_PCMMU2;

sbit MRCR2_VideoReset;
sbit MCSR3_VideoCLK1;
sbit MCSR3_VideoCLK0;
sbit MCSR4_MCUCLK1;
sbit MCSR4_MCUCLK0;
sbit MCSR5_JRAM3CLK1;
sbit MCSR5_JRAM3CLK0;
sbit MCSR6_JRAM1CLK1;
sbit MCSR6_JRAM1CLK0;
sbit MCSR7_JRAM2CLK1;
sbit MCSR7_JRAM2CLK0;
sbit MCSR7_B1CLK2;
sbit MCSR7_B1CLK1;
sbit MCSR7_B1CLK0;

sbit MCSR8_MURAM1MCLK2;
sbit MCSR8_MURAM1MCLK1;
sbit MCSR8_MURAM1MCLK0;
sbit MCSR8_MURAM1LCLK2;
sbit MCSR8_MURAM1LCLK1;
sbit MCSR8_MURAM1LCLK0;
sbit MCSR9_DRAM251BCLK2;
sbit MCSR9_DRAM251BCLK1;
sbit MCSR9_DRAM251BCLK0;

sbit AMVCTL_end;
sbit AMVEN_TimeOutInt;
sbit AMVEN_IntDis;
sbit AMVEN_DecStart;
sbit AMVIE_Timeout;
sbit AMVIE_SliceOK;
sbit AMVIE_StreamError;
sbit AMVIE_BufferEmpty;
sbit AMVIE_DecReady;
sbit AMVIE_IntBit;
sbit JPGMemMapReg_JRAM12Map;

sbit MRCR1_AudReset;
sbit MRCR2_DACReset;
sbit MCSR1_AudCLK1;
sbit MCSR1_AudCLK0;
sbit MCSR3_I2SRCLK;

sbit MCSR5_PCMRAMCLK1;
sbit MCSR5_PCMRAMCLK0;

sbit MCSR6_MURAM2CLK1;
sbit MCSR6_MURAM2CLK0;

sbit MCSR7_MURAM1HCLK2;
sbit MCSR7_MURAM1HCLK1;
sbit MCSR7_MURAM1HCLK0;

sbit MCSR11_PCMRAMCLKEN;
sbit MCSR11_MURAM2CLKEN;
sbit MCSR11_MURAM1HCLKEN;
sbit MCSR11_MURAM1MCLKEN;
sbit MCSR11_MURAM1LCLKEN;

sbit MCSR11_D251BCLKEN;
sbit MCSR12_JRAM3CLKEN;

sbit AuCodecFIFOCtl_AlmostEmpty;
sbit AuCodecFIFOCtl_empty;
sbit AuDecEncCtl_NewFrame;
sbit AuCodecIP_FrameRdyIP;
sbit AuCodecDebug_State;
sbit DAC_CTL3_DADDBG;

sbit  MRCR2_DMA012Reset;
sbit  MCSR4_DMA012CLK;
sbit  DMAxCTL0_Abort;
sbit  AIPH_DMA012;
sbit  AIPL_DMA012;
sbit  AIE_DMA012;
sbit  DMAxCTL1_Start;
sbit  DMAxIP_Tcirq;

sbit MCSR11_PCMRAMCLKEN;
sbit MCSR11_MURAM2CLKEN;
sbit MCSR11_MURAM1HCLKEN;
sbit MCSR12_JRAM1CLKEN;
sbit MCSR12_JRAM2CLKEN;

sbit MemMapReg_dis1;
sbit MemMapReg_dis0;

sbit BEPCtl1_Fadeinena;
sbit AuDecEncCtl_Fadeoutena;
sbit BEPCtl1_Fadeoutena;
sbit BEPCtl1_Fadeoutp;
sbit BEPCtl2_PCMOUT;
sbit AUDIOPLL_CTL_EN;
sbit DAC_CTL2_02;

#endif
