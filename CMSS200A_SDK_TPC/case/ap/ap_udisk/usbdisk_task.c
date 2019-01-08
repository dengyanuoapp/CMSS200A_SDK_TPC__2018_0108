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
    if (i == Msg_KeyPower) //MODE������  UHOST��CARD��FM��USBAUDIO��UDISKUSB  ѭ���л�
    {
        // ��Ӧ����ң�������Power��
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
	else if (i == Msg_KeyMenu) //�յ�Menu��
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
            LightOnOff(TRUE);//���ر��⣨TRUE: ��, FALSE:��)
            g_light_flag = TRUE;
        }
    }
#endif

    else if ((i == MSG_RTC2HZ) && (dispIndi < 0x10)) //dispIndi>=0x10������״̬,���ܵ����漰����flashSD���Ĳ���.
    {
#ifdef  BACKLIGHTCTL
        g_rtc_counter++;
        if(g_rtc_counter >= 120) //�Զ��ر���

        {
            //�ر���
            g_rtc_counter = 130;
            LightOnOff(FALSE); //���ر��⣨true: ��, false:��)
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
    IE0 = IE0 & 0x7f; //��ʾʱ�����жϣ������Ƶ���жϷ����������ήƵ��������Ч��

    if (dispIndi < 0x10)
    {
        if (dispIndi <= 2)
        {
            LEDClearScreen();
            LEDPuts(NUMBER2, display_str[dispIndi], 1);
        }

#if 0
        if(dispIndi == 0x00) //��ʾ����ָʾ 0:����ʾ����  01:�ϴ�  02:�´�  03:�������

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

    IE0 = IE0Bak; //�ָ��ж�
#endif
}

void FlashLed(void)
{
}
