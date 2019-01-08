/*
********************************************************************************
*                       ACTOS
*                       ACTOS
*                  time define head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : mem.h
* By     : Gongee
* Version: 1> v1.00     first version     12/4/2003 11:13PM
********************************************************************************
********************************************************************************
*/
#ifndef _MEM_H
#define _MEM_H

#include "actapi.h"

//vm address
//系统参数区
#define         VM_KERNEL       	0x0000      //分配0x40
#define         VM_AP_FWCFG       0x0040        //分配0x140
#define         VM_AP_AMV           0x0180       //分配0x80

#define         VM_SYSTEM       	0x0200   //分配0x80
#define         VM_AP_RECORD    0x0280  //分配0x80
#define         VM_AP_MUSIC_CARD	0x0300   //分配0x80
#define         VM_AP_MUSIC_HOST    0x0380  //分配0x80	
#define         VM_AP_MUSIC_COMMON  0x400    //分配0x80

#define         VM_EXFAT_INFO_UH       0x480        //分配0x80
#define         VM_EXFAT_INFO_CARD     	0x0500     //分配0x80
#define        VM_FAVI_HEAD      0x0580     //分配0x80

#define         VM_AP_RADIO     	0x0600     //分配0x100
#define  	      VM_ALARM		0x0700     //分配0x80
#define         VM_PCSET_EQ       0x0780     //分配0x80


#define			    VM_DIR_LIST				0x0800						//目录列表5k
#define         VM_FAVI_CARD      0x0800             // 2.5k
#define         VM_FAVI_HOST      0x1200             // 2.5k
#define         VM_PCSET_SRS      0x1200

#define         VM_LYRIC        	0x1c00             // 6k

//专门给文件系统使用
#define         VM_FSYS         	0x3400             // 2k

#define			VM_SHUFFLE			0x3C00			//随机播放0x80
//end--0x3400, 剩余1k备用


#define APINO_VMRead      0x0000+API_VM_Base
#define APINO_VMWrite     0x0001+API_VM_Base
//#define APINO_VMExpRead   0x0002+API_VM_Base
//#define APINO_VMExpWrite  0x0003+API_VM_Base

#ifdef __C251__



/*  必须保证BufAlloc于BufFree匹配，而且一一对应，如同栈的使用
//such as:
  right use:
       int    bufpoint0,bufpoint1;
       bufpoint0=BufAlloc(20);  //------
              ...               //     |
       bufpoint1=BufAlloc(30);  //--   |
              ...               // |   |
       BufFree(bufpoint1);      //--   |
              ...               //     |
       BufFree(bufpoint0);      //------

  wrong use:
       int    bufpoint0,bufpoint1;
       bufpoint0=BufAlloc(20);  //---
              ...               //  |
       bufpoint1=BufAlloc(30);  //--|----
              ...               //  |   |
       BufFree(bufpoint0);      //---   |
              ...               //      |
       BufFree(bufpoint1);      //------
*/

#define VMRead(pbuf,address,len) 				API_VMRead(APINO_VMRead,pbuf,address,len)
#define VMExpRead(pbuf,address,len) 			API_VMExpRead(APINO_VMExpRead,pbuf,address,len)
#define VMWrite(pbuf,address,len) 					API_VMWrite(APINO_VMWrite,pbuf,address,len)
#define VMExpWrite(pbuf,address,len,page) 		API_VMExpWrite(APINO_VMExpWrite,pbuf,address,len,page )


//读取指定长度的数据。
//pbuf: 接受数据的Buffer 指针。
//Address: vram 地址
//Len[1~512]: 读取的长度。
//能从zram或uram中读，也能从ipm/idm中读，由port05决定
bool API_VMRead(uint8 cmd, void *pbuf, uint16 address, uint16 len);            //len: 字节为单位
bool API_VMExpRead(uint8 cmd, void *pbuf, uint32 address, uint16 len);        //len: 字节为单位

//将pbuf 中的数据写入VRam, 固定长度为512Byte。
//pbuf: 接受数据的Buffer 指针。
//Address: vram 地址
//由于VMWrite非常的慢，请不要频繁的调用，一般是在ap退出或对速度要求不高的地方使用。
//只能从zram或uram中写，不能从ipm/idm中写
bool API_VMWrite(uint8 cmd, void *pbuf, uint16 address, uint16 len);
bool API_VMExpWrite(uint8 cmd, void *pbuf, uint32 address, uint16 len, int8 page );      //address单位为Bytes，len: 扇区为单位

#endif /* __C251__ */

#ifdef __A251__





//定义汇编调用的宏
mVMRead			macro
MOV R11, #APINO_VMRead
ECALL     RSTSYSAPI
endm

mVMWrite		macro
MOV R11, #APINO_VMWrite
ECALL     RSTSYSAPI
endm

mVMExpRead      macro
MOV R11, #APINO_VMExpRead
ECALL     RSTSYSAPI
endm

mVMExpWrite     macro
MOV R11, #APINO_VMExpWrite
ECALL     RSTSYSAPI
endm



#endif /* __A251__ */
#endif/*_MEM_H*/


