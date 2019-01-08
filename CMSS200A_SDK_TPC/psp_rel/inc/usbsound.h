/********************************************************************************
*                              AS211A
*                            Module: USB Sound Card
*                 Copyright(c) 2001-2010 Actions Semiconductor,
*                            All Rights Reserved.
*
* History:
*      <author>    <time>            <version >             <desc>
*      wanghaijing 2010-8-24 10:11     1.0                build this file
********************************************************************************/
/*!
* \file     usbsound.h
* \brief    .
* \author   wanghaijing
* \par      GENERAL DESCRIPTION:
*               这里对文件进行描述
* \par      EXTERNALIZED FUNCTIONS:
*               这里描述调用到外面的模块
*
*      Copyright(c) 2001-2010 Actions Semiconductor, All Rights Reserved.
*
* \version 1.0
* \date  2011/01/18
*******************************************************************************/


#ifndef _USBDISK_H
#define _USBDISK_H

#ifdef __C251__
typedef struct
{
    uint32 line_sta;//if is insert
    uint32 set_sample;//sample flag
    uint32 sample_rate;//rate
    uint32 start_play;//if is start play
    uint32 start_record;//if is start record
    uint32 volume_chg;//if voluem syn
    uint32 volume_value;//syn volume value
    uint32 hid_idle_time;//hid idle time
} usound_status_t;

typedef struct
{
    uint8 play_sample_rate;//download sample rate 44.4k,48k
    uint8 record_sample_rate;//record sample rate 16k,48k...
    uint8 bit_depth; //bit depth 16bit,24bit
} usound_param;


//for commond set
typedef enum
{
    SET_PLAY_FLAG = 0,//play status set
    SET_HID_OPERS, //cmd use
    SET_RECORD_SAMPLE_RATE, //sample set
    SET_ADJUST_TIMER, //adjust
    SET_LINE_STATUS,//status
    SET_VOLUME_FLAG,//volume syn flag
    SET_HID_CHANGE,//hid need report
    SET_CARD_INOUT,
    SET_TTS_FLAG,
    SET_USPEAKER_24BIT
} usound_set_cmd_e;

//frequence sample type
typedef enum
{
    SAMPLE_16K=0,
    SAMPLE_44K1,
    SAMPLE_48K,
    SAMPLE_96K
}sample_rate_type;

enum
{
    BIT_DEPTH16 = 16,
    BIT_DEPTH24 = 24
};


extern uint8 usound_init(usound_param *pud_param);
void usound_exit(void);

uint8 usound_status(void);
void usound_key_msg(uint8 keymsg); //keymsg:0x10,switch to next music; 0x20,switch to pre music; 0x08,play/pause music
extern void usound_inner_set_cmd(uint8 cmd, uint8 cmd_param);
extern void get_usound_status(usound_status_t *pstatus);

#endif /*__C251__*/

#endif /*_USBDISK_H*/

