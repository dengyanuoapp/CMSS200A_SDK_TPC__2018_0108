/*
********************************************************************************
*                       AS260A
*                  I2C  head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : I2C.h
* By     : Mikeyang
* Version: 1> v1.00     first version     2003-11-20 9:14
********************************************************************************
********************************************************************************
*/
#ifndef _I2C_H
#define _I2C_H

#include "actos.h"

//#define debug_mode

#ifdef debug_mode
/* Ä£ÄâIIC  Òý½ÅÅäÖÃºê*/
#define     SFR_SCL_OUT_EN          GPIODOUTEN   //GPIOBOUTEN
#define     SFR_SCL_IN_EN             GPIODINEN //GPIOBINEN
#define     SFR_SCL_DATA              GPIODDAT //GPIOBDAT

#define     SFR_SDA_OUT_EN          GPIOCOUTEN
#define     SFR_SDA_IN_EN             GPIOCINEN
#define     SFR_SDA_DATA              GPIOCDAT
//use GPIO_C2 as SDA     GPIO_D2
#define     SDA_PORT_NUM            0x02   //0x00         //0x01          //c1 output enalbe

//use GPIO_D2 as SCL     GPIO_C2
#define     SCL_PORT_NUM            0x02  //0x05          //0x07           //b7 output enable

#else
/* Ä£ÄâIIC  Òý½ÅÅäÖÃºê*/
#define     SFR_SCL_OUT_EN          GPIODOUTEN   //GPIOBOUTEN
#define     SFR_SCL_IN_EN             GPIODINEN //GPIOBINEN
#define     SFR_SCL_DATA              GPIODDAT //GPIOBDAT

#define     SFR_SDA_OUT_EN          GPIODOUTEN
#define     SFR_SDA_IN_EN             GPIODINEN
#define     SFR_SDA_DATA              GPIODDAT

//use GPIO_D5 as SDA     GPIO_D5
#define     SDA_PORT_NUM            0x05   //0x00         //0x01          //c1 output enalbe

//use GPIO_D4 as SCL     GPIO_D4
#define     SCL_PORT_NUM            0x04  //0x05          //0x07           //b7 output enable
#endif

/* Ä£ÄâIIC ÑÓÊ±ºê*/
#define     IIC_DELAY_TIME         50

#endif /*_I2C_H */

