/*
********************************************************************************
*                       ACTOS
*                  system define
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : sysdef.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 14:38
********************************************************************************
********************************************************************************
*/
#ifndef _SYSDEF_H
#define _SYSDEF_H

//#define SDK35FD
#define EccSupport
//#define USE_EXTERNAL_RTC
//#define ColorSrcreen

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处定义actos版本
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SysVer  0x30

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处定义SD的容量
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SD_CAP              0x400000//0x2000000          //system disk cap,4096KB. flash block的倍数, 现取128KB
#define VM_CAP              0x010000//0x1200000                //vram & hds locationarea: 8MB. 需为flash block容量的倍数, 现取128KB
#define MI_CAP              0x0//0x1440000                //mtp infor area cap: 0x1440 sector
#define UI_CAP              0x002000//0x02000                //usb disk infor area cap


#define DEVCERTOFFSET       0x000080                //DEVCERT起始偏移（扇区为单位）占用16K, (VM_CAP内) 0~0x7f是系统VRAM
#define SYNCLISTOFFSET      0x0000a0                //SYNCLIST，占用0.5K
#define SECURETIMEOFFSET    0x0000a1                //SECURETIME，占用0.5K
#define SECCLKCHLGOFFSET    0x0000a2                //SECCLKCHLG，占用0.5K
#define SYNCLISTDYNAMIC     0x0000a3                //SYNCLIST，占用20K
#define SECCLKRESPOFFSET    0x0000a3                //SECCLKRESP，占用20K
#define XMLINFOOFFSET       0x0000a3                //XMLINFO，占用20K
#define METERCERTOFFSET     0x0000a3                //METERCERT，占用20K
#define METERCHLGOFFSET     0x0000a3                //METERCHLG，占用20K
#define METERRESPOFFSET     0x0000a3                //METERRESPOFFSET，占用20K
#define STATEQUERYOFFSET    0x0000a3                //METERRESPOFFSET，占用20K

#define HDSOFFSET           0x000100                //drm hds文件起始偏移（扇区为单位）(VM_CAP内)

#define FlashDiskOffset     0x9000                  //VM_CAP >>9 mtp infor(0x1450) start addr in ud area
#define VMFSOffset          0x8000
#define DrvOffset           0x410                  //(VM_CAP + MI_CAP + UI_CAP)>>9  usb disk start addr in ud area


#define DrvOffset0      DrvOffset                   //此参数用于usbdisk和文件系统做
#define DrvOffset1      0x0000                      //偏移用，他将同checkpartition与
#define DrvOffset2      0x0000                      //文件系统驱动的输入参数配合使用
#define DrvOffset3      0x0000                      //xxxxnnnn,其中xxxx为0~3表示偏移的种类(drvoffsetmod)
//nnnn表示ud号（checkpartition)或分区号(fs_init)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ic 版本
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define ICVer3935
#define ICVer3951

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// chip enable 分配
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef ICVer3935
#define BootFlashCE     0x06
#define SMCardCE        0x01
#define DisplayCE       0x03
#define ATJxxxx         0x2073
#endif
#ifdef ICVer3951
#define BootFlashCE     0x01
#define SMCardCE        0x06
#define DisplayCE       0x05
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//定义DMA延时
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define    WaitState    0x03

#define Watchdog
#ifdef  Watchdog
#define WatchdogTime        2
#endif

//为了加快开机速度，不定义此变量将不校验fontlib(*.$$$)
//#define  CheckFontLib

#endif  /*_SYSDEF_H*/

