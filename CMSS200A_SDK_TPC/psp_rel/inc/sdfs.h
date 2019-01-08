/*
********************************************************************************
*                       ACTOS
*               System Disk file system head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : sdfs.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 9:14
********************************************************************************
*/
#ifndef _SDFS_H
#define _SDFS_H

#include "actapi.h"
#include "typeext.h"



#define MODE_READ   0

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#define SDFILE_CLOSED           0xFF
#define  MFSLOCATEPAGE  0xf0        //IPM第0页
#define  MAX_HEANDLE_NUM 4
#define  SD_DIR_LEN		32 //目录项长度

#define APINO_SD_FOpen            0x0000+RAMAPI_SD_BASE
#define APINO_SD_FClose           0x0001+API_SDFS_Base

#ifdef __C251__
typedef struct
{
    int8    mode;
    uint32   startaddr;
    uint32   endaddr;
    uint32   rwpointer;
} SD_FILE;

typedef struct
{
    int8    fname[11];
    int8    fattr;
    int8    reserve0[2];
    uint16    version;
    uint32   offset;
    uint32   size;
    uint32   reserve1;
    uint32   checksum;
} SD_DIR;

#define SD_FOpen(filename,mode) 		API_SD_FOpen(APINO_SD_FOpen,filename,mode)
#define SD_FClose(fp) 					API_SD_FClose(APINO_SD_FClose,fp)
#define SD_FSeek(fp,nFrom,offset) 		API_SD_FSeek(APINO_SD_FSeek,fp,nFrom,offset)
#define SD_FTell(fp) 					API_SD_FTell(APINO_SD_FTell,fp)
#define SD_FRead(fp,buffer,Length) 		API_SD_FRead(APINO_SD_FRead,fp,buffer,Length)

//同时可以打开4个文件
SD_FILE *API_SD_FOpen(uint8 cmd, int8 *filename, uint8 mode);
int8 API_SD_FClose(uint8 cmd, SD_FILE *fp);
int8 API_SD_FSeek(uint8 cmd, SD_FILE *fp, uint8 nFrom, int32 offset);
int32 API_SD_FTell(uint8 cmd, SD_FILE *fp);
int16 API_SD_FRead(uint8 cmd, SD_FILE *fp, void *buffer, uint16 Length);
// bool sMfsRcodeRead(int8 *filename, int8 * mfs_ap_head);
int8 Check_SDFile_Handle(SD_FILE *fp);
#endif /* __C251__ */
#ifdef __A251__

mSD_FOpen        macro
MOV 	  R11, #APINO_SD_FOpen
ECALL     RSTSYSAPI
endm
mSD_FClose        macro
MOV 		  R11 , #APINO_SD_FClose
ECALL   	  RSTSYSAPI
endm
mSD_FSeek         macro
MOV 		  R11, #APINO_SD_FSeek
ECALL  	  RSTSYSAPI1
endm
mSD_FTell         macro
MOV 		  R11, #APINO_SD_FTell
ECALL  	  RSTSYSAPI1
endm
mSD_FRead         macro
MOV       R11, #APINO_SD_FRead
ECALL     RSTSYSAPI1
endm
#endif /* __A251__ */

#endif /*_SDFS_H */
