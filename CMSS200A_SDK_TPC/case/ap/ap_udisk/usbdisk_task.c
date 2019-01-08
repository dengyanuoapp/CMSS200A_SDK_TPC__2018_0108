/*
 ********************************************************************************
 *                       ACTOS
 *                      usbfunctions
 *
 *                (c) Copyright 2002-2009, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : usbdisk_2.c
 * By     : Mars
 * Version: v1.00     first version     2009-11-10 14:05  by Mars
 ********************************************************************************
 */
#include <reg251s.h>
#include "Udisk.h"

#pragma name(AP_TASK)

extern uint8 UdiskStatus;
extern uint8 MenuKeyPress;
extern uint8 IsCardOut;
//extern uint8 ChargeCheckCounter;
//extern uint8 gRtcCounter;
extern uint8 IRUSBExit;
extern uint8 reckeypress;
#pragma renamecode(?PR?AP_TASK)
const uint8 display_str[3][4] =
{ "RDY", "UPZ", "DON" };//ready,upload,download


void MsgManager(uint8 dispIndi)
{
    uint8 i;

    if (MenuKeyPress == 1)
    {
        return;
    }

    i = GetSysMsg();
    if (i == Msg_KeyPower) //MODE键功能  UHOST、CARD、FM、USBAUDIO、UDISKUSB  循环切换
    {
        // 对应红外遥控上面的Power键
        IRUSBExit = 1;
        return;
    }
#if 0
	else if (i == Msg_KeyVolAdd)
    {
		MenuKeyPress = 1;
        return;
    }
#endif
	else if (i == Msg_KeyMenu) //收到Menu键
    {
#ifdef  BACKLIGHTCTL
        if(UdiskStatus == 0)
#else
        if (UdiskStatus == 0)
#endif
        {
            MenuKeyPress = 1;
        }
        return;
    }
    else if (i == MSG_SD_OUT)
    {
        IsCardOut = 1;
        return;
    }
    else if (i == Msg_KeyNext)
    {
        if (UdiskStatus == 0)
        {
            MenuKeyPress = 1;
            reckeypress = 1;
        }
        return;
    }
    else if (i == Msg_KeyUSBTest)
    {
        if (UdiskStatus == 0)
        {
            MenuKeyPress = 1;
            reckeypress = 2;
        }
        return;
    }
#ifdef  BACKLIGHTCTL
    else if((i == Msg_KeyLongUp) || (i == Msg_KeyShortUp))
    {
        g_rtc_counter = 0;
        if (!g_light_flag)
        {
            LightOnOff(TRUE);//开关背光（TRUE: 开, FALSE:关)
            g_light_flag = TRUE;
        }
    }
#endif

    else if ((i == MSG_RTC2HZ) && (dispIndi < 0x10)) //dispIndi>=0x10，升级状态,不能调用涉及到读flashSD区的操做.
    {
#ifdef  BACKLIGHTCTL
        g_rtc_counter++;
        if(g_rtc_counter >= 120) //自动关背光

        {
            //关背光
            g_rtc_counter = 130;
            LightOnOff(FALSE); //开关背光（true: 开, false:关)
            g_light_flag = FALSE;
        }
#endif

    }
}

void Display(uint8 dispIndi)
{

#if 0
    uint8 disptemp = dispIndi & 0x0f;
    uint8 IE0Bak;
    dispIndi = dispIndi;
    IE0Bak = IE0;
    IE0 = IE0 & 0x7f; //显示时不开中断，避免低频跑中断服务（下面代码会降频），降低效率

    if (dispIndi < 0x10)
    {
        if (dispIndi <= 2)
        {
            LEDClearScreen();
            LEDPuts(NUMBER2, display_str[dispIndi], 1);
        }

#if 0
        if(dispIndi == 0x00) //显示任务指示 0:无显示动作  01:上传  02:下传  03:传输结束

        {
            ResShowPic(UDISKRDY, 0, 16);
        }
        if(dispIndi == 0x01)
        {
            ResShowPic(UDISKUPL, 0, 16);
        }
        if(dispIndi == 0x02)
        {
            ResShowPic(UDISKDL, 0, 16);
        }
#endif
    }
    else
    {
    }

    IE0 = IE0Bak; //恢复中断
#endif
}

void FlashLed(void)
{
}
