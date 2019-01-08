/*
********************************************************************************
*                       ACTOS
*               System Disk driver head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : sddrv.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 9:14
********************************************************************************
********************************************************************************
*/
#ifndef _SDDRV_INCLUDED
#define _SDDRV_INCLUDED

#include "actapi.h"

//#define APINO_SD_Read               0x17
#ifdef __C251__

#define APINO_UD_NorInit             0x00
#define APINO_UD_NorExit             0x01
#define APINO_UD_NorSectorRead       0x02
#define APINO_UD_NorSectorWrite      0x03
#define APINO_UD_NorUpdate           0x04
#define APINO_UD_NorGetCap           0x05
#define APINO_UD_NorWPDetect         0x06
#define APINO_UD_NorPhyRead          0x09
#define APINO_UD_NorPhyWrite         0x0a


typedef struct
{
    uint32   faddr;
    uint8    reserve;
    uint8    srampage;
    IntAddr  sramaddr;
} SD_RW;

#define SD_Read(sdrwptr,length) API_SD_Read(APINO_SD_Read,sdrwptr,length)
#define	SD_Bre_SectorRead(sdrw) API_SD_Bre_SectorRead(APINO_UD_NorPhyRead,sdrw)
#define	SD_Bre_SectorWrite(sdrw) API_SD_Bre_SectorWrite(APINO_UD_NorPhyWrite,sdrw)

bool SD_SimpleInit(uint8 mode);
void SD_SimpleExit(void);
bool API_SD_Read(uint8 cmd, SD_RW *sdrwptr, uint16 length);
bool SD_SimpleSectorRead(SD_RW *sdrw);

bool SD_SectorRead(SD_RW *sdrw);
bool SD_SectorWrite(SD_RW *sdrw);
bool API_SD_Bre_SectorRead(uint8 cmd, SD_RW *sdrw);
bool API_SD_Bre_SectorWrite(uint8 cmd, SD_RW *sdrw);
bool SD_Update(void);
int8 SetSWAPAddress(int8 page);  //ret: ¦Ì¡À?page

#endif /* ____C251__ */

#ifdef __A251__


#define API_SD_SectorRead           0x00
#define API_SD_SectorWrite          0x01
#define API_Bre_SectorRead          0x08
#define API_Bre_SectorWrite         0x09
#define API_CardBre_SectorRead      0x0004+API_SDCardFF_BaseNoA
#define API_CardBre_SectorWrite     0x1004+API_SDCardFF_BaseNoA

#define API_SD_Update           0x04
mSD_Read        macro
MOV       R11, #APINO_SD_Read
ECALL     RSTSYSAPI1
endm

mSD_SectorRead  macro
MOV  	  R10, #API_SD_SectorRead
LCALL     RSTStgAPI
endm

mSD_SectorWrite macro
MOV  R10, #API_SD_SectorWrite
LCALL     RSTStgAPI
endm

mBrec_SectorRead macro
MOV		  WR8, #API_Bre_SectorRead
LCALL     RSTBankAPI
endm

mBrec_SectorWrite macro
MOV	  WR8, API_Bre_SectorWrite
LCALL     RSTBankAPI
endm

mSD_Update macro
MOV       R10, #API_SD_Update
LCALL     RSTStgAPI
endm
#endif /* __A251__ */

#endif /*_SDDRV_H */
