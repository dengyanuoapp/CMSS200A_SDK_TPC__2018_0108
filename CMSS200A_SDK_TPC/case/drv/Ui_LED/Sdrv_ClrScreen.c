/*
 ********************************************************************************
 *                       ACTOS
 *                  ui dirver show picture
 *
 *                (c) Copyright 2002-2008, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : Sdrv_ClrScreen.c
 * By     : fiona
 * Version: 1> v1.00     first version     2010/10/13  14:40
 * Description: support unicode font
 ********************************************************************************
 */
#include "drv_led.h"

#pragma name(SDRV_CLRS)
//const uint8 LEDAddr[ADDRNUM] =
//{ 0x00, 0x01, 0x02, 0x03, 0x04 };
////*******************************************************************************/
///*
//** FUNCTION:     ClearScreen
//**
//** Description:  ÃðµôËùÓÐsegment
//**
//**  input
//**     none
//**
//**  output
//**     none
//********************************************************************************/

void Sdrv_ClearScreen(uint8 api_no)
{
    //    uint8 sendaddr;
    uint8 i = 0;
    //every segment send zero
    api_no = api_no;
    while (i < ADDRNUM)
    {
        //     sendaddr = LEDAddr[i];
        Hdrv_DataSend(i, 0, 0);
        i++;
    }
}

