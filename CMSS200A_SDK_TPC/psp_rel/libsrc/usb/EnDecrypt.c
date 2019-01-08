///*
//*********************************************************************************************************
//*                                       ACTOS GL3976
//*
//*
//*                           (c) Copyright 2002-2007, Actions Co,Ld.
//*                                   All Right Reserved
//*
//* File   : EnDecrypt.msa
//* By     : zhouxl
//* Version: v1.02     first version     2004-12-13 12:45
//*********************************************************************************************************
//*/

#include "udisklib.h"
#pragma name(UDISKENCRYP_C)

#pragma ROM(HUGE)
static uint8 sEncrypParseInRD(void);
static void sActWrite(void);
#pragma ROM(HUGE)

#pragma renamecode(?PR?UDISKENCRYP_C)
#pragma ROM(HUGE)
/*
 *********************************************************************************************************
 *                                           Encrypt_Read_Check
 *
 * Description: 为加密送伪数据.
 *
 * Arguments  : None.
 *
 * Returns    : a=0: 读  01h:不读.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 sReadCheckForEncryp(void)
{
    uint8 ret = 0;

    if (AccessFlashPhySec1 != 0x00)
    {
        AccessFlashPhySec1 = 0;
        //输出参数a  0:要读 01:不读
        ret = sEncrypParseInRD();
    }
    else
    {
        //读其他扇区，清读物理扇区0计数标志
        Phy0Sec1RdCounter1 = 0;
        if (EncryptFlag != 0x00)
        {
            //LBA与DiskB扇区比较  a=0,小于  01h:大于或等于,
            //为了防止软件破解者从LUN0发DiskB的LBA来读取加密盘数据
            //LBA与DiskC扇区比较  a=0,小于  01h:大于或等于
            if ((PasswdPassOk != 0x00) || (LBA < EncryptDiskBStartAddr) || (LBA >= EncryptDiskCStartAddr))
            {
                ret = 0x00;
            }
            else
            {
                //清Uram Buffer 2Kbytes数据为零
                memset(DATABUFFER, 0x00, 0x200);
                ret = 0x01;
            }
        }
        else
        {
            ret = 0x00;
        }
    }
    return ret;
}

/*
 *********************************************************************************************************
 *                                           sWriteCheckForEncryp
 *
 * Description: 判定是否为加密盘上的操作.
 *
 * Arguments  : None.
 *
 * Returns    : a=0: 不是  01h:是.
 *
 * Note(s)    : 默认数据是在中.
 *********************************************************************************************************
 */
uint8 sWriteCheckForEncryp(void)
{
    uint8 ret = 0;
    if (AccessFlashPhySec1 != 0x00)
    {
        AccessFlashPhySec1 = 0;
        ret = 0x01;
    }

    //是有分区
    if ((ret == 0x00) && (SplitDiskFlag != 0x00))
    {
        ret = 0x00;
        //LBA与DiskC扇区比较  a=0,小于  01h:大于或等于
        if (LBA >= EncryptDiskCStartAddr)
        {
            ret = 0x01;
        }
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           sEncrypParseInWR
 *
 * Description: 判断是否写加密盘
 *
 * Arguments  : None.
 *
 * Returns    : a=0:写  01h:不写.
 *
 * Note(s)    : 默认协议数据收到B2中.
 *********************************************************************************************************
 */
uint8 sEncrypParseInWR(void)
{
    uint8 ret = 0x01;
    encrypt_info *encrypt_info_p;

    encrypt_info_p = (encrypt_info *) DATABUFFER;
    if (ActWRLength != 0x00)
    {
        //分区软件写磁盘,只是累加ActWRLBA,数据写还需在后续过程中完成
        sActWrite();
        ret = 0x00;
    }
    else if (memcmp(&encrypt_info_p->CMDSignature, &ActCMDSignatureC, 7) != 0x00)
    {
        ret = 0x00;
    }
    else
    {
        ret = 0x01;
        switch (encrypt_info_p->CMDIDRecord)
        {
            //取得磁盘信息
        case 0x05:
            CMDPerformCondition = GETDISKINFO;//GETDISKINFO;
            break;
            //设置用户名密码
        case 0x08:
            //prints("set name&passwd -");
            memcpy(TempBuffer, &encrypt_info_p->OldUserNameLengthRecord, 0x40);
            //读加密情况记录扇区
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //检测旧密码
            if (encrypt_info_p->IfEncryptRecord == 0x01) //处于没加密状态?
            {
                //检测旧用户名,长度加1，加上长度记录本身1字节
                if (0x00 != memcmp(&encrypt_info_p->UserNameLengthRecord, TempBuffer,
                                   encrypt_info_p->UserNameLengthRecord + 1))
                {
                    //prints("set name fail-");
                    CMDPerformCondition = SETNAMEORPASSWDFAIL;
                    break;
                }
                //检测旧密码,长度加1，加上长度记录本身1字节
                if (0x00 != memcmp(&encrypt_info_p->PasswdLengthRecord, TempBuffer + 0x10,
                                   (encrypt_info_p->PasswdLengthRecord + 1)))
                {
                    //prints("set passwd fail-");
                    CMDPerformCondition = SETNAMEORPASSWDFAIL;
                    break;
                }
            }
            //记录新密码
            memcpy(&encrypt_info_p->UserNameLengthRecord, TempBuffer + 0x20, 0x20);
            //设密码设置成功标记,并写回flash
            encrypt_info_p->IfEncryptRecord = 0x01;
            sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            CMDPerformCondition = SETPASSWDOK;
            break;
            //写u盘,加密盘写总是只写LBA1，且每次写一个扇区，数据放在B2种
        case 0x15:
            EndianReverse((uint8 *) & (encrypt_info_p->PhySecWRAddrRecord), sizeof(encrypt_info_p->PhySecWRAddrRecord));
            ActWRLBA = UdiskStartAddr + encrypt_info_p->PhySecWRAddrRecord;
            EndianReverse((uint8 *) & (encrypt_info_p->PhySecWRLengthRecord),
                          sizeof(encrypt_info_p->PhySecWRLengthRecord));
            ActWRLength = encrypt_info_p->PhySecWRLengthRecord;
            //不允许拔线，即不进入Ready显示，否则进入主循环耗费了显示时间
            Removable = 0x00;
            break;
            //设置加密盘大小
        case 0x18:
            memcpy(&TempBuffer, &encrypt_info_p->DiskACapacityRecord, 0x30);
            memset(DATABUFFER, 0, 0x200);
            memcpy(&encrypt_info_p->DiskACapacityRecord, TempBuffer, 0x30);
            if (encrypt_info_p->DiskBCapacityRecord != 0x00)
            {
                //已分成DiskA,B,C结构
                encrypt_info_p->SplitDiskRecord = 0x01;
            }
            sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            break;
            //置分区弹出标志
        case 0x20:
            PartiEjectFlag = 0x01;
            break;
            //检测用户输入的用户名和密码
        case 0x80:
            //prints("check name&passwd -");
            memcpy(TempBuffer, &encrypt_info_p->UserNameLengthRecord, 0x20);
            //读加密情况记录扇区
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //检测用户名,长度加1，加上长度记录本身1字节
            if (0x00 != memcmp(&encrypt_info_p->UserNameLengthRecord, TempBuffer, encrypt_info_p->UserNameLengthRecord
                               + 1))
            {
                //prints("name fail-");
                CMDPerformCondition = NAMEORPASSWDFAIL;
                break;
            }
            //检测密码,长度加1，加上长度记录本身1字节
            if (0x00 != memcmp(&encrypt_info_p->PasswdLengthRecord, TempBuffer + 0x10,
                               encrypt_info_p->PasswdLengthRecord + 1))
            {
                //prints("passwd fail-");
                CMDPerformCondition = NAMEORPASSWDFAIL;
                break;
            }
            //密码通过,允许报错,以引发PC重读LUN2磁盘结构
            MediaChgFlag = 0x01;
            CMDPerformCondition = NAMEANDPASSWDOK;
            break;
        case 0xa0:
            //是否Ready显示。为了XP的TestReady
            if (USB_State == 0x00)
            {
                CMDPerformCondition = UDBUSYINSWITCH2ADFU;
            }
            //是否Win98系统,0:Win2000 (XP)系统1:Win98系统 2:Linux系统
            else if (encrypt_info_p->SystemInfoRecord == 0x02)
            {
                //不忙,可以切换。Linux系统下不需要读(没法读，读出的是缓存内容)即弹出切换
                SwitchToADFUFlag = 0x01;
            }
            else
            {
                CMDPerformCondition = UDUNBUSYINSWITCH2ADFU;
            }
            break;
        default:
            break;
        }
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           sActWrite
 *
 * Description: 分区软件写磁盘,将B2中的1 block数据按ActWRLBA%4对齐搬到B1中,然后写入Flash.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void sActWrite(void)
{
    //将真正要写入的地址更新到LBA中,写动作不在加密协议处理中完成
    LBA = ActWRLBA;
    ActWRLBA++;

    ActWRLength--;
    if (ActWRLength == 0x00)
    {
        //分区软件写完磁盘，进入Ready显示
        Removable = 0x01;
    }
}

/*
 *********************************************************************************************************
 *                                           sEncrypParseInRD
 *
 * Description: 判断是否加密盘协议.
 *
 * Arguments  : None.
 *
 * Returns    : a: 0:要读 01:不读.
 *
 * Note(s)    : sEncrypParseInRD不支持linux读,buffer指针不是0x4800
 *********************************************************************************************************
 */
static uint8 sEncrypParseInRD(void)
{
    uint8 ret = 0x01, tmp, tmp1;
    encrypt_info *encrypt_info_p;

    encrypt_info_p = (encrypt_info *) DATABUFFER;

    if (CMDPerformCondition != 0x00)
    {
        //要返回内容,把ix起始地址(ix值不会被改动) 0.5KbytesUram Buffer清0，防止把加密信息扇区返回了
        memset(DATABUFFER, 0, 0x200);
    }

    switch (CMDPerformCondition)
    {
    case 0x00:
        Phy0Sec1RdCounter1++;
        //=================识别命令读重复次数=================
        if (Phy0Sec1RdCounter1 == 0x05)
        {
            //prints("Phy0Sec1RdCounter1=0x05-");
            Phy0Sec1RdCounter1 = 0;
            sFlash_Rds(RW_FIX, LBA, DATABUFFER);
            sectors_trans_once = 0x01;
            memcpy(&encrypt_info_p->PhysicsSec0TagRecord, Phy0DiskTag, 0x04);
            //写保护信息
            encrypt_info_p->WRProtectInfoRecord = FlashWPFlag;
            ret = 0x01;
        }
        else
        {
            ret = 0x00;
        }

        tmp = 0;
        break;
    case 0x01: //GetDiskInfo
        if (SplitDiskFlag != 0x00) //是有分区
        {
            //读加密情况记录扇区
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //把DiskABC地址和容量保存在加密信息扇区
            memcpy(TempBuffer, &encrypt_info_p->DiskACapacityRecord, 0x30);
            //是否下密码了
            tmp1 = encrypt_info_p->IfEncryptRecord;
            //要返回内容,把ix起始地址(ix值不会被改动) 2KbytesUram Buffer清0，防止把加密信息扇区返回了.
            memset(DATABUFFER, 0, 0x200);
            encrypt_info_p->IfEncryptRecord = tmp1;
            encrypt_info_p->SplitDiskRecord = 0x01;
            memcpy(&encrypt_info_p->DiskACapacityRecord, TempBuffer, 0x30);
        }
        encrypt_info_p->CMDIDRecord = 0x00;
        encrypt_info_p->Phy0EquLog0Record = Phy0EquLog0Flag;
        //磁盘总容量
        encrypt_info_p->FormatCapacityRecord = udisk_total_capacity;
        EndianReverse((uint8 *) & (encrypt_info_p->FormatCapacityRecord), sizeof(encrypt_info_p->FormatCapacityRecord));
        //vidpidstring
        memcpy(&encrypt_info_p->VidPidRecord, &HS_Device_Dscrptr.idVendor, 0x04);
        //卡磁盘容量
        encrypt_info_p->CardCapacityRecord = CardCapacity;
        EndianReverse((uint8 *) & (encrypt_info_p->CardCapacityRecord), sizeof(encrypt_info_p->CardCapacityRecord));
        encrypt_info_p->CardExistRecord = CardExistFlag;
        tmp = 0x01;
        break;
    case 0x02: //SETPASSWDOK
        encrypt_info_p->CMDIDRecord = 0x05;
        tmp = 0x01;
        break;
    case 0x03: //SetNameOrPasswdFail
        encrypt_info_p->CMDIDRecord = 0x06;
        tmp = 0x01;
        break;
    case 0x04: //NameAndPasswdOk
        encrypt_info_p->CMDIDRecord = 0x10;
        tmp = 0x02;
        break;
    case 0x05: //NameOrPasswdFail
        encrypt_info_p->CMDIDRecord = 0x11;
        tmp = 0x02;
        break;
    case 0x06: //lsReplyUdUnBusyInSwitch2ADFU
        encrypt_info_p->CMDIDRecord = 0x20;
        //不忙,可以切换
        SwitchToADFUFlag = 0x01;
        tmp = 0x02;
        break;
    default: //lsReplyUdBusyInSwitch2ADFU
        encrypt_info_p->CMDIDRecord = 0x21;
        tmp = 0x02;
        break;
    }

    if (tmp != 0x00)
    {
        if (tmp == 0x01)
        {
            Phy0Sec1RdCounter2++;
            //=================普通命令读重复次数+1=================
            if (Phy0Sec1RdCounter2 == 0x08)
            {
                Phy0Sec1RdCounter2 = 0x00;
            }
        }
        memcpy(DATABUFFER, ActCMDSignatureS, 0x07);
        CMDPerformCondition = 0x00;
    }

    return ret;
}
#pragma ROM(HUGE)
