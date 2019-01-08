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
        sFlashLedFlag = 1; //��Ҫ���Ʊ�־
        sFlashLed = (uint8 *) pcallback; //�����ַ��Rcode
    }

    udisk_setting = (uint8) udisk_set & 0x03;
    if (udisk_setting == 0)
    {
        HS_Device_Dscrptr.iSerialNumber = 0;
    }

    GetUsbVidPid((vidpid_t *) &HS_Device_Dscrptr.idVendor);
    GetUsbAttribute((usbattri_t *) (&(CMD_Inquiry_information_FOB + 8)));

#ifdef USB_HOST_SUPPORT
    //    usb_detectfunc_dereg(); //ע��Timer�ж���USB��ѯ�������
#endif

    /*fixme:needϵͳ��������
     Vram_Init();                    //���Vram��Flash�Ϳ�������Ч���,�����˳�������Ƿ���дFlash���������ϱ�־λ
     */

    //װ��usb�Ŀ�code
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

    USB_State = 0; //״̬�ṹ����   0:����״̬ �ٵ���ʱҪ��ʼ��
    illegalLBAFlag = 0; //LBA�Ƿ�Ƿ�,���ڷ�����������зǷ���
    //����ֵ ��MLC flash:0  MLC flash:1; 2:SPI Nor flash; 3:LBA flash; 4:card base; 5:TLC flash; 6:Toshiba 24nm flash
    flash_type = (uint8) UD_GetFlashType();

    //SectorType flash������С,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;
    //UD_GetSctrSize ����ֵ:0=512bytes/sector; 1=1024bytes/sector; 2=2048bytes/sector;
    if ((flash_type != 2) && (flash_type != 4))
    {
        SectorType = (1 << ((uint8) UD_GetSctrSize()));
    }

    //SectorType = 0x02; //flash������С,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;
    if (flash_type == 4)//���ڿ�base����8k���룬��ô��Ҫ�ָ�ÿ�������Ĵ�С�Լ����һ�������ĵ�ַ
    {
        SectorType = 0x01;
    }

    if (fake_udisk_cap != 0x00)
    {
        //����Udisk�ϱ���PC������,sFillDiskACap�Ĳ�����512byteΪ��λ,��fake_udisk_cap��MΪ��λ
        //note:����A���������ü��������,�˽ӿڱ�����UDiskInit֮ǰ����,��������̻���������.
        sFillDiskACap((uint32) fake_udisk_cap << 11);
        set_fake_ucap_flag = 0x01;
    }

    sFlashCapCheck(); //Flash�Ϳ�������飬�����ϱ�PC

    flash_page_sctrnum = 0x0002; // need fix ???  //Get_Flash_Page_Size();

    //��װ�Ͽ�����
    sCardCapCheck();

    if (flash_type != 2)
    {
        sPartitionTableCheck(); //�������飬ȷ���Ƿ��ڼ���״̬
        sFlashBootSectorCheck(); //�����������,�����߱����̽ṹ���ƻ�ʹ��Ϊδ��ʽ��
        if (flash_type != 0x04)
        {
            Partion_MBR_Initial(); //���FAT MBR/DBR,�������DBRƫ��,��������ʼ��ַ
        }
    }

    sSetLUN(); //����û����õĴ�����Ŀ,������ȷ��LUN

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
 *            Ԥ��40h����������,��Ϊ��ƻ��ϵͳ��ʽ��
 *            ƻ����ʽ������XP��ʽ����һ�β��ɹ�,�ڶ��βųɹ�
 *            ����������:��MAC�¸�ʽ��С��ʱ,��дMBR���豸������,��MBR�ķ�������д��DBR����ʼ����
 *            Ϊ0X3F����(1��ͷ0����1����)��ʼ,�����豸��������������Ϊ�Ǵ�DBR��ʼ���õ�������,д��MBR��һ������������.
 *            �豸�ٲ���XPϵͳ���и�ʽ��ʱ,XPϵͳ���Ȼ����MBR�е�����ȥ����õ����һ��������LBA(0x3F + �豸����),��
 *            д��0�����������һ������,Ȼ��XP����MBR����õ������һ��������LBA������Flash������,���¶�д�豸���һ��
 *            ������һ��,��������ʽ��ʧ�ܴ���.����ʹ����д����Flash0������ʼ��һЩ����,���Եڶ��θ�ʽ����ɹ�.
 *            ������������豸ʶ�����MACϵͳʱ�����豸����Ϊʵ������ - 0x40
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

        tmp = (uint32) GetHideDiskCap(); //���������̵�����(����Ϊ��λ)
        AutoRunDiskStartAddr = SDDiskCap + VMCap + MICap + UICap + tmp;

        AutoRunDiskCapacity = (uint32) GetAutoRunDiskCap(); //����AutoRun���̵�����(����Ϊ��λ)
        if (AutoRunDiskCapacity == 0)
        {
            AutoRunDiskFlag = 0; //AutoRun���̴��ڱ�־: 0:������; ��0:����
        }
        else
        {
            AutoRunDiskFlag = 0xff;
        }

        UdiskStartAddr = (AutoRunDiskStartAddr + AutoRunDiskCapacity);
        //U�̵���ʼ������,ʹU����ʼ����ģ16Ϊ��(��Flash������д��16sectorΪ��λ,������Ҫ����)
        if ((UdiskStartAddr & 0x0000000f) != 0x00)
        {
            UdiskStartAddr = (UdiskStartAddr & (uint32) 0xfffffff0) + 0x10;
        }

        Cap_cmd_info_DiskCDROM.LastLogBlockAddr = AutoRunDiskCapacity >> 2;

        sFlashDrvCheck(); //�л�Flash Driver

        tmp = (uint32) UD_GetCap(); //��1MByteΪ��λ
        flash_capacity = tmp << 3; //��128KΪ��λ

        //����U���ܿ�����������,��Ԥ��0x40����������.
        udisk_total_capacity = (tmp << 11) - UdiskStartAddr - 0x100;

        if (set_fake_ucap_flag == 0x00)
        {
            sFillDiskACap(udisk_total_capacity);
        }

        //U�̶�дʱ��Ե�ַ�����붯��,��д��ַ��ƫ��DataAreaOffset,������������Ҳ��ͬ��,��ֹ�����Ƿ�Խ��ʱ����
        udisk_total_capacity = udisk_total_capacity + 0x100;
    }
}

/*
 *********************************************************************************************************
 *                                           sPartitionTableCheck
 *
 * Description: Partition Table Check, ȷ���Ƿ��ڼ���״̬.
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

    //��ʼ��������־
    SplitDiskFlag = 0;
    EncryptFlag = 0;
    PasswdPassOk = 0;

    //���ü���������֧�ּ��ܷ�������
    if (set_fake_ucap_flag == 0x00)
    {
        encrypt_info_p = (encrypt_info *) DATABUFFER;
        //��Flash 1�������ݵ�Uram��UramRWStartAdd��ʼ�ĵط�ȥ
        sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);

        //�Ƿ��з���
        if (encrypt_info_p->SplitDiskRecord == 0x01)
        {
            sFlash_Rds(RW_FIX, UdiskStartAddr, DATABUFFER);

            //�Ƿ�Actions������, 0:��  ��ʼֵ������; ��Actions MBR�����Ǽ���
            if (memcmp(MBR_Bak, DATABUFFER, 0xe0) == 0)
            {
                //������Ч���ñ�־λ
                SplitDiskFlag = 1;

                sFlash_Rds(RW_FIX, EncryptInfoSector, DATABUFFER);
                //�Ƿ�������
                EncryptFlag = encrypt_info_p->IfEncryptRecord;

                //�ָ�A������
                tmp = encrypt_info_p->DiskACapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                tmp = tmp / SectorType;
                Cap_cmd_info_DiskA.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskA.CurrentCap = tmp;
                Formatcap_cmd_info_DiskA.FomatableCap = tmp;

                //���DiskB��Flash��������ʼ��ַ
                tmp = encrypt_info_p->DiskBStartAddrRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                EncryptDiskBStartAddr = tmp + UdiskStartAddr;
                //�ָ�B������
                tmp = encrypt_info_p->DiskBCapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                Cap_cmd_info_DiskB.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskB.CurrentCap = tmp;
                Formatcap_cmd_info_DiskB.FomatableCap = tmp;

                //���DiskC��Flash��������ʼ��ַ
                tmp = encrypt_info_p->DiskCStartAddrRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                EncryptDiskCStartAddr = tmp + UdiskStartAddr;
                //�ָ�C������
                tmp = encrypt_info_p->DiskCCapacityRecord;
                EndianReverse((uint8 *) &tmp, sizeof(tmp));
                Cap_cmd_info_DiskC.LastLogBlockAddr = tmp;
                Formatcap_cmd_info_DiskC.CurrentCap = tmp;
                Formatcap_cmd_info_DiskC.FomatableCap = tmp;

                if (encrypt_info_p->DataSectorsForEncOkRecord != 0xddcc)
                {
                    encrypt_info_p->DataSectorsForEncOkRecord = 0xddcc;
                    //������Uram Buffer��1����������д����Ӧ�����ݵ�Flash��
                    sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
                }
            }
            else
            {
                memset(DATABUFFER, 0, 0x200);
                //������Uram Buffer��1����������д����Ӧ�����ݵ�Flash��
                sFlash_Wrts(RW_FIX, EncryptInfoSector, DATABUFFER);
            }
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sSetLUN
 *
 * Description: ����û����õĴ�����Ŀ,Set LUN Number.
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

    if (DiskAppearSelect == 0) //�û����̷�ѡ��  0:���̷�(Ĭ��)  01h:��ͨ��  02h:������  03h:�忨
    {
        cLun = SplitDiskFlag;
        if (ShowMultiDiskFlag == 0) //�Ƿ���Ҫ��ʾ���̷�
        {
            cLun += CardExistFlag;
        }
        else
        {
            cLun++; //�����Ƿ�忨����Ҫ��ʾ�̷������LUN����Ϊ1
        }
    }

    //for autorun
    if (AutoRunDiskFlag != 0x00) //AutoRun���̴��ڱ�־: 0:������; ��0:����
    {
        cLun = cLun + 1;
    }

    MaxLUN = cLun;
}

/*
 *********************************************************************************************************
 *                                           sFlashBootSectorCheck
 *
 * Description: Flash Udisk�����������,�����߱����̽ṹ���ƻ�ʹ��Ϊδ��ʽ��.
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

    //������0�������߼�0�����Ƿ��غϱ���
    Phy0EquLog0Flag = 0;
    //DATABUFFERΪsFlash_Lgcl_RdFix��������Uram B2�е���ʼ��ַ
    pcDataBuffer = DATABUFFER;
    //��MBR
    sFlash_Rds(RW_FIX, UdiskStartAddr, DATABUFFER);

    if ((*pcDataBuffer == 0xeb) && (*(pcDataBuffer + 2) == 0x90))
    {
        //������ȷ,����0�������߼�0�����غ�
        Phy0EquLog0Flag = 1;
    }
    //�Ƿ�Actions������, 0:��  ��0:����; ��Actions MBR�����Ǽ���
    else if (memcmp(MBR_Bak, UDISKSTARTADDRURAMADD, 0xe0) != 0)
    {
        //����δ��ʽ��,����0�������߼�0�����غ�
        Phy0EquLog0Flag = 0x01;
        if ((*(pcDataBuffer + 510) != 0x55) || (*(pcDataBuffer + 511) != 0xaa))
        {
            //��fat��
            sFlash_Rds(RW_FIX, UdiskStartAddr + 1, DATABUFFER);
            if ((*pcDataBuffer != 0xff) || (*(pcDataBuffer + 1) != 0xf8))
            {
                memset(DATABUFFER, 0, 0x200);
                //�ƻ�Boot��,ʹ��δ��ʽ��
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
 * Description: ���ô��̳�����Ŀ,Ҫ�ڵ���UdiskInitǰ���� 0:���̷�(Ĭ��)  01h:��ͨ��  02h:������  03h:�忨.
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
 * Description: ���ô���д����,Ӧ��UDiskRunǰ����.
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
 * Description: ���Vram��Flash�Ϳ�������Ч���,�����˳�������Ƿ���дFlash���������ϱ�־λ.
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
 *((uint8*)DATABUFFER+0x04) = 0xff;                //Flash������Ч���
 *((uint8*)DATABUFFER+0x0C) = 0xff;                //��������Ч���
 sFlash_Wrts(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 }
 */

/*
 *********************************************************************************************************
 *                                           Udisk_Exit_Handle
 *
 * Description: Udisk�����˳�ʱ�������,��Vram��ǻ�д,USB��Ϣ����Timer�ж�����ע��.
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
    /*fixme:needϵͳ��������
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
 * Description: ���Vram��Flash�Ϳ�������Ч���,�����˳�������Ƿ���дFlash���������ϱ�־λ.
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
 //������Udiskд����,��Vram����ڽ���ʱ�Ѿ����������Ҫ�ָ�;û��д����,������Vram��Ǳ�����û��,Ҳ����Ҫ�ָ�
 if ((flash_disk_write_flag==0x00) && (flash_vram_vld!=0x00))
 {
 sFlash_Rds(RW_FIX,(SDDiskCap + VM_FS_FLAG/0x200), DATABUFFER);
 //Flash������Ч���
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
 * Arguments  : udiskcap:DiskA Capacity,512byte��λ
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFillDiskACap(uint32 udiskcap)
{
    //��ʼ��blocklength
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
