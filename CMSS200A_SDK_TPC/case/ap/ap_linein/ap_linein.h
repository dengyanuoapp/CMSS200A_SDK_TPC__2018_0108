/*
 ********************************************************************************
 *                       ACTOS
 *                  ap_radio head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : ap_radio.h
 * By     : ybfu
 * Version: 1> v1.00     first version     2004-1-12 13:45
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _APRADIO_H
#define _APRADIO_H
#include "actos.h"
#include "ap_common.h"
#include "filesys.h"
#include "basal.h"
#include "stringH.h"
#include "ap_report.h"
#include "eqtool.h"

#ifdef __C251__

extern int8 AuCodecRun;
extern int8 AuCodecEffect;
extern int16 SentenceDectTime;
extern uint8 SentenceDectEnergy;
extern int8 SetenceFlag;
extern uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
extern uint32 m_decSpdifddrSav;

/* main.c */
uint16 show_ui_func(uint8 mode);
void init_linein_channel(void);
void exit_linein_func(void);
void linein_open_dac(void);
void linein_open_pa(void);
void show_volume_ui(void);
void show_linein_ui(void);
uint8 callback_eq(eq_t eq_type, uint8 *buf);
uint8 deal_eq_msg(uint8 key);
void exit_linein_func(void);

//uint8 deal_vol_msg(uint8 key);
void ui_display(void);

/* deal_key_message.c */
int16 deal_key_msg(void);

/* display.c */
void RefreshMainScreen(void);

/* function.c */
void OutPutVolume(uint16 vol);
void volume_callback(uint16 Vol);

/* Analog */
void decinitAnalog(adin_t ain);
void decstartAnalog(adin_t ain);
void decAnalogexit(void);
#ifdef LINEIN_AP
uint8 report_cur_ap(uint8 report_msg);
#endif

#ifdef EAR_PROTECT
extern int16 g_autoswitch_counter;
#endif
#endif /* __C251__ */
#endif/*_APRADIO_H*/

