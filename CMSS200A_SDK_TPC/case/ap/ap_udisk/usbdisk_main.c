/********************************************************************************
 *                              AS211A
 *                            Module: USB MAIN
 *                 Copyright(c) 2001-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      wanghaijing 2010-3-10         1.0                build this file
 ********************************************************************************/
/*!
 * \file     usbdisk.c
 * \brief
 * \author   wanghaijing
 * \par      GENERAL DESCRIPTION:
 *               ������ļ���������
 * \par      EXTERNALIZED FUNCTIONS:
 *               �����������õ������ģ��
 *
 *      Copyright(c) 2001-2010 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2010-3-10
 *******************************************************************************/

#include "Udisk.h"

#pragma name(AP_MAIN)

uint8 DisplayIndi, UdiskStatus, IRUSBExit;
uint8 MenuKeyPress;
uint8 IsCardOut;
//uint8 ChargeCheckCounter;
//uint8 gRtcCounter;
uint8 HoldDispState;
uint8 reckeypress;
res_infor_t res_infor[5];
const uint8 usb_str[] =
{ "USB" };
int16 main(void)
{
    uint8 para = 0x01;
    uint8 dispIndibak = 0xff;
    DisplayIndi = 0x00;
    IRUSBExit = 0;
    /*AP��ʼ��*/
    if (!UDiskInitAll(para))
    {
        //��ǰ�����˳�
        UdiskStatus = 0x40;
        return UdiskExitAll();
    }
    while (1)
    {
        ClearWatchDog();
        /*deal message */
        MsgManager(DisplayIndi);
        if (IsCardOut == 1)
        {
            UDiskPullCardOut();
            IsCardOut = 0;
        }

        //����״̬��ִ������
        UDiskHandle();
        //��ȡUSB״̬
        //0:����  10h:�ϴ�  20h:�´�  30h:����Ҫ���˳�  40h:����Ҫ���˳�  50h:�л����ͼ�����Ӧ��
        //60h:��������״̬  70h:����Ҫ���˳�  80h:����Suspend״̬  90h:�˳�Suspend״̬
        UdiskStatus = UDiskGetStatus();
        if ((DisplayIndi == 0xff) && (UdiskStatus != 0x80))
        {
            //StandbyScreen(1);
        }
        if ((UdiskStatus == 0x30) || (UdiskStatus == 0x40) || (UdiskStatus == 0x50) || (UdiskStatus == 0x70))
        {
            break;
        }

        switch (UdiskStatus)
        {
        case 0: //����
            if (DisplayIndi != 0x00)
            {
                DisplayIndi = 0x00;
            }
#ifdef  BACKLIGHTCTL
            if(KeyMsg == 1)
            {
                Scrcounter = 0;
                KeyMsg = 0;
                LightOnOff(1);
            }
#endif
            break;

        case 0x10: //�ϴ�
            if (DisplayIndi != 0x01)
            {
                DisplayIndi = 0x01;
            }
            break;

        case 0x11://��ʾCommu
        case 0x21://��ʾDownLoad
        case 0x31://��ʾUpLoad
        case 0x41://��ʾSucc
        case 0x61://��ʾSucc���˳�
            DisplayIndi = UdiskStatus >> 4;
            if (UdiskStatus == 0x61)
            {
                DisplayIndi -= 1;
            }
            DisplayIndi += 0x10;
            if (dispIndibak != DisplayIndi)
            {
                dispIndibak = DisplayIndi;
            }
            break;

        case 0x51: //�л���adfu
            //�ؼ��̺�RTC�ж�
            IE0 = IE0 & 0xf7;
            DisplayIndi = 0x10;
            //��ֻ֤��ʾһ��
            if (dispIndibak != DisplayIndi)
            {
                dispIndibak = DisplayIndi;
            }
            break;

        case 0x20: //�´�
            if (DisplayIndi != 0x02)
            {
                DisplayIndi = 0x02;
                Display(DisplayIndi);
            }
            break;

        case 0x80: //����suspend
            DisplayIndi = 0xff;
            break;

        case 0x90: //�˳�Suspend״̬
            //StandbyScreen(1);
            //SetContrast(g_comval.DisplayContrast);
            break;

        default:
            break;
        }
        if (((MenuKeyPress == 1) && (UdiskStatus == 0)) || (UdiskStatus == 0x90) || (IRUSBExit == 1))// ��Ӧ����ң�������Power��
        {
            longkey_flag = 1;
            break;
        }
        else
        {
            MenuKeyPress = 0;
        }
    }/*end of while loop*/

#ifdef  BACKLIGHTCTL
    LightOnOff(1);
#endif

    return UdiskExitAll();
}
