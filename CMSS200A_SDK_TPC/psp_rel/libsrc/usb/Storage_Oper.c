/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : storage_Oper.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-16 18:37
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(STORAGE_C)
uint8 Is_ADFURW_illegal(void);
uint8 Is_BulkRW_illegal(void);
void Udisk_Mbr_Check(void);
uint8 Check_Dbr_Data(uint8 *DBRAddr);

#pragma renamecode(?PR?STORAGE_C)
/*
 *********************************************************************************************************
 *                                           sFlash_Lgcl_Wrts
 *
 * Description: 写Flash逻辑扇区.
 *
 * Arguments  : ix: data Address; hlde: 起始扇区;
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFlash_Wrts(uint8 rw_type, uint32 lba_addr, uint8 *dst_buffer)
{
    uint8 tmp;
    if (rw_type == RW_DYNAMIC)
    {
        tmp = (uint8) sectors_trans_once;
    }
    else
    {
        //判断是否是SPINor
        if (flash_type == 0x02)
        {
            return;
        }
        tmp = 1;
    }

    SD_UDRWStruct.lba = lba_addr;
    SD_UDRWStruct.sectornum = tmp;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;

    if (illegalLBAFlag == 0x00)
    {
        UD_SectorWrite((UD_RW *) &SD_UDRWStruct);
    }
}

/*
 *********************************************************************************************************
 *                                           sFlash_Lgcl_Rds
 *
 * Description: 读Flash.
 *
 * Arguments  : ix: data Address; hlde: 起始扇区;
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFlash_Rds(uint8 rw_type, uint32 lba_addr, uint8 *dst_buffer)
{
    uint8 tmp;
    if (rw_type == RW_DYNAMIC)
    {
        tmp = (uint8) sectors_trans_once;
    }
    else
    {
        //判断是否是SPINor
        if (flash_type == 0x02)
        {
            return;
        }
        tmp = 1;
    }

    SD_UDRWStruct.lba = lba_addr;
    SD_UDRWStruct.sectornum = tmp;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;

    if (illegalLBAFlag == 0x00)
    {
        UD_SectorRead((UD_RW *) &SD_UDRWStruct);
    }
}

/*
 *********************************************************************************************************
 *                                           sCardWriteDynamic
 *
 * Description: 写若干扇区数据到SDCard中.
 *
 * Arguments  : ix: data Address;
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sCardWrite(uint8 *dst_buffer)
{
    SD_UDRWStruct.lba = LBA;
    SD_UDRWStruct.sectornum = (uint8) sectors_trans_once;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;
    //返回0: 正确， 1: 不正确，
    RWCardError = (uint8) UD_SDCardSectorWrite((UD_RW *) &SD_UDRWStruct);
}

/*
 *********************************************************************************************************
 *                                           sCardRead
 *
 * Description: 读若干扇区SDCard数据到指定Uram地址.
 *
 * Arguments  : ix: data Address.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sCardRead(uint8 *dst_buffer)
{
    SD_UDRWStruct.lba = LBA;
    SD_UDRWStruct.sectornum = (uint8) sectors_trans_once;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;
    //返回0: 正确， 1: 不正确，
    RWCardError = (uint8) UD_SDCardSectorRead((UD_RW *) &SD_UDRWStruct);
    if (RWCardError != 0x00)
    {
        CardExistFlag = 0;
    }
}

/*
 *********************************************************************************************************
 *                                           sFlashDrvCheck
 *
 * Description: 是否要切换Driver.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFlashDrvCheck(void)
{
}

/*
 *********************************************************************************************************
 *                                           sProgramSectorsToStorage
 *
 * Description: 写BlockNumInUram扇区数据到Flash.
 *
 * Arguments  : ix: 数据源地址.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sProgramSectorsToStorage(uint8 *dst_buffer)
{
    //Flash或卡读写标志: 0,Flash; 1:Card
    if (flashorcard_rwflag != 0)
    {
        //读Flash数据到Buffer不同位置,输入参数ix
        sCardWrite(dst_buffer);
    }
    else
    {
        sFlash_Wrts(RW_DYNAMIC, LBA, dst_buffer);
    }
    //LBA地址累加
    LBA = LBA + (uint32) sectors_trans_once;
}

/*
 *********************************************************************************************************
 *                                           sReadSectorsToBuff
 *
 * Description: 把数据从Flash读到Buffer.
 *
 * Arguments  : ix: 数据目的Ram地址.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sReadSectorsToBuff(uint8 *dst_buffer)
{
    //Flash或卡读写标志: 0,Flash; 1:Card
    if (flashorcard_rwflag != 0)
    {
        //读Flash数据到Buffer不同位置
        sCardRead(dst_buffer);
    }
    else
    {
        sFlash_Rds(RW_DYNAMIC, LBA, dst_buffer);
    }
    //LBA地址累加
    LBA = LBA + (uint32) sectors_trans_once;
}

/*
 *********************************************************************************************************
 *                                           Compute_LBA
 *
 * Description: 计算Flash/Card读写地址和数据长度.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Compute_LBA(void)
{
    //Flash或卡读写标志: 0,Flash; 1:Card
    flashorcard_rwflag = 0x00;

    //读写Flash1扇区标志,用于加密盘
    AccessFlashPhySec1 = 0x00;
    UdiskFSInfo.rw_dbr_flag = 0x00;

    if (IsAdfuCondition != 0x00)
    {
        EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[2]), 4);
        LBA = *((uint32 *) &CBW_data_buffer._CBWCB[2]);
        EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[7]), 2);
        udisk_rw_sctr_num = *((uint16 *) &CBW_data_buffer._CBWCB[7]);
        Is_ADFURW_illegal();
    }
    else
    {
        udisk_rw_sctr_num = *((uint16 *) &CBW_data_buffer._CBWCB[7]);
        LBA = *((uint32 *) &CBW_data_buffer._CBWCB[2]);
        //设置假容量后不再支持加密分区功能
        if ((set_fake_ucap_flag == 0x00) && (LBA == 0x00000001))
        {
            AccessFlashPhySec1 = 0x01;
        }

        if (CBW_data_buffer._bCBWLUN == 0x00)
        {
            //CDROM不响应软件的Physec0,1读
            AccessFlashPhySec1 = 0x00;
            //得到CDROM首扇区在Flash的实际扇区地址
            LBA = LBA * 4 + AutoRunDiskStartAddr;
            udisk_rw_sctr_num = udisk_rw_sctr_num * 4;
        }
        else if (CBW_data_buffer._bCBWLUN == 0x01)
        {
            //将地址和长度还原为0x200byte单位
            LBA = LBA * SectorType;
            udisk_rw_sctr_num = udisk_rw_sctr_num * SectorType;

            if (flash_type != 4)//卡base不做8k对齐
            {
                //check if is writing the DBR
                if (LBA != UdiskFSInfo.DBRStartAddress)
                {
                    //check if address is >= DataAreaStartAddress.
                    if (LBA >= UdiskFSInfo.DataAreaStartAddress)
                    {
                        LBA = LBA + UdiskFSInfo.DataAreaOffset;
                    }
                }
                else
                {
                    UdiskFSInfo.rw_dbr_flag = 0x01;
                }
            }

            //判断对Flash读写是否超过Flash容量
            Is_BulkRW_illegal();

            //得到MBR扇区在Flash的实际扇区地址
            LBA += UdiskStartAddr;
        }
        else if (CBW_data_buffer._bCBWLUN == 0x02)
        {
            if ((EncryptFlag == 0x00) || (PasswdPassOk != 0x00))
            {
                LBA += EncryptDiskBStartAddr;

            }
            else
            {
                LBA += EncryptDiskCStartAddr;
            }
        }
        else if (CBW_data_buffer._bCBWLUN == 0x03)
        {
            //卡不响应工具软件的Physec1读写
            AccessFlashPhySec1 = 0x00;
            //Flash或卡读写标志: 0,Flash; 1:Card
            flashorcard_rwflag = 0x01;
        }
        else
        {
        }
    }
}

/*
 *********************************************************************************************************
 *                                           Is_ADFURW_illegal
 *
 * Description: 除了对Flash SD区DIR和DRMINFO的读合法,其它读均非法,防范LFI被读出.
 *
 * Arguments  : None.
 *
 * Returns    : 地址是否合法: 0: legal;  1:illegal.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 Is_ADFURW_illegal(void)
{
    uint8 ret = 1;

    if ((CBW_data_buffer._CBWCB[1] & 0x80) != 0x00)
    {
        if (udisk_rw_sctr_num <= 0x20)
        {
            if (LBA == 0) //读Flash SD区的DIR信息
            {
                ret = 0;
            }
            //else if((LBA>=DRMINFOADDR) && (LBA <(SDDiskCap/2))) //读DRMINFO,不能读超过SD区内容
            else if (LBA < (SDDiskCap / 2)) //读DRMINFO,不能读超过SD区内容
            {
                ret = 0;
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            //最大只能每次读0x20个扇区的SD区数据,否则认为非法
            ret = 1;
        }
    }
    else
    {
        //写Flash物理区总认为是合法
        ret = 0;
    }

    illegalLBAFlag = ret;

    return ret;
}

/*
 *********************************************************************************************************
 *                                           Is_BulkRW_illegal
 *
 * Description: 判断对Flash读写是否超过Flash容量,防范LFI被读出.
 *
 * Arguments  : None.
 *
 * Returns    : 地址是否合法，0: legal;  1:illegal.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 Is_BulkRW_illegal(void)
{
    if ((LBA + udisk_rw_sctr_num) < udisk_total_capacity)
    {
        illegalLBAFlag = 0x00;
    }
    else
    {
        //LBA地址越界
        illegalLBAFlag = 0x01;
    }

    return illegalLBAFlag;
}
