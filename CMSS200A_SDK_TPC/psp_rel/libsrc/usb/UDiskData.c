/*
 *********************************************************************************************************
 *                                       ACTOS GL5102
 *
 *
 *                           (c) Copyright 2009-2019, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UdiskData.msa
 * By     : wanghaijing
 * Version: v1.00     first version     2009-12-13 12:45
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma userclass(hconst=FDATA)
//note:�˱�������AP bank 0xa0����ʼ�Ĺ̶�λ��,���ݹ��߽���udisk.ap�ļ�,��д��������
uint16 const far fake_udisk_cap = 0x00;
#pragma userclass(hconst=default)

#pragma userclass(hconst=RDATA)
const uint8 FFSDCardDrvName[11] = "UD_FFSD.DRV"; //SD card driver name
const uint8 FFMSCardDrvName[] = "UD_FFMS.DRV"; //MS card driver name
const uint8 FFCardBaseDrvName[] = "UD_FF648.DRV"; //MS card driver name
const uint8 ActionsFlag[11] =
{ 'A', 'C', 'T', 'I', 'O', 'N', 'S', 'U', 'S', 'B', 'D' };
/*actions_solution_flag�����ĵ����ݽṹ����
 struct system_info
 {
 char reserved_id;
 char company_id;         //��C�� stands for ���麣������; ��E�� stands for ������������
 // ��F�� stands for �����ھ�š�; Others reserved;
 char device_id[5];         //��ADFUD�� stands for ��Actions firmware update device��;
 //��USBD �� stands for ��mass storage USB disk��;
 unsigned short brom_pid;            // ��0x5005
 char brom_version;      //  �确A��;
 unsigned short ext_brom_id;      // ��չ brom id�����ͬһϵ��IC�������г���Ŷ��޸�brom id
 char ext_brom_version;      //��չ brom �汾�����ͬһϵ��IC�������г���Ŷ��޸�
 char case_type;           //���ָ�����ͬ�ķ�����adfuʱ���ظ�����Ϊ0����ʾ�������ַ�����Ϣ
 //case_type��Ӧ���������ı�ţ�
 //5105/5101                          0x0a
 //5102                               0x0b
 //3976/5103/5107/5108                0x0c
 char reserved[4];        //Ĭ��Ϊ0
 };
 */
const uint8 actions_solution_flag[18] =
{ 0, 'C', 'U', 'S', 'B', 'D', ' ', 0x02, 0x51, 'A', 0, 0, 0, 'B', 0, 0, 0, 0 };
uint8 const MBR_Bak[224] = //0xE0���ֽ�,Master Boot Record
{
    0xfa, 0x33, 0xc0, 0x8e, 0xd0, 0xbc, 0x00, 0x7c, 0x8b, 0xf4, 0x50, 0x07, 0x50, 0x1f, 0xfb, 0xfc, 0xbf, 0x00,
    0x06, 0xb9, 0x00, 0x01, 0xf2, 0xa5, 0xea, 0x1d, 0x06, 0x00, 0x00, 0xbe, 0xbe, 0x07, 0xb3, 0x04, 0x80,
    0x3c, 0x80, 0x74, 0x0e, 0x80, 0x3c, 0x00, 0x75, 0x1c, 0x83, 0xc6, 0x10, 0xfe, 0xcb, 0x75, 0xef, 0xcd,
    0x18, 0x8b, 0x14, 0x8b, 0x4c, 0x02, 0x8b, 0xee, 0x83, 0xc6, 0x10, 0xfe, 0xcb, 0x74, 0x1a, 0x80, 0x3c,
    0x00, 0x74, 0xf4, 0xbe, 0x8b, 0x06, 0xac, 0x3c, 0x00, 0x74, 0x0b, 0x56, 0xbb, 0x07, 0x00, 0xb4, 0x0e,
    0xcd, 0x10, 0x5e, 0xeb, 0xf0, 0xeb, 0xfe, 0xbf, 0x05, 0x00, 0xbb, 0x00, 0x7c, 0xb8, 0x01, 0x02, 0x57,
    0xcd, 0x13, 0x5f, 0x73, 0x0c, 0x33, 0xc0, 0xcd, 0x13, 0x4f, 0x75, 0xed, 0xbe, 0xa3, 0x06, 0xeb, 0xd3,
    0xbe, 0xc2, 0x06, 0xbf, 0xfe, 0x7d, 0x81, 0x3d, 0x55, 0xaa, 0x75, 0xc7, 0x8b, 0xf5, 0xea, 0x00, 0x7c,
    0x00, 0x00, 0x49, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x70, 0x61, 0x72, 0x74, 0x69, 0x74, 0x69,
    0x6f, 0x6e, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x00, 0x45, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x6c, 0x6f,
    0x61, 0x64, 0x69, 0x6e, 0x67, 0x20, 0x6f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x73,
    0x79, 0x73, 0x74, 0x65, 0x6d, 0x00, 0x4d, 0x69, 0x73, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6f, 0x70, 0x65,
    0x72, 0x61, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x00, 0x00, 0x81, 0x22,
    0x08, 0x15,
};
const uint8 ActCMDSignatureC[7] =
{ 'A', 'c', 't', 'U', 'S', 'B', 'C', };
const uint8 ActCMDSignatureS[7] =
{ 'A', 'c', 't', 'U', 'S', 'B', 'S', };
const uint8 Phy0DiskTag[4] =
{ 'P', 'h', 'y', '0', };
const uint8 decrypt_key = 0x76;
#pragma userclass(hconst=default)

#pragma userclass(near=RDATA)
/*
 *********************************************************************************************************
 for globle varible
 *********************************************************************************************************
 */
uint8 CurrentBankNo = 0;
uint32 OldIsrPtr = 0;
uint8 sfr_bak = 0x00;
uint32 SP_Keeper = 0x00;
uint8 sFlashLedFlag = 0;
uint8 *sFlashLed = 0;
uint8 TempBuffer[0x40] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
uint8 MCSR2_bak = 0;
uint8 MCSR4_bak = 0;
//uint8 MCUPLL_bak = 0;
/*
 *********************************************************************************************************
 for disk information
 *********************************************************************************************************
 */

uint8 OSType = 0; //0: mac OSX system    1: windows system  ����ʵ����ƻ��ϵͳ�¿��ε����̷���ʧЧ��
uint8 MaxLUN = 0; //�ϱ�PC���̷���
uint8 DiskAppearSelect = 0; //�û����̷�ѡ��  0:���̷�(Ĭ��)  01h:��ͨ��  02h:������  03h:�忨

uint8 AutoRunDiskFlag = 0; //AutoRun���̴��ڱ�־: 0:������; ��0:����
uint8 bWriteCD_ROM = 0; //AutoRun����д���� 0:������д 0x80:����д

uint32 SDDiskCap = 0x00; //SD������,��512bytesΪ��λ
uint32 VMCap = 0x00; //VRAM������,��512bytesΪ��λ
uint32 MICap = 0x00; //MI������,��512bytesΪ��λ
uint32 UICap = 0x00; //UI������,��512bytesΪ��λ
uint32 AutoRunDiskCapacity = 0; //AutoRun��������,��512bytesΪ��λ
uint32 udisk_total_capacity = 0; //UDISK��,��512bytesΪ��λ

uint32 AutoRunDiskStartAddr = 0; //AutoRun������ʼ����(512bytes)ƫ�Ƶ�ַ
uint32 UdiskStartAddr = 0; //UDISK����ʼ����(512bytes)ƫ�Ƶ�ַ
uint32 EncryptInfoSector = 0x00; //��������Ϣ��ʼ����(512bytes)ƫ�Ƶ�ַ(��UI����)
uint32 EncryptDiskBStartAddr = 0; //������B����ʼ����(512bytes)ƫ�Ƶ�ַ(�����̽��ܺ�Ĵ��̻�û���ܵļ�����)
uint32 EncryptDiskCStartAddr = 0; //������C����ʼ����(512bytes)ƫ�Ƶ�ַ(�����̽���ǰ�Ĵ���)

//���洢�������ͣ���MLC flash:0; MLC flash:1; 2:SPI Nor flash; 3:LBA flash; 4:card base; 5:TLC flash
uint8 flash_type = 0;
uint32 flash_capacity = 0; //���洢��������,���̼�����������������,��128KΪ��λ
//���洢���ʵ�page��С,��byte��ʾpage��С��page��sector��Ϊ2��������;��byte��ʾsector��С,0:512bytes ����ֵ��1024bytes
uint16 flash_page_sctrnum = 0;
uint8 FlashWPFlag = 0; //��д������־
//uint8 flash_vram_vld = 0;

uint8 ShowMultiDiskFlag = 0; //���߼���ʾ���̷�  0:����ʾ���̷� 1:��ʾ���̷�
uint32 CardCapacity = 0; //������,��512bytesΪ��λ
uint8 CardExistFlag = 0; //���Ƿ���ڱ�־
uint8 CardWPFlag = 0; //��д������־
uint8 RWCardError = 0; //�ж϶�д���Ƿ����0���޴�ֱ�ӷ���CSW��1���д���Ҫ����̽�⿨

uint8 flashorcard_rwflag = 0; //Flash�򿨶�д��־: 0,Flash; 1:Card
ud_rw_struct_t SD_UDRWStruct = //Flash�����ṹ
{ 0x00, 0x00, 0x00, 0x00, };
uint32 LBA = 0;
uint32 sectors_trans_once = 0;
uint32 udisk_rw_sctr_num = 0;
uint8 illegalLBAFlag = 0; //LBA�Ƿ�Ƿ�,����Ƿ�SD����Խ��Flash������,�˱�־ֻ��Flash��д��Ч
uint8 flash_normal_rw_sctrs = 0; //Udisk��ͨ��дFlash������
uint8 flash_disk_write_flag = 0; //Flash��(A��)д���,��д����ʾ��Ҫ����FAT����
uint8 fifo_trans_mode = 0; //0:nomal ģʽ; 1:fifoֱͨģʽ

uint8 IsNotFormat = 0x00;
uint8 bMediaChange = 0;
uint8 MediaChgFlag = 0; //���̽��ʱ任��־�����ڴ����л�

udisk_fs_info_t UdiskFSInfo = //8k page����
{ 0xffffffff, 0, 0, 0, 0, 0, 0, 0, 0, };

uint8 set_fake_ucap_flag = 0; //u�����ݱ�־,0:��ʵflash������1:��������
uint8 SectorType = 0; //flash������С,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;

/*
 *********************************************************************************************************
 for udisk status record
 *********************************************************************************************************
 */
uint8 USB_State = 0;
uint8 TransferFlag = 0;

uint8 StatusCheckFlag = 0; //��ʼ״̬��ѯ��־
uint8 USBBusy = 0; //����Ep1����

uint8 SwitchToADFUFlag = 0; //�����л���ADFU
uint8 CanSwitchToADFU = 0x00;
uint8 detach_cmd_rcv = 0;
uint8 FWOpStatus = 0;
uint8 IsAdfuCondition = 0;
uint8 IsInAdfuState = 0;
uint8 PartiEjectFlag = 0; //����������־
uint8 PullLineFlag = 0; //���߱�־
uint8 PcEjectFlag = 0; //PC������־
uint8 SplitDiskFlag = 0; //�Ƿ�ΪDiskA,B,C�ṹ
uint8 EjectFlag = 0; //�Ƿ��ѷ�������
uint8 usb_pwr_suspnd_fnsh = 0; //USB����ʱ��suspend�Ѿ�����
uint8 Suspend_Flag = 0; //bit[7]:1 need process suspend,0 don't process suspend; bit[1:0]:0 no suspend; 1 recieve suspend; 2 has handle suspend
uint8 DisableSuspendFlag = 0; //0:enable suspend   01h:disable suspend
uint8 UpOrDownLoad = 0; //���ػ�������  01h:���� 02:����
uint8 UpOrDownLoadBak = 0; //���ػ������ر���  01h:���� 02:����

uint8 UsbIdleCounter = 0; //����������м���
uint8 EjectTimeCtr = 0; //����������ʱ����
uint8 AdfuTimerCtl = 0;
uint8 connect_idle_cnt = 0; //USB���Ӻ����Idle״̬ʱ�����(2Hz)
uint8 Counter_30s = 0; //��ʱ������м���
uint8 PrevRTCTimerValue1 = 0; //RTCTimer��ֵ,UsbIdleCounter,EjectTimeCtr,AdfuTimerCtl����ʹ��
uint8 PrevRTCTimerValue2 = 0; //RTCTimer��ֵ,30s����ʹ��
uint8 prv_connect_idle_tm = 0; //USB���Ӻ����Idle״̬�ϴ�Timer����ֵ

uint8 cbw_count = 0; //���ڼ�¼��һ��CBW������ƻ����windowsϵͳ�б�ƻ��ϵͳ��һ��CBW�϶���Inquire
uint8 bTestNoready = 0;
uint8 short_pckt_flag = 0;
uint8 ErrorReportFlag = 0; //�����ر�־
uint8 ErrorIndicator = 0; //��������
uint8 IsReadCapAndReadRec0 = 0;
uint8 Removable = 0; //�Ƿ���԰��� 0:���ɰ��ߣ�01h:�ɰ���

/*
 *********************************************************************************************************
 for adfu
 *********************************************************************************************************
 */
uint8 LogicOrPhy = 0; //ADFU �߼��������д��־   0:�߼�������д 1:���������д
uint8 AccessFlash = 0; //ADFU д�߼������� 0:��д���� 1:��д����

uint8 brec_count = 0; //BREC�߼���ţ�0-3
uint8 first_brec_sector = 0; //ÿ��BREC�ĵ�һ��������־��0:д������������ֵ����������
uint16 total_breclogo_sector = 0; //BRECLOGO length��BREC length + LOGO length
uint16 phy_block_addr = 0; //Flash��������ţ�MBRC:0-3;BREC:4-7
uint16 brec_check_sum = 0; //BREC��У���,����LOGO

uint32 CallingTaskAdd = 0; //���ܵ�����ڵ�ַ
uint8 CallingTaskFlag = 0; //���ܵ��������־λ��0���޹��ܵ�������1���й��ܵ�������
uint16 CallingTaskResult = 0; //״̬�ṹ�ĵ�ַ
uint16 StatusLength = 0; //״̬�ṹ�ĳ���

uint8 adfu_flag = 0; // 1:�ѿ�ʼ����

/*
 *********************************************************************************************************
 for EnDecrypt
 *********************************************************************************************************
 */
uint8 AccessFlashPhySec1 = 0; //������������1
uint8 EncryptFlag = 0; //�Ƿ����  0:û����  1:û����
uint8 PasswdPassOk = 0; //����У���Ƿ�ͨ��  0:ûͨ��  1:ͨ��
uint8 CMDPerformCondition = 0; //Act U��Э��ִ�������¼
uint16 ActWRLength = 0; //�������Ҫд������Ŀ
uint32 ActWRLBA = 0; //�������Ҫд��Flash��ַ
uint8 Phy0Sec1RdCounter1 = 0; //������Phy0Sec1������1,������U��
uint8 Phy0Sec1RdCounter2 = 0; //������Phy0Sec1������2,����Э�齻��
uint8 Phy0EquLog0Flag = 0; //����0�Ƿ����߼�0�غ�

/*
 *********************************************************************************************************
 usb bulk packet
 *********************************************************************************************************
 */
//usb bulk only protocal cbw buffer
CBW_WRAP CBW_data_buffer =
{ 0x55534243, 0, 0, 0, 0, 0, 0, };

CSW_WRAP CSWBuffer =
{ 0x55534253, 0, 0, 0, };

uint8 CMD_Inquiry_information_FOB[60] =
{
    0x00, //Direct-access device
    0x80, //removable
    0x00, 0x01, //response data format as this spec
    0x1f, //(92��)additional length
    0x00, 0x00, 0x00, //reserved
    'A', 'C', 'T', 'I', 'O', 'N', 'S', 0x20, //Vendor information
    'U', 'S', 'B', ' ', 'D', 'I', 'S', 'K', 0x20, 'F', 'O', 'B', ' ', ' ', ' ', 0x20, //product identification
    '1', '.', '0', '0', //product revision level
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //vendor specific
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00,
};
//for autorun
uint8 CMD_Inquiry_information_CDROM[] =
{
    0x05, //Direct-access device
    0x80, //removable
    0x02, 0x02, //response data format as this spec
    0x1f, //(92��)additional length
    0x00, 0x00, 0x00, //reserved
    'A', 'C', 'T', 'I', 'O', 'N', 'S', 0x20, //Vendor information
    'U', 'S', 'B', ' ', 'D', 'I', 'S', 'K', 0x20, 'C', 'D', 'R', 'O', 'M', ' ', 0x20, //product identification
    '1', '.', '0', '0', //product revision level
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //vendor specific
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00,
};

uint8 *Cap_Info_DiskCDROM_p = (uint8 *) &Cap_cmd_info_DiskCDROM;
uint8 *Cap_Info_DiskA_p = (uint8 *) &Cap_cmd_info_DiskA;
uint8 *Cap_Info_DiskB_p = (uint8 *) &Cap_cmd_info_DiskB;
uint8 *Cap_Info_DiskC_p = (uint8 *) &Cap_cmd_info_DiskC;
uint8 *Cap_Info_DiskCard_p = (uint8 *) &Cap_cmd_info_DiskCard;
cap_info Cap_cmd_info_DiskCDROM =
{ 0, 0x0800, };
cap_info Cap_cmd_info_DiskA =
{ 0, 0x0200, };
cap_info Cap_cmd_info_DiskB =
{ 0, 0x0200, };
cap_info Cap_cmd_info_DiskC =
{ 0, 0x0200, };
cap_info Cap_cmd_info_DiskCard =
{ 0, 0x0200, };
format_cap_info Formatcap_cmd_info_DiskA =
{
    0, 0, 0, 0x10,
    //Current/Max Capacity
    0, 0x02, 0x00, 0x02, 0x00,
    //Fomattable Capacity
    0, 0x00, 0x00, 0x02, 0x00,
};
format_cap_info Formatcap_cmd_info_DiskB =
{
    0, 0, 0, 0x10,
    //Current/Max Capacity
    0, 0x02, 0x00, 0x02, 0x00,
    //Fomattable Capacity
    0, 0x00, 0x00, 0x02, 0x00,
};
format_cap_info Formatcap_cmd_info_DiskC =
{
    0, 0, 0, 0x10,
    //Current/Max Capacity
    0, 0x02, 0x00, 0x02, 0x00,
    //Fomattable Capacity
    0, 0x00, 0x00, 0x02, 0x00,
};
format_cap_info Formatcap_cmd_info_DiskCard =
{
    0, 0, 0, 0x10,
    //Current/Max Capacity
    0, 0x02, 0x00, 0x02, 0x00,
    //Fomattable Capacity
    0, 0x00, 0x00, 0x02, 0x00,
};

uint8 Time_Protect_Page_information[16] =
{
    0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Mode Parameter Header
    0x1c, 0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
};
uint8 All_Page_information[52] =
{
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00,
    0x00, //Mode Parameter Header
    0x01, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0a, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1c, 0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
};
uint8 Request_Sense_cmd_info_NoError[18] =
{ 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
uint8 Request_Sense_cmd_info_CommandReset[18] =
{ 0x70, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, };
uint8 Request_Sense_cmd_info_MediaProtect[18] =
{ 0x70, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, };
uint8 Request_Sense_cmd_info_MediaChg[18] =
{ 0x70, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, };
uint8 Request_Sense_cmd_info_MediaNotPresent[] =
{ 0x70, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, };
uint8 CDRom_Request_Sense_command_Invalid_Operation_Code[18] =
{ 0x70, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, };

//;;;;;;;;;;;;;;;;;;;;;;;CD_ROM Command In table;;;;;;;;;;;;;;;;;;;;;;;;;;;
uint8 Read_TOC_command_information[24] =
{
    0x00, 0x12, 0x01, 0x01, 0x00, 0x14, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x14, 0xaa, 0x00, 0x00, 0x00, 0x0a,
    0x00, 0x00, 0x00, 0x00, 0x00,
};
uint8 CDRom_ModeSense_information[8] =
{
    0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Mode Parameter Header
};
uint8 Mechanism_Status_command_information[13] =
{
    0x00, 0x00, //this field is reserved for changer
    0x00, 0x00, 0x00, 0x00, //Current LBA
    0x01, //Number of Slots Available
    0x00, 0x04, //Length of Slots Table(s) ,4byte for a slot
    0x80, 0x00, 0x00, 0x00, //Slot Table Response format
};
uint8 CDRom_Request_DISCINFORMATION[28] =
{
    0x00, 0x20, 0x0e, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/*
 *********************************************************************************************************
 for setup
 *********************************************************************************************************
 */
//bit0~bit1: Udisk���к�֧��:0:no sn; 1:unified sn; 2:random sn;
//bit4: Udisk��֤ģʽ֧��: 1:����Udisk��֤ģʽ,����ʱ��ӿ�,֧��suspend��resume��USB��֤����
uint8 udisk_setting = 0;
uint8 set_pkt_size = 0;
uint8 usb_reset_enter_cnt = 0;
uint16 max_pkt_size = 0;

usb_ctrlrequest setup_paket_data;
uint8 has_set_usb_dscrptr = 0;
uint8 ConfigValue = 0;
uint8 AlternateSetting = 0;
uint16 DeviceStatus = 0x0100;
uint16 InterfaceStatus = 0;
uint16 EndPoint1Status = 0;
uint16 EndPoint2Status = 0;

usb_languageid_descriptor language_string =
{
    0x04, //bLength
    USB_DT_STRING, //bDescriptorType
    0x0904, //LANGID code array, US english
};

usb_manufacure_descriptor manufacure_string =
{
    0x12, //bLength
    USB_DT_STRING, //bDescriptorType
    //manufacure code array, US english
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

usb_product_descriptor device_name_string =
{
    0x22, //bLength
    USB_DT_STRING, //bDescriptorType
    //product code array, US english
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

usb_serialnum_descriptor SerialNumber =
{
    0x42, //bLength
    USB_DT_STRING, //bDescriptorType
    //SerialNumber code array, US english
    'U', '2', 'd', 'c', '0', 'z', '7', 'w', 's', '5', '3', 'a', '7', '9', 'r', 'n', 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/*
 *********************************************************************************************************
 *                                           �����豸������
 *
 * Description: �����豸�豸������(USB2.0).
 *
 * Note(s)    : ����һ������, һ���ӿ�, ����Bulk�˵�.
 *********************************************************************************************************
 */
usb_device_descriptor HS_Device_Dscrptr =
{
    sizeof(usb_device_descriptor), //bLength
    USB_DT_DEVICE, //bDescriptorType

    0x0002, //bcdUSB, USB release version 2.0,0x200תС��ģʽ,
    0x00, //bDeviceClass, class code
    0x00, //bDeviceSubClass, sub-class code
    0x00, //bDeviceProtocol, protocol
    0x40, //bMaxPacketSize0, max packet size of endpoint0= 64byte

    0xD610, //idVendor, vendor ID
    0x0111, //idProduct, product ID
    0x0001, //bcdDevice, device release number
    0x01, //iManufacturer, index of string descriptor of manufacturer
    0x02, //iProduct, index of string descriptor of product
    0x03, //iSerialNumber, index of string descriptor of serial number
    0x01, //bNumConfigurations, number of possible configuration
};

usb_config_descriptor
HS_Config_Dscrptr =
{
    sizeof(usb_config_descriptor), //bLength
    USB_DT_CONFIG, //bDescriptorType
    ((sizeof(usb_config_descriptor) + sizeof(usb_interface_descriptor) + sizeof(usb_endpoint_descriptor)
    * 2) << 8), //wTotalLength
    0x01, //bNumInterfaces, number of interface = 1
    0x01, //bConfigurationValue, configuration value
    0x00, //iConfiguration, configuration string index
    0x80, //bmAttributes, attribute (bus powered, remote wakeup disable)
    0xfa, //bMaxPower, max power (500mA),96h(300mA)
};

usb_interface_descriptor HS_Interface_Dscrptr =
{
    sizeof(usb_interface_descriptor), //bLength
    USB_DT_INTERFACE, //bDescriptorType

    0x00, //bInterfaceNumber
    0x00, //bAlternateSetting;
    0x02, //bNumEndpoints, number of endpoint
    0x08, //bInterfaceClass, MASS STORAGE class
    0x05, //bInterfaceSubClass,command block specification SFF-8070i;
    0x50, //bInterfaceProtocol,BULK-ONLY TRANSPORT;
    0x00, //iInterface, interface string index
};

usb_endpoint_descriptor HS_EndpointA_Dscrptr =
{
    sizeof(usb_endpoint_descriptor), //bLength
    USB_DT_ENDPOINT, //bDescriptorType

    0x81, //bEndpointAddress, endpoint address (in endpoint addressed at 1)
    0x02, //bmAttributes, endpoint type (bulk)
    0x0002, //wMaxPacketSize, max packet size (512 bytes)
    0x00, //bInterval, polling interval (ignored)
};

usb_endpoint_descriptor HS_EndpointB_Dscrptr =
{
    sizeof(usb_endpoint_descriptor), //bLength
    USB_DT_ENDPOINT, //bDescriptorType

    0x02, //bEndpointAddress, endpoint address (out endpoint addressed at 2)
    0x02, //bmAttributes, endpoint type (bulk)
    0x0002, //wMaxPacketSize, max packet size (512 bytes)
    0x00, //bInterval, polling interval (ignored)
};

/*
 *********************************************************************************************************
 *                                           �޶������豸������
 *
 * Description: �޶������豸�豸������(USB2.0).
 *
 * Note(s)    : ����һ������, һ���ӿ�, ����Bulk�˵�.
 *********************************************************************************************************
 */
usb_qualifier_descriptor HS_Device_Qualifier_Dscrptr =
{
    sizeof(usb_qualifier_descriptor), //bLength
    USB_DT_DEVICE_QUALIFIER, //bDescriptorType

    0x0002, //bcdUSB, USB release version 2.0
    0x00, //bDeviceClass, class code
    0x00, //bDeviceSubClass, sub-class code
    0x00, //bDeviceProtocol, protocol
    0x40, //bMaxPacketSize0, max packet size of endpoint0= 64byte
    0x01, //bNumConfigurations, number of possible configuration
    0x00, //bRESERVED
};

usb_config_descriptor
HS_OtherSpeed_Config_Dscrptr =
{
    sizeof(usb_config_descriptor), //bLength
    USB_DT_OTHER_SPEED_CONFIG, //bDescriptorType

    ((sizeof(usb_config_descriptor) + sizeof(usb_interface_descriptor) + sizeof(usb_endpoint_descriptor)
    * 2) << 8), //wTotalLength
    0x01, //bNumInterfaces, number of interface = 1
    0x01, //bConfigurationValue, configuration value
    0x00, //iConfiguration, configuration string index
    0x80, //bmAttributes, attribute (bus powered, remote wakeup disable)
    0xfa, //bMaxPower, max power (500mA) //96h (300mA)
};

usb_interface_descriptor HS_OtherSpeed_Interface_Dscrptr =
{
    sizeof(usb_interface_descriptor), //bLength
    USB_DT_INTERFACE, //bDescriptorType

    0x00, //bInterfaceNumber
    0x00, //bAlternateSetting;
    0x02, //bNumEndpoints, number of endpoint
    0x08, //bInterfaceClass, MASS STORAGE class;
    0x06, //bInterfaceSubClass,command block specification SFF-8070i    //for autorun;
    0x50, //bInterfaceProtocol,BULK-ONLY TRANSPORT;
    0x00, //iInterface, interface string index
};

usb_endpoint_descriptor HS_OtherSpeed_EndpointA_Dscrptr =
{
    sizeof(usb_endpoint_descriptor), //bLength
    USB_DT_ENDPOINT, //bDescriptorType
    0x81, //bEndpointAddress, endpoint address (in endpoint addressed at 1)
    0x02, //bmAttributes, endpoint type (bulk)
    0x4000, //wMaxPacketSize, max packet size (64 bytes)
    0x00, //bInterval, polling interval (ignored)
};

usb_endpoint_descriptor HS_OtherSpeed_EndpointB_Dscrptr =
{
    sizeof(usb_endpoint_descriptor), //bLength
    USB_DT_ENDPOINT, //bDescriptorType

    0x02, //bEndpointAddress, endpoint address (out endpoint addressed at 2)
    0x02, //bmAttributes, endpoint type (bulk)
    0x4000, //wMaxPacketSize, max packet size (64 bytes)
    0x00, //bInterval, polling interval (ignored)
};
#pragma userclass(near=default)
