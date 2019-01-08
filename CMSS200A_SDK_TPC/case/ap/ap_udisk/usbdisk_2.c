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

#pragma name(UDISK_RC_2)

//extern char DisplayIndi;
//extern char *DispBuf;
//extern ADFU_SysInfo_t sysinfo;
extern uint8 UdiskStatus;
extern uint8 MenuKeyPress;
extern uint8 IsCardOut;
extern bool UDiskRdyFlag;

//ADFU_SysInfo_t *sysinfopt;
//BYTE HoldDispState;
//BYTE ChargeCheckCounter;
//WORD wUDiskDisp[3]={UDISKRDY,UDISKUPL,UDISKDL};

#pragma renamecode(UDISK_RC_2)

void MsgManager(void)
{
    char i;
    if (MenuKeyPress == 1)
    {
        return;
    }
    if ((i = GetSysMsg()) == Msg_KeyEQ)
    {
        if (UdiskStatus == 0)
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
    //    else if(i == MSG_RTC2HZ)
    //    {
    //         if(ChargeCheckCounter >= 60)
    //         {
    //             ChargeCheckCounter = 0;
    //             sKY_ChargeGetandSet();
    //             return;
    //         }
    //         ChargeCheckCounter ++;
    //    }
}

void Display(char dispIndi)
{
    
}

bool UDiskInitAll(int para)
{
    //	int i;

    //安装ud驱动
    res_fp = ResOpen("ui30.res");
    ClearScreen(NULL); //清成白屏
    SetPenColor(Color_WHITE_def);
    //UpdateScreen(NULL);
    ResShowPic(UDISKRDY, 0, 0); //显示空闲画面

    if (DRV_DetectUD(0x00) == 0xff)
    {
        return 0;
    }
    else
    {
        DRV_ActiveUD(0x00, MODE_FAST); //快速模式ud驱动
    }

    UDiskSetDiskNum(0);

    //U盘初始终化,输入参数:callback函数，属性...
    UDiskInit(0, 3);
    UDiskRun(para); //启动USB引擎
    return 1;

    //	int i;
    //
    //	//安装ud驱动
    //	res_fp = ResOpen("ui30.res");
    //	ClearScreen(NULL); //清成白屏
    //	UpdateScreen(NULL);
    //	ResShowPic(UDISKRDY, 0, 0); //显示空闲画面
    //
    //	if (DRV_DetectUD(0x00) == 0xff)
    //		return 0;
    //	else
    //		DRV_ActiveUD(0x00, MODE_FAST); //快速模式ud驱动
    //
    //	//read comval
    //	VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval)); //读VRAM
    //
    //	//check if valid
    //	if (g_comval.magic != MAGIC_COMVAL) //取联机模式设定
    //		COMVAL_INIT(g_comval);
    //
    //	if (g_comval.LightTime != 0)
    //		sKY_OpenBacklight(); //开背光
    //
    //	//==================================================================================
    //	//**********************以下函数调用次序不能改动***************************************
    //	//==================================================================================
    //	UDiskSetDiskNum(g_comval.OnlineMode);       //设置联机模式
    //	//UDiskSetDiskNum(3);                         //设置单卡盘联机模式
    //
    //	AlwaysShowCardDisk(g_comval.SuppCard);      //设置是否显示多盘符  //reagan modify 2009-1-6 13:39
    //
    //    //U盘初始化,输入参数:callback函数,是否支持Udisk序列号(随机数,每个Udisk均不一致),是否是Udisk认证模式
    //	UDiskInit((void *) FlashLed, g_comval.udisk_setting);
    //
    //	if (sKY_HoldCheck()) //HOLD表示需要写保护
    //	{
    //		UDiskSetWP('C');
    //		HoldDispState = 1;
    //		ResShowPic(UDLOCK_S, 115, 16);
    //	}
    //	/*    #ifdef SupportCard
    //	 if (CardWPCheck())            //sCardWPCheck返回值：1:有卡并且卡写保护  0:没卡或卡没有写保护
    //	 { //有卡
    //	 UDiskSetWP('H');
    //	 HoldDispState=1;
    //	 ResShowPic(UDLOCK_S,115,16);
    //	 }
    //	 #endif */
    //
    //	UDiskRun(para); //启动USB引擎
    //
    //	return 1;
}

//void FlashLed(void)
///{
//   memset(&sysinfo,0,sizeof(ADFU_SysInfo_t));
//}
