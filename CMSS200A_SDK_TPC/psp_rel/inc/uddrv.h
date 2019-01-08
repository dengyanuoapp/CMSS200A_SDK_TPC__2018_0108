/*
********************************************************************************
*                       ACTOS
*                  usb disk driver head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : uddrv.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 11:34
********************************************************************************
********************************************************************************
*/
#ifndef _UDDRV_H
#define _UDDRV_H

#define  MODE_FAST  0x00
#define  MODE_SLOW  0x01
#define  FULL_SPEED  0x00
#define  HIGH_SPEED  0x02

#include "actapi.h"
#include "TypeExt.h"
#include "sysdef.h"
#include "driver.h"
#include "port.h"
#include "irq.h"

#define APINO_UD_Init             0x00
#define APINO_UD_Exit             0x01
#define APINO_UD_SectorRead       0x02
#define APINO_UD_SectorWrite      0x03
#define APINO_UD_Update           0x04
#define APINO_UD_GetCap           0x05
#define APINO_UD_WPDetect         0x06
#define APINO_UD_FlashType        0x07
#define APINO_UD_GET_SCTR_SIZE    0x08
#define APINO_UD_STRAT_READ_WRITE 0x09
#define APINO_UD_GET_REMAIN       0x0A
#define APINO_UD_LOSE             0x0B
#define APINO_UD_DIR_STATUS				0x0C
#define APINO_UD_ERASE            0x07
#define APINO_UD_ClrVram            0x0B


#define APINO_UD_SDCardInit             0x00
#define APINO_UD_SDCardExit             0x01
#define APINO_UD_SDCardSectorRead       0x02
#define APINO_UD_SDCardSectorWrite      0x03
#define APINO_UD_SDCardUpdate           0x04
#define APINO_UD_SDCardGetCap           0x05
#define APINO_UD_SDCardWPDetect         0x06
#define APINO_UD_SDCardStartTran    		0x07
#define APINO_UD_SDCardEndF2F						0x08
//#define APINO_UD_SDCardWordRead         0x02
//#define APINO_UD_SDCardDwordRead        0x03
//#define APINO_UD_SDCardWriteInit        0x05
//#define APINO_UD_SDCardReadInit         0x06
//#define APINO_UD_SDCardPhyGetCap        0x00
//#define API_UD_SetHighFreq      0x0001+API_STG_BaseNoB
//#define API_UD_SetLowFreq       0x1001+API_STG_BaseNoB



//#define APINO_UD_PhyGetCap        0x0001+API_STG_BaseNoA
//#define API_UD_GetCap           0x0002+API_STG_BaseNoA
//#define API_UD_SetHighFreq      0x0005+API_STG_BaseNoA
//#define API_UD_SetLowFreq       0x1005+API_STG_BaseNoA
#define APINO_UD_EblSvMd          0x0006+API_STG_Base
#define APINO_UD_DisblSvMd        0x1006+API_STG_Base

#ifdef ICVer3951
#define EccSupport
#endif

#ifdef __A251__
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* for assemble
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define SDCardFFEntry   0x7f80


//#define APINO_UD_Get_Flash_Page_Size      0x09
// 8 and 9 have used for bred read write


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//for card fast driver

mUD_SDCardInit	macro
MOV		R11, #APINO_UD_SDCardInit
ECALL  RSTSDFFAPI
endm

mUD_SDCardExit  macro
MOV  R11, #APINO_UD_SDCardExit
ECALL  RSTSDFFAPI
endm



//for flash and card
mUD_SectorRead  macro
MOV		R11, #APINO_UD_SectorRead
ECALL     RSTSTGAPI
endm
/*
mGD_SectorRead  macro
        MOV		R10, #API_UD_SectorRead
        LCALL     RSTSTGAPI
        endm
*/
mUD_SDCardSectorRead macro
MOV		R11, #APINO_UD_SDCardSectorRead
ECALL	RSTSDFFAPI
endm
mUD_SDCardSectorWrite macro
MOV		R11, #APINO_UD_SDCardSectorWrite
ECALL	RSTSDFFAPI
endm

mUD_SectorWrite macro
MOV		R11, #APINO_UD_SectorWrite
ECALL     RSTSTGAPI
endm

mUD_Update      macro
MOV		R11, #APINO_UD_Update
ECALL     RSTSTGAPI
endm

mUD_GetFlashType  macro                 //返回值  非MLC flash:0    MLC flash:1
MOV		R11, #APINO_UD_FlashType
ECALL     RSTSDFFAPI
endm


mUD_GetCap macro
MOV  R11, #APINO_UD_GetCap
ECALL  RSTSTGAPI
endm
mUD_GetSctrSize macro
MOV  R11, #APINO_UD_GET_SCTR_SIZE
ECALL  RSTSTGAPI
endm
mUD_WPDetect macro
MOV  R11, #APINO_UD_WPDetect
ECALL  RSTSTGAPI
endm

mUD_Init        macro
MOV  R11, #APINO_UD_Init
ECALL  RSTSTGAPI
endm
mUD_Exit        macro
MOV  R11, #APINO_UD_Exit
ECALL  RSTSTGAPI
endm
mUH_Fast_Read_num        macro
MOV  R11, #APINO_UD_STRAT_READ_WRITE
ECALL  RSTSTGAPI
endm
mUH_GetCurrentNum        macro
MOV  R11, #APINO_UD_GET_REMAIN
ECALL  RSTSTGAPI
endm
mUH_Lose_Data        macro
MOV  R11, #APINO_UD_LOSE
ECALL  RSTSTGAPI
endm

#endif /* __A251__ */


#ifdef __C251__

#define UD_SectorRead(rwaddr) 			API_UD_SectorRead(APINO_UD_SectorRead,rwaddr)
#define UD_SectorWrite(rwaddr) 			API_UD_SectorWrite(APINO_UD_SectorWrite,rwaddr)
#define UD_GetCap(void) 				API_UD_GetCap(APINO_UD_GetCap)
#define UD_GetSctrSize(void) 			API_UD_GetSctrSize(APINO_UD_GET_SCTR_SIZE)
#define UD_Update(void) 				API_UD_Update(APINO_UD_Update)
#define UD_Init(mode) 					API_UD_Init(APINO_UD_Init,mode)
#define UD_Exit(void) 					API_UD_Exit(APINO_UD_Exit)
#define UD_GetFlashType(void)			API_UD_GetFlashType(APINO_UD_FlashType)
#define UD_WPDetect() 					API_UD_WPDetect(APINO_UD_WPDetect)
#define UD_ERASE(rwaddr)                API_UD_ERASE(APINO_UD_ERASE,rwaddr)
#define UH_Fast_Read_num(lba, sec_num) 		API_UD_StartReadWrite(APINO_UD_STRAT_READ_WRITE, lba, sec_num)
#define UH_GetCurrentNum(void) 					API_UD_GetRemain(APINO_UD_GET_REMAIN)
#define UH_Lose_Data(void) 					    API_UD_Lose(APINO_UD_LOSE)
#define UH_direct_status(void)					API_UH_direct_status(APINO_UD_DIR_STATUS)
#define UD_ClrVram(void)                API_UD_ClrVram(APINO_UD_ClrVram)

uint8 API_UD_StartReadWrite (uint8 cmd, uint32 lba, uint8 sec_num);
uint8 API_UD_GetRemain (uint8 cmd);
uint8 API_UD_Lose (uint8 cmd);
uint8 API_UH_direct_status(uint8 cmd);

typedef int lw;

typedef struct
{
    uint32        lba;
    int8         reserv;
    uint8         srampage;
    uint16       sramaddr;

} UD_RW;

typedef struct
{
    int8    ceinfo;
    int8    type;
} CE_INFO;

/*cap[0~3] is for nand flash
cap[4] is for the SM card, cap[5] is for the sd/mmc card ,cap[6] is for cf card, CAP[7] is for hd card*/

typedef struct
{
    CE_INFO ce_info[4];
    //        uint16    cap[8];
} STG_INFO;

//added by dongzh


//**UD_SectorRead**//
/*
函数名称：  UD_SectorRead
函数功能：  读512 byte数据到指定内存位置
函数输入：  UD_RW *RWStruct
函数返回：  0   success;
            1   地址输入错误，如不存在或越界的地址
            3   其他错误，需要重新激活卡
数据结构：
        typedef struct
        {
            uint32   lba;        //LBA读写地址，以512 BYTE为单位
            int8    endflag;    //控制参数，输入5A用于结束连续读写，正常为0
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/

int8 API_UD_SectorRead(uint8 cmd, UD_RW *rwaddr);

//**UD_SectorWrite**//
/*
函数名称：  UD_SectorWrite
函数功能：  写512 byte数据到指定卡的指定扇区
函数输入：  UD_RW *RWStruct
函数返回：  0   success;
            1   地址输入错误，如不存在或越界的地址
            2   写保护错误
            3   其他错误，需要重新激活卡
数据结构：
        typedef struct
        {
            uint32   lba;        //LBA读写地址，以512 BYTE为单位
            int8    endflag;    //控制参数，输入5A用于结束连续读写，正常为0
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/
int8 API_UD_SectorWrite(uint8 cmd, UD_RW *rwaddr);

//**UD_WPDetect**//
/*
函数名称：  UD_WPDetect
函数描述：  检测FLASH是否写保护
函数输入：
函数返回：  0 没有写保护
            1 写保护
数据结构：  无
*/
int8 API_UD_WPDetect(uint8 cmd);

//**UD_GetCap**//
/*
函数名称：  UD_GetCap
函数描述：  检测FLASH容量
函数输入：
函数返回：  HL FLASH容量，以128K为单位
数据结构：  无
*/
uint32 API_UD_GetCap(uint8 cmd);
/*
*********************************************************************************************************
*                                           UD_GetSctrSize
*
* Description: 获取当前所激活盘符一个扇区的容量(插入的U盘一个扇区可能为512byte的整数倍)
*
* Arguments  : None.
*
* Returns    : a: sector size, 2表示为1024byte, so on.
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8 API_UD_GetSctrSize(uint8 cmd);
//**UD_Update**//
/*
函数名称：  UD_Update
函数描述：  写入最后一笔数据到FLASH
函数输入：
函数返回：  无
数据结构：  无
*/
void API_UD_Update(uint8 cmd);


//**UD_GetFlashType**//
/*
函数名称：  UD_GetFlashType
函数描述：  取得flash 类型是否MLC
函数输入：
函数返回：  返回值  非MLC flash:0    MLC flash:1
数据结构：  无
*/
char API_UD_GetFlashType(uint8 cmd);  //返回值  非MLC flash:0    MLC flash:1

//**Get_Flash_Page_Size**//
/*
函数名称：  Get_Flash_Page_Size
函数描述：  取得flash page size大小，以512字节为单位，并返回是否为1K读写
函数输入：
函数返回：  返回值   高字节：0：512字节读写 1：1024字节读写 低字节：flash物理page内的扇区（512字节）数
数据结构：  无
*/
// uint16 Get_Flash_Page_Size(void);


//**UD_Init**//
/*
函数名称：  UD_Init
函数描述：  初始化FLASH，建立数据结构
函数输入：
函数返回：  无
数据结构：  无
*/
bool API_UD_Init(uint8 cmd, int8 mode);

//**UD_Exit**//
/*
函数名称：  UD_Exit
函数描述：  回写数据结构到FLASH
函数输入：
函数返回：  无
数据结构：  无
*/
void API_UD_Exit(uint8 cmd);

void API_UD_ERASE(uint8 cmd, UD_RW *rwaddr);

void API_UD_ClrVram(uint8 cmd);

//********************************for SD Card (快速驱动)**************************************

//**UD_SDCardReadInit**//
/*
函数名称：  UD_SDCardReadInit
函数功能：  初始化连续读的扇区，仅在USB模式下使用
函数输入：  输入连续读的扇区数目
            输入连续读的起始扇区的地址
函数返回：  0   success;
数据结构：
        typedef struct
        {
            uint32   lba;        //连续读的起始扇区的地址
            int8    SecNum;     //连续读的扇区数目
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/

int8 UD_SDCardReadInit(UD_RW *rwaddr);


//**UD_SDCardWriteInit**//
/*
函数名称：  UD_SDCardWriteInit
函数描述：  初始化连续写的扇区，初始化操作包括：
            1)擦除从起始地址开始的连续N个BLK
            2)设置连续写技术器
            仅在USB模式下使用
函数输入：  输入连续写的扇区数目
            输入连续写的起始扇区的地址
函数返回：  0   success;
            1   地址输入错误，如不存在或越界的地址
            2   写保护错误
            3   其他错误，需要重新激活卡
数据结构：
        typedef struct
        {
            uint32   lba;        //连续写的起始扇区的地址
            int8    SecNum;     //连续写的扇区数目
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/
int8 UD_SDCardWriteInit(UD_RW *rwaddr);

//**UD_SDCardSectorRead**//
/*
函数名称：  UD_SDCardSectorRead
函数功能：  读512 byte数据到指定内存位置
函数输入：  UD_RW *RWStruct
函数返回：  0   success;
            1   地址输入错误，如不存在或越界的地址
            3   其他错误，需要重新激活卡
数据结构：
        typedef struct
        {
            uint32   lba;        //LBA读写地址，以512 BYTE为单位
            int8    endflag;    //控制参数，输入5A用于结束连续读写，正常为0
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/
#define UD_SDCardSectorRead(rwaddr) API_UD_SDCardSectorRead(APINO_UD_SDCardSectorRead,rwaddr)
int8 API_UD_SDCardSectorRead(uint8 cmd, UD_RW *rwaddr);

//**UD_SDCardSectorWrite**//
/*
函数名称：  UD_SDCardSectorWrite
函数功能：  写512 byte数据到指定卡的指定扇区
函数输入：  UD_RW *RWStruct
函数返回：  0   success;
            1   地址输入错误，如不存在或越界的地址
            2   写保护错误
            3   其他错误，需要重新激活卡
数据结构：
        typedef struct
        {
            uint32   lba;        //LBA读写地址，以512 BYTE为单位
            int8    endflag;    //控制参数，输入5A用于结束连续读写，正常为0
            void    *sramaddr;  //读写到内存中的地址
            uint8    srampage;   //内存页 0/1/2 ipml/m/h, 4/5/6 idml/m/h
        }UD_RW;
*/
#define UD_SDCardSectorWrite(rwaddr) API_UD_SDCardSectorWrite(APINO_UD_SDCardSectorWrite,rwaddr)
int8 API_UD_SDCardSectorWrite(uint8 cmd, UD_RW *rwaddr);

//**UD_SDCardWPDetect**//
/*
函数名称：  UD_SDCardWPDetect
函数描述：  检测卡是否写保护
函数输入：
函数返回：  0 没有写保护
            1 写保护
数据结构：  无
*/
#define UD_SDCardWPDetect() 	API_UD_SDCardWPDetect(APINO_UD_SDCardWPDetect)
int8 API_UD_SDCardWPDetect(uint8 cmd);

//**UD_SDCardGetCap**//
/*
函数名称：  UD_SDCardGetCap
函数描述：  检测卡容量
函数输入：
函数返回：  HL 卡容量，以128K为单位
数据结构：  无
*/
#define UD_SDCardGetCap() API_UD_SDCardGetCap(APINO_UD_SDCardGetCap)
uint32 API_UD_SDCardGetCap(uint8 cmd);

//**UD_Init**//
/*
函数名称：  UD_Init
函数描述：  初始化FLASH，建立数据结构
函数输入：
函数返回：  无
数据结构：  无
*/
#define UD_SDCardInit(mode) API_UD_SDCardInit(APINO_UD_SDCardInit,mode)
bool API_UD_SDCardInit(uint8 cmd, int8 mode);

//**UD_SDCardExit**//
/*
函数名称：  UD_SDCardExit
函数描述：  关闭卡
函数输入：
函数返回：  无
数据结构：  无
*/
#define UD_SDCardExit() API_UD_SDCardExit(APINO_UD_SDCardExit)
void API_UD_SDCardExit(uint8 cmd);

#define UD_SDCardStartTran() API_UD_SDCardStartTran(APINO_UD_SDCardStartTran)
void API_UD_SDCardStartTran(uint8 cmd);

#define UD_SDCardEndF2F() API_UD_SDCardEndF2F(APINO_UD_SDCardEndF2F)
uint8 API_UD_SDCardEndF2F(uint8 cmd);

#endif /* __C251__ */

#endif /*_UDDRV_H */











