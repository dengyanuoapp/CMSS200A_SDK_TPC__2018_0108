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
 * Description: Ϊ������α����.
 *
 * Arguments  : None.
 *
 * Returns    : a=0: ��  01h:����.
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
        //�������a  0:Ҫ�� 01:����
        ret = sEncrypParseInRD();
    }
    else
    {
        //�����������������������0������־
        Phy0Sec1RdCounter1 = 0;
        if (EncryptFlag != 0x00)
        {
            //LBA��DiskB�����Ƚ�  a=0,С��  01h:���ڻ����,
            //Ϊ�˷�ֹ����ƽ��ߴ�LUN0��DiskB��LBA����ȡ����������
            //LBA��DiskC�����Ƚ�  a=0,С��  01h:���ڻ����
            if ((PasswdPassOk != 0x00) || (LBA < EncryptDiskBStartAddr) || (LBA >= EncryptDiskCStartAddr))
            {
                ret = 0x00;
            }
            else
            {
                //��Uram Buffer 2Kbytes����Ϊ��
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
 * Description: �ж��Ƿ�Ϊ�������ϵĲ���.
 *
 * Arguments  : None.
 *
 * Returns    : a=0: ����  01h:��.
 *
 * Note(s)    : Ĭ������������.
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

    //���з���
    if ((ret == 0x00) && (SplitDiskFlag != 0x00))
    {
        ret = 0x00;
        //LBA��DiskC�����Ƚ�  a=0,С��  01h:���ڻ����
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
 * Description: �ж��Ƿ�д������
 *
 * Arguments  : None.
 *
 * Returns    : a=0:д  01h:��д.
 *
 * Note(s)    : Ĭ��Э�������յ�B2��.
 *********************************************************************************************************
 */
uint8 sEncrypParseInWR(void)
{
    uint8 ret = 0x01;
    encrypt_info *encrypt_info_p;

    encrypt_info_p = (encrypt_info *) DATABUFFER;
    if (ActWRLength != 0x00)
    {
        //�������д����,ֻ���ۼ�ActWRLBA,����д�����ں������������
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
            //ȡ�ô�����Ϣ
        case 0x05:
            CMDPerformCondition = GETDISKINFO;//GETDISKINFO;
            break;
            //�����û�������
        case 0x08:
            //prints("set name&passwd -");
            memcpy(TempBuffer, &encrypt_info_p->OldUserNameLengthRecord, 0x40);
            //�����������¼����
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //��������
            if (encrypt_info_p->IfEncryptRecord == 0x01) //����û����״̬?
            {
                //�����û���,���ȼ�1�����ϳ��ȼ�¼����1�ֽ�
                if (0x00 != memcmp(&encrypt_info_p->UserNameLengthRecord, TempBuffer,
                                   encrypt_info_p->UserNameLengthRecord + 1))
                {
                    //prints("set name fail-");
                    CMDPerformCondition = SETNAMEORPASSWDFAIL;
                    break;
                }
                //��������,���ȼ�1�����ϳ��ȼ�¼����1�ֽ�
                if (0x00 != memcmp(&encrypt_info_p->PasswdLengthRecord, TempBuffer + 0x10,
                                   (encrypt_info_p->PasswdLengthRecord + 1)))
                {
                    //prints("set passwd fail-");
                    CMDPerformCondition = SETNAMEORPASSWDFAIL;
                    break;
                }
            }
            //��¼������
            memcpy(&encrypt_info_p->UserNameLengthRecord, TempBuffer + 0x20, 0x20);
            //���������óɹ����,��д��flash
            encrypt_info_p->IfEncryptRecord = 0x01;
            sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            CMDPerformCondition = SETPASSWDOK;
            break;
            //дu��,������д����ֻдLBA1����ÿ��дһ�����������ݷ���B2��
        case 0x15:
            EndianReverse((uint8 *) & (encrypt_info_p->PhySecWRAddrRecord), sizeof(encrypt_info_p->PhySecWRAddrRecord));
            ActWRLBA = UdiskStartAddr + encrypt_info_p->PhySecWRAddrRecord;
            EndianReverse((uint8 *) & (encrypt_info_p->PhySecWRLengthRecord),
                          sizeof(encrypt_info_p->PhySecWRLengthRecord));
            ActWRLength = encrypt_info_p->PhySecWRLengthRecord;
            //��������ߣ���������Ready��ʾ�����������ѭ���ķ�����ʾʱ��
            Removable = 0x00;
            break;
            //���ü����̴�С
        case 0x18:
            memcpy(&TempBuffer, &encrypt_info_p->DiskACapacityRecord, 0x30);
            memset(DATABUFFER, 0, 0x200);
            memcpy(&encrypt_info_p->DiskACapacityRecord, TempBuffer, 0x30);
            if (encrypt_info_p->DiskBCapacityRecord != 0x00)
            {
                //�ѷֳ�DiskA,B,C�ṹ
                encrypt_info_p->SplitDiskRecord = 0x01;
            }
            sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            break;
            //�÷���������־
        case 0x20:
            PartiEjectFlag = 0x01;
            break;
            //����û�������û���������
        case 0x80:
            //prints("check name&passwd -");
            memcpy(TempBuffer, &encrypt_info_p->UserNameLengthRecord, 0x20);
            //�����������¼����
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //����û���,���ȼ�1�����ϳ��ȼ�¼����1�ֽ�
            if (0x00 != memcmp(&encrypt_info_p->UserNameLengthRecord, TempBuffer, encrypt_info_p->UserNameLengthRecord
                               + 1))
            {
                //prints("name fail-");
                CMDPerformCondition = NAMEORPASSWDFAIL;
                break;
            }
            //�������,���ȼ�1�����ϳ��ȼ�¼����1�ֽ�
            if (0x00 != memcmp(&encrypt_info_p->PasswdLengthRecord, TempBuffer + 0x10,
                               encrypt_info_p->PasswdLengthRecord + 1))
            {
                //prints("passwd fail-");
                CMDPerformCondition = NAMEORPASSWDFAIL;
                break;
            }
            //����ͨ��,������,������PC�ض�LUN2���̽ṹ
            MediaChgFlag = 0x01;
            CMDPerformCondition = NAMEANDPASSWDOK;
            break;
        case 0xa0:
            //�Ƿ�Ready��ʾ��Ϊ��XP��TestReady
            if (USB_State == 0x00)
            {
                CMDPerformCondition = UDBUSYINSWITCH2ADFU;
            }
            //�Ƿ�Win98ϵͳ,0:Win2000 (XP)ϵͳ1:Win98ϵͳ 2:Linuxϵͳ
            else if (encrypt_info_p->SystemInfoRecord == 0x02)
            {
                //��æ,�����л���Linuxϵͳ�²���Ҫ��(û�������������ǻ�������)�������л�
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
 * Description: �������д����,��B2�е�1 block���ݰ�ActWRLBA%4����ᵽB1��,Ȼ��д��Flash.
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
    //������Ҫд��ĵ�ַ���µ�LBA��,д�������ڼ���Э�鴦�������
    LBA = ActWRLBA;
    ActWRLBA++;

    ActWRLength--;
    if (ActWRLength == 0x00)
    {
        //�������д����̣�����Ready��ʾ
        Removable = 0x01;
    }
}

/*
 *********************************************************************************************************
 *                                           sEncrypParseInRD
 *
 * Description: �ж��Ƿ������Э��.
 *
 * Arguments  : None.
 *
 * Returns    : a: 0:Ҫ�� 01:����.
 *
 * Note(s)    : sEncrypParseInRD��֧��linux��,bufferָ�벻��0x4800
 *********************************************************************************************************
 */
static uint8 sEncrypParseInRD(void)
{
    uint8 ret = 0x01, tmp, tmp1;
    encrypt_info *encrypt_info_p;

    encrypt_info_p = (encrypt_info *) DATABUFFER;

    if (CMDPerformCondition != 0x00)
    {
        //Ҫ��������,��ix��ʼ��ַ(ixֵ���ᱻ�Ķ�) 0.5KbytesUram Buffer��0����ֹ�Ѽ�����Ϣ����������
        memset(DATABUFFER, 0, 0x200);
    }

    switch (CMDPerformCondition)
    {
    case 0x00:
        Phy0Sec1RdCounter1++;
        //=================ʶ��������ظ�����=================
        if (Phy0Sec1RdCounter1 == 0x05)
        {
            //prints("Phy0Sec1RdCounter1=0x05-");
            Phy0Sec1RdCounter1 = 0;
            sFlash_Rds(RW_FIX, LBA, DATABUFFER);
            sectors_trans_once = 0x01;
            memcpy(&encrypt_info_p->PhysicsSec0TagRecord, Phy0DiskTag, 0x04);
            //д������Ϣ
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
        if (SplitDiskFlag != 0x00) //���з���
        {
            //�����������¼����
            sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
            //��DiskABC��ַ�����������ڼ�����Ϣ����
            memcpy(TempBuffer, &encrypt_info_p->DiskACapacityRecord, 0x30);
            //�Ƿ���������
            tmp1 = encrypt_info_p->IfEncryptRecord;
            //Ҫ��������,��ix��ʼ��ַ(ixֵ���ᱻ�Ķ�) 2KbytesUram Buffer��0����ֹ�Ѽ�����Ϣ����������.
            memset(DATABUFFER, 0, 0x200);
            encrypt_info_p->IfEncryptRecord = tmp1;
            encrypt_info_p->SplitDiskRecord = 0x01;
            memcpy(&encrypt_info_p->DiskACapacityRecord, TempBuffer, 0x30);
        }
        encrypt_info_p->CMDIDRecord = 0x00;
        encrypt_info_p->Phy0EquLog0Record = Phy0EquLog0Flag;
        //����������
        encrypt_info_p->FormatCapacityRecord = udisk_total_capacity;
        EndianReverse((uint8 *) & (encrypt_info_p->FormatCapacityRecord), sizeof(encrypt_info_p->FormatCapacityRecord));
        //vidpidstring
        memcpy(&encrypt_info_p->VidPidRecord, &HS_Device_Dscrptr.idVendor, 0x04);
        //����������
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
        //��æ,�����л�
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
            //=================��ͨ������ظ�����+1=================
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
