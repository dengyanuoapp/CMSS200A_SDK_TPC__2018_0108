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
#include "actos.h"
#include "ui_res.h"
#include "ap_common.h"
#include "usbdisk.h"

//#define  BACKLIGHTCTL

/***************func of ap*********************/
bool UDiskInitAll(int para);
int16 UdiskExitAll(void);
void RamToMcu(void);
void MsgManager(uint8 dispIndi);
void Display(uint8 dispIndi);

void FlashLed(void);

void ShowFwVersion(void);
void ShowLogoUPG_DONE(void);
/***************************************************/

#endif
//#endif /*__IAR_SYSTEMS_ICC*/
