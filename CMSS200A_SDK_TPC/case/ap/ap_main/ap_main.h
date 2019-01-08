/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main header file.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_record.h,v 1.5 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */

#ifndef AP_MAIN_H
#define AP_MAIN_H

#include "actos.h"
#include "ap_common.h"
#include "ap_report.h"


extern uint8 ap_id[MAX_AP_NUM];
extern bool with_fm;
extern vm_cfg_t g_modifyval;

extern char report_buf[16]; //存诸播报数据的buffer
extern info_report_t info;

uint16 near USBDevice(int16 State);
uint16 near USBDeviceService (int16 State);
void ReadModifyInfor(void);
void read_var(void);
int16 AP_Loop_Exec(int16 ap_name);
void open_USBAndCard_device(void);
void  OpenPA(uint8 volume);
void  OpenDAC(void);
uint8 start_init(uint8 first_init);
int16 InitSystemtime(void);
uint8 check_alarm_msg(void);

#endif  /* AP_MAIN_H*/
