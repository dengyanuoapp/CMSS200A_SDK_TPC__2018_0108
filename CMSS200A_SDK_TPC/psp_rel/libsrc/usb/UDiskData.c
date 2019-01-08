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
//note:此变量放在AP bank 0xa0的起始的固定位置,扩容工具解析udisk.ap文件,填写扩容容量
uint16 const far fake_udisk_cap = 0x00;
#pragma userclass(hconst=default)

#pragma userclass(hconst=RDATA)
const uint8 FFSDCardDrvName[11] = "UD_FFSD.DRV"; //SD card driver name
const uint8 FFMSCardDrvName[] = "UD_FFMS.DRV"; //MS card driver name
const uint8 FFCardBaseDrvName[] = "UD_FF648.DRV"; //MS card driver name
const uint8 ActionsFlag[11] =
{ 'A', 'C', 'T', 'I', 'O', 'N', 'S', 'U', 'S', 'B', 'D' };
/*actions_solution_flag变量的的数据结构如下
 struct system_info
 {
 char reserved_id;
 char company_id;         //’C’ stands for “珠海炬力”; ‘E’ stands for “北方炬力”
 // ‘F’ stands for “深圳炬才”; Others reserved;
 char device_id[5];         //’ADFUD’ stands for “Actions firmware update device”;
 //’USBD ’ stands for “mass storage USB disk”;
 unsigned short brom_pid;            // 如0x5005
 char brom_version;      //  如‘A’;
 unsigned short ext_brom_id;      // 扩展 brom id，针对同一系列IC，换了市场编号而修改brom id
 char ext_brom_version;      //扩展 brom 版本，针对同一系列IC，换了市场编号而修改
 char case_type;           //区分各个不同的方案，adfu时返回给工具为0，表示不用区分方案信息
 //case_type对应各个方案的编号：
 //5105/5101                          0x0a
 //5102                               0x0b
 //3976/5103/5107/5108                0x0c
 char reserved[4];        //默认为0
 };
 */
const uint8 actions_solution_flag[18] =
{ 0, 'C', 'U', 'S', 'B', 'D', ' ', 0x02, 0x51, 'A', 0, 0, 0, 'B', 0, 0, 0, 0 };
uint8 const MBR_Bak[224] = //0xE0个字节,Master Boot Record
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

uint8 OSType = 0; //0: mac OSX system    1: windows system  用于实现在苹果系统下卡拔掉后盘符消失效果
uint8 MaxLUN = 0; //上报PC的盘符数
uint8 DiskAppearSelect = 0; //用户的盘符选择  0:多盘符(默认)  01h:普通盘  02h:加密盘  03h:插卡

uint8 AutoRunDiskFlag = 0; //AutoRun光盘存在标志: 0:不存在; 非0:存在
uint8 bWriteCD_ROM = 0; //AutoRun光盘写属性 0:不允许写 0x80:允许写

uint32 SDDiskCap = 0x00; //SD区容量,以512bytes为单位
uint32 VMCap = 0x00; //VRAM区容量,以512bytes为单位
uint32 MICap = 0x00; //MI区容量,以512bytes为单位
uint32 UICap = 0x00; //UI区容量,以512bytes为单位
uint32 AutoRunDiskCapacity = 0; //AutoRun光盘容量,以512bytes为单位
uint32 udisk_total_capacity = 0; //UDISK区,以512bytes为单位

uint32 AutoRunDiskStartAddr = 0; //AutoRun光盘起始扇区(512bytes)偏移地址
uint32 UdiskStartAddr = 0; //UDISK区起始扇区(512bytes)偏移地址
uint32 EncryptInfoSector = 0x00; //加密盘信息起始扇区(512bytes)偏移地址(在UI区内)
uint32 EncryptDiskBStartAddr = 0; //加密盘B盘起始扇区(512bytes)偏移地址(加密盘解密后的磁盘或没加密的加密盘)
uint32 EncryptDiskCStartAddr = 0; //加密盘C盘起始扇区(512bytes)偏移地址(加密盘解密前的磁盘)

//主存储介质类型：非MLC flash:0; MLC flash:1; 2:SPI Nor flash; 3:LBA flash; 4:card base; 5:TLC flash
uint8 flash_type = 0;
uint32 flash_capacity = 0; //主存储介质容量,含固件区等其他区总容量,以128K为单位
//主存储介质的page大小,低byte表示page大小，page内sector数为2的整次幂;高byte表示sector大小,0:512bytes 其他值：1024bytes
uint16 flash_page_sctrnum = 0;
uint8 FlashWPFlag = 0; //卡写保护标志
//uint8 flash_vram_vld = 0;

uint8 ShowMultiDiskFlag = 0; //插线即显示卡盘符  0:不显示卡盘符 1:显示卡盘符
uint32 CardCapacity = 0; //卡容量,以512bytes为单位
uint8 CardExistFlag = 0; //卡是否存在标志
uint8 CardWPFlag = 0; //卡写保护标志
uint8 RWCardError = 0; //判断读写卡是否出错，0：无错，直接返回CSW；1：有错，需要重新探测卡

uint8 flashorcard_rwflag = 0; //Flash或卡读写标志: 0,Flash; 1:Card
ud_rw_struct_t SD_UDRWStruct = //Flash操作结构
{ 0x00, 0x00, 0x00, 0x00, };
uint32 LBA = 0;
uint32 sectors_trans_once = 0;
uint32 udisk_rw_sctr_num = 0;
uint8 illegalLBAFlag = 0; //LBA是否非法,如读非法SD区或越过Flash容量等,此标志只对Flash读写有效
uint8 flash_normal_rw_sctrs = 0; //Udisk普通读写Flash扇区数
uint8 flash_disk_write_flag = 0; //Flash盘(A盘)写标记,如写过表示需要更新FAT簇链
uint8 fifo_trans_mode = 0; //0:nomal 模式; 1:fifo直通模式

uint8 IsNotFormat = 0x00;
uint8 bMediaChange = 0;
uint8 MediaChgFlag = 0; //磁盘介质变换标志，用于磁盘切换

udisk_fs_info_t UdiskFSInfo = //8k page对齐
{ 0xffffffff, 0, 0, 0, 0, 0, 0, 0, 0, };

uint8 set_fake_ucap_flag = 0; //u盘扩容标志,0:真实flash容量；1:扩容容量
uint8 SectorType = 0; //flash扇区大小,1=512bytes/sector, 2=1024bytes/sector,4=2048bytes/sector;

/*
 *********************************************************************************************************
 for udisk status record
 *********************************************************************************************************
 */
uint8 USB_State = 0;
uint8 TransferFlag = 0;

uint8 StatusCheckFlag = 0; //开始状态查询标志
uint8 USBBusy = 0; //进入Ep1操作

uint8 SwitchToADFUFlag = 0; //允许切换到ADFU
uint8 CanSwitchToADFU = 0x00;
uint8 detach_cmd_rcv = 0;
uint8 FWOpStatus = 0;
uint8 IsAdfuCondition = 0;
uint8 IsInAdfuState = 0;
uint8 PartiEjectFlag = 0; //分区弹出标志
uint8 PullLineFlag = 0; //拔线标志
uint8 PcEjectFlag = 0; //PC弹出标志
uint8 SplitDiskFlag = 0; //是否为DiskA,B,C结构
uint8 EjectFlag = 0; //是否已分区弹出
uint8 usb_pwr_suspnd_fnsh = 0; //USB连线时的suspend已经结束
uint8 Suspend_Flag = 0; //bit[7]:1 need process suspend,0 don't process suspend; bit[1:0]:0 no suspend; 1 recieve suspend; 2 has handle suspend
uint8 DisableSuspendFlag = 0; //0:enable suspend   01h:disable suspend
uint8 UpOrDownLoad = 0; //上载还是下载  01h:上载 02:下载
uint8 UpOrDownLoadBak = 0; //上载还是下载备份  01h:上载 02:下载

uint8 UsbIdleCounter = 0; //正常进入空闲计数
uint8 EjectTimeCtr = 0; //分区弹出延时计数
uint8 AdfuTimerCtl = 0;
uint8 connect_idle_cnt = 0; //USB连接后出于Idle状态时间计数(2Hz)
uint8 Counter_30s = 0; //超时进入空闲计数
uint8 PrevRTCTimerValue1 = 0; //RTCTimer数值,UsbIdleCounter,EjectTimeCtr,AdfuTimerCtl计数使用
uint8 PrevRTCTimerValue2 = 0; //RTCTimer数值,30s计数使用
uint8 prv_connect_idle_tm = 0; //USB连接后出于Idle状态上次Timer的数值

uint8 cbw_count = 0; //用于记录第一个CBW，用于苹果与windows系统判别，苹果系统第一个CBW肯定是Inquire
uint8 bTestNoready = 0;
uint8 short_pckt_flag = 0;
uint8 ErrorReportFlag = 0; //出错返回标志
uint8 ErrorIndicator = 0; //出错类型
uint8 IsReadCapAndReadRec0 = 0;
uint8 Removable = 0; //是否可以拔线 0:不可拔线，01h:可拔线

/*
 *********************************************************************************************************
 for adfu
 *********************************************************************************************************
 */
uint8 LogicOrPhy = 0; //ADFU 逻辑和物理读写标志   0:逻辑分区读写 1:物理分区读写
uint8 AccessFlash = 0; //ADFU 写逻辑区操作 0:无写操作 1:有写操作

uint8 brec_count = 0; //BREC逻辑编号，0-3
uint8 first_brec_sector = 0; //每份BREC的第一个扇区标志，0:写首扇区，其他值：非首扇区
uint16 total_breclogo_sector = 0; //BRECLOGO length：BREC length + LOGO length
uint16 phy_block_addr = 0; //Flash物理区编号，MBRC:0-3;BREC:4-7
uint16 brec_check_sum = 0; //BREC的校验和,不含LOGO

uint32 CallingTaskAdd = 0; //功能调用入口地址
uint8 CallingTaskFlag = 0; //功能调用任务标志位，0：无功能调用任务；1：有功能调用任务
uint16 CallingTaskResult = 0; //状态结构的地址
uint16 StatusLength = 0; //状态结构的长度

uint8 adfu_flag = 0; // 1:已开始量产

/*
 *********************************************************************************************************
 for EnDecrypt
 *********************************************************************************************************
 */
uint8 AccessFlashPhySec1 = 0; //访问物理扇区1
uint8 EncryptFlag = 0; //是否加密  0:没加密  1:没加密
uint8 PasswdPassOk = 0; //密码校验是否通过  0:没通过  1:通过
uint8 CMDPerformCondition = 0; //Act U盘协议执行情况记录
uint16 ActWRLength = 0; //分区软件要写扇区数目
uint32 ActWRLBA = 0; //分区软件要写的Flash地址
uint8 Phy0Sec1RdCounter1 = 0; //连续读Phy0Sec1计数器1,用于找U盘
uint8 Phy0Sec1RdCounter2 = 0; //连续读Phy0Sec1计数器2,用于协议交流
uint8 Phy0EquLog0Flag = 0; //物理0是否与逻辑0重合

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
    0x1f, //(92个)additional length
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
    0x1f, //(92个)additional length
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
//bit0~bit1: Udisk序列号支持:0:no sn; 1:unified sn; 2:random sn;
//bit4: Udisk认证模式支持: 1:进入Udisk认证模式,启动时间加快,支持suspend和resume等USB认证功能
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
 *                                           高速设备描述符
 *
 * Description: 高速设备设备描述符(USB2.0).
 *
 * Note(s)    : 包含一个配置, 一个接口, 两个Bulk端点.
 *********************************************************************************************************
 */
usb_device_descriptor HS_Device_Dscrptr =
{
    sizeof(usb_device_descriptor), //bLength
    USB_DT_DEVICE, //bDescriptorType

    0x0002, //bcdUSB, USB release version 2.0,0x200转小端模式,
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
 *                                           限定高速设备描述符
 *
 * Description: 限定高速设备设备描述符(USB2.0).
 *
 * Note(s)    : 包含一个配置, 一个接口, 两个Bulk端点.
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
