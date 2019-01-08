/*
*******************************************************************************
*                ACTOS AP
*        AG decoder header file
*
*        (c) Copyright, Actions Co,Ld.
*             All Right Reserved
*
*******************************************************************************
*/

#ifndef         _MP3_DE_H
#define         _MP3_DE_H
#include        <actos.h>
#include        <stringH.h>
#include        <codec.h>



//接口变量
//FIXME:

extern  decControlInfor_t g_decControlInfor;     //codec需要的控制信息，包括快进快退，淡入淡出
#ifndef PC
extern decSongInfor_t g_decSongInfor;  //codec计算的时间和比特率都存在这个变量里面,audio解码中断时会更新
#else
extern  _declspec(dllimport) decSongInfor_t g_decSongInfor;  //codec计算的时间和比特率都存在这个变量里面,audio解码中断时会更新
#endif
extern  decCurStatus_t g_decCurStatus;  // codec解码中断会更新状态song等，InitCodecType更新status根据g_decInit的songtype来更新,f1表示错误，时间中断更新时间信息

extern  decReadFilePara_t g_decReadFilePara;    //获得文件的信息，包括断点信息，codec根据该参数进行文件读写，read hook根据这个变量来读数据

extern  decBreakPointInfor_t g_decBreakPointInfor;  //断点控制信息，decode结构体变量，解码中断时保存断点的控制信息
extern  decEffectInfor_t   g_decEffectInfor;
extern uint8 InputMode;


//接口函数

extern void decInit(void);
extern bool decstart(void);
extern void decPause(void);
extern void decContinue(void);
extern bool decExit(void);
extern bool decSetVolume(void);


#ifdef CODEC_TEST
extern void codec_test(void);
#endif
#ifndef PC

#pragma ROM(large)
extern void decAudioIntSub(void);
#pragma ROM(huge)

#pragma ROM(large)
extern void decDMAIntSub(void);
#pragma ROM(huge)

//#pragma ROM(large)
extern void decTimerIntSub(void);
#pragma ROM(huge)

#endif		/* #ifndef PC */
extern void decFFRevDeal(void);
extern void decEffectDeal(void);
extern void deal_Effectcoff(void);
extern void deal_Effectcof_Switch(void);
extern void Init_Effectcoff(uint8 SampleRate);

extern void decAddainit(adin_t ain);
extern void decAddaexit(void);





#endif  /* _MP3_DE_H */
