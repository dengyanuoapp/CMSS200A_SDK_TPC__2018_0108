/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UdiskBulkCommand.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-13 12:45
 *********************************************************************************************************
 */
#include "udisklib.h"

#pragma name(BULKCMD_C)
extern unsigned long systemtime;

#pragma ROM(HUGE)
//record operations of Udisk mode;0x00 没有进行usidk写操作；bit0 == 1, UDisk write;bit4 == 1, MTP write.
extern uint8 UDiskOperationInfo;

static void lsNotSuppCMD(void);
static void CMD_TestUnitReady_(void);
static void CMD_PreventMediumRemoval_(void);
static void CMD_ReadCapacities_(void);
static void CMD_ReadFormatCapacities_(void);
static void ReadCapacities(void);
static void CMD_Inquiry_(void);
static void CMD_ModeSense_(void);
static void CMD_ModeSelect_(void);
static void CMD_RequestSense_(void);
static void CMD_Verify_(void);
static void CMD_StartStopUnit_(void);
static void CMD_Philips_(void);

static void CMD_SwitchToADFU(void);
static void CMD_IsActions(void);
static void Get_User_DefinedID(void);
static void Data_Decrypt_Xor(uint8 *dataAddr, uint8 dataLength);
static void Set_Device_Info(void);
static void Access_Device_Info(void);

static void CMD_Read_TOC(void);
void CMD_Mechanism_Status(void);
static void CMD_ReadDiscInformation(void);
static void CMD_WriteCD_(void);
static void CMD_PlayAudio(void);
static void CMD_Read_Autorun_Record_(void);

static uint16 Fix_Nrmlmode_RWSctrs(void);
static uint32 Ajust_Data_Length(uint32 length, uint32 largestLength);
static void Udisk_NrmlMode_Read(uint32 readnum);
static void Udisk_NrmlMode_Write(uint32 readnum);
static void Udisk_FIFOMode_Read(uint32 readnum);
static void Udisk_FIFOMode_Write(uint32 readnum);
#pragma ROM(HUGE)

#pragma renamecode(?PR?BULKCMD_C)
//#pragma ROM(HUGE)
#pragma ROM(HUGE)

//此定义为避免屏蔽掉打印后，udisk应用编译常量段定义未使用导致的编译错误。
#ifndef _PRINT_DEBUG_
const uint8 ConstSeg1 = 'A';
#endif

/*
 *********************************************************************************************************
 *                                           Bulk_Out_Handle
 *
 * Description: Endpoint a interrupt handle function.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Bulk_Out_Handle(void)
{
    uint8 i;
    //标志在查状态过程有USB中断发生
    StatusCheckFlag = 0;
    illegalLBAFlag = 0;
    flash_normal_rw_sctrs = 0x00;

    for (i = 0; i < 24; i++)
    {
        ((uint8 *) &CBW_data_buffer)[i] = Fifo2dat;
    }

    Out2cs_hcin2cs = Out2cs_hcin2cs | 0x02; //clear EPB fifo
    USBEIRQ = USBEIRQ | (uint8) 0x80;
    SHORTPCKIRQ = SHORTPCKIRQ | 0x20;

    //将_dCBWDataTransferLength长度从小端转为大端
    EndianReverse((uint8 *) (&CBW_data_buffer._dCBWDataTransferLength), 4);

    if (cbw_count != 2)
    {
        cbw_count++;
    }

    //"USBC":0x55534243
    if (CBW_data_buffer._dCBWSignature != 0x55534243)
    {
        lsNotSuppCMD();
    }
    else
    {
        //save _dCBWTag using at _dCSWTag reply
        CSWBuffer._dCSWTag = CBW_data_buffer._dCBWTag;

        //AutoRun光盘存在标志: 0:不存在; 非0:存在
        if (AutoRunDiskFlag == 0)
        {
            CBW_data_buffer._bCBWLUN++;
        }

        //用户的盘符选择  0:多盘符(默认)  01h:普通盘  02h:加密盘  03h:插卡
        if (DiskAppearSelect == 2)
        {
            CBW_data_buffer._bCBWLUN++;
        }

        if (DiskAppearSelect == 3)
        {
            CBW_data_buffer._bCBWLUN += 2;
        }

        //没有加密盘,访问加密盘则默认在访问卡盘符
        if (CBW_data_buffer._bCBWLUN == 0x02)
        {
            if (SplitDiskFlag == 0x00)
            {
                CBW_data_buffer._bCBWLUN = 0x03;
            }
        }

        //CBWCB第一个字节是命令字节
        switch (CBW_data_buffer._CBWCB[0])
        {
        case 0x00:
            CMD_TestUnitReady_();
            break;
        case 0x28:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_Read10_();
            break;
        case 0x2a:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            //for autorun
            CMD_Write10_();
            break;
        case 0x1e:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            //for autorun
            CMD_PreventMediumRemoval_();
            break;
        case 0x25:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            //for autorun
            CMD_ReadCapacities_();
            break;
        case 0x23:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            //for autorun
            CMD_ReadFormatCapacities_();
            break;
        case 0x12:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            //for autorun
            CMD_Inquiry_();
            break;
        case 0x5a: //CMD_ModeSense10_
        case 0x1a: //CMD_ModeSense06_,Command Descriptor Block(CDB) For SCSI Device

            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_ModeSense_();
            break;
        case 0x55:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_ModeSelect_();
            break;
        case 0x03:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_RequestSense_();
            break;
        case 0x2f:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_Verify_();
            break;
        case 0x1b:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_StartStopUnit_();
            break;

        case 0xdd:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_Philips_();
            break;

            //for autorun cmd
        case 0x43:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_Read_TOC();
            break;
        case 0xbd:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_WriteCD_();
            break;
        case 0x51:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_ReadDiscInformation();
            break;
        case 0x45:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_PlayAudio();
            break;
        case 0xce:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_Read_Autorun_Record_();
            break;
            //for autorun end


            //自定义类协议
        case 0xcb:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_SwitchToADFU();
            break;
        case 0xcc:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            CMD_IsActions();
            break;
        case 0xca:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            Get_User_DefinedID();
            break;
        case 0xb0:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            IsAdfuCondition = 0;
            USBBusy = 1;
            Set_Device_Info();
            break;
        case 0xcd:
            //除了TestUnitReady_,其他操作认为忙。USBBusy在state0时被查询
            //cd命令为统一后的set/get infomation协议命令字
            IsAdfuCondition = 0;
            USBBusy = 1;
            Access_Device_Info();
            break;

            //进入ADFU命令
        case 0x05:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Access_Internal_RAM_Command();
            break;
        case 0x08:
        case 0x09:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Access_Nand_Flash_Command();
            break;
        case 0x10:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Switch_Firmware_Command();
            break;
        case 0x16:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Detach_Device_Command();
            break;
        case 0x18:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Polling_Ready_Command();
            break;
            //case 0x1a:
            //IsAdfuCondition = 1;
            //USBBusy = 1;
            //Get_Status_Command();
            //break;
        case 0x20:
            IsAdfuCondition = 1;
            USBBusy = 1;
            Calling_Entry_Command();
            break;
        default:
            lsNotSuppCMD();
            break;
        }
    }
}

/*
 *********************************************************************************************************
 *                                           lsNotSuppCMD
 *
 * Description: USB SFF-8020i protocol handle, command not support.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void lsNotSuppCMD(void)
{
    IsAdfuCondition = 0x01;
    //返回任意数，避免98死机
    Send_Data_To_PC((uint8 *) &Cap_cmd_info_DiskA, CBW_data_buffer._dCBWDataTransferLength);
    CSWBuffer._bCSWStatus = 0x01;
    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_TestUnitReady_
 *
 * Description: USB SFF-8020i protocol handle, test if unit is ready; There will be 1 command every 1s in WINXP.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_TestUnitReady_(void)
{
    uint8 errorflag;
    uint8 cardstatus;

    errorflag = 0;

    //updata，mast call card operate
    if (IsInAdfuState != 0xff)
    {
        switch (CBW_data_buffer._bCBWLUN)
        {
        case 0x00: //for autorun
            if (bTestNoready == 0x00)
            {
                bTestNoready = 0x80;
                //Media Not Present
                errorflag = 0x04;
            }

            if (OSType == 0x00) //mac OSX system
            {
                //实现在苹果系统下取消Autorun盘符的效果
                if (AutoRunDiskFlag == 0xff)
                {
#ifdef _PRINT_DEBUG_
                    prints("no autorun\n");
#endif

                    //Media Not Present
                    errorflag = 0x04;
                }
            }
            break;
        case 0x01: //普通盘
            break;
        case 0x02: //加密盘
            //密码通过该变量会置位
            if (MediaChgFlag != 0)
            {
                MediaChgFlag = 0;
                //密码通过,地址计算切换到DiskB
                PasswdPassOk++;
                //Media Change
                errorflag = 0x03;
            }
            break;
        case 0x03: //卡盘
            cardstatus = 0x00;
            //判断卡存在标志，0：需要探测卡； 1：继续
            if (CardExistFlag == 0x00)
            {
                //卡没有插入的情况下不需去做卡驱动的重新初始化
                cardstatus = 0x01;
            }
            else
            {
                CardWPCheck();
                if (RWCardError != 0)
                {
                    //判断读写卡是否出错，0：无错，直接返回CSW；1：有错，需要重新探测卡
                    cardstatus = 0x01;
                }
            }
            if (cardstatus == 0x01)
            {
                if (CardDetectState != 0x00)
                {
                    //调用卡初始化，会修改CardExistFlag
                    sCardCapCheck();
                }

                if (CardExistFlag == 0x00)
                {
                    //Media Not Present
                    errorflag = 0x04;
                }
                else
                {
                    //Media Change
                    errorflag = 0x03;
                }
            }
            break;
        default:
            break;
        }
    }
    ReportCSW(errorflag);
}

/*
 *********************************************************************************************************
 *                                           CMD_Read10_
 *
 * Description: USB SFF-8020i protocol handle, read device.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void CMD_Read10_(void)
{
    uint8 errorflag = 0;
    DMA_DT_Trnsfr_t dma_transfer;

    //upload
    UpOrDownLoad = 0x01;

    Compute_LBA();
    EPA_In_Rdy();

    //nand base,不插卡的情况下PC会有一次读卡的操作,不装载卡驱动,直接送随机数据到PC
    if ((CardExistFlag == 0x00) && (flashorcard_rwflag == 0x01))
    {
        while (udisk_rw_sctr_num != 0x00)
        {
            udisk_rw_sctr_num--;

            ClearWatchdog();

            dma_transfer.source_addr = DATABUFFER;
            dma_transfer.dst_addr = URAMSTARTADDR;
            dma_transfer.length = 0x200;
            dma_transfer.pagenum = DRAM_TO_USBFIFO;
            DMA_Data_Transfer(&dma_transfer);
            Wait_PrevDMA_TransEnd();
        }

        //Media Not Present
        errorflag = 0x04;
    }
    else if ((sReadCheckForEncryp() != 0x00) || (illegalLBAFlag != 0x00))
    {
        if (illegalLBAFlag != 0x00)
        {
            memset(DATABUFFER, 0, 0x200);
        }

        while (udisk_rw_sctr_num != 0x00)
        {
            ClearWatchdog();

            udisk_rw_sctr_num--;

            dma_transfer.source_addr = DATABUFFER;
            dma_transfer.dst_addr = URAMSTARTADDR;
            dma_transfer.length = 0x200;
            dma_transfer.pagenum = DRAM_TO_USBFIFO;
            DMA_Data_Transfer(&dma_transfer);
            Wait_PrevDMA_TransEnd();
        }
    }
    else
    {
        //#ifdef FIFO_TRANS_MODE
        if (IsAdfuCondition == 0x00)
        {
            // card fifo read
            Udisk_FIFOMode_Read(udisk_rw_sctr_num);
        }
        else
        {
            //#else
            Udisk_NrmlMode_Read(udisk_rw_sctr_num);
        }
        //#endif
    }

    if (CBW_data_buffer._bCBWLUN == 0x03)
    {
        if (IsInAdfuState != 0xff)
        {
            //卡是否出错
            if ((CardExistFlag == 0x00) && (RWCardError != 0x00))
            {
                errorflag = 0x04;
            }
            if ((CardExistFlag != 0x00) && (RWCardError != 0x00))
            {
                errorflag = 0x03;
            }
        }
        else
        {
            errorflag = 0x00;
        }
    }

    ReportCSW(errorflag);

    //nand flash的读写调用完成后，清除是否进入空闲状态的计数值
    PrevRTCTimerValue1 = 0x00;
    PrevRTCTimerValue2 = 0x00;
}

/*
 *********************************************************************************************************
 *                                           CMD_Write10_
 *
 * Description: USB SFF-8020i protocol handle, write device.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void CMD_Write10_(void)
{
    DMA_DT_Trnsfr_t dma_transfer;
    uint8 errorflag = 0;

    //download
    UpOrDownLoad = 0x02;

    /* record udisk write 2010-12-31 16:29*/
    UDiskOperationInfo = UDiskOperationInfo | 0x01;

    //输入LBA，(udisk_rw_sctr_num)
    Compute_LBA();

    EPB_OutRdy_FIFOFullNAK();

    if (udisk_rw_sctr_num != 0x00)
    {
        flash_disk_write_flag++;
    }

    //检查是否加密盘协议操作
    if ((sWriteCheckForEncryp() != 0x00) || (illegalLBAFlag != 0x00))
    {
        while (udisk_rw_sctr_num != 0)
        {
            udisk_rw_sctr_num--;

            ClearWatchdog();

            dma_transfer.source_addr = URAMSTARTADDR;
            dma_transfer.dst_addr = DATABUFFER;
            dma_transfer.length = 0x200;
            dma_transfer.pagenum = USBFIFO_TO_DRAM;
            DMA_Data_Transfer(&dma_transfer);
            Wait_PrevDMA_TransEnd();

            //检查加密盘是否要写
            if (sEncrypParseInWR() == 0x00)
            {
                sectors_trans_once = 0x01;
                sProgramSectorsToStorage((uint8 *) DATABUFFER);
            }
        }
    }
    else
    {
        //#ifdef FIFO_TRANS_MODE
        if (IsAdfuCondition == 0x00)
        {
            // card fifo read
            Udisk_FIFOMode_Write(udisk_rw_sctr_num);
        }
        //#else
        else
        {
            Udisk_NrmlMode_Write(udisk_rw_sctr_num);
        }
        //#endif

    }

    if (flash_type != 4)
    {
        //        Udisk_Parse_MBR();
    }

    //升级中，屏蔽所有有可能对卡的访问，因为由于无卡而报错，会使升级失败
    if (IsInAdfuState != 0xff)
    {
        if ((flashorcard_rwflag != 0x00) && (RWCardError != 0x00))
        {
            //会修改CardExistFlag,RWCardError
            //sCardCapCheck();
            RWCardError = 0x01;
        }

        if (CBW_data_buffer._bCBWLUN == 0x00)
        {
            //Cannot Write CD_ROM
            if (bWriteCD_ROM == 0x00)
            {
                errorflag = 2;
            }
            //光盘写属性清0
            bWriteCD_ROM = 0;
        }
        else if (CBW_data_buffer._bCBWLUN == 0x01)
        {
            if (FlashWPFlag != 0x00)
            {
                errorflag = 2;
            }
        }
        else
        {
            if (CardWPFlag != 0x00)
            {
                errorflag = 2;
            }
            if ((CardExistFlag == 0x00) && (RWCardError != 0x00))
            {
                errorflag = 4;
            }
            if ((CardExistFlag != 0x00) && (RWCardError != 0x00))
            {
                errorflag = 3;
            }
        }
    }
    else
    {
        errorflag = 0;
    }

    ReportCSW(errorflag);

    //nand flash的读写调用完成后，清除是否进入空闲状态的计数值
    PrevRTCTimerValue1 = 0x00;
    PrevRTCTimerValue2 = 0x00;
}

/*
 *********************************************************************************************************
 *                                           CMD_PreventMediumRemoval_
 *
 * Description: USB SFF-8020i protocol handle, decide whether the device can be moved.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_PreventMediumRemoval_(void)
{
    if ((CBW_data_buffer._CBWCB[4] & 0x01) == 0x00)
    {
        //在PC写文件时,	PC允许拔线后,立即update flash,减少PC写完文件小机立即掉电,写的文件数据错误的机率.
        UD_Update(); // need fix
        //USB Not Busy
        Removable = 0x01;
    }
    else
    {
        //USB Busy
        Removable = 0x00;
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_ReadCapacities_
 *
 * Description: USB SFF-8020i protocol handle, read device capacity.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_ReadCapacities_(void)
{
    Cap_Info_DiskA_p = (uint8 *) &Cap_cmd_info_DiskA;
    Cap_Info_DiskB_p = (uint8 *) &Cap_cmd_info_DiskB;
    Cap_Info_DiskC_p = (uint8 *) &Cap_cmd_info_DiskC;
    Cap_Info_DiskCard_p = (uint8 *) &Cap_cmd_info_DiskCard;
    ReadCapacities();
}

/*
 *********************************************************************************************************
 *                                           CMD_ReadFormatCapacities_
 *
 * Description: USB SFF-8020i protocol handle, read device formatted capacity.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_ReadFormatCapacities_(void)
{
    Cap_Info_DiskA_p = (uint8 *) &Formatcap_cmd_info_DiskA;
    Cap_Info_DiskB_p = (uint8 *) &Formatcap_cmd_info_DiskB;
    Cap_Info_DiskC_p = (uint8 *) &Formatcap_cmd_info_DiskC;
    Cap_Info_DiskCard_p = (uint8 *) &Formatcap_cmd_info_DiskCard;

    if (CBW_data_buffer._dCBWDataTransferLength > 20)
    {
        CSWBuffer._dCSWDataResidue = CBW_data_buffer._dCBWDataTransferLength - 20;
        CBW_data_buffer._dCBWDataTransferLength = 20;
    }

    ReadCapacities();
}

/*
 *********************************************************************************************************
 *                                           ReadCapacities
 *
 * Description: USB SFF-8020i protocol handle, send device capacity.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void ReadCapacities(void)
{
    uint8 errorflag = 0;
    uint8 *data_p;

    Cap_Info_DiskCDROM_p = (uint8 *) &Cap_cmd_info_DiskCDROM;

    switch (CBW_data_buffer._bCBWLUN)
    {
    case 0x00: //for autorun
        if (bMediaChange == 0x00)
        {
            bMediaChange = 0x80;
            //media change
            errorflag = 0x03;
        }
        data_p = Cap_Info_DiskCDROM_p;
        break;
    case 0x01:
        data_p = (uint8 *) Cap_Info_DiskA_p;
        break;
    case 0x02:
        if ((EncryptFlag == 0x00) || (PasswdPassOk == 1))
        {
            data_p = (uint8 *) Cap_Info_DiskB_p;
        }
        else
        {
            data_p = (uint8 *) Cap_Info_DiskC_p;
        }
        break;
    case 0x03:
        if (IsInAdfuState != 0xff)
        {
            if (CardExistFlag == 0x00)
            {
                //Media Not Present
                errorflag = 0x04;
            }
        }
        data_p = (uint8 *) Cap_Info_DiskCard_p;
        break;
    default:
        data_p = (uint8 *) Cap_Info_DiskA_p;
        break;
    }
    Send_Data_To_PC(data_p, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(errorflag);
}

/*
 *********************************************************************************************************
 *                                           CMD_Inquiry_
 *
 * Description: USB SFF-8020i protocol handle, inquiry the device infomation.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : Windows系统进入U盘后,第一个CBW命令为0x12.
 *********************************************************************************************************
 */
static void CMD_Inquiry_(void)
{
    uint8 *data_p;
    if (cbw_count == 1)
    {
        //0: mac OSX system    1: windows system  用于实现在苹果系统下卡拔掉后盘符消失效果
        OSType = 1;
    }
    if (CBW_data_buffer._bCBWLUN == 0x00)
    {
        //for autorun
        data_p = (uint8 *) &CMD_Inquiry_information_CDROM;
    }
    else
    {
        data_p = (uint8 *) &CMD_Inquiry_information_FOB;
    }

    Send_Data_To_PC(data_p, CBW_data_buffer._dCBWDataTransferLength);
    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_ModeSense06_
 *
 * Description: USB SFF-8020i protocol handle, return the usb mode information.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_ModeSense_(void)
{
    uint8 *data_p;
    uint8 tmp = 0x00;

    IsReadCapAndReadRec0 = 0;
    if (CBW_data_buffer._bCBWLUN == 0x00)
    {
        data_p = (uint8 *) &CDRom_ModeSense_information;
    }
    else
    {
        if (CBW_data_buffer._bCBWLUN == 0x03)
        {
            if (CardWPFlag != 0x00)
            {
                tmp = 0x80;
            }
        }
        else
        {
            if (FlashWPFlag != 0x00)
            {
                tmp = 0x80;
            }
        }

        Time_Protect_Page_information[3] = tmp;
        All_Page_information[3] = tmp;

        //PC and Page Code
        if (CBW_data_buffer._CBWCB[2] == 0x3f)
        {
            data_p = (uint8 *) &All_Page_information;
        }
        else
        {
            data_p = (uint8 *) &Time_Protect_Page_information;
        }
    }

    Send_Data_To_PC(data_p, CBW_data_buffer._dCBWDataTransferLength);
    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_ModeSelect_
 *
 * Description: USB SFF-8020i protocol handle, usb mode select.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_ModeSelect_(void)
{
    IsReadCapAndReadRec0 = 0;
    EPB_OutRdy_FIFOFullNAK();

    //Data Packet Received Interrupt
    while ((SHORTPCKIRQ & 0x20) == 0x00)
    {
#ifdef _PRINT_DEBUG_
        prints("cmd:0x55 err\n");
#endif
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_RequestSense_
 *
 * Description: USB SFF-8020i protocol handle, request the error type for last wrong transfer.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_RequestSense_(void)
{
    uint8 *data_p;

    //XP下当卡不存在时会在TestReady中不停报错，此句使小机不进入忙状态
    USBBusy = 0;
    ErrorReportFlag = 0;

    switch (ErrorIndicator)
    {
    case 0x00:
        data_p = (uint8 *) &Request_Sense_cmd_info_NoError;
        break;
    case 0x01:
        data_p = (uint8 *) &Request_Sense_cmd_info_CommandReset;
        break;
    case 0x02:
        data_p = (uint8 *) &Request_Sense_cmd_info_MediaProtect;
        break;
    case 0x03:
        data_p = (uint8 *) &Request_Sense_cmd_info_MediaChg;
        break;
    case 0x04:
        data_p = (uint8 *) &Request_Sense_cmd_info_MediaNotPresent;
        break;
        //for autorun
    case 0x05:
        data_p = (uint8 *) &CDRom_Request_Sense_command_Invalid_Operation_Code;
        break;
    default:
        data_p = (uint8 *) &Request_Sense_cmd_info_NoError;
        break;
    }

    Send_Data_To_PC(data_p, CBW_data_buffer._dCBWDataTransferLength);
    ReportCSW(0);

    ErrorIndicator = 0;
}

/*
 *********************************************************************************************************
 *                                           CMD_Verify_
 *
 * Description: USB SFF-8020i protocol handle, device sector verify.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_Verify_(void)
{
    IsReadCapAndReadRec0 = 0;
    ReportCSW(0);

}

/*
 *********************************************************************************************************
 *                                           CMD_StartStopUnit_
 *
 * Description: USB SFF-8020i protocol handle, pc eject the device.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_StartStopUnit_(void)
{
    if (CBW_data_buffer._CBWCB[4] == 0x02)
    {
        //置PC弹出标志
        PcEjectFlag = 0x01;
        //量产工具在win7和win vista下不能发送suspend,通过此命令实现断线
        FWOpStatus = 0x01;
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_Philips_
 *
 * Description: 新增协议,为支持PD客户philips,上报客户的信息
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void CMD_Philips_(void)
{
    uint32 tmp;
    DMA_DT_Trnsfr_t dma_transfer;

    EPA_In_Rdy();

    sFlash_Rds(RW_FIX, 0, DATABUFFER);
    tmp = *(uint32 *) (DATABUFFER + 512 - 6);

    uprintf("addr=%x\n", tmp);

    sFlash_Rds(RW_FIX, tmp, DATABUFFER);
    dma_transfer.source_addr = DATABUFFER;
    dma_transfer.dst_addr = URAMSTARTADDR;
    dma_transfer.length = 0x200;
    dma_transfer.pagenum = DRAM_TO_USBFIFO;
    DMA_Data_Transfer(&dma_transfer);
    Wait_PrevDMA_TransEnd();

    sFlash_Rds(RW_FIX, tmp + 1, DATABUFFER);
    dma_transfer.source_addr = DATABUFFER;
    dma_transfer.dst_addr = URAMSTARTADDR;
    dma_transfer.length = 0x200;
    dma_transfer.pagenum = DRAM_TO_USBFIFO;
    DMA_Data_Transfer(&dma_transfer);
    Wait_PrevDMA_TransEnd();

    sFlash_Rds(RW_FIX, tmp + 2, DATABUFFER);
    dma_transfer.source_addr = DATABUFFER;
    dma_transfer.dst_addr = URAMSTARTADDR;
    dma_transfer.length = 0x200;
    dma_transfer.pagenum = DRAM_TO_USBFIFO;
    DMA_Data_Transfer(&dma_transfer);
    Wait_PrevDMA_TransEnd();

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_SwitchToADFU
 *
 * Description: ADFU protocol handle, switch from udisk mode to adfu upgrade mode.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : Will load the adfu segment and cover encrypt disk segment.
 *********************************************************************************************************
 */
static void CMD_SwitchToADFU(void)
{
    //flash非保护/单盘符情况下不需要判断是否是写Flash盘
    if ((FlashWPFlag == 0x00) && ((DiskAppearSelect != 0x00) || (CBW_data_buffer._bCBWLUN == 0x01)))
    {
        CanSwitchToADFU = 0xff;
        SwitchToADFUFlag = 0xff;
        //认为是进入升级状态，用于升级时屏蔽PC对卡的read操作
        IsInAdfuState = 0xff;
    }
    else
    {
        CanSwitchToADFU = 0x00;
    }

    Send_Data_To_PC(&CanSwitchToADFU, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           CMD_IsActions
 *
 * Description: ADFU protocol handle, inquiry if the device is base on ACTIONS ic.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void CMD_IsActions(void)
{
    if (IsReadCapAndReadRec0 == 0x02)
    {
        IsNotFormat = 0x01;
    }
    else
    {
        IsNotFormat = 0x00;
    }

    IsReadCapAndReadRec0 = 0x00;

    //单盘符情况下不需要判断是否是写Flash盘，写Flash?
    if ((DiskAppearSelect != 0x00) || (CBW_data_buffer._bCBWLUN == 0x01))
    {
        if (CBW_data_buffer._dCBWDataTransferLength == 0x0B)
        {
            Send_Data_To_PC(ActionsFlag, CBW_data_buffer._dCBWDataTransferLength);
        }
        else
        {
            Send_Data_To_PC(actions_solution_flag, CBW_data_buffer._dCBWDataTransferLength);
        }
    }
    else
    {
        Send_Data_To_PC(DATABUFFER, CBW_data_buffer._dCBWDataTransferLength);
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Get_User_DefinedID
 *
 * Description:ADFU protocol handle, 读取设备固件量产时写入LFI_Head中的用户自定义ID.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void Get_User_DefinedID(void)
{
    //读SD区头扇区数据到Uram中DataBuffer起始的地方去
    sFlash_Rds(RW_FIX, 0x00, DATABUFFER);

    *((uint16 *) ((uint8 *) DATABUFFER + 284)) = 50;
    *((uint16 *) ((uint8 *) DATABUFFER + 286)) = 0;

    Data_Decrypt_Xor((DATABUFFER + 284), 50);

    Send_Data_To_PC((DATABUFFER + 284), CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Data_Decrypt_Xor
 *
 * Description: 对数据进行解密(不能大于256byte).
 *
 * Arguments  : dataAddr:data address; dataLength:data decrypt length.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void Data_Decrypt_Xor(uint8 *dataAddr, uint8 dataLength)
{
    uint8 i;

    for (i = 0; i < dataLength; i++)
    {
        *dataAddr = (*dataAddr - decrypt_key) ^ decrypt_key + 0x7f;
        dataAddr++;
    }
}

/*
 *********************************************************************************************************
 *                                           Set_Device_Info
 *
 * Description: 设置设备信息(如DeviceID、时间等)
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : 默认只有写方向,设置时间.
 *********************************************************************************************************
 */
static void Set_Device_Info(void)
{
    uint8 i;
    kval_t *kval_p;

    EPB_OutRdy_FIFOFullNAK();

    //Data Packet Received Interrupt
    while ((SHORTPCKIRQ & 0x20) == 0x00)
    {
#ifdef _PRINT_DEBUG_
        prints("cmd:0xb0 err\n");
#endif
    }

    for (i = 0; i < CBW_data_buffer._dCBWDataTransferLength; i++)
    {
        TempBuffer[i] = Fifo2dat;
    }

    EndianReverse(TempBuffer, 2);

    TM_SetDate((date_t *) TempBuffer);
    TM_SetTime((time_t *) (TempBuffer + 4));

    //将systemtime 写回到VRAM中
    sFlash_Rds(RW_FIX, SDDiskCap, DATABUFFER);
    //将全局变量systemtime更新Vram中去
    kval_p = DATABUFFER;
    kval_p->systemtime = systemtime; //全局变量
    sFlash_Wrts(RW_FIX, SDDiskCap, DATABUFFER);

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Access_Device_Info
 *
 * Description: set/get infomation，此协议为统一后的set/get infomation协议
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : 当前此协议仅用来处理图片、视频转换工具获取屏的大小
 *********************************************************************************************************
 */
static void Access_Device_Info(void)
{
    uint16 *data_p;

    data_p = DATABUFFER;
    if ((CBW_data_buffer._CBWCB[1] == 0xc0) && (CBW_data_buffer._CBWCB[2] == 0x0c))
    {
        *data_p = 0; //(uint16)GetLCDWidth();
        data_p++;
        *data_p = 0; //(uint16)GetLCDHeight();
        Send_Data_To_PC(DATABUFFER, CBW_data_buffer._dCBWDataTransferLength);
        ReportCSW(0);
    }
    else
    {
        lsNotSuppCMD();
    }
}

/*
 *********************************************************************************************************
 *                                           CMD_Read_TOC
 *
 * Description: for autorun.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
//for autorun
static void CMD_Read_TOC(void)
{
    if (CBW_data_buffer._dCBWDataTransferLength > 24)
    {
        CSWBuffer._dCSWDataResidue = CBW_data_buffer._dCBWDataTransferLength - 24;
        CBW_data_buffer._dCBWDataTransferLength = 24;
    }

    Send_Data_To_PC((uint8 *) &Read_TOC_command_information, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

void CMD_Mechanism_Status(void)
{
    if (CBW_data_buffer._dCBWDataTransferLength > 13)
    {
        CSWBuffer._dCSWDataResidue = CBW_data_buffer._dCBWDataTransferLength - 13;
        CBW_data_buffer._dCBWDataTransferLength = 13;
    }

    Send_Data_To_PC((uint8 *) &Mechanism_Status_command_information, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

static void CMD_ReadDiscInformation(void)
{
    if (CBW_data_buffer._dCBWDataTransferLength > 28)
    {
        CSWBuffer._dCSWDataResidue = CBW_data_buffer._dCBWDataTransferLength - 28;
        CBW_data_buffer._dCBWDataTransferLength = 28;
    }

    Send_Data_To_PC((uint8 *) &CDRom_Request_DISCINFORMATION, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

static void CMD_WriteCD_(void)
{
    bWriteCD_ROM = 0x80;
    CMD_Write10_();
}

static void CMD_PlayAudio(void)
{
    ReportCSW(0x05); //media change
}

static void CMD_Read_Autorun_Record_(void)
{
    uint8 i;
    for (i = 0; i < 8; i++)
    {
    }
    CMD_Read10_();
}

//for autorun end


/*
 *********************************************************************************************************
 *                                           sRestoreNormalCSW
 *
 * Description: send CSW.
 *
 * Arguments  : err_type-error type
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void ReportCSW(uint8 err_type)
{
    if (err_type != 0x00)
    {
        ErrorIndicator = err_type;
        CSWBuffer._bCSWStatus = 0x01;
        CSWBuffer._dCSWDataResidue = CBW_data_buffer._dCBWDataTransferLength;
    }

    EndianReverse((uint8 *) (&CSWBuffer._dCSWDataResidue), 4);

    Send_CSW();
    bulk_out_back();

    CSWBuffer._bCSWStatus = 0x00;
    CSWBuffer._dCSWDataResidue = 0x00;
}

/*
 *********************************************************************************************************
 *                                           bulk_out_back
 *
 * Description: Epa interrupt function return.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void bulk_out_back(void)
{
    if ((sFlashLedFlag) != 0x00)
    {
        //是否要闪灯
        (*((void( *)()) (sFlashLed)))();
    }

    //收到ADFU detach命令后,不会立即断线,需要将之后的命令转换为ADFU命令,防止USB状态更新,刷图重启
    IsAdfuCondition = detach_cmd_rcv | IsAdfuCondition;
    if (IsAdfuCondition == 0x00)
    {
        //CMD_TestUnitReady_命令不能影响UpOrDownLoad，也即不能影响显示
        if (USBBusy != 0x00)
        {
            if (UpOrDownLoadBak != UpOrDownLoad)
            {
                //01:上传  02:下传
                UpOrDownLoadBak = UpOrDownLoad;
                //disable USB INT,做显示任务
                AIE = AIE & (uint8) 0xfe;
            }
        }
    }
    else
    {
        //disable USB INT
        AIE = AIE & (uint8) 0xfe;
    }
}

/*
 *********************************************************************************************************
 *                                           Fix_Nrmlmode_RWSctrs
 *
 * Description: 计算Udisk读写Flash需要用普通模式和Umode读写的扇区数,并计算后一次需要用普通模式读写扇区数.
 *
 * Arguments  : None.
 *
 * Returns    : a: 本次UDisk需用普通模式读写的扇区数.
 *
 * Note(s)    : 会更新总的Umode读写扇区数和结尾的普通模式读写扇区数,没有保护使用过的寄存器.
 *********************************************************************************************************
 */
static uint16 Fix_Nrmlmode_RWSctrs(void)
{
    uint16 tmp;
    uint16 tmp1;
    uint16 ret;
    uint8 pagesize;

    pagesize = (uint8) flash_page_sctrnum;
    //page内sector数为2的整次幂
    tmp = (uint16) LBA & (pagesize - 1);

    if (tmp != 0x00)
    {
        tmp = pagesize - tmp;
    }

    if (udisk_rw_sctr_num < tmp)
    {
        ret = udisk_rw_sctr_num;
        flash_normal_rw_sctrs = 0;
        udisk_rw_sctr_num = 0;
    }
    else
    {
        ret = tmp;
        tmp1 = udisk_rw_sctr_num - tmp;
        flash_normal_rw_sctrs = tmp1 & (pagesize - 1);
        udisk_rw_sctr_num = tmp1 - flash_normal_rw_sctrs;
    }
    return ret;
}

/*
 *********************************************************************************************************
 *                                           Ajust_Data_Length
 *
 * Description: Calculate the maximun data length in this data transfer.
 *
 * Arguments  : length: the total data length; largestLength: the largest packet length.
 *
 * Returns    :  the transfer data length.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static uint32 Ajust_Data_Length(uint32 length, uint32 largestLength)
{
    if (length >= largestLength)
    {
        return largestLength;
    }
    else
    {
        return length;
    }
}

/*
 *********************************************************************************************************
 *                                           Udisk_NrmlMode_Read
 *
 * Description: Udisk下普通模式读(因Flash只有Page对齐地址才能使用快速模式读写).
 *
 * Arguments  : e: Udisk非快速模式读扇区数.
 *
 * Returns    : None.
 *
 * Note(s)    : 只针对Flash作此Page对齐读动作,没有保护使用过的寄存器.
 *********************************************************************************************************
 */
static void Udisk_NrmlMode_Read(uint32 readnum)
{
    DMA_DT_Trnsfr_t dma_transfer;

    while (readnum != 0)
    {
        ClearWatchdog();
        sectors_trans_once = Ajust_Data_Length(readnum, MAX_NORMAL_RW_SETCORS);
        readnum = readnum - sectors_trans_once;

        sReadSectorsToBuff((uint8 *) DATABUFFER);

        dma_transfer.source_addr = DATABUFFER;
        dma_transfer.dst_addr = URAMSTARTADDR;
        dma_transfer.length = sectors_trans_once * 0x200;
        dma_transfer.pagenum = DRAM_TO_USBFIFO;
        DMA_Data_Transfer(&dma_transfer);
        Wait_PrevDMA_TransEnd();
    }
}

/*
 *********************************************************************************************************
 *                                           Udisk_NrmlMode_Write
 *
 * Description: Udisk下普通模式写(因Flash只有Page对齐地址才能使用快速模式读写).
 *
 * Arguments  : e: Udisk普通模式写扇区数.
 *
 * Returns    : None.
 *
 * Note(s)    : 只针对Flash作此Page对齐写动作,没有保护使用过的寄存器.
 *********************************************************************************************************
 */
static void Udisk_NrmlMode_Write(uint32 readnum)
{
    DMA_DT_Trnsfr_t dma_transfer;

    while (readnum != 0)
    {
        ClearWatchdog();
        sectors_trans_once = Ajust_Data_Length(readnum, MAX_NORMAL_RW_SETCORS);

        dma_transfer.source_addr = URAMSTARTADDR;
        dma_transfer.dst_addr = DATABUFFER;
        dma_transfer.length = sectors_trans_once * 0x200;
        dma_transfer.pagenum = USBFIFO_TO_DRAM;
        DMA_Data_Transfer(&dma_transfer);
        Wait_PrevDMA_TransEnd();

        sProgramSectorsToStorage((uint8 *) DATABUFFER);

        readnum = readnum - sectors_trans_once;
    }
}

/*
 *********************************************************************************************************
 *                                           Udisk_FIFOMode_Read
 *
 * Description: Udisk下FIFO直通模式读.
 *
 * Arguments  : readnum: Udisk读扇区数.
 *
 * Returns    : None.
 *
 * Note(s)    :
 *********************************************************************************************************
 */
static void Udisk_FIFOMode_Read(uint32 readnum)
{
    DMA_DT_Trnsfr_t dma_transfer;

    fifo_trans_mode = 0x01;

    while (readnum != 0)
    {
        ClearWatchdog();
        sectors_trans_once = Ajust_Data_Length(readnum, MAX_FIFO_RW_SETCORS);
        readnum = readnum - sectors_trans_once;

        dma_transfer.source_addr = 0;
        dma_transfer.dst_addr = 0;
        dma_transfer.length = (uint16) sectors_trans_once * 0x200;

        dma_transfer.pagenum = FLASHFIFO_TO_USBFIFO;
        if ((flashorcard_rwflag != 0) || (flash_type == 0x04))
        {
            dma_transfer.pagenum = CARDFIFO_TO_USBFIFO;
            DMA_Data_Transfer(&dma_transfer);
        }
        else
        {
            SFR_BANK = BANK_DMA_CARD;
            DMAnCTL1 = FLASHFIFO_TO_USBFIFO;
            SFR_BANK = BANK_USB;
        }

        sReadSectorsToBuff((uint8*) DATABUFFER);
        Wait_PrevDMA_TransEnd();

    }

    fifo_trans_mode = 0x00;
}

/*
 *********************************************************************************************************
 *                                           Udisk_FIFOMode_Write
 *
 * Description: Udisk下FIFO直通模式写(因Flash只有Page对齐地址才能使用快速模式读写).
 *
 * Arguments  : readnum: Udisk写扇区数.
 *
 * Returns    : None.
 *
 * Note(s)    :只针对Flash作此Page对齐写动作
 *********************************************************************************************************
 */
static void Udisk_FIFOMode_Write(uint32 readnum)
{
    DMA_DT_Trnsfr_t dma_transfer;

    fifo_trans_mode = 0x01;
    while (readnum != 0)
    {
        ClearWatchdog();
        sectors_trans_once = Ajust_Data_Length(readnum, MAX_FIFO_RW_SETCORS);

        dma_transfer.source_addr = 0;
        dma_transfer.dst_addr = 0;
        dma_transfer.length = (uint16) sectors_trans_once * 0x200;
        dma_transfer.pagenum = USBFIFO_TO_FLASHFIFO;
        if ((flashorcard_rwflag != 0) || (flash_type == 0x04))
        {
            dma_transfer.pagenum = USBFIFO_TO_CARDFIFO;
            DMA_Data_Transfer(&dma_transfer);
        }
        else
        {
            SFR_BANK = BANK_DMA_CARD;
            DMAnCTL1 = USBFIFO_TO_FLASHFIFO;
            SFR_BANK = BANK_USB;
        }
        sProgramSectorsToStorage((uint8*) DATABUFFER);
        Wait_PrevDMA_TransEnd();

        readnum = readnum - sectors_trans_once;
    }
    fifo_trans_mode = 0x00;
}
#pragma ROM(HUGE)
