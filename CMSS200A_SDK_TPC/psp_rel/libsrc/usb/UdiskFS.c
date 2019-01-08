/*
 *********************************************************************************************************
 *                                       ACTOS GL5102
 *
 *
 *                           Copyright(c) 2001-2010, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UdiskFS.c
 * By     : wanghaijing
 * Version: v1.00     first version     2010/06/17
 * brief  : ������������ʼ��ַ,�Զ����������ж���
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(UDISK_FS)
void Udisk_Mbr_Check(void);
uint8 Check_Dbr_Data(uint8 *DBRAddr);

#pragma renamecode(?PR?UDISK_FS)
/*
 *********************************************************************************************************
 *                                           Partion_MBR_Initial
 *
 * Description: ��ʼ��MBR��DBR��ر���,������Ƿ���MBR, DBR,�������ҵ�������(��������Ŀ¼�״�)��ʼ��ַ.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Partion_MBR_Initial(void)
{
    UdiskFSInfo.DBRStartAddress = 0x00;
    UdiskFSInfo.DataAreaStartAddress = 0xffffffff;
    UdiskFSInfo.DataAreaOffset = 0x00;
    UdiskFSInfo.rw_dbr_flag = 0x00;

    Udisk_Mbr_Check();
}

/*
 *********************************************************************************************************
 *                                           Udisk_Parse_MBR
 *
 * Description: ����Ƿ���дDBR,�������½���DBR���ݲ�����������ƫ�ƺͶ���.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Udisk_Parse_MBR(void)
{
    if (UdiskFSInfo.rw_dbr_flag != 0x00)
    {
        Udisk_Mbr_Check();
        UdiskFSInfo.rw_dbr_flag = 0x00;
    }
}

/*
 *********************************************************************************************************
 *                                           Udisk_Mbr_Check
 *
 * Description: ����Ƿ���MBR,�������ļ�ϵͳ,��ʹ������8Kbyte����.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Udisk_Mbr_Check(void)
{
    sFlash_Rds(RW_FIX, UdiskStartAddr, DATABUFFER); //Read first sector of Udisk

    /* check if MBR.*/
    if (*(uint16 *) (DATABUFFER + 0x1fe) == 0x55aa)
    {
        if ((*(uint8 *) (DATABUFFER + 0x1be) == 0x80) || (*(uint8 *) (DATABUFFER + 0x1be) == 0x00))
        {
            //byte per sector is 1024, convert it to 512
            UdiskFSInfo.DBRStartAddress = *((uint32 *) ((uint8 *) DATABUFFER + 0x1be + 8));
            EndianReverse((uint8 *) & (UdiskFSInfo.DBRStartAddress), (uint16) (sizeof(UdiskFSInfo.DBRStartAddress)));
            UdiskFSInfo.DBRStartAddress = (UdiskFSInfo.DBRStartAddress) * SectorType;

            //check if MBR is valid, partition offset is not 0
            if (UdiskFSInfo.DBRStartAddress != 0x00)
            {
                //read DBR according to partition table
                sFlash_Rds(RW_FIX, (UdiskStartAddr + UdiskFSInfo.DBRStartAddress), DATABUFFER);
            }
        }

        //check if DBR is valid
        Check_Dbr_Data(DATABUFFER);
    }
    else
    {
        UdiskFSInfo.DataAreaStartAddress = 0xffffffff;
        UdiskFSInfo.DataAreaOffset = 0x00;
    }
}

/*
 *********************************************************************************************************
 *                                           Check_Dbr_Data
 *
 * Description: ���DBR�Ƿ�Ϸ�,��Ϸ�������������Ŀ¼�״غ�������ʼ��ַ,�Լ��������ʱ��Ҫ�����������.
 *
 * Arguments  : DBRAddr:DBR������ʼ��ַ.
 *
 * Returns    : DBR���ݺϷ���; 0: illegal; 1:legal.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 Check_Dbr_Data(uint8 *DBRAddr)
{
    uint8 ret = 1;
    /* check if DBR is valid.    */
    if ((*((uint16 *) (DBRAddr + 0x1fe)) == 0x55aa) && ((*((uint8 *) DBRAddr) == 0xeb) || (*((uint8 *) DBRAddr) == 0xe9)))
    {
        if ((*((uint32 *) (DBRAddr + 3)) == 0x45584641) && (*((uint8 *) (DBRAddr + 7)) == 0x54))
        {
            return ret;
        }

        /* parse dbr.    */
        UdiskFSInfo.SectorPerCluster = *((uint8 *) (DBRAddr + 13));

        UdiskFSInfo.ReservedSecotr = *((uint16 *) (DBRAddr + 14));
        EndianReverse((uint8 *) (&(UdiskFSInfo.ReservedSecotr)), (uint16) (sizeof(UdiskFSInfo.ReservedSecotr)));

        /* if root directory entry number is 0, it is FAT32, else FAT16.    */
        UdiskFSInfo.BPB_RootEntCnt = *((uint16 *) (DBRAddr + 17));
        EndianReverse((uint8 *) (&(UdiskFSInfo.BPB_RootEntCnt)), sizeof(UdiskFSInfo.BPB_RootEntCnt));

        UdiskFSInfo.FATSize = (uint32) ((((uint16) (*((uint8 *) (DBRAddr + 23)))) << 8) + * ((uint8 *) (DBRAddr + 22)));
        if (UdiskFSInfo.FATSize == 0x00)
        {
            UdiskFSInfo.FATSize = *((uint32 *) (DBRAddr + 36));
            EndianReverse((uint8 *) (&(UdiskFSInfo.FATSize)), sizeof(UdiskFSInfo.FATSize));
        }

        /* FAT number is 2 defaultly.    */
        UdiskFSInfo.DataAreaStartAddress = UdiskFSInfo.ReservedSecotr + 2 * UdiskFSInfo.FATSize;

        if (UdiskFSInfo.BPB_RootEntCnt != 0x00)
        {
            /*
             *��ǰ�ļ�ϵͳFAT16Ĭ�ϸ�Ŀ¼Ϊ32������������ʵ�ʵ�Ӧ�ó��ϣ����ּ�����û�е����
             *�������¹�ʽ������RootDirSectors=((BPB_RootEntCnt*32)+(BPB_BytePerSec-1))/BPB_BytePerSec
             *2009-8-16 14:56 husanxi
             */
            UdiskFSInfo.DataAreaStartAddress = UdiskFSInfo.DataAreaStartAddress + (((UdiskFSInfo.BPB_RootEntCnt * 32)
                                               + 0x3ff) / 0x400);

        }
        else
        {
            UdiskFSInfo.DataAreaStartAddress += UdiskFSInfo.SectorPerCluster;
        }

        //convert byte/sector to 512bytes unit
        UdiskFSInfo.DataAreaStartAddress = SectorType * UdiskFSInfo.DataAreaStartAddress + UdiskFSInfo.DBRStartAddress;

        if ((uint8) ((UdiskFSInfo.DataAreaStartAddress) & 0xff) == 0x00)
        {
            UdiskFSInfo.DataAreaOffset = 0x00;
        }
        else
        {
            //ʹ��������Ŀ¼�״�֮��LBA��ַģ16����
            UdiskFSInfo.DataAreaOffset = (uint8) (0x100 - (UdiskFSInfo.DataAreaStartAddress & 0xff));
        }

        ret = 0x01;
    }
    else
    {
        UdiskFSInfo.DataAreaStartAddress = 0xffffffff;
        UdiskFSInfo.DataAreaOffset = 0x00;
        ret = 0x00;
    }

    return ret;
}
