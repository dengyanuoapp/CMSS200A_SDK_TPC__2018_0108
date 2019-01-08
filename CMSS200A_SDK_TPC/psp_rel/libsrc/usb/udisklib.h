/*
 ********************************************************************************
 *                       ACTOS
 *               device manager head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : usbdisk.inc
 * By     : Gongee
 * Version: 1> v1.00     first version     2005-11-20 11:35
 ********************************************************************************
 */
#ifndef __USBDISK_INC__
#define __USBDISK_INC__

#include "actos.h"
#include "as260x.h"
#include "stringH.h"
#include "typeext.h"
#include "usb.h"
#include "usbdisk.h"
#include "device.h"
#include "irq.h"
#include "uddrv.h"
//#include "usbdisk.h"
//#include "adfu.h"
#include "otg.h"

/*
 * The following MACROS create references for the bank number of segments
 * and classes.
 */
//#define SROM_PS_BNK(n) ((unsigned int) &_PR_##n##_B_)
/* segment bank address */
//extern unsigned char far  _PR_BULKCMD_C_B_;
//extern unsigned char far  _PR_SETUPPKT_C_B_;

//#define _PRINT_DEBUG_
#define FIFO_TRANS_MODE
//#define _FPGA_VERTION_

/* 获取函数bank号*/
#define BYTE2_OF(x) ((unsigned char)((((unsigned long)((void far*)x))&0xff0000) >>16))

/* bank之间函数调用,使用LCALL */
#define     LCALL(x)						(*((void (*)()) ((uint16)&x)))

#define     USB_IC_A_BUG               		//A版ICbug规避，1kflash，umode下，udma异常中止;两台机同时操作问题.
//#define     SDDISKCAP                       SD_CAP/0x200
#define     VM_FS_FLAG       				0xF000
//#define     USERSPACESTARTADDR              SDDISKCAP + DrvOffset           //用户空间的起始扇区号,8sector对齐
//#define     ENCRYPTINFOSECTOR               USERSPACESTARTADDR-1
//DRMInfo的最小地址(sector,3Mbyte),此地址之后才被认为可能是DRMINFO,用于屏蔽对SD区的读
#define     DRMINFOADDR                     0x1800
#define     SYSINFORAMADDR                  0x800       //SYSINFO在ZRAM1中的地址
#define     ClearWatchdog 		            ClearWatchDog
/*
 *********************************************************************************************************
 *                                       USB STATIC VALUE
 *********************************************************************************************************
 */
#define     URAM_BUFFER_LENGTH              0x0400      //1K
#define     URAMXADDR                       0x4000
#define     URAMYADDR                       (URAMXADDR+URAM_BUFFER_LENGTH)
#define     DATABUFFER                      0x4000
#define     DATABUFFERLENGTH                0x08        //8K,以512bytes为单位
#define     URAMSTARTADDR                   0x0000
#define     ENCRYPTINFOURAMADD              DATABUFFER                  //EncryptInfoSector读到Uram B2中的起始地址
#define     UDISKSTARTADDRURAMADD           DATABUFFER                  //UdiskStartAddr读到Uram B2中的起始地址
#define     MAX_NORMAL_RW_SETCORS           0x08
#define     MAX_FIFO_RW_SETCORS             0x80                        //MAX_FIFO_RW_SETCORS 必须<=0x80,因DMA最大传输大小为0x10000个byte
#define     USBFIFO_TO_DRAM                 0x04                        //[bit7:4]=0,目标地址为dram space; [bit3:0]=4,源地址为usbfifo
#define     DRAM_TO_USBFIFO                 0x40                        //[bit7:4]=4,目标地址usbfifo; [bit3:0]=0,源地址在dram space,sequential access
#define     USBFIFO_TO_CARDFIFO             0x64
#define     CARDFIFO_TO_USBFIFO             0x46
#define     USBFIFO_TO_FLASHFIFO            0x24      	// spi Nor flash
#define     FLASHFIFO_TO_USBFIFO            0x42 		// spi Nor flash
//                                               |  |
//                          set DMA clk          | 00h:30M  04h:60M
//                                               |
//                          set wait state: 0x10*0,1,2,...: 0wait, 1wait, 2wait, 3wait...

#define     ENDPOINT_0                      0
#define     ENDPOINT_A                      1
#define     ENDPOINT_B                      2
#define     ENDPOINT_C                      3

//BREC在Flash中起始Block号,默认为一个byte,如果大于一个byte长度需修改相应函数
#define     BREC_START_BLOCK                2

#define     USBIDLECOUNTERVALUE             2           //2秒
#define     USBEJECTCTRVALUE                3           //3秒
#define     COUNTERVALUE_30S                6           //6秒
/*
 *********************************************************************************************************
 *                                       USB Protocol requeset value
 *********************************************************************************************************
 */
//bmRequestType
#define     GET_STATUS                      0
#define     CLR_FEATURE                     1
#define     SET_FEATURE                     3
#define     SET_ADDRESS                     5
#define     GET_DSCRPTOR                    6
#define     SET_DSCRPTOR                    7
#define     GET_CONFIG                      8
#define     SET_CONFIG                      9
#define     GET_INTERFACE                   10
#define     SET_INTERFACE                   11
#define     SYNCH_FRAME                     12
//descriptor type
#define     DEVICE                          1
#define     CONFIG                          2
#define     STRING                          3
#define     INTERFACE                       4
#define     ENDPOINT                        5
#define     DEVICE_QUALIFIER                6
#define     OTHER_SPEED_CONFIGURATION       7

//standard feature
#define     DEV_RMT_WAKE                    1
#define     EP_HALT                         0
#define     TEST_MODE                       2

//test mode selector
#define     Test_J                          1
#define     Test_K                          2
#define     Test_SE0_NAK                    3
#define     TEST_PACKET                     4
#define     TEST_FORCE_ENABLE               5

//bRequest
#define     RD_STD_DEV                      0x80
#define     RD_STD_IF                       0x81
#define     RD_STD_EP                       0x82

#define     WR_STD_DEV                      0
#define     WR_STD_IF                       1
#define     WR_STD_EP                       2

#define     RD_VEND_DEV                     0xc0
#define     RD_VEND_IF                      0xc1
#define     RD_VEND_EP                      0xc2

#define     WR_VEND_DEV                     0x40
#define     WR_VEND_IF                      0x41
#define     WR_VEND_EP                      0x42

#define     RD_CLASS_IF                     0xa1         //read class interface
#define     WR_CLASS_IF                     0x21         //write class interface
//command set for HID class
#define     GET_REPORT                      1
#define     GET_IDLE                        2
#define     GET_PROTOCOL                    3
#define     SET_REPORT                      9
#define     SET_IDLE                        10
#define     SET_PROTOCOL                    11

//class descriptor type
#define     HID                             0x21
#define     REPORT                          0x22
#define     PHYSICAL                        0x23

#define		RW_DYNAMIC						1
#define		RW_FIX							2

#define     GETDISKINFO                     0x01
#define     SETPASSWDOK                     0x02
#define     SETNAMEORPASSWDFAIL             0x03
#define     NAMEANDPASSWDOK                 0x04
#define     NAMEORPASSWDFAIL                0x05
#define     UDUNBUSYINSWITCH2ADFU           0x06
#define     UDBUSYINSWITCH2ADFU             0x07

#ifdef __C251__
/*
 *********************************************************************************************************
 *  structure  declare
 *********************************************************************************************************
 */
//加密信息交流协议
typedef struct
{
    uint8 CMDSignature[7]; //0x00
    uint8 CMDIDRecord; //0x07
    uint8 SystemInfoRecord; //0x08
    uint8 Phy0EquLog0Record; //0x09
    uint8 SystemInfoReserved[6]; //0x0a
    uint32 FormatCapacityRecord; //10h     //4Bytes
    uint32 VidPidRecord; //14h     //4Bytes
    uint32 CardCapacityRecord; //18h
    uint32 CardCapacityReserved; //1ch
    uint8 SplitDiskRecord; //20h     //1Bytes
    uint8 IfEncryptRecord; //21h     //1Bytes
    uint8 CardExistRecord; //22h     //1Bytes
    uint8 CardExistReserved[0x0d]; //23h
    uint8 OldUserNameLengthRecord; //30h     //1Bytes
    uint8 OldUserNameRecord[0x0c]; //31h     //12Bytes
    uint8 OldUserNameReserved[3]; //3dh
    uint8 OldPasswdLengthRecord; //40h     //1Bytes
    uint8 OldPasswdRecord[0x0c]; //41h     //12Bytes
    uint8 OldPasswdReserved[3]; //4dh
    uint8 UserNameLengthRecord; //50h     //1Bytes
    uint8 UserNameRecord[0x0c]; //51h     //12Bytes
    uint8 UserNameReserved[3]; //5dh
    uint8 PasswdLengthRecord; //60h     //1Bytes
    uint8 PasswdRecord[0x0c]; //61h     //12Bytes
    uint8 PasswdReserved[3]; //6dh
    uint32 PhySecWRAddrRecord; //70h     //4Bytes
    uint16 PhySecWRLengthRecord; //74h     //2Bytes
    uint16 PhySecWRLengthReserved[0x05]; //76h
    uint32 DiskACapacityRecord; //80h     //4Bytes
    uint8 DiskACapacityReserved[0x0c]; //84h
    uint32 DiskBStartAddrRecord; //90h     //4Bytes
    uint32 DiskBCapacityRecord; //94h     //4Bytes
    uint8 DiskBCapacityReserved[8]; //98h
    uint32 DiskCStartAddrRecord; //0a0h    //4Bytes
    uint32 DiskCCapacityRecord; //0a4h    //4Bytes
    uint8 DiskCCapacityReserved[8]; //a8h
    uint8 Reserved0[0xf0]; //b0h
    uint8 PhysicsSec0TagRecord[4]; //1a0h    //4Bytes
    uint8 PhysicsSec0TagReserved[0x0c]; //1a4h
    uint8 WRProtectInfoRecord; //1b0h    //1Bytes
    uint8 WRProtectInfoReserved[0x0f]; //1b1h
    uint8 Reserved1[0x30]; //1c0h
    uint16 DataSectorsForEncOkRecord; //1f0h	  //2Bytes
    uint8 Reserved2[0x0e]; //1f2h
} encrypt_info;

typedef struct
{
    uint16 source_addr;
    uint16 dst_addr;
    uint16 length;
    uint8 pagenum;
} DMA_DT_Trnsfr_t;

typedef struct
{
    uint32 _dCBWSignature;
    uint32 _dCBWTag;
    uint32 _dCBWDataTransferLength;
    uint8 _bmCBWFlags;
    uint8 _bCBWLUN;
    uint8 _bCBWCBLength;
    uint8 _CBWCB[9];
} CBW_WRAP;

typedef struct
{
    uint32 _dCSWSignature;
    uint32 _dCSWTag;
    uint32 _dCSWDataResidue;
    uint8 _bCSWStatus;
} CSW_WRAP;

typedef struct
{
    uint32 LastLogBlockAddr;
    uint32 BlockLengthInBytes;
} cap_info;

typedef struct
{
    //Capacity List Header
    uint8 reserved[3];
    uint8 CapListLength;

    //Current/Maximum Capacity Descriptor
    uint32 CurrentCap;
    uint8 CurrentCapDescriptorCode;
    uint8 CurrentCapBlockLength[3];

    //Formattable Capacity Descriptor
    uint32 FomatableCap;
    uint8 FomatableCapDescriptorCode;
    uint8 FomatableCapBlockLength[3];
} format_cap_info;

typedef struct
{
    uint32 lba;
    uint8 sectornum;
    uint8 dstaddrh;
    uint16 dstaddr;
} ud_rw_struct_t;

typedef struct
{
    uint32 DataAreaStartAddress; //data area(after the first cluster) start address
    uint8 DataAreaOffset; //DBR  start address
    uint32 DBRStartAddress; //data offset, to align 8Kbyte
    uint8 rw_dbr_flag; //if DBR is operated, set flag.
    uint16 BPB_RootEntCnt; //FAT16中根目录项数,FAT32为0
    uint32 FATSize;
    uint8 SectorPerCluster;
    uint16 ReservedSecotr;
    uint8 mbr_exit_flag; //MBR存在标记
} udisk_fs_info_t;

/*
 *********************************************************************************************************
 *  function  declare
 *********************************************************************************************************
 */
//all
#pragma ROM(HUGE)
extern void U20_ISRMain(void);
extern void ifIsConnect(void);
#pragma ROM(HUGE)
extern void OTG_PHY_Setting(uint8 addr, uint8 value);
extern void sCardCapCheck(void);
extern void smDelay(uint8 mscount);
extern void EndianReverse(uint8 *data_p, uint16 data_length);
//extern void ClearWatchdog(void);

//init
extern uint8 UDiskInitInternal(void);
extern void UDiskCardInitInternal(void);
extern void Udisk_Exit_Handle(void);

//setup
#pragma ROM(HUGE)
extern void Setup_Packet_Handle(void);
extern void Control_Read(char *data_p, int data_length, int request_length);
extern void NoData_PhaseReturn(void);
extern void Set_EPA_BulkInMode(void);
extern void Set_EPB_BulkOutMode(void);
extern void Rest_Endp_Toggle(void);
extern void Stall_Endpoint(char Ep_Num);
extern void Recover_Stall_Endpoint(char Ep_Num);
extern void TestModeTest(byte mode_selector);
extern void Set_HighSpeed_Mode(void);
#pragma ROM(HUGE)

//bulk
#pragma ROM(HUGE)
extern void Bulk_Out_Handle(void);
extern void CMD_Read10_(void);
extern void CMD_Write10_(void);
extern void ReportCSW(uint8 err_type);
extern void bulk_out_back(void);

extern uint8 sReadCheckForEncryp(void);
extern uint8 sWriteCheckForEncryp(void);
extern uint8 sEncrypParseInWR(void);

extern void DMA_Data_Transfer(DMA_DT_Trnsfr_t *dma_struct);
extern void Wait_PrevDMA_TransEnd(void);
extern void Send_CSW(void);
extern void Send_Data_To_PC(uint8 *data_p, uint16 length);
extern void EPB_OutRdy_ShortPacketNAK(void);
extern void EPB_OutRdy_FIFOFullNAK(void);
extern void EPA_In_Rdy(void);
#pragma ROM(HUGE)

extern void sFlash_Wrts(uint8 rw_type, uint32 lba_addr, uint8 *dst_buffer);
extern void sFlash_Rds(uint8 rw_type, uint32 lba_addr, uint8 *dst_buffer);
extern void sCardWrite(uint8 *dst_buffer);
extern void sCardRead(uint8 *dst_buffer);
extern void sReadSectorsToBuff(uint8 *dst_buffer);
extern void sProgramSectorsToStorage(uint8 *dst_buffer);
extern void sFlashDrvCheck(void);
extern void Compute_LBA(void);
extern void Partion_MBR_Initial(void);
extern void Udisk_Parse_MBR(void);

//adfu
#pragma ROM(HUGE)
extern void Access_Internal_RAM_Command(void);
extern void Access_Nand_Flash_Command(void);
extern void Switch_Firmware_Command(void);
extern void Detach_Device_Command(void);
extern void Polling_Ready_Command(void);
extern void Calling_Entry_Command(void);
extern void Write_Internal_RAM(void);
extern void Read_Internal_RAM(void);
#pragma ROM(HUGE)

//host
extern void usb_detectfunc_dereg(void);
extern void USB_DetectFnc_Initial(void);

/*
 *********************************************************************************************************
 *  varible  declare
 *********************************************************************************************************
 */

#if 0
extern uint16 pic_data_offset[8];
extern region_t pic_region[8];
extern uint8 pic_data_buff[2048];
//extern const uint16 pic_id[8];

#endif

//卡状态标志,全局变量,在Timer中断中扫描卡电压得到(不支持卡时此标记位总是为0)
extern uint8 CardDetectState;

//for const
extern uint16 const far fake_udisk_cap;

extern const uint8 FFSDCardDrvName[11];
extern const uint8 FFMSCardDrvName[];
extern const uint8 FFCardBaseDrvName[];
extern const uint8 ActionsFlag[11];
extern const uint8 actions_solution_flag[18];
extern const uint8 MBR_Bak[224];
extern const uint8 ActCMDSignatureC[7];
extern const uint8 ActCMDSignatureS[7];
extern const uint8 Phy0DiskTag[4];
extern const uint8 decrypt_key;

//for globle varible
extern uint8 CurrentBankNo;
extern uint32 OldIsrPtr;
extern uint8 sfr_bak;
extern uint32 SP_Keeper;
extern uint8 sFlashLedFlag;
extern uint8 *sFlashLed;
extern uint8 TempBuffer[0x40];
extern uint8 MCSR2_bak;
extern uint8 MCSR4_bak;
//extern uint8 MCUPLL_bak;

//for disk information
extern uint8 OSType;
extern uint8 MaxLUN;
extern uint8 DiskAppearSelect;

extern uint8 AutoRunDiskFlag;
extern uint8 bWriteCD_ROM;

extern uint32 SDDiskCap;
extern uint32 VMCap;
extern uint32 MICap;
extern uint32 UICap;
extern uint32 AutoRunDiskCapacity;
extern uint32 udisk_total_capacity;

extern uint32 AutoRunDiskStartAddr;
extern uint32 UdiskStartAddr;
extern uint32 EncryptInfoSector;
extern uint32 EncryptDiskBStartAddr;
extern uint32 EncryptDiskCStartAddr;

extern uint8 flash_type;
extern uint32 flash_capacity;
extern uint16 flash_page_sctrnum;
extern uint8 FlashWPFlag;
//extern uint8 flash_vram_vld;

extern uint8 ShowMultiDiskFlag;
extern uint32 CardCapacity;
extern uint8 CardExistFlag;
extern uint8 CardWPFlag;
extern uint8 RWCardError;

extern uint8 flashorcard_rwflag;
extern ud_rw_struct_t SD_UDRWStruct;
extern uint32 LBA;
extern uint32 sectors_trans_once;
extern uint32 udisk_rw_sctr_num;
extern uint8 illegalLBAFlag;
extern uint8 flash_normal_rw_sctrs;
extern uint8 flash_disk_write_flag;
extern uint8 fifo_trans_mode;

extern uint8 IsNotFormat;
extern uint8 bMediaChange;
extern uint8 MediaChgFlag;

extern udisk_fs_info_t UdiskFSInfo;
extern uint8 set_fake_ucap_flag;
extern uint8 SectorType;

//for udisk status record
extern uint8 USB_State;
extern uint8 TransferFlag;

extern uint8 StatusCheckFlag;
extern uint8 USBBusy;

extern uint8 SwitchToADFUFlag;
extern uint8 CanSwitchToADFU;
extern uint8 detach_cmd_rcv;
extern uint8 FWOpStatus;
extern uint8 IsAdfuCondition;
extern uint8 IsInAdfuState;
extern uint8 PartiEjectFlag;
extern uint8 PullLineFlag;
extern uint8 PcEjectFlag;
extern uint8 SplitDiskFlag;
extern uint8 EjectFlag;
extern uint8 usb_pwr_suspnd_fnsh;
extern uint8 Suspend_Flag;
extern uint8 DisableSuspendFlag;
extern uint8 UpOrDownLoad;
extern uint8 UpOrDownLoadBak;

extern uint8 UsbIdleCounter;
extern uint8 EjectTimeCtr;
extern uint8 AdfuTimerCtl;
extern uint8 connect_idle_cnt;
extern uint8 Counter_30s;
extern uint8 PrevRTCTimerValue1;
extern uint8 PrevRTCTimerValue2;
extern uint8 prv_connect_idle_tm;

extern uint8 cbw_count;
extern uint8 bTestNoready;
extern uint8 short_pckt_flag;
extern uint8 ErrorReportFlag;
extern uint8 ErrorIndicator;
extern uint8 IsReadCapAndReadRec0;
extern uint8 Removable;

//for adfu
extern uint8 LogicOrPhy;
extern uint8 AccessFlash;

extern uint8 brec_count;
extern uint8 first_brec_sector;
extern uint16 total_breclogo_sector;
extern uint16 phy_block_addr;
extern uint16 brec_check_sum;

extern uint32 CallingTaskAdd;
extern uint8 CallingTaskFlag;
extern uint16 CallingTaskResult;
extern uint16 StatusLength;
extern uint8 adfu_flag;

//for EnDecrypt
extern uint8 AccessFlashPhySec1;
extern uint8 EncryptFlag;
extern uint8 PasswdPassOk;
extern uint8 CMDPerformCondition;
extern uint16 ActWRLength;
extern uint32 ActWRLBA;
extern uint8 Phy0Sec1RdCounter1;
extern uint8 Phy0Sec1RdCounter2;
extern uint8 Phy0EquLog0Flag;

//usb bulk packet
extern CBW_WRAP CBW_data_buffer;
extern CSW_WRAP CSWBuffer;

extern uint8 CMD_Inquiry_information_FOB[60];
extern uint8 CMD_Inquiry_information_CDROM[60];

extern uint8 *Cap_Info_DiskCDROM_p;
extern uint8 *Cap_Info_DiskA_p;
extern uint8 *Cap_Info_DiskB_p;
extern uint8 *Cap_Info_DiskC_p;
extern uint8 *Cap_Info_DiskCard_p;
extern cap_info Cap_cmd_info_DiskCDROM;
extern cap_info Cap_cmd_info_DiskA;
extern cap_info Cap_cmd_info_DiskB;
extern cap_info Cap_cmd_info_DiskC;
extern cap_info Cap_cmd_info_DiskCard;
extern format_cap_info Formatcap_cmd_info_DiskA;
extern format_cap_info Formatcap_cmd_info_DiskB;
extern format_cap_info Formatcap_cmd_info_DiskC;
extern format_cap_info Formatcap_cmd_info_DiskCard;

extern uint8 Time_Protect_Page_information[16];
extern uint8 All_Page_information[52];
extern uint8 Request_Sense_cmd_info_NoError[18];
extern uint8 Request_Sense_cmd_info_CommandReset[18];
extern uint8 Request_Sense_cmd_info_MediaProtect[18];
extern uint8 Request_Sense_cmd_info_MediaChg[18];
extern uint8 Request_Sense_cmd_info_MediaNotPresent[18];

extern uint8 CDRom_Request_Sense_command_Invalid_Operation_Code[18];
extern uint8 Read_TOC_command_information[24];
extern uint8 CDRom_ModeSense_information[8];
extern uint8 Mechanism_Status_command_information[13];
extern uint8 CDRom_Request_DISCINFORMATION[28];

//for setup
extern uint8 udisk_setting;
extern uint8 set_pkt_size;
extern uint8 usb_reset_enter_cnt;
extern uint16 max_pkt_size;

extern usb_ctrlrequest setup_paket_data;
extern uint8 has_set_usb_dscrptr;
extern uint8 ConfigValue;
extern uint8 AlternateSetting;
extern uint16 DeviceStatus;
extern uint16 InterfaceStatus;
extern uint16 EndPoint1Status;
extern uint16 EndPoint2Status;

extern usb_languageid_descriptor language_string;
extern usb_manufacure_descriptor manufacure_string;
extern usb_product_descriptor device_name_string;
extern usb_serialnum_descriptor SerialNumber;

extern usb_device_descriptor HS_Device_Dscrptr;
extern usb_config_descriptor HS_Config_Dscrptr;
extern usb_interface_descriptor HS_Interface_Dscrptr;
extern usb_endpoint_descriptor HS_EndpointA_Dscrptr;
extern usb_endpoint_descriptor HS_EndpointB_Dscrptr;

extern usb_qualifier_descriptor HS_Device_Qualifier_Dscrptr;
extern usb_config_descriptor HS_OtherSpeed_Config_Dscrptr;
extern usb_interface_descriptor HS_OtherSpeed_Interface_Dscrptr;
extern usb_endpoint_descriptor HS_OtherSpeed_EndpointA_Dscrptr;
extern usb_endpoint_descriptor HS_OtherSpeed_EndpointB_Dscrptr;

/*
 *********************************************************************************************************
 *  other function declare
 *********************************************************************************************************
 */
//#define myenable_interrupt()  {(*((char data*)0x007f))--;if(!(*((char data*)0x007f))) EA=1;}
//#define mydisable_interrupt() {EA=0; (*((char data*)0x007f))++;}

//unsigned char DRV_CheckCard(unsigned char cardno);

#endif /* __C251__*/

#endif /*__USBDISK_INC__*/
