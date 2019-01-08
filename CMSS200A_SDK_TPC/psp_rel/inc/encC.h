#ifndef _ADPCMENC_H
#define _ADPCMENC_H

#include "codec.h"
#include "typeext.h"

void    encInit(void);                       //encoder初始化函数

void    encStart(void);                     //开始编码函数

uchar    encGetBitstream(void);             //获得编码后的音频数据，具体数据放在相应的buffer中

void    encPause(void);                    //暂停录音子程序

void    encStop(void);

void    encContinue(void);                 //继续录音子程序

void    encExit(void);                     //退出处理子程序

#ifndef PC

//#pragma ROM(large)
extern void    encTimerIntSub(void);              //timer中断处理子程序
extern void    encAudioIntSub(void);                //音频中断处理子程序
//extern void    encDMAIntSub(void);
//#pragma ROM(huge)

#else
extern void    encTimerIntSub(void);              //timer中断处理子程序
extern void    encAudioIntSub(void);                //音频中断处理子程序
#endif
extern encControlInfor_t g_encControlInfor;
extern encWriteFilePara_t g_encWriteFilePara;
extern encStatus_t g_encStatus;


#endif      //_ADPCMENC_H 
