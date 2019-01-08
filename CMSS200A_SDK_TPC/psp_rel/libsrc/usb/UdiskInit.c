/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : Udiskinit.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-20 08:09
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(UDISK_INIT)
void sFlashCapCheck(void);
void sPartitionTableCheck(void);
void sSetLUN(void);
void sFlashBootSectorCheck(void);
void Vram_Init(void);
void Vram_Update(void);
void sFillDiskACap(uint32 udiskcap);

#pragma renamecode(?PR?UDISK_INIT)

/*
 *********************************************************************************************************
 *                                           UDiskInit
 *
 * Description: Udisk init.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskInit(void *pcallback, int8 udisk_set)
{
    uint8 sfrbak;

    if (pcallback != 0)
    {
        sFlashLedFlag = 1; //置要闪灯标志
        sFlashLed = (uint8 *) pcallback; //保存地址到Rcode
    }

    udisk_setting = (uint8) udisk_set & 0x03;
    if (udisk_setting == 0)
    {
        HS_Device_Dscrptr.iSerialNumber = 0;
    }

    GetUsbVidPid((vidpid_t *) &HS_Device_Dscrptr.idVendor);
    GetUsbAttribute((usbattri_t *) (&(CMD_Inquiry_information_FOB + 8)));

#ifdef USB_HOST_SUPPORT
    //    usb_detectfunc_dereg(); //注销Timer中断中USB查询处理入口
#endif

    /*fixme:need系统读出不对
     Vram_Init();                    //清除Vram中Flash和卡簇链有效标记,并在退出后根据是否有写Flash或卡重新置上标志位
     */

    //装载usb的库code
    sfrbak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    //    MemBankCtl = MemBankCtl & (uint8)0xf7; //PCMRAM and MURAM2 to code FF9000H~FFBFFFH
    //    MCSR5 = (MCSR5 & (uint8)0xf9) | 0x04; //PCMRAM:MCU clk
    //    MCSR6 = (MCSR6 & (uint8)0xf3) | 0x08; //MURAM2:MCU clk
    MEMCLKSELCTL1 = 0x00; //PCMRAM and URAM to MCU clk

    return UDiskInitInternal();
}

/*
 *********************************************************************************************************
 *                                           UDiskInitInternal
 *
 * Description: Udisk init.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskInitInternal(void)
{
    ClearWatchdog();

    USB_State = 0; //状态结构定义   0:空闲状态 再调用时要初始化
    illegalLBAFlag = 0; //LBA是否非法,用于防范发命令进行非法读
    //返回值 非MLC flash:0  MLC flash:1; 2:SPI Nor flash; 3:LBA flash; 4:card base; 5:TLC flash; 6:Toshiba 24nm flash
    flash_type = (uint8) UD_GetFlashType();

    //SectorType flash扇区大小,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;
    //UD_GetSctrSize 返回值:0=512bytes/sector; 1=1024bytes/sector; 2=2048bytes/sector;
    if ((flash_type != 2) && (flash_type != 4))
    {
        SectorType = (1 << ((uint8) UD_GetSctrSize()));
    }

    //SectorType = 0x02; //flash扇区大小,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;
    if (flash_type == 4)//对于卡base不做8k对齐，那么需要恢复每个扇区的大小以及最后一个扇区的地址
    {
        SectorType = 0x01;
    }

    if (fake_udisk_cap != 0x00)
    {
        //设置Udisk上报给PC的容量,sFillDiskACap的参数以512byte为单位,而fake_udisk_cap以M为单位
        //note:设置A盘容量并置假容量标记,此接口必须在UDiskInit之前调用,否则加密盘会引起问题.
        sFillDiskACap((uint32) fake_udisk_cap << 11);
        set_fake_ucap_flag = 0x01;
    }

    sFlashCapCheck(); //Flash和卡容量检查，用于上报PC

    flash_page_sctrnum = 0x0002; // need fix ???  //Get_Flash_Page_Size();

    //会装上卡驱动
    sCardCapCheck();

    if (flash_type != 2)
    {
        sPartitionTableCheck(); //分区表检查，确认是否在加密状态
        sFlashBootSectorCheck(); //引导扇区检查,若不具备磁盘结构便破坏使其为未格式化
        if (flash_type != 0x04)
        {
            Partion_MBR_Initial(); //检查FAT MBR/DBR,并计算出DBR偏移,数据区起始地址
        }
    }

    sSetLUN(); //检查用户设置的磁盘数目,设置正确的LUN

    ClearWatchdog();

    return 1;
}

/*
 *********************************************************************************************************
 *                                           sFlashCapCheck
 *
 * Description: Flash Capacity Check
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    :
 *            预留40h个扇区不用,是为了苹果系统格式化
 *            苹果格式化后在XP格式化第一次不成功,第二次才成功
 *            发现问题是:在MAC下格式化小机时,会写MBR至设备零扇区,在MBR的分区表里写入DBR的起始扇区
 *            为0X3F扇区(1磁头0柱面1扇区)开始,并将设备报上来的容量认为是从DBR开始可用的扇区数,写至MBR第一个分区表项里.
 *            设备再插入XP系统进行格式化时,XP系统首先会根据MBR中的内容去计算得到最后一个扇区的LBA(0x3F + 设备容量),并
 *            写第0个扇区和最后一个扇区,然而XP根据MBR计算得到的最后一个扇区的LBA超出了Flash的容量,导致读写设备最后一个
 *            扇区不一致,进而报格式化失败错误.并致使驱动写坏了Flash0扇区开始的一些内容,所以第二次格式化会成功.
 *            解决方法：在设备识别出是MAC系统时，报设备容量为实际容量 - 0x40
 *********************************************************************************************************
 */
static void sFlashCapCheck(void)
{
    uint32 tmp;

    SDDiskCap = (uint32) GetSDCap();
    VMCap = (uint32) GetVMCap();
    //    MICap = (uint32)GetMICap();
    UICap = (uint32) GetUICap();

    if (flash_type != 2)
    {
        EncryptInfoSector = SDDiskCap + VMCap + MICap + UICap - 1;

        tmp = (uint32) GetHideDiskCap(); //返回隐藏盘的容量(扇区为单位)
        AutoRunDiskStartAddr = SDDiskCap + VMCap + MICap + UICap + tmp;

        AutoRunDiskCapacity = (uint32) GetAutoRunDiskCap(); //返回AutoRun光盘的容量(扇区为单位)
        if (AutoRunDiskCapacity == 0)
        {
            AutoRunDiskFlag = 0; //AutoRun光盘存在标志: 0:不存在; 非0:存在
        }
        else
        {
            AutoRunDiskFlag = 0xff;
        }

        UdiskStartAddr = (AutoRunDiskStartAddr + AutoRunDiskCapacity);
        //U盘的起始扇区号,使U盘起始扇区模16为零(因Flash驱动读写以16sector为单位,且数据要对齐)
        if ((UdiskStartAddr & 0x0000000f) != 0x00)
        {
            UdiskStartAddr = (UdiskStartAddr & (uint32) 0xfffffff0) + 0x10;
        }

        Cap_cmd_info_DiskCDROM.LastLogBlockAddr = AutoRunDiskCapacity >> 2;

        sFlashDrvCheck(); //切换Flash Driver

        tmp = (uint32) UD_GetCap(); //以1MByte为单位
        flash_capacity = tmp << 3; //以128K为单位

        //保存U盘总可用扇区总数,并预留0x40个扇区不用.
        udisk_total_capacity = (tmp << 11) - UdiskStartAddr - 0x100;

        if (set_fake_ucap_flag == 0x00)
        {
            sFillDiskACap(udisk_total_capacity);
        }

        //U盘读写时会对地址做对齐动作,读写地址加偏移DataAreaOffset,而磁盘总容量也需同步,防止计算是否越界时错误
        udisk_total_capacity = udisk_total_capacity + 0x100;
    }
}

/*
 *********************************************************************************************************
 *                                           sPartitionTableCheck
 *
 * Description: Partition Table Check, 确认是否在加密状态.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void sPartitionTableCheck(void)
{
    encrypt_info *encrypt_info_p;
    uint32 tmp;

    //初始化分区标志
    SplitDiskFlag = 0;
    EncryptFlag = 0;
    PasswdPassOk = 0;

    //设置假容量后不再支持加密分区功能
    if (set_fake_ucap_flag == 0x00)
    {
        encrypt_info_p = (encrypt_info *) DATABUFFER;
        //读Flash 1扇区数据到Uram中UramRWStartAdd起始的地方去
        sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);

        //是否有分区
        if (encrypt_info_p->SplitDiskRecord == 0x01)
        {
            sFlash_Rds(RW_FIX, UdiskStartAddr, DATABUFFER);

            //是否Actions分区表, 0:是  起始值：不是; 非Actions MBR，即非加密
            if (memcmp(MBR_Bak, DATABUFFER, 0xe0) == 0)
            {
                //分区有效，置标志位
                SplitDiskFlag = 1;

                sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
                //是否有密码
                EncryptFlag = encrypt_info_p->IfEncryptRecord;

                //恢复A盘容量
                tmp = encrypt_info_p->DiskACapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                tmp = tmp / SectorType;
                Cap_cmd_info_DiskA.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskA.CurrentCap = tmp;
                Formatcap_cmd_info_DiskA.FomatableCap = tmp;

                //算出DiskB在Flash的真正起始地址
                tmp = encrypt_info_p->DiskBStartAddrRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                EncryptDiskBStartAddr = tmp + UdiskStartAddr;
                //恢复B盘容量
                tmp = encrypt_info_p->DiskBCapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                Cap_cmd_info_DiskB.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskB.CurrentCap = tmp;
                Formatcap_cmd_info_DiskB.FomatableCap = tmp;

                //算出DiskC在Flash的真正起始地址
                tmp = encrypt_info_p->DiskCStartAddrRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                EncryptDiskCStartAddr = tmp + UdiskStartAddr;
                //恢复C盘容量
                tmp = encrypt_info_p->DiskCCapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                Cap_cmd_info_DiskC.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskC.CurrentCap = tmp;
                Formatcap_cmd_info_DiskC.FomatableCap = tmp;

                if (encrypt_info_p->DataSectorsForEncOkRecord != 0xddcc)
                {
                    encrypt_info_p->DataSectorsForEncOkRecord = 0xddcc;
                    //将读至Uram Buffer中1扇区数据再写至对应此数据的Flash中
                    sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
                }
            }
            else
            {
                memset(DATABUFFER, 0, 0x200);
                //将读至Uram Buffer中1扇区数据再写至对应此数据的Flash中
                sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            }
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sSetLUN
 *
 * Description: 检查用户设置的磁盘数目,Set LUN Number.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void sSetLUN(void)
{
    uint8 cLun;

    cLun = 0;

    if (DiskAppearSelect == 0) //用户的盘符选择  0:多盘符(默认)  01h:普通盘  02h:加密盘  03h:插卡
    {
        cLun = SplitDiskFlag;
        if (ShowMultiDiskFlag == 0) //是否需要显示卡盘符
        {
            cLun += CardExistFlag;
        }
        else
        {
            cLun++; //无论是否插卡，都要显示盘符，因此LUN至少为1
        }
    }

    //for autorun
    if (AutoRunDiskFlag != 0x00) //AutoRun光盘存在标志: 0:不存在; 非0:存在
    {
        cLun = cLun + 1;
    }

    MaxLUN = cLun;
}

/*
 *********************************************************************************************************
 *                                           sFlashBootSectorCheck
 *
 * Description: Flash Udisk引导扇区检查,若不具备磁盘结构便破坏使其为未格式化.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void sFlashBootSectorCheck(void)
{
    char *pcDataBuffer;

    //清物理0扇区与逻辑0扇区是否重合变量
    Phy0EquLog0Flag = 0;
    //DATABUFFER为sFlash_Lgcl_RdFix函数读到Uram B2中的起始地址
    pcDataBuffer = DATABUFFER;
    //读MBR
    sFlash_Rds(RW_FIX, UdiskStartAddr, DATABUFFER);

    if ((*pcDataBuffer == 0xeb) && (*(pcDataBuffer + 2) == 0x90))
    {
        //磁盘正确,物理0扇区与逻辑0扇区重合
        Phy0EquLog0Flag = 1;
    }
    //是否Actions分区表, 0:是  非0:不是; 非Actions MBR，即非加密
    else if (memcmp(MBR_Bak, UDISKSTARTADDRURAMADD, 0xe0) != 0)
    {
        //磁盘未格式化,物理0扇区与逻辑0扇区重合
        Phy0EquLog0Flag = 0x01;
        if ((*(pcDataBuffer + 510) != 0x55) || (*(pcDataBuffer + 511) != 0xaa))
        {
            //读fat表
            sFlash_Rds(RW_FIX, UdiskStartAddr + 1, DATABUFFER);
            if ((*pcDataBuffer != 0xff) || (*(pcDataBuffer + 1) != 0xf8))
            {
                memset(DATABUFFER, 0, 0x200);
                //破坏Boot区,使其未格式化
                sFlash_Wrts(RW_FIX, UdiskStartAddr, DATABUFFER);
                sFlash_Wrts(RW_FIX, UdiskStartAddr + 1, DATABUFFER);
            }
        }
    }
    else
    {
    }
}

/*
 *********************************************************************************************************
 *                                           UDiskSetDiskNum
 *
 * Description: 设置磁盘出现数目,要在调用UdiskInit前调用 0:多盘符(默认)  01h:普通盘  02h:加密盘  03h:插卡.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UDiskSetDiskNum(int8 selector)
{
    DiskAppearSelect = 0;
    if (selector < 4)
    {
        DiskAppearSelect = (uint8) selector;
    }
}

/*
 *********************************************************************************************************
 *                                           UDiskSetWP
 *
 * Description: 设置磁盘写保护,应在UDiskRun前调用.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskSetWP(int8 letter)
{
    uint8 ret;

    ret = 0;
    if (letter == 'C')
    {
        FlashWPFlag = 0x01;
        ret = 1;
    }
    if (letter == 'H')
    {
        CardWPFlag = 0x01;
        ret = 1;
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           Vram_Init
 *
 * Description: 清除Vram中Flash和卡簇链有效标记,并在退出后根据是否有写Flash或卡重新置上标志位.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
/*
 void Vram_Init(void)
 {
 sFlash_Rds(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 flash_vram_vld =  *((uint8*)DATABUFFER+0x04);
 *((uint8*)DATABUFFER+0x04) = 0xff;                //Flash簇链有效标记
 *((uint8*)DATABUFFER+0x0C) = 0xff;                //卡簇链有效标记
 sFlash_Wrts(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 }
 */

/*
 *********************************************************************************************************
 *                                           Udisk_Exit_Handle
 *
 * Description: Udisk正常退出时处理入口,如Vram标记回写,USB消息处理Timer中断重新注册.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Udisk_Exit_Handle(void)
{
    /*fixme:need系统读出不对
     Vram_Update();
     */
#ifdef USB_HOST_SUPPORT
    //    USB_DetectFnc_Initial();
#endif
}

/*
 *********************************************************************************************************
 *                                           Vram_Update
 *
 * Description: 清除Vram中Flash和卡簇链有效标记,并在退出后根据是否有写Flash或卡重新置上标志位.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
/*
 void Vram_Update(void)
 {
 //发生过Udisk写动作,因Vram标记在进入时已经被请掉不需要恢复;没有写动作,但是其Vram标记本来就没有,也不需要恢复
 if ((flash_disk_write_flag==0x00) && (flash_vram_vld!=0x00))
 {
 sFlash_Rds(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 //Flash簇链有效标记
 *((uint8*)DATABUFFER+0x04) = flash_vram_vld;
 sFlash_Wrts(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 }
 }
 */

/*
 *********************************************************************************************************
 *                                           sFillDiskACap
 *
 * Description: Fill DiskA Capacity.
 *
 * Arguments  : udiskcap:DiskA Capacity,512byte单位
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFillDiskACap(uint32 udiskcap)
{
    //初始化blocklength
    Cap_cmd_info_DiskA.BlockLengthInBytes = 0x200;

    Cap_cmd_info_DiskA.BlockLengthInBytes = Cap_cmd_info_DiskA.BlockLengthInBytes * SectorType;
    Cap_cmd_info_DiskA.LastLogBlockAddr = udiskcap / SectorType - 1;
    Formatcap_cmd_info_DiskA.CurrentCapBlockLength[0] = 0x00;
    Formatcap_cmd_info_DiskA.CurrentCapBlockLength[1] = 0x02 * SectorType;
    Formatcap_cmd_info_DiskA.CurrentCapBlockLength[2] = 0x00;
    Formatcap_cmd_info_DiskA.FomatableCapBlockLength[0] = 0x00;
    Formatcap_cmd_info_DiskA.FomatableCapBlockLength[1] = 0x02 * SectorType;
    Formatcap_cmd_info_DiskA.FomatableCapBlockLength[2] = 0x00;
    Formatcap_cmd_info_DiskA.CurrentCap = udiskcap / SectorType;
    Formatcap_cmd_info_DiskA.FomatableCap = udiskcap / SectorType;
}
