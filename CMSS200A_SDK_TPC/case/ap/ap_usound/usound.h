/*
 *********************************************************************
 *File Name :udsik.h
 *author    : Mars
 *Date      : 2009-11-13 16:39
 *Version   : 1.00
 *******************************************************************
 */
#ifndef _UDISK_H_
#define _UDISK_H_
#include "ap_common.h"
#include "actos.h"
#include "ui_res.h"
#include "usbsound.h"
//#include "ap_music.h"

extern bool urec_en;
//extern uint8 USB_Status;
extern uint8 max_gain; /*max_gain is Q4.4 format*/
extern bool need_draw;
extern int16 g_result;
extern uint8 cur_vol_index;
extern uint8 vol_change_flag;

/***************func of ap*********************/
void us_open_pa(void);
void us_open_dac(void);
int16 Usound_loop(void);
int16 usound_rec(void);
int16 ChangeVolume(void);

void OutPutVolume(uint8 vol);
void volume_callback(int16 Vol);
void change_vol(uint8 index);
void set_vol(uint8 index);
/***************************************************/

#endif /*end of _UDISK_H_*/
