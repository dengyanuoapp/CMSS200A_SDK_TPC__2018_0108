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
 * Description: дFlash�߼�����.
 *
 * Arguments  : ix: data Address; hlde: ��ʼ����;
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
        //�ж��Ƿ���SPINor
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
 * Description: ��Flash.
 *
 * Arguments  : ix: data Address; hlde: ��ʼ����;
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
        //�ж��Ƿ���SPINor
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
 * Description: д�����������ݵ�SDCard��.
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
    //����0: ��ȷ�� 1: ����ȷ��
    RWCardError = (uint8) UD_SDCardSectorWrite((UD_RW *) &SD_UDRWStruct);
}

/*
 *********************************************************************************************************
 *                                           sCardRead
 *
 * Description: ����������SDCard���ݵ�ָ��Uram��ַ.
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
    //����0: ��ȷ�� 1: ����ȷ��
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
 * Description: �Ƿ�Ҫ�л�Driver.
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
 * Description: дBlockNumInUram�������ݵ�Flash.
 *
 * Arguments  : ix: ����Դ��ַ.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sProgramSectorsToStorage(uint8 *dst_buffer)
{
    //Flash�򿨶�д��־: 0,Flash; 1:Card
    if (flashorcard_rwflag != 0)
    {
        //��Flash���ݵ�Buffer��ͬλ��,�������ix
        sCardWrite(dst_buffer);
    }
    else
    {
        sFlash_Wrts(RW_DYNAMIC, LBA, dst_buffer);
    }
    //LBA��ַ�ۼ�
    LBA = LBA + (uint32) sectors_trans_once;
}

/*
 *********************************************************************************************************
 *                                           sReadSectorsToBuff
 *
 * Description: �����ݴ�Flash����Buffer.
 *
 * Arguments  : ix: ����Ŀ��Ram��ַ.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sReadSectorsToBuff(uint8 *dst_buffer)
{
    //Flash�򿨶�д��־: 0,Flash; 1:Card
    if (flashorcard_rwflag != 0)
    {
        //��Flash���ݵ�Buffer��ͬλ��
        sCardRead(dst_buffer);
    }
    else
    {
        sFlash_Rds(RW_DYNAMIC, LBA, dst_buffer);
    }
    //LBA��ַ�ۼ�
    LBA = LBA + (uint32) sectors_trans_once;
}

/*
 *********************************************************************************************************
 *                                           Compute_LBA
 *
 * Description: ����Flash/Card��д��ַ�����ݳ���.
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
    //Flash�򿨶�д��־: 0,Flash; 1:Card
    flashorcard_rwflag = 0x00;

    //��дFlash1������־,���ڼ�����
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
        //���ü���������֧�ּ��ܷ�������
        if ((set_fake_ucap_flag == 0x00) && (LBA == 0x00000001))
        {
            AccessFlashPhySec1 = 0x01;
        }

        if (CBW_data_buffer._bCBWLUN == 0x00)
        {
            //CDROM����Ӧ�����Physec0,1��
            AccessFlashPhySec1 = 0x00;
            //�õ�CDROM��������Flash��ʵ��������ַ
            LBA = LBA * 4 + AutoRunDiskStartAddr;
            udisk_rw_sctr_num = udisk_rw_sctr_num * 4;
        }
        else if (CBW_data_buffer._bCBWLUN == 0x01)
        {
            //����ַ�ͳ��Ȼ�ԭΪ0x200byte��λ
            LBA = LBA * SectorType;
            udisk_rw_sctr_num = udisk_rw_sctr_num * SectorType;

            if (flash_type != 4)//��base����8k����
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

            //�ж϶�Flash��д�Ƿ񳬹�Flash����
            Is_BulkRW_illegal();

            //�õ�MBR������Flash��ʵ��������ַ
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
            //������Ӧ���������Physec1��д
            AccessFlashPhySec1 = 0x00;
            //Flash�򿨶�д��־: 0,Flash; 1:Card
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
 * Description: ���˶�Flash SD��DIR��DRMINFO�Ķ��Ϸ�,���������Ƿ�,����LFI������.
 *
 * Arguments  : None.
 *
 * Returns    : ��ַ�Ƿ�Ϸ�: 0: legal;  1:illegal.
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
            if (LBA == 0) //��Flash SD����DIR��Ϣ
            {
                ret = 0;
            }
            //else if((LBA>=DRMINFOADDR) && (LBA <(SDDiskCap/2))) //��DRMINFO,���ܶ�����SD������
            else if (LBA < (SDDiskCap / 2)) //��DRMINFO,���ܶ�����SD������
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
            //���ֻ��ÿ�ζ�0x20��������SD������,������Ϊ�Ƿ�
            ret = 1;
        }
    }
    else
    {
        //дFlash����������Ϊ�ǺϷ�
        ret = 0;
    }

    illegalLBAFlag = ret;

    return ret;
}

/*
 *********************************************************************************************************
 *                                           Is_BulkRW_illegal
 *
 * Description: �ж϶�Flash��д�Ƿ񳬹�Flash����,����LFI������.
 *
 * Arguments  : None.
 *
 * Returns    : ��ַ�Ƿ�Ϸ���0: legal;  1:illegal.
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
        //LBA��ַԽ��
        illegalLBAFlag = 0x01;
    }

    return illegalLBAFlag;
}
