/*
 ********************************************************************************
 *                       ACTOS
 *                  UI Init function
 *
 *                (c) Copyright 2002-2008, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : Hdrv_HardDeal.c
 * By     : fiona
 * Version: 1> v1.00     first version     2010/10/12  14:00
 * Description: support unicode font
 ********************************************************************************
 */
#include "drv_led.h"

#pragma name(HARD_DEAL)
////********************************************************/
///*    void Hdrv_DataSend(uint8 addr,uint8 val)
//** FUNCTION:     Hdrv_DataSend
//**
//** Description:  send data to lcd
//**
//**  input:
//**     addr :	data register addr
//**     val  : which will be send to lcd
//**
//**  output
//**     none
//**********************************************************/
void Hdrv_DataSend(uint8 addr, uint8 val, uint8 distype)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_LCD;
    //send data to led data register
    switch (addr)
    {
    case 0:
        if (distype != 0)
        {
            //LCD_DATA0 |= val;
            LCD_DATA0 = val;
        }
        else
        {
            LCD_DATA0 &= val;
        }
        break;
    case 1:
        if (distype != 0)
        {
            //LCD_DATA1 |= val;
            LCD_DATA1 = val;
        }
        else
        {
            LCD_DATA1 &= val;
        }
        break;
    case 2:
        if (distype != 0)
        {
            //LCD_DATA2 |= val;
            LCD_DATA2 |= val;
        }
        else
        {
            LCD_DATA2 &= val;
        }
        break;
    case 3:
        if (distype != 0)
        {
            //LCD_DATA3 |= val;
            LCD_DATA3 = val;
        }
        else
        {
            LCD_DATA3 &= val;
        }
        break;
    case 4:
        if (distype != 0)
        {
            LCD_DATA4 = val;
            //LCD_DATA4 = val;
        }
        else
        {
            LCD_DATA4 &= val;
        }
        break;
    default:
        break;
    }
    //wait for data trans end
    LCD_MODE |= 0x08;
    //while ((LCD_MODE & 0x08) != 0)
    while (1)
    {
        if ((LCD_MODE & 0x08) == 0)
        {
            break;
        }
    }
    SFR_BANK = sfr_bak;
}

