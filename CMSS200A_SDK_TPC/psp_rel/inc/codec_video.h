#ifndef _BASELVIDEO_H
#define _BASELVIDEO_H
#include "typeext.h"


#define VIDEO_AMV              0
#define VIDEO_AVI              1

#define VIDEODEC_OK            0
#define VIDEODEC_ERROR         0xff
#define VIDEODEC_UNSUPPORTED   0xfe
#define VIDEODEC_STRM_ERROR    0xfd
#define VIDEODEC_TIMEOUT       0xfc
#define VIDEODEC_FILEHEAD      0xfb
#define VIDEODEC_FILEEND       0xfa
#define VIDEODEC_END           0xf9
#define VIDEODEC_PAUSE	       0x01
#define	VIDEODEC_FADEOUT	   0xf8


typedef struct
{
    uint16 x;//x坐标
    uint16 y;//y坐标
    uint16 w;//宽
    uint16 h;//高
} VideoRegion_t;


typedef  struct
{
    uint16 LcmWidth;// Displaying deviece width;
    uint16 LcmHeigh;// Displaying deviece height;
    uint8 RGBScale;//RGB 565
    uint8 FadeoutPara;// 0,无淡出; 1-0.1s;2-0.
    uint8 ShowProgressBar;//进度条显示
    uint8 USetW_spare1;//软音量
    uint8 Reserve;//保留
} ColorLcmInfor_t;


typedef struct
{
    uint32     VideoFileFLashDot;  // breakpointer  flash counter
    uint32     Framecnt;
    uint16     blockbyte;  //breakpointer  byte start
    uint8  TimeSec;          // second
    uint8  TimeMin;          // minute
    uint8  TimeHou;          // hour
    uint8  Reserve;
} VideoFileBreakPntInfor_t; //


typedef struct
{
    handle fileHandle;
    ColorLcmInfor_t *pLcmInfor;
    VideoFileBreakPntInfor_t *pBreakInfor;
    VideoRegion_t  *pregion;
} VideoDataParam_t;


void videosetvol(uint8 vol);
uint8 VideoViewInit(uint8 video_type, VideoDataParam_t *pVideoDataParam);
void VideoViewExit(uint8 video_type);
uint8 VideoDecPlay(uint8 video_type);
uint8 VideoDecStep(uint8 video_type,  int8 step);
uint8 VideoDecStop(uint8 video_type);
uint8 VideoDecPause(uint8 video_type);
uint8 CheckFadeoutend(void);
void FadeOutSetting(void); 	//fadeout  must >0;
void Video_DMA0Init();
uint8 CheckFile(uint8 video_type, handle fp);
#endif
