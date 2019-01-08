/********************************************************************************
*                              AS212A
*                            Module: USB Host
*                 Copyright(c) 2001-2010 Actions Semiconductor,
*                            All Rights Reserved.
*
* History:
*      <author>    <time>           <version >             <desc>
*       xiaomaky   2009-12-4 13:54     1.0                build this file
********************************************************************************/



#ifndef _UHDRV_H
#define _UHDRV_H

#include "string.h"
#include "typeext.h"
#include "port.h"
#include "irq.h"
#include "message.h"
#include "uddrv.h"

/* error  meaning */
// #define GENERAL_OK                     0x00
// #define GENERAL_ERROR                  0x01
// #define DEVICE_NOT_IN                  0x02
// #define ENUMERATION_FAIL               0x03
// #define BULKSTART_FAIL                 0x04
// #define ERROR_NOT_SUPPORT_DEVICE       0X05
// #define ERROR_SETUP_OVERTIME           0X06
// #define ERROR_CONTROL_IN_DATA          0X07
// #define ERROR_STATUS_OVERTIME          0X08
// #define ERROR_NOT_SUPPORT_PROTOCOL     0X09
// #define ERROR_UNIT_READY_OVERTIME      0X0A
// #define ERROR_IN_DATA_WRONG            0X0B
// #define ERROR_LUN_BLOCK_SIZE           0X0C
// #define ERROR_BUS_RESET_OVERTIME       0X0D

// #define ERROR_TRANSFER_BUSY            0X80
// #define ERROR_IRQ_OCCUR                0X81
// #define ERROR_EP_BUFFER_NOT_EMPTY      0X82  /* not busy, but some endpoint sub-buffer is not empty */
// #define ERROR_OTG_STATE                0X83
// #define ERROR_EP_STALL                 0X84
// #define ERROR_NO_EP_HANDSHAKE		   0X85

// #define ERROR_PARAMETER_IN             0XF0
// #define ERROR_MIDIUM_NOT_PRESENT	   0XF1
// #define ERROR_MIDIA_CHANGED	   		   0XF2


/* function number  */
#define APINO_UH_Init                  APINO_UD_Init          //0x0000+API_STG_BaseNoB
#define APINO_UH_Exit                  APINO_UD_Exit          //0x0001+API_STG_BaseNoB
#define APINO_UH_SectorRead            APINO_UD_SectorRead    //0x00
#define APINO_UH_SectorWrite           APINO_UD_SectorWrite   //0x01
#define APINO_UH_Update                APINO_UD_Update        //0x04
#define APINO_UH_GetCap                APINO_UD_GetCap        //0x0002+API_STG_BaseNoB                   
#define APINO_UH_GetMaxLun             0x06
#define APINO_UH_GetCurrentLun         0x07
#define APINO_UH_GetSectorSize         APINO_UD_GET_SCTR_SIZE //0x1002+API_STG_BaseNoB



#ifdef __C251__


/*!
 *  structure of read or write info   \n
 *  \li SectorAddress: logical block address by 512BYTE.
 *  \li SectorNumber : sector number.
 *  \li RAMAddress   : RAM address.
 *
 */
/***********************************************************
函数名称：  UH_Init
函数功能：  uhost驱动的初始化函数，在DRV_Install的时候调用，DRV_Install
            之后也可以随时调用，用来获取usb连接状态相关的信息
函数输入：  mode:  盘符
函数返回：  0x01       success;
            0xff    没有插入miniA线
            0xfe    插入了miniA线，但是没有插入u盘，或者u盘没有连接
            0xfd    设备不支持
            //0xfc    数据传输错误，说明电气性能不好----------暂时不实现
            其他    resverd
*************************************************************/
//non_banked byte UH_Init(unsigned char mode);
#define UH_Init(mode) API_UH_Init(APINO_UD_Init,mode)
uint8 API_UH_Init(uint8 cmd, char mode);

/***********************************************************
函数名称：  UH_Exit
函数功能：  uhost驱动的退出函数，在DRV_UnInstall的时候调用
函数输入：
函数返回：
*************************************************************/
#define UH_Exit() API_UH_Exit(APINO_UH_Exit)
void API_UH_Exit(uint8 cmd);

/***********************************************************
函数名称：  UH_SectorRead
函数功能：  扇区读函数
函数输入：  shro:   参见struct UD_RW
函数返回：  0       success;
            0xff    地址输入错误，如不存在或越界的地址
            0xfe    U盘上报了读失败
            其他    resverd
*************************************************************/
#define UH_SectorRead(shro) API_UH_SectorRead(APINO_UH_SectorRead,shro)
uint8 API_UH_SectorRead(uint8 cmd, UD_RW *shro);

/***********************************************************
函数名称：  UH_SectorWrite
函数功能：  扇区写函数
函数输入：  shro:   参见struct UD_RW
函数返回：  0       success;
            0xff    地址输入错误，如不存在或越界的地址
            0xfe    U盘上报了写失败
            其他    resverd
*************************************************************/
#define UH_SectorWrite(shro) API_UH_SectorWrite(APINO_UH_SectorWrite,shro)
uint8 API_UH_SectorWrite(uint8 cmd, UD_RW *shro);

/***********************************************************
函数名称：  UH_GetMaxLun
函数功能：  获取分区数目
函数输入：
函数返回：  0～15   分区数目，根据协议规定取值范围是0～15。
                    注：返回3，代表0～3，共四个盘
            其他    resverd
*************************************************************/
#define UH_GetMaxLun() API_UH_GetMaxLun(APINO_UH_GetMaxLun)
uint8 API_UH_GetMaxLun(uint8 cmd);

/***********************************************************
函数名称：  UH_TestLunReady
函数功能：  测试对应分区有没有加载磁盘
函数输入：  lun     待测试的分区号
函数返回：  0       该分区加载了磁盘
            0xfa       该分区没有加载磁盘
            0xfc       错误的lun参数等
            其他    resverd
*************************************************************/
//uint8 UH_TestLunReady(unsigned char lun);

/***********************************************************
函数名称：  UH_GetCap
函数功能：  获得对应磁盘的大小
函数输入：  lun     对应磁盘的分区号
函数返回：  分区的大小，以512 byte为单位，一个block大于512的u盘，uhost驱动会将其转换成以512 byte为单位
            0       error
*************************************************************/
#define UH_GetCap() API_UH_GetCap(APINO_UH_GetCap)
uint32 API_UH_GetCap(uint8 cmd);

/***********************************************************
函数名称：  UH_GetSectorSize
函数功能：  获得u盘一个Sector的大小
函数输入：  lun     对应磁盘的分区号
函数返回：  u盘一个Sector的大小，如果返回n，则一个Sector的大小是n*512 byte
            0       error
*************************************************************/
#define UH_GetSectorSize() API_UH_GetSectorSize(APINO_UH_GetSectorSize)
uint8 API_UH_GetSectorSize(uint8 cmd);

/***********************************************************
函数名称：  UH_TestUdiskReady
函数功能：  测试当前连接状态
函数输入：
函数返回：  0       成功。miniA插入，u盘也插入，并连接。此时可以进行读，得到容量等操作
            0xff    没有插入miniA线
            0xfe    插入了miniA线，但是没有插入u盘，或者u盘没有连接
            0xfd    设备不支持
            其他    resverd
注：会将这个函数做成UH_Init的简化版，主要通过查询一些软件标志位来返回对应的值
*************************************************************/
//uint8 UH_TestUdiskReady(void);

/***********************************************************
函数名称：  UH_GetCurrentLun
函数功能：  获取当前激活的磁盘分区号
函数输入：
函数返回：  0～15   当前激活的磁盘分区号
            其他    resverd
*************************************************************/
#define UH_GetCurrentLun() API_UH_GetCurrentLun(APINO_UH_GetCurrentLun)
uint8 API_UH_GetCurrentLun(uint8 cmd);

/***********************************************************
函数名称：  UH_SetCurrentLun
函数功能：  设定当前激活的磁盘分区号，如果不设定的话，默认为0
函数输入：  lun     对应磁盘的分区号
函数返回：  0       success
            0xfc       错误的lun参数等
            0xfd    设备不支持
            其他    resverd
*************************************************************/
//uint8 UH_SetCurrentLun(unsigned char lun);

/***********************************************************
函数名称：  UH_WordRead
函数功能：  读取某一个lba，某个偏移量的双字节数据
函数输入：  UD_RW
            typedef struct
            {
                dword   lba;        //word所在扇区内的地址
                char    reserv;     //未用
                void    byteaddr;   //word在扇区内的字节偏移
                byte    reserv;     //未用
            }UD_RW;
函数返回：  0       success
            1       0xff    地址输入错误，如不存在或越界的地址
            其他    resverd
*************************************************************/
//int16 UH_WordRead(UD_RW *rwaddr);

/***********************************************************
函数名称：  UH_DwordRead
函数功能：  读取某一个lba，某个偏移量的四字节数据
函数输入：  UD_RW
            typedef struct
            {
                dword   lba;        //word所在扇区内的地址
                char    reserv;     //未用
                void    byteaddr;   //word在扇区内的字节偏移
                byte    reserv;     //未用
            }UD_RW;
函数返回：  0       success
            1       0xff    地址输入错误，如不存在或越界的地址
            其他    resverd
*************************************************************/
//int16 UH_DwordRead(UD_RW *rwaddr);


/***********************************************************
函数名称：  UH_Update
函数功能：  空函数，FS_FClose()关闭一个文件时被调用
函数输入：
函数返回：
*************************************************************/
#define UH_Update() API_UH_Update(APINO_UH_Update)
void API_UH_Update(uint8 cmd);


/*
message.h：
#define MSG_UH_IN               0xc0   //U盘插入
#define MSG_UH_OUT              0xc1   //U盘拔出
#define MSG_UH_UDISK_OUT        0xc2   //卡槽中的卡拔出
*/
#define     ISR_CHECK_MODE
extern uint8  sHOST_Glbal;
#define     g_uhost_state               sHOST_Glbal

/* function parameter for reset_usb_controller() */
#define     CONTROLLER_INIT             0x00
#define     CONTROLLER_ENABLE           0x01
#define     ISR_UHOST                   0x02

#define     old_isr                     (*(uint8 *)0x27a9)

/* OTG state machine */
#define FSM_A_HOST                     0x03 /* The USB FSM State: A_host */

#define GENERAL_OK                     0x00


#define ERROR_DEVICE_NOT_IN            0x02
#define ERROR_BUS_RESET_OVERTIME       0x06

#define     BULK_IN_EP_NUM                                                  2 // BULK IN 端点号,
#define     BULK_OUT_EP_NUM                                                 1 // BULK OUT 端点号

/* 1) 非 卡U对拷场景下，使用全速模式，URAM（512byte）做端点缓冲区，IN端点和OUT端点各用256byte(= 64 * 4)
 * 2) 卡U对拷场景下，使用高速模式，PCMRAM（4KB）做端点缓冲区，IN端点和OUT端点各用2KB(= 512byte * 4)
 */
#ifdef  __CARD_UDISK_TRANSFER__ // 卡U对拷的场景, Bulk in/out各用2KB

#define     EP_MAX_BUF_SIZE_BULK_IN                                         0x800 // 2KB
#define     EP_MAX_BUF_SIZE_BULK_OUT                                        0x800 // 2KB

#else

#define     EP_MAX_BUF_SIZE_BULK_IN                                         192 // 192 = 64 * 3
#define     EP_MAX_BUF_SIZE_BULK_OUT                                        128 // 128 = 64 * 2

#endif  // #ifdef  __CARD_UDISK_TRANSFER__

#ifdef  __CARD_UDISK_TRANSFER__ // 卡U对拷的场景, Bulk in/out各用2KB
#define     EP_BUF_STA_ADDR_BULK_IN     0x380 // 0x380 bytes的空间指 "URAM + FIR_RDS_RAM"
#define     EP_BUF_STA_ADDR_BULK_OUT    (EP_BUF_STA_ADDR_BULK_IN + EP_MAX_BUF_SIZE_BULK_IN)
#else
#define     EP_BUF_STA_ADDR_BULK_IN     192 // 192 bytes的空间指 EP0_IN、EP0_OUT和EP3用的各64 bytes的大小
#define     EP_BUF_STA_ADDR_BULK_OUT    (EP_BUF_STA_ADDR_BULK_IN + EP_MAX_BUF_SIZE_BULK_IN)
#endif  // #ifdef  __CARD_UDISK_TRANSFER__

#define WATCHDOG_CLEAR()                ClearWatchDog()
#define delay_ms(x)                     TM_DelayMS(x)

// enable external USB interrupt request(这是USB中断对外的总使能位)
#define     ENABLE_EXT_USB_IRQ()                            (USBEIRQ = USBEIRQ | 0x08)
// disable external USB interrupt request(这是USB中断对外的总使能位)
#define     DISABLE_EXT_USB_IRQ()                           (USBEIRQ = USBEIRQ & 0xf7)
/* close USB clock (这样USB controller部分可以省功耗，DP&DM的时钟源应该是AHBCLK，所以关闭的clock跟DP&DM的时钟源无关).
 * 关闭PLLEN即可,其他像PLLEN, USBRESET 和 GoSuspend都不用管,另外Phy_DALLUALLEN是内部用的,也可关闭
 */
#define     CLOSE_USB_CONTROLLER_CLOCK() \
            (USB_PHYCTRL = USB_PHYCTRL & 0x3f)
#define     OPEN_USB_CONTROLLER_CLOCK()  \
            (USB_PHYCTRL = USB_PHYCTRL | 0xc0)

/* enable Line status detect, enable 15Kohm DP/DM pull down, disable 500Kohm DP/DM pull up resistor */
#define     ENABLE_DEVICE_DETECT() \
            (DPDMCTRL = (DPDMCTRL & 0xf0) | 0x10)
#define     DISABLE_DEVICE_DETECT() \
            (DPDMCTRL = DPDMCTRL & 0xef)
#pragma ROM(HUGE)
void reset_usb_controller (uint8 call_from);
#endif /* __C251__ */

#endif /*_UHDRV_H */











