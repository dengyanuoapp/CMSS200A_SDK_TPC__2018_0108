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

#pragma name(UDISK_RC)
uint8 DisplayIndi, UdiskStatus;
uint8 MenuKeyPress;
uint8 IsCardOut;
//BYTE OldVddVol;

BOOL UDiskRdyFlag;

//extern BYTE ChargeCheckCounter;
//extern BYTE HoldDispState;

#pragma renamecode(UDISK_RC)

int main(void)
{
    uint8 para = 0x01;

    uint8 dispIndibak = 0xff;
    uint8 rii = 0;
    if (USBH_Check_Udisk())
        return 0;
    DisplayIndi = 0x00;
    //    HoldDispState = 0;
    //    ChargeCheckCounter = 0;


    //    /*低电模式下进行充电*/
    //    if(GetBattery() < 0x13)
    //    {
    //       sKY_ChargeSet(1);
    //       sKY_ChargeGetandSet();
    //    }
    /*AP初始化*/

    if (!UDiskInitAll(para))
    {
        return 0;
    }
    while (1)
    {
        //        /*clear watchdog*/
        //        CLEAR_WD();
        /*deal message */
        MsgManager();
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
            //           StandbyScreen(1);
            //           SetContrast(g_comval.DisplayContrast);
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
                Display(DisplayIndi);
            }
            break;

        case 0x10: //上传
            if (DisplayIndi != 0x01)
            {
                DisplayIndi = 0x01;
                Display(DisplayIndi);
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
                Display(DisplayIndi);
            }
            break;

        case 0x51: //切换到adfu
            //关键盘和RTC中断
            //output8(0x27, input8(0x27) & 0x9f);
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
            }
            break;

        case 0x80: //进入suspend
            DisplayIndi = 0xff;
            break;

        case 0x90: //退出Suspend状态
            //                StandbyScreen(1);
            //                SetContrast(g_comval.DisplayContrast);
            break;

        default:
            break;
        }

        if ((MenuKeyPress == 1) && (UdiskStatus == 0))
        {
            break;
        }
        else
        {
            MenuKeyPress = 0;
        }
    }/*end of while loop*/

    UDiskExit();
    ResClose(res_fp);
    /*拔线退出usb模式由于VCC有100mv波动，需重新初始化触摸按键的状态*/
    //#ifdef  Use_RotateKey
    ////    Interrupt27Reg=input8(0x27);
    ////    output8(0x27, 0x00);
    //    if(!DRV_UnInstall(DRV_KY))
    //    {
    //        while(1);
    //    }
    //    if(!DRV_Install("ky_hard.drv",0))
    //    {
    //        while(1);
    //	}
    ////    output8(0x27, Interrupt27Reg);
    //#endif
    DRV_ActiveUD(0x00, MODE_SLOW);

    //按Mode退出
    if (UdiskStatus == 0x00)
    {
        return RESULT_MAIN;
    }
    //升级退出
    else if (UdiskStatus == 0x50)
    {
        return RESULT_UPGRADE;
    }
    else
    {
        //拔线,弹出
        return 0;
    }
    return 0;
}
