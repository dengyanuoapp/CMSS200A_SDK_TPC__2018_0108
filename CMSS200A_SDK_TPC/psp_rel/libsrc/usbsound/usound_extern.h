/********************************************************************************
 *                              AS260A
 *                            Module: USB Sound
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2012-10-23 16:40     1.0                build this file
 ********************************************************************************/
/*!
 * \file     usound_extern.h
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               lib包出的函数接口、宏及变量声明
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#ifndef __USOUND_EXTERN_H__
#define __USOUND_EXTERN_H__

#define USB_IDLE         0x00
#define PLAY_RUNNING     0x01
#define RECORD_RUNNING   0x02
#define USOUND_PLUG_OUT  0x04

#define  USB_IS_CONNECT    0x00
#define  USB_CONNECT_RESET 0x01
#define  USB_DISCONNECT    0x02

// sample rate conversion selection
#define K_SRC_1_1            0
#define K_SRC_6_1            1
#define K_SRC_4_1            2
#define K_SRC_3_1            3
#define K_SRC_2_1            4
#define K_SRC_3_2            5
#define K_SRC_1_2            6
#define K_SRC_1_4            7

typedef struct
{
    uint8 play_sample_rate;//download sample rate 44.4k,48k
    uint8 record_sample_rate;//record sample rate 16k,48k...
    uint8 bit_depth; //bit depth 16bit,24bit
} usound_param;

//frequence sample type
typedef enum
{
    SAMPLE_16K=0,
    SAMPLE_44K1,
    SAMPLE_48K,
    SAMPLE_96K
}sample_rate_type;

extern uint8 usb_state;
extern uint8 g_PAOpenFlag;
extern usound_param gud_param;
extern uint8 g_record_packet_size;

extern void usound_exit(void);
extern uint8 usound_init(usound_param *pud_param);
extern void record_test_init(void);
extern uint8 is_usb_connect(void);
extern void usound_key_msg(uint8 key_value);
extern void descriptor_init(usound_param *pud_param);

#endif
