/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : Udisk20IRQMain.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-13 12:45
 *********************************************************************************************************
 */

#include "udisklib.h"

STG_INFO edata StorageInfor _at_ 0x00E0;
#pragma name(RCODE)

#ifndef	UDISK_AP
#pragma asm
EXTRN EDATA(SP_Keeper)
#pragma endasm
#endif

#pragma ROM(HUGE)
void Reset_Recover(void);
void USB_Bus_Reset_ISR(void);
void VBus_Change_Handle(void);
void Suspend_ISR(void);
void Hs_mode_ISR(void);
void Other_ISR(void);
#pragma ROM(HUGE)

void ADFUHandle(void);
void sFWUpgOkDelay(uint8 fwup_direction);
void sDelete_FlashVram(uint8 sectorNum, uint32 lbaAddr);
void sState_0_Handle(void);
void sState_10_Handle(void);
void sState_20_Handle(void);
void sGoInState0(void);
void sSplitDiskEject(void);
void UdiskReady(void);
void UdiskUnReady(void);
static uint8 CheckTimer(uint8 *timerrecord, uint8 *countrecord, uint8 count);

#pragma renamecode(?PR?RCODE)
#pragma ROM(HUGE)

//此定义为避免屏蔽掉打印后，udisk应用编译常量段定义未使用导致的编译错误。
#ifndef _PRINT_DEBUG_
const uint8 ConstSeg3 = 'A';
#endif
/*
 *********************************************************************************************************
 *                                           U20_ISRMain
 *
 * Description: USB2.0 Interrupt Server Entry.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */

void U20_ISRMain(void) //interrupt 8
{
#ifndef	UDISK_AP
#pragma asm
    MOV SP_Keeper, DR60
#pragma endasm
#endif

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;

    ClearWatchdog();

    USBEIRQ = USBEIRQ | (uint8) 0x80;

    if ((SHORTPCKIRQ & 0x20) != 0x00)
    {
        Bulk_Out_Handle();
    }
    else if ((Usbirq_hcusbirq & 0x01) != 0x00)
    {
        Setup_Packet_Handle();
    }
    else if ((Usbirq_hcusbirq & 0x10) != 0x00)
    {
        USB_Bus_Reset_ISR();
    }

    else if ((Usbirq_hcusbirq & 0x80) != 0x00)
    {
        VBus_Change_Handle();
    }

    else if ((Usbirq_hcusbirq & 0x08) != 0x00)
    {
        Suspend_ISR();
    }

    else if ((Usbirq_hcusbirq & 0x20) != 0x00)
    {
        Hs_mode_ISR();
    }
    else
    {
        Other_ISR();
    }

    Reset_Recover();
}

void Reset_Recover(void)
{
    SFR_BANK = sfr_bak;
#ifndef	UDISK_AP
#pragma asm
    MOV DR60, SP_Keeper
#pragma endasm
#endif
}

/*
 *********************************************************************************************************
 *                                           IfIsConnect
 *
 * Description: 判断是否联线或传输Timeout, 如果有异常, 就直接退出中断服务程序.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : 数据传输失败时,PC会发送Bus Reset到小机;或者读写过程中拔线.
 *********************************************************************************************************
 */
void ifIsConnect(void)
{
    uint8 usbrstflag, disconnectflag;
    uint8 sfrbak;
    ClearWatchdog();

    sfrbak = SFR_BANK;
    usbrstflag = 0x00;
    disconnectflag = 0x00;
    SFR_BANK = BANK_USB;
    if ((Usbirq_hcusbirq & 0x10) != 0x00)
    {
        usbrstflag = 0x01;
    }
#ifndef _FPGA_VERTION_
    if ((DPDMCTRL & 0x40) == 0x00)
    {
        disconnectflag = 0x01;
    }
#endif

    //USB disconnect or usb reset
    if ((usbrstflag == 0x01) || (disconnectflag == 0x01))
    {
#ifdef _PRINT_DEBUG_
        prints("timeout\n");
#endif

        if (fifo_trans_mode == 0x01)
        {
            SFR_BANK = BANK_DMA_CARD;
        }
        else
        {
            SFR_BANK = BANK_DMA0;
        }
        //将DMA停止
        DMAnCTL0 = DMA0CTL0 & (uint8) 0xfe;
        //FIFO直通模式结束后关闭直通配置，避免DMA的配置处于fifo直通模式,导致后续存储介质处理异常
        DMAnCTL1 = 0x00;
        fifo_trans_mode = 0x00;

        SFR_BANK = BANK_USB;
		UDMAM = 0x0c;
        //清FIFO和EPA中断为下一个CBW准备
        EPB_OutRdy_ShortPacketNAK();
    }

    if (usbrstflag == 0x01)
    {
        USB_Bus_Reset_ISR();
        Reset_Recover();
    }
    else if (disconnectflag == 0x01)
    {
        VBus_Change_Handle();
        Reset_Recover();
    }
    else
    {
    }
    SFR_BANK = sfrbak;
}

/*
 *********************************************************************************************************
 *                                           USB_Bus_Reset_ISR
 *
 * Description: Root Port Reset Interrupt handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void USB_Bus_Reset_ISR(void)
{
    Usbirq_hcusbirq |= 0x10;

    //正常Udisk枚举只会发送两次Reset,之后的Reset均为错误后PC发送的
    usb_reset_enter_cnt++;

    //发生bus reset后对部分变量进行重置，解决传输中发生bus reset后退出u盘问题。
    Suspend_Flag = 0x00;

    ConfigValue = 0;
    AlternateSetting = 0;
    DeviceStatus = 0;
    InterfaceStatus = 0;
    EndPoint1Status = 0;
    EndPoint2Status = 0;
}

/*
 *********************************************************************************************************
 *                                           VBus_Change_Handle
 *
 * Description: Connect/Disconnect Interrupt.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void VBus_Change_Handle(void)
{
    smDelay(3);

    PullLineFlag = 0x00;

#ifdef _PRINT_DEBUG_
    prints("L1\n");
#endif

    if ((DPDMCTRL & 0x40) == 0x00)
    {
#ifdef _PRINT_DEBUG_
        prints("L2\n");
#endif

        PullLineFlag = 0x01;
    }

    //clear VBus interrupt
    Usbirq_hcusbirq |= (uint8) 0x80;
}

/*
 *********************************************************************************************************
 *                                           Suspend_ISR
 *
 * Description: Suspend Interrupt handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Suspend_ISR(void)
{
    uint8 tmp;
    Usbirq_hcusbirq |= 0x08;

    //bit[7]:1 need process suspend,0 don't process suspend; bit[1:0]:0 no suspend; 1 recieve suspend; 2 has handle suspend
    if ((Suspend_Flag & 0x80) == 0x00)
    {
    }
    else if (detach_cmd_rcv != 0x00)
    {
        FWOpStatus = 1;
        IsAdfuCondition = 1;
    }
    else
    {
#ifdef _PRINT_DEBUG_
        prints("S1\n");
#endif
        //USB plug-in标志反映真实情况需要最长3ms延迟时间
        smDelay(3);

        //if plug off usb line,don't send suspend message to AP
        if ((DPDMCTRL & 0x40) != 0x00)
        {
            //ESD测试产生suspend，延时1s左右,如PC发送bus reset,则不处理suspend
            tmp = 4;
            while ((tmp--) != 0x00)
            {
                if ((Usbirq_hcusbirq & 0x10) != 0x00)
                {
                    break;
                }
                TM_DelayMS(250);
                ClearWatchdog();
            }

            if ((Usbirq_hcusbirq & 0x10) == 0x00)
            {
                //真正的suspend状态,硬件恢复默认状态
                usb_pwr_suspnd_fnsh = 0;
                //清reset标记,不让设备轻易进入Full Speed模式
                usb_reset_enter_cnt = 0;
                //0:no suspend; 1:recieve suspend; 2:has handle suspend
                Suspend_Flag++;

                //enable high speed
                BKDOOR = (BKDOOR & 0x7f) | 0x10;
            }
            else
            {
#ifdef _PRINT_DEBUG_
                prints("S2\n");
#endif
            }
        }
        //如果发生写操作,把最后一次传输数据写入Flash
        UD_Update();
    }
}

/*
 *********************************************************************************************************
 *                                           Hs_mode_ISR
 *
 * Description: Enter high speed mode interrupt occur.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : 设备在收到Bus reset,并完成Chirp J/K的发送后,会产生High Speed中断.
 *********************************************************************************************************
 */
void Hs_mode_ISR(void)
{
    Usbirq_hcusbirq |= 0x20;

    //the first connect to PC,usb reset has more than twice,change to full speed
    //after setting config,don't change to full speed
    if (usb_pwr_suspnd_fnsh == 0x00)
    {
        if (usb_reset_enter_cnt >= 2)
        {
            //disable high speed,bit4设计错误,此位应写1,但每次读出均是0
#ifdef _PRINT_DEBUG_
            prints("set fs \n");
#endif
            BKDOOR = BKDOOR | (uint8) 0x90;
        }
    }
}

/*
 *********************************************************************************************************
 *                                           Other_ISR
 *
 * Description: Other ISR handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Other_ISR(void)
{
    Usbirq_hcusbirq = Usbirq_hcusbirq;
}
#pragma ROM(HUGE)

/*
 ********************************************************************************
 *                           OTG_PHY_Setting
 * Description : USB PHY设置
 * Arguments   : d: address; e: value.
 * Returns     : none
 * Notes       : FPGA version, Just for GL6005 PHY setting.
 ********************************************************************************
 */
void OTG_PHY_Setting(unsigned char addr, unsigned char value)
{
    unsigned char tmp;

    VDSTAT = value; //set Value

    tmp = addr;
    VDCTRL = tmp & 0x0f; //set low addr
    VDCTRL = tmp | 0x10;

    tmp = (addr >> 4);
    VDCTRL = tmp & 0x0f; //set high addr
    VDCTRL = tmp | 0x10;
}

/*
 *********************************************************************************************************
 *                                           UDiskHandle
 *
 * Description: 查询、更新UDisk状态，并根据状态作相应处理.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UDiskHandle(void)
{
    uint8 IE0bak, sfrbak, timecount;

    IE0bak = IE0;
    IE0 = IE0 & 0x7f;
    StatusCheckFlag = 0x01;
    IE0 = IE0bak;

    adfu_flag = USB_State;

    if (IsAdfuCondition == 0x00)
    {
        //set config 之后设置usb_reset_enter_cnt为0x00,不再进行充电器判断
        if (usb_pwr_suspnd_fnsh == 0x00)
        {
            //Suspend_Flag 0:no suspend; 81:recieve suspend; 82:has handle suspend
            if ((usb_reset_enter_cnt == 0x00) && (Suspend_Flag == 0x00))
            {
                //查定时,connect_idle_cnt:计数器指针  prv_connect_idle_tm:1Hz变化记录变量  c-1:计数值,单位为1秒

                timecount = 15;
                if (SplitDiskFlag != 0x00)
                {
                    timecount = 0x05;
                }
                if (CheckTimer(&connect_idle_cnt, &prv_connect_idle_tm, timecount) != 0x00)
                {
                    //插入的是充电器,长时间没有bus reset
#ifdef _PRINT_DEBUG_
                    prints("IsCharge?-");
#endif
                    USB_State = 0x70;
                    return;
                }
            }
        }
    }
    else
    {
        //ADFU status
        ADFUHandle();
        return;
    }

    //拔线标志
    if (PullLineFlag != 0x00)
    {
        //进入退出状态
        USB_State = 0x40;
        TransferFlag = 0x40;
        return;
    }
    else if ((Suspend_Flag & 0x7f) == 0x01)
    {
        //Suspend_Flag 0:no suspend; 81:recieve suspend; 82:has handle suspend
        Suspend_Flag++;
        USB_State = 0x80;
        return;
    }
    else if (SwitchToADFUFlag != 0x00)
    {
        SwitchToADFUFlag = 0x00;
        //切换到ADFU
        USB_State = 0x51;
        TransferFlag = 0x51;
        return;
    }
    //PC弹出标志
    else if (PcEjectFlag != 0x00)
    {
        //进入退出状态
        USB_State = 0x30;
        return;
    }
    else if (PartiEjectFlag != 0x00)
    {
        PartiEjectFlag = 0x00;
        //进入分区退出状态
        USB_State = 0x60;
        return;
    }
    else
    {
    }

    switch (USB_State)
    {
    case 0x00:
        //空闲状态
        sState_0_Handle();
        break;
    case 0x10:
        //上传状态
        sState_10_Handle();
        break;
    case 0x20:
        //下传状态
        sState_20_Handle();
        break;
    case 0x51:
        //该状态主要处理切换到ADFU后，收到的第一个 CMD_TestUnitReady_命令,恢复中断
        UdiskReady();
        break;
    case 0x60:
        //分区弹出处理
        sSplitDiskEject();
        break;
    case 0x80:
        USB_State = 0x90;
        break;
    default:
        sfrbak = SFR_BANK;
        SFR_BANK = BANK_USB;
        if ((DPDMCTRL & 0x40) == 0x00)
        {
            //已拔出
            USB_State = 0x40;
        }
        else
        {
            //空闲状态
            USB_State = 0x00;
        }
        SFR_BANK = sfrbak;
        break;
    }
}

/*
 *********************************************************************************************************
 *                                           ADFUHandle
 *
 * Description: 查询、更新ADFU状态，并根据状态作相应处理.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void ADFUHandle(void)
{
    uint32 callentryRetValue;

    //是否已经拔线 55h表示连接，77h表示不连接
    if (PullLineFlag == 0x00)
    {
        //FWOpStatus:上载还是下载操作完成 01:下载 02:上载
        if (FWOpStatus != 0x00)
        {
            //FW上载或下载完成延时2秒
            sFWUpgOkDelay(FWOpStatus);
        }
        else
        {
            //是否有功能调用任务
            if (CallingTaskFlag != 0x00)
            {
                CallingTaskFlag = 0x00;
                callentryRetValue = (*((uint32( *)()) (CallingTaskAdd)))(); //功能调用入口地址
                CallingTaskResult = callentryRetValue >> 16; //状态结构的地址
                StatusLength = (uint16) callentryRetValue; //状态结构的长度
            }
        }
        UdiskReady();
    }
    else
    {
        //prints("ADFU PullLine-");
        PullLineFlag = 0x00;
        FWOpStatus = 0x01;
        TransferFlag = 0x61; //ADFU的状态
    }
}

/*
 *********************************************************************************************************
 *                                           sFWUpgOkDelay
 *
 * Description: FW上载或下载完成延时2秒.
 *
 * Arguments  : fwup_direction: 上载还是下载操作完成 01:下载 02:上载.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFWUpgOkDelay(uint8 fwup_direction)
{
    if (fwup_direction == 0x01) //need reboot
    {
        //升完级不再响应中断
        IE0 &= 0x7f;
        //不允许在UdiskReady打开中断
        StatusCheckFlag = 0x00;

        if (CheckTimer(&AdfuTimerCtl, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) != 0x00)
        {
            //prints("udisk exit -");
            ClearWatchdog();
            if (AccessFlash != 0x00)
            {
                UD_ClrVram();
            }
            UDiskExit();
            //UD_Exit();

            //LBA-NAND flash不支持0xFF命令，所以特此修改
            if (flash_type == 0x03)
            {
                /*fixme:need
                 reboot_flash();
                 */
            }
            //prints("watchdog-");
            IE0 &= 0x7f;

            SFR_BANK = BANK_RTC;

            WDCTL =  0x8a; //watchdog使能并重启

            while (1)
            {
                ; //Watch Dog起来会重启
            }
        }
    }
    else
    {
        //prints("go idle-");
        //延时四秒,回空闲状态
        if (CheckTimer(&AdfuTimerCtl, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE * 2) != 0x00)
        {
            FWOpStatus = 0x00;
            TransferFlag = 0x00; //回到Ready状态
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sState_0_Handle
 *
 * Description: 空闲状态处理函数.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_0_Handle(void)
{
    //是否开始忙,UpOrDownLoad还未被初始化,EP1在做除read,write外的交流,01:上传  02:下传
    if ((USBBusy == 0x00) || (UpOrDownLoad == 0x00))
    {
        //为下一次收CBW准备Ep1
        UdiskReady();
    }
    else
    {
        if (UpOrDownLoad == 0x01)
        {
            //进入上传状态
            USB_State = 0x10;
        }
        else if (UpOrDownLoad == 0x02)
        {
            //进入下传状态
            USB_State = 0x20;
        }
        else
        {
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sState_10_Handle
 *
 * Description: 上传状态.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_10_Handle(void)
{
    //01:上传  02:下传
    if (UpOrDownLoad == 0x01)
    {
        //查定时,UsbIdleCounter:计数器指针  PrevRTCTimerValue1:1Hz变化记录变量  USBIDLECOUNTERVALUE:计数值,单位为1秒
        if (CheckTimer(&UsbIdleCounter, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) == 0x00)
        {
            //为下一次收CBW准备Ep1。显示不占用DMA，可以与U盘响应一起进行
            UdiskReady();
        }
        else
        {
            //一段时间收不到USB中断,进入到空闲状态
            sGoInState0();
        }
    }
    else
    {
        USB_State = 0x20;
    }
}

/*
 *********************************************************************************************************
 *                                           sState_20_Handle
 *
 * Description: 下传状态处理函数.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_20_Handle(void)
{
    if (UpOrDownLoad == 0x02)
    {
        //是否进入空闲
        if (CheckTimer(&Counter_30s, &PrevRTCTimerValue2, COUNTERVALUE_30S) == 0x00)
        {
            //发了可拔U盘命令没有  0:不可拔线，01h:可拔线。不可依靠该变量决定是否拔线
            //因为在XP下在传输过程中如果有包插进(我们的工具软件)也会发可拔线
            if ((Removable == 0x00) || (CheckTimer(&UsbIdleCounter, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) == 0x00))
            {
                UdiskReady();
            }
            else
            {
                //一段时间收不到USB中断,进入到空闲状态
                sGoInState0();
            }
        }
        else
        {
            //一段时间收不到USB中断,进入到空闲状态
            sGoInState0();
        }
    }
    else
    {
        USB_State = 0x10;
    }
}

/*
 *********************************************************************************************************
 *                                           sGoInState0
 *
 * Description: 一段时间收不到USB中断, 进入到空闲状态.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sGoInState0(void)
{
    //如果发生写操作,把最后一次传输数据写入Flash
    UD_Update();

    //为进入空闲状态的显示而不响应USB中断
    UdiskUnReady();
    USB_State = 0x00;
    USBBusy = 0x00;
    //从空闲到读写的显示必须发在关USB中断条件下(for brom can't run at 48M clock)
    UpOrDownLoadBak = 0x00;
}

/*
 *********************************************************************************************************
 *                                           sCardCapCheck
 *
 * Description: Card Capacity Check
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : 如果报上卡盘符,PC就会读卡盘,在U-mode模式下,必须读卡才能结束,故必须安装卡驱动.
 *********************************************************************************************************
 */
void sCardCapCheck(void)
{
    uint8 reg_bak, drv_install_flag;
    reg_bak = IE0;
    IE0 = IE0 & 0x7f;
    //single lun of card mode need install card driver
    if ((DiskAppearSelect != 0x03) && (ShowMultiDiskFlag == 0))
    {
        CardExistFlag = 0; //安装失败,无卡标志

    }
    else
    {
        if(StorageInfor.ce_info[0].type == 3)
        {
        	  drv_install_flag = (uint8) DRV_Install(FFCardBaseDrvName, 0);
        }
        else
        {
            if (DRV_CheckCard(1) == 2) //检测卡是否插上和卡类型:0:卡不存在; 1:MS卡; 2:SD/MMC卡,无写保护; 0x11:写保护的SD卡.
            {
                //char DRV_Install(const char *filename, word param);; param=0:快速 01:慢速
                drv_install_flag = (uint8) DRV_Install(FFSDCardDrvName, 0);
            }
            else
            {
                drv_install_flag = 0;
            }
        }

        //0:失败  01h:成功
        if (drv_install_flag == 0x01)
        {
            CardCapacity = (uint32) UD_SDCardGetCap(); //以扇区为单位
            CardExistFlag = 1; //有卡标志
            RWCardError = 0; //清card读写错误标志

            Formatcap_cmd_info_DiskCard.CurrentCap = CardCapacity;
            Formatcap_cmd_info_DiskCard.FomatableCap = CardCapacity;
            Cap_cmd_info_DiskCard.LastLogBlockAddr = CardCapacity - 1;
        }
        else
        {
            CardExistFlag = 0;
        }
    }
    IE0 = reg_bak;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetDiskInfo
 *
 * Description: 取得磁盘数目情况,要在调用UdiskInit后调用.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
int8 UDiskGetDiskInfo(void)
{
    int8 cDiskInfo;

    cDiskInfo = 0;
    if (SplitDiskFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x80; //bit7:是否分区
    }
    if (EncryptFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x40; //bit6:是否加密
    }
    if (CardExistFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x20; //bit5:是否插卡
    }

    return cDiskInfo;
}

/*
 *********************************************************************************************************
 *                                           UDiskCheckWP
 *
 * Description: 设置磁盘写保护检查.
 *
 * Arguments  : e:Card Or Flash.
 *
 * Returns    : l:0 Not Write Protect.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskCheckWP(int8 letter)
{
    uint8 ret;

    ret = 0;
    if (letter == 'C')
    {
        ret = FlashWPFlag;
    }
    if (letter == 'H')
    {
        ret = CardWPFlag;
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetStatus
 *
 * Description: Return the Udisk transfer status.
 *
 * Arguments  : None.
 *
 * Returns    : a: the status.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskGetStatus(void)
{
    uint8 ret;

    ret = USB_State;
    if (IsAdfuCondition != 0x00 && TransferFlag != 0)
    {
        ret = TransferFlag;
    }
    return ret;
}

/*
 *********************************************************************************************************
 *                                           DisableSuspend
 *
 * Description: 禁止suspend.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void DisableSuspend(void)
{
    DisableSuspendFlag = 0x01;
}

/*
 *********************************************************************************************************
 *                                           AlwaysShowCardDisk
 *
 * Description: 插线即显示卡盘符.
 *
 * Arguments  : 0: disable 1: enable.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void AlwaysShowCardDisk(uint8 para)
{
    ShowMultiDiskFlag = para;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetCardStatus
 *
 * Description: 查询卡是否存在.
 *
 * Arguments  : 0: disable 1: enable.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskGetCardStatus(void)
{
    return CardExistFlag;
}

/*
 *********************************************************************************************************
 *                                           UDiskCardInitInternal
 *
 * Description: 卡重初始化.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UDiskCardInitInternal(void)
{
    sCardCapCheck();

    ClearWatchdog();

    CardWPCheck();
}

/*
 *********************************************************************************************************
 *                                           smDelay
 *
 * Description: Delay n ms.
 *
 * Arguments  : e: n.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void smDelay(uint8 mscount)
{
    long count;
    count = 0x00004000 * mscount;
    while (count != 0x00)
    {
        count = count - 1;
    }
}

/*
 ********************************************************************************
 *                           EndianReverse
 * Description : Endian reverse
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void EndianReverse(uint8 *data_p, uint16 data_length)
{
    char tmp;
    char *head_p, *end_p;
    head_p = data_p;
    end_p = data_p + data_length - 1;

    for (; head_p < end_p; ++head_p, --end_p)
    {
        tmp = *end_p;
        *end_p = *head_p;
        *head_p = tmp;
    }
}

/*
 *********************************************************************************************************
 *                                           CheckTimer
 *
 * Description: 查定时.
 *
 * Arguments  : hl: 计数器指针; de: 1Hz变化记录变量; c-1: 计数值, 单位为1秒.
 *
 * Returns    : a: 指定时间是否已到: 1:yes; 0:no.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static uint8 CheckTimer(uint8 *timerrecord, uint8 *countrecord, uint8 count)
{
    uint8 ret = 0;
    uint8 sfrbak;

    sfrbak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    if (RTCTimeS == *timerrecord)
    {
        // not reach one second
        ret = 0;
    }
    else
    {
        *timerrecord = RTCTimeS;
        //与到达空闲时的数值比较
        if (*countrecord < count)
        {
            *countrecord += 1;
            ret = 0;
        }
        else
        {
            *countrecord = 0;
            ret = 1;
        }
    }
    SFR_BANK = sfrbak;
    return ret;
}

/*
 *********************************************************************************************************
 *                                           sSplitDiskEject
 *
 * Description: 加密盘完成后, 分区弹出处理.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sSplitDiskEject(void)
{
    if (EjectFlag == 0x00)
    {
        EjectFlag++;

        //退出前清理现场程序入口
        UDiskExit();
        UDiskInitInternal();
        //恢复init时被清的状态
        USB_State = 0x60;
    }
    else
    {
        if (CheckTimer(&EjectTimeCtr, &PrevRTCTimerValue1, USBEJECTCTRVALUE) != 0x00)
        {
            EjectTimeCtr = 0x00;
            USB_State = 0x00;
            EjectFlag = 0x00;

            UDiskRun(0x00);
        }
    }
}

/*
 *********************************************************************************************************
 *                                           UdiskReady
 *
 * Description: For the next usb transfer.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UdiskReady(void)
{
    uint8 IE0bak;
    //Global Interrupt Disable
    IE0bak = IE0;
    IE0 = IE0 & 0x7f;

    //在状态查询过程发生了中断，应出去跑一下显示任务再打开USB，保证显示及时
    if (StatusCheckFlag != 0x00)
    {
        //跑到这里新的CBW实际已经收到EPA FIFO，只需开中断响应即可,Enable USB Interrupt
        AIE = AIE | 0x01;
    }

    //Global Interrupt Enable
    IE0 = IE0bak;
}

/*
 *********************************************************************************************************
 *                                           UdiskReady
 *
 * Description: For the next usb transfer.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UdiskUnReady(void)
{
    //为进入空闲状态的显示而不响应USB中断,Disable USB Interrupt
    AIE = AIE & (uint8) 0xfe;
}

/*
 *********************************************************************************************************
 *                                           UDiskPullCardOut
 *
 * Description: Handle the card plug out message.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskPullCardOut(void)
{
    uint8 IE0bak;
    IE0bak = IE0;
    IE0 = IE0 & 0x7f;

    //	mydisable_interrupt();

    //0: mac OSX system    1: windows system
    if (OSType == 0x00)
    {
        //usb disconnect,实现在苹果系统下卡拔掉后盘符消失效果
        UsbCTRL_STUS |= 0x40;
    }

    //卡ID
    if (DRV_DetectUD(1) != 0x20)
    {
        CardExistFlag = 0x00; //清空卡存在标志

        //卡在做umode写时，拔卡，会有继续写的动作，如果卸载卡驱动，会导致后续的umode写错，故注释卸载动作
        //DRV_UnInstall(DRV_SDFF); //卸载卡驱动，避免卡电源没有关闭，导致插卡时小机重启
    }

    //开USB中断
    AIE = AIE | 0x01;

    //0: mac OSX system    1: windows system
    if (OSType == 0x00)
    {
        //D+ Pull high, 此时，PC 才检测到有usb device 插上
        UsbCTRL_STUS &= (uint8) 0xbf;
    }
    IE0 = IE0bak;

    //	myenable_interrupt();

    return 0x01;
}

/*
 *********************************************************************************************************
 *                                           CardWPCheck
 *
 * Description: Check is the sdcard is write protected.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 CardWPCheck(void)
{
    CardWPFlag = 0;
    //如果卡不存在就不用判断卡的写保护标志，防止卡的驱动没安装成功时，调用mUD_SDCardWPDetect时程序跑飞
    if (CardExistFlag != 0x00)
    {
        //将检测结果放入卡写保护标志
        //        CardWPFlag = (uint8)UD_SDCardWPDetect();		// not support
    }

    return CardWPFlag;
}

