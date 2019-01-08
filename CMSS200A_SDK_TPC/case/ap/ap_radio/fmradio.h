/*
 ********************************************************************************
 *                       ACTOS
 *                  fm radio head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : fmradio.h
 * By     : rcmai,gongee
 * Version: 1> v1.00     first version     2003-11-20 9:14
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _FMRADIO_H
#define _FMRADIO_H

#include "actos.h"
#include "basal.h"
#ifdef __C251__

//Open VCCOUT(FM Power) add for s2 by ccm 04-06-18
//#define FMPOWER_CTRL_PORT     (uint8*) GPIOEOUTEN
//#define FMPOWER_CTRL_PORTNUM      0x04


//#define OpenFMPower()     //output8(REGDF_POWER_CTRL,(input8(REGDF_POWER_CTRL)|0x03))
//#define CloseFMPower()   //output8(REGDF_POWER_CTRL,((input8(REGDF_POWER_CTRL)&0xfd)|0x01))
typedef enum
{
    DOWN, //[0]下降
    UP //[1]上升
} FM_direct_t;

typedef enum
{
    Stereo, //[0]立体声
    Mono //[1]单声道
} FM_Audio_t;

typedef enum
{
    releaseMUTE, //解除静音
    SetMUTE //静音
} FM_MUTE_t;

typedef struct
{
    FM_Audio_t FM_Stereo_Status; //0，立体声；非0，单声道
    uint16 FM_CurrentFreq; //当前频率
} FMStatus_Tab_t;

typedef enum //波段模式

{
    Bank_US_Europe,
    Bank_Japan
} Station_bank_t;

extern uint8 FM_Init(uint8 band, uint8 slevel);
extern void FM_Exit(void);
extern uint8 FM_SetFreq(uint16 Freq);
extern uint8 FM_GetStatus(FMStatus_Tab_t *Buf);
extern uint8 FM_Mute(FM_MUTE_t mode);
extern uint8 FM_Standby(void);
//extern uint8 FM_Search(fm_play_status_t * Buf,uint8 SeekDirect);
extern void FM_VolumeSet(uint8 Vol);
//extern uint8 FM_Search(uint16 Freq,uint8 SeekDirect);
extern uint8 FM_Search(fm_play_status_t *Buf, uint16 Freq, uint8 SeekDirect);
extern void SetVolume(uint8 Vol);
#endif /* __C251_ */
#endif/*_FMRADIO_H*/
