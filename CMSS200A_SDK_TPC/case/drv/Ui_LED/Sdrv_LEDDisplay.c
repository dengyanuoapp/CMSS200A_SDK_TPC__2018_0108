/*
 ********************************************************************************
 *                       ACTOS
 *                  ui dirver show picture
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : Sdrv_LEDDisplay.c
 * By     : fiona
 * Version: 1> v1.00     first version     10/17/2010 10:46PM
 ********************************************************************************
 */
#include "drv_led.h"
#pragma name(SDRV_DISPLAY)
#if 0
const uint8 iconshowval[SEGNUM] =
{ 0x08, 0x04, 0x10, 0x01, 0x02, 0x20, 0x40 };
#else
const uint8 iconshowval[SEGNUM] =
{ 0x01, 0x08, 0x04, 0x40, 0x020, 0x02, 0x10 };
#endif
//{ 0x08, 0x10, 0x40, 0x02, 0x20, 0x01, 0x04 };

const uint8 LED_com_add[NUMBERADDR] =
{ 0x03, 0x04, 0x01, 0x0};
#if 1
const uint8 numbershowval[NUMBER] =
{
    0x7B, //number0
    0x28, //number1
    0x67, //number2
    0x6d, //number3
    0x3c, //number4
    0x5d, //number5
    0x5f, //number6
    0x68, //number7
    0x7f, //number8
    0x7d, //number9
    0x7e, //'A'
    0x7f, //'b'
    0x53, //'C'
    0x7b, //'d'
    0x57, //'E'
    0x56, //'F'
    0x7d, //'g'
    0x3e, //'H'
    0x28, //'I'
    0x2a, //'J'
    0x0,//k,nodisplay
    0x13, //'L'
    0x0, //m,nodisplay
    0x7a, //'n'	big 0x0e   littal 0x3a
    0x7b, //'O'
    0x36, //'P'
    0x3c,//'q'
    0x7e, //'r'
    0x5d, //'s'
    0x0, //t,nodisplay
    0x3b, //'u'
    0x0,//v,nodisplay
    0x0,//w,nodisplay
    0x0,//x,nodisplay
    0x3c, //'y'
};
#else
const uint8 numbershowval[NUMBER] =
{
    0x3f, //number0
    0x06, //number1
    0x5b, //number2
    0x4f, //number3
    0x66, //number4
    0x6d, //number5
    0x7d, //number6
    0x07, //number7
    0x7f, //number8
    0x6f, //number9
    0x77, //'A'
    0x7f, //'b'
    0x39, //'C'
    0x3f, //'d'
    0x79, //'E'
    0x71, //'F'
    0x6f, //'g'
    0x76, //'H'
    0x06, //'I'
    0x0E, //'J'
    0x0,//k,nodisplay
    0x38, //'L'
    0x0, //m,nodisplay
    0x37, //'n'	big 0x37   littal 0x54
    0x3f, //'O'
    0x73, //'P'
    0x6f,//'q'
    0x77, //'r'
    0x6d, //'s'
    0x0, //t,nodisplay
    0x3e, //'u'
    0x0,//v,nodisplay
    0x0,//w,nodisplay
    0x0,//x,nodisplay
    0x66, //'y'

};

#endif
////*******************************************************************************/
///*
//** FUNCTION:     Sdrv_LEDDisplay
//**
//** Description:  在四个8上输出一定范围内的字符，具体可以显示的字符见display.h
//**
//**  input
//**     addr: segment address
//**     str: show content
//**     type: signature it is show or hide
//**
//**  output
//**     none如果输出的字符不在可输出范围内，则会将当前的icom灭掉
//********************************************************************************/

void Sdrv_LEDPuts(uint8 api_no, uint8 addr, uint8 *str, uint8 type)
{
    uint8 i = 0, tmp_val;
    uint8 displayval;
    api_no = api_no;
    if (type == 0)
    {
        for (i = addr; i < 4; i++)
        {
            Hdrv_DataSend(LED_com_add[i], 0, 0);
        }
    }
    else
    {
        for (i = addr; i < 4; i++)
        {
            tmp_val = *str++;
            if (tmp_val >= 0 && tmp_val <= 9)//从0到9
            {
            }
            else if (tmp_val >= 0x30 && tmp_val <= 0x39)//从'0'到'9'
            {
                tmp_val = tmp_val - 0x30;
            }
            else if (tmp_val >= 'A' && tmp_val < 'Z')//从"A"到'U'
            {
                tmp_val = tmp_val - 'A' + 10;
            }
            else if (tmp_val >= 'a' && tmp_val < 'z')//从'a'到'u'
            {
                tmp_val = tmp_val - 'a' + 10;
            }
            else
            {
                Hdrv_DataSend(LED_com_add[i], 0, 0);
                continue;
            }
            displayval = numbershowval[tmp_val];
            Hdrv_DataSend(LED_com_add[i], displayval, type);
        }
    }

}

////*******************************************************************************/
///*
//** FUNCTION:     Sdrv_LEDDisplay
//**
//** Description:  根据输入，显示or隐藏段码屏上相应的segment
//**
//**  input
//**     addr: segment address
//**     content: show content
//**     type: signature it is show or hide
//**
//**  output
//**     none
//********************************************************************************/
void Sdrv_LEDDisplay(uint8 api_no, uint8 addr, uint8 content, uint8 type)
{
    uint8 displayval;
    uint8 displayaddr;
    //judge it is icon or number
    api_no = api_no;
    if (content == 0xff)
    {
        //       uint8 real_addr;
        //      real_addr = addr;//icon_set[addr];
        //get icon display address
        displayaddr = 0x02;// iconaddr[real_addr];
        //get icon display value
        if (type != 0)
        {
            displayval = iconshowval[addr];
        }
        else
        {
            displayval = 0xff - iconshowval[addr];//  iconhideval[real_addr];
        }
    }
    else
    {
        //get number display address
        displayaddr = LED_com_add[addr];
        //get number display value
        if (type != 0)
        {
            if (content > (NUMBER - 1))
            {
                content = NUMBER - 1;
            }
            displayval = numbershowval[content];
        }
        else
        {
            displayval = 0x0;// numberhideval[addr];
        }
    }
    Hdrv_DataSend(displayaddr, displayval, type);
}
