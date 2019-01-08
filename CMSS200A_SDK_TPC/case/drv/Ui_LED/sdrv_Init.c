/*
 ********************************************************************************
 *                       ACTOS
 *                  UI Init function
 *
 *                (c) Copyright 2002-2008, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : Sdrv_Init.c
 * By     : fiona
 * Version: 1> v1.00     first version     2010/10/13  14:00
 * Description: support unicode font
 ********************************************************************************
 */

#include "drv_led.h"

#pragma name(BBC_INIT)

bool Sdrv_Init(uint8 api_no)
{
    api_no = api_no;
    //Hdrv_HardInit();
    //Sdrv_ClearScreen(0);
    return 1;
}

void Sdrv_Exit(uint8 api_no)
{
    api_no = api_no;
    return;
}
