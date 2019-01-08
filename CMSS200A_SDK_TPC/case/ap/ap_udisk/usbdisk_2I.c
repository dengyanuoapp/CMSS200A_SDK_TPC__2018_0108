//#pragma function=non_banked

#include "Udisk.h"

//#pragma memory=dataseg(UDISK_RD)
#pragma userclass(near=UDISKRD)

ADFU_SysInfo_t *sysinfopt;
BYTE MenuKeyPress;
#ifdef ONLINEDEVICE_SWITCH
BYTE PlayKeyPress;
#endif
BYTE HoldDispState;
BYTE ChargeCheckCounter;
BYTE IsCardOut; //jpu
BOOL UDiskRdyFlag;
extern char DisplayIndi;
extern char *DispBuf;
extern ADFU_SysInfo_t sysinfo;
char UD_SDCardWPDetect();
extern char USBStatus;

//#pragma memory=default
#pragma userclass(near=default)

//#pragma memory=constseg(UDISK_RS)
//#pragma userclass(near=UDISK_RS)

WORD wUDiskDisp[3] =
{ UDISKRDY, UDISKUPL, UDISKDL };

//#pragma memory=default
#pragma userclass(near=default)

//#pragma codeseg(UDISK_RC)
#pragma renamecode(UDISK_RC)

//void MsgManager(void)
//{
//    char i;
//#ifdef ONLINEDEVICE_SWITCH
//    //已收到Menu键,不再收消息,以防非空闲状态下跑该函数把KeyUp收走    // quinn 070427 modify
//    if(MenuKeyPress==1 || PlayKeyPress==1)
//#else
//    if(MenuKeyPress==1)
//#endif
//            return;
//    //收到Menu键
//    if((i = GetSysMsg()) == Msg_KeyMenu)
//    {
//        if(USBStatus==0)
//          MenuKeyPress=1;
//        return;
//    }
//#ifdef ONLINEDEVICE_SWITCH
//    else if(i == Msg_KeyPlayPause)        // quinn 070427 add
//    {
//	    if(USBStatus == 0)
//	      PlayKeyPress = 1;
//	    return;
//    }
//#endif
//    else if(i == MSG_SD_OUT)
//    {
//        IsCardOut = 1;
//        return;
//    }
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
//}
//
//
//void Display(char dispIndi)
//{
//    char    disptemp = dispIndi & 0x0f;
//    char    i,j,k;
//    char    *card_wp_flag = (char *)0x9c; //card hold status
//    k = input8(0x27);
//    output8(0x27,0x0);          //显示时不开中断，避免低频跑中断服务（下面代码会降频），降低效率
//
//    i = input8(0x00);
//    output8(0x00,i | 0x81);     //显示屏不支持60MHz送屏，降频
//
//    for(j=0; j<0x40; j++){}
//    if(dispIndi<0x10)
//    {
//        if(dispIndi==0x00)               //显示任务指示 0:无显示动作  01:上传  02:下传  03:传输结束
//        {
//                ResShowPic(UDISKRDY, PIC_X_B_UDISKRDY, PIC_Y_B_UDISKRDY);
//                UDiskRdyFlag = TRUE;
//        }
//        else if(dispIndi==0x01)
//        {
//            if(UDiskRdyFlag)
//            {
//                ResShowPic(UDISKBCK, PIC_X_B_UDISKBCK, PIC_Y_B_UDISKBCK);
//                UDiskRdyFlag = FALSE;
//            }
//            ResShowPic(UDISKUPL, PIC_X_UD, PIC_Y_UD);
//        }
//        else if(dispIndi==0x02)
//        {
//            if(UDiskRdyFlag)
//            {
//                ResShowPic(UDISKBCK,PIC_X_B_UDISKBCK,PIC_Y_B_UDISKBCK);
//                UDiskRdyFlag = FALSE;
//            }
//            ResShowPic(UDISKDL, PIC_X_UD, PIC_Y_UD);
//        }
//#ifdef SupportCard    //card disk hold display
//       if((*card_wp_flag)|| HoldDispState==1)
//       {
//         ResShowPic(PIC_ID_UDLOCK,PIC_X_UDLOCK,PIC_Y_UDLOCK);
//       }
//#else
//        if( HoldDispState==1 )
//        {
//            ResShowPic(PIC_ID_UDLOCK,PIC_X_UDLOCK,PIC_Y_UDLOCK);
//        }
//#endif
//    }
//    else
//    {
//        switch (disptemp)
//        {
//            case 0:
//                ResShowPic(UDISKRDY,PIC_X_B_UDISKRDY,PIC_Y_B_UDISKRDY);
//                break;
//            case 1:
//                ResShowPic(UPG_BUSY,0,30);
//                break;
//            case 2:
//                ResShowPic(UDISKBCK,PIC_X_B_UDISKBCK,PIC_Y_B_UDISKBCK);
//                ResShowPic(UDISKDL, PIC_X_UD, PIC_Y_UD);
//                break;
//            case 3:
//                ResShowPic(UDISKBCK,PIC_X_B_UDISKBCK,PIC_Y_B_UDISKBCK);
//                ResShowPic(UDISKUPL, PIC_X_UD, PIC_Y_UD);
//                break;
//            case 4:
//                break;
//            case 5:
//                break;
//            case 6:
//                break;
//            default:
//                break;
//        }
//    }
//
//displayend:
//    output8(0x00,i);        //升回频率
//    for(j=0;j<0x40;j++){}
//    output8(0x27,k);      //恢复中断
//}


bool UDiskInitAll(int para)
{
    int i;
    //    //安装ud驱动
    //    res_fp = ResOpen("ui30.res");
    //    ResShowPic(UDISKRDY,0,0);                  //显示空闲画面
    //    if(DRV_DetectUD(0x00) == 0xff)
    //    {
    //        return 0;
    //    }
    //    else
    //    {
    //        DRV_ActiveUD(0x00,MODE_FAST);       //快速模式ud驱动
    //    }
    //#ifndef NEWFORMATRES
    //    //FILE_IM//VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));     //读VRAM
    //#endif
    //    if(g_comval.magic != MAGIC_COMVAL)      //取联机模式设定
    //    {
    //           GetDefaultComval(&g_comval);
    //    }
    //    //==================================================================================
    //    //**********************以下函数调用次序不能改动***************************************
    //    //==================================================================================
    //     //DISPLAY_IM//DispBuf=(char *)DISPLAY_BUFFER_ADDR;
    //    UDiskSetDiskNum(g_comval.OnlineMode);   //设置联机模式
    //#ifdef SupportCard
    //    AlwaysShowCardDisk(1);                  //设置是否显示多盘符
    //#endif
    //    UDiskInit((void *)FlashLed);            //U盘初始终化,输入参数:callback函数，属性...

    //    /*检测hold时去抖*/
    //    if(sKY_HoldCheck())                     //HOLD表示需要写保护
    //    {
    //        int i,j;
    //        for(i=0;i<20;i++)
    //        {
    //            if(sKY_HoldCheck() == 0)
    //            {
    //               break;
    //            }
    //            /*delay*/
    //            for(j=0;j++;j<1000)
    //            {
    //                output8(0x4e,input8(0x4e)|0x08);
    //            }
    //        }
    //        if(i==20)
    //        {
    //            UDiskSetWP('C');
    //            HoldDispState=1;
    //            ResShowPic(PIC_ID_UDLOCK,PIC_X_UDLOCK,PIC_Y_UDLOCK);
    //        }
    //    }
    //    sysinfopt=&sysinfo;
    //    GetSysInfo(sysinfopt);          //取得ADFU UPD的信息头
    UDiskRun(para); //启动USB引擎
    return 1;
}
void FlashLed(void)
{
}

//#pragma memory=default
#pragma userclass(near=default)
