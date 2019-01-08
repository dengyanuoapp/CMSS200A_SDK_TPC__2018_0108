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
 *               这里对文件进行描述
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
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
    /*AP初始化*/
    if (!UDiskInitAll(para))
    {
        //提前拔线退出
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

        //调用状态机执行命令
        UDiskHandle();
        //获取USB状态
        //0:空闲  10h:上传  20h:下传  30h:弹出要求退出  40h:拔线要求退出  50h:切换到韧件升级应用
        //60h:分区处理状态  70h:出错要求退出  80h:进入Suspend状态  90h:退出Suspend状态
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
        case 0: //空闲
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

        case 0x10: //上传
            if (DisplayIndi != 0x01)
            {
                DisplayIndi = 0x01;
            }
            break;

        case 0x11://显示Commu
        case 0x21://显示DownLoad
        case 0x31://显示UpLoad
        case 0x41://显示Succ
        case 0x61://显示Succ，退出
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

        case 0x51: //切换到adfu
            //关键盘和RTC中断
            IE0 = IE0 & 0xf7;
            DisplayIndi = 0x10;
            //保证只显示一次
            if (dispIndibak != DisplayIndi)
            {
                dispIndibak = DisplayIndi;
            }
            break;

        case 0x20: //下传
            if (DisplayIndi != 0x02)
            {
                DisplayIndi = 0x02;
                Display(DisplayIndi);
            }
            break;

        case 0x80: //进入suspend
            DisplayIndi = 0xff;
            break;

        case 0x90: //退出Suspend状态
            //StandbyScreen(1);
            //SetContrast(g_comval.DisplayContrast);
            break;

        default:
            break;
        }
        if (((MenuKeyPress == 1) && (UdiskStatus == 0)) || (UdiskStatus == 0x90) || (IRUSBExit == 1))// 对应红外遥控上面的Power键
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
