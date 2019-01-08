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
//ϵͳ������
#define         VM_KERNEL       	0x0000      //����0x40
#define         VM_AP_FWCFG       0x0040        //����0x140
#define         VM_AP_AMV           0x0180       //����0x80

#define         VM_SYSTEM       	0x0200   //����0x80
#define         VM_AP_RECORD    0x0280  //����0x80
#define         VM_AP_MUSIC_CARD	0x0300   //����0x80
#define         VM_AP_MUSIC_HOST    0x0380  //����0x80	
#define         VM_AP_MUSIC_COMMON  0x400    //����0x80

#define         VM_EXFAT_INFO_UH       0x480        //����0x80
#define         VM_EXFAT_INFO_CARD     	0x0500     //����0x80
#define        VM_FAVI_HEAD      0x0580     //����0x80

#define         VM_AP_RADIO     	0x0600     //����0x100
#define  	      VM_ALARM		0x0700     //����0x80
#define         VM_PCSET_EQ       0x0780     //����0x80


#define			    VM_DIR_LIST				0x0800						//Ŀ¼�б�5k
#define         VM_FAVI_CARD      0x0800             // 2.5k
#define         VM_FAVI_HOST      0x1200             // 2.5k
#define         VM_PCSET_SRS      0x1200

#define         VM_LYRIC        	0x1c00             // 6k

//ר�Ÿ��ļ�ϵͳʹ��
#define         VM_FSYS         	0x3400             // 2k

#define			VM_SHUFFLE			0x3C00			//�������0x80
//end--0x3400, ʣ��1k����


#define APINO_VMRead      0x0000+API_VM_Base
#define APINO_VMWrite     0x0001+API_VM_Base
//#define APINO_VMExpRead   0x0002+API_VM_Base
//#define APINO_VMExpWrite  0x0003+API_VM_Base

#ifdef __C251__



/*  ���뱣֤BufAlloc��BufFreeƥ�䣬����һһ��Ӧ����ͬջ��ʹ��
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


//��ȡָ�����ȵ����ݡ�
//pbuf: �������ݵ�Buffer ָ�롣
//Address: vram ��ַ
//Len[1~512]: ��ȡ�ĳ��ȡ�
//�ܴ�zram��uram�ж���Ҳ�ܴ�ipm/idm�ж�����port05����
bool API_VMRead(uint8 cmd, void *pbuf, uint16 address, uint16 len);            //len: �ֽ�Ϊ��λ
bool API_VMExpRead(uint8 cmd, void *pbuf, uint32 address, uint16 len);        //len: �ֽ�Ϊ��λ

//��pbuf �е�����д��VRam, �̶�����Ϊ512Byte��
//pbuf: �������ݵ�Buffer ָ�롣
//Address: vram ��ַ
//����VMWrite�ǳ��������벻ҪƵ���ĵ��ã�һ������ap�˳�����ٶ�Ҫ�󲻸ߵĵط�ʹ�á�
//ֻ�ܴ�zram��uram��д�����ܴ�ipm/idm��д
bool API_VMWrite(uint8 cmd, void *pbuf, uint16 address, uint16 len);
bool API_VMExpWrite(uint8 cmd, void *pbuf, uint32 address, uint16 len, int8 page );      //address��λΪBytes��len: ����Ϊ��λ

#endif /* __C251__ */

#ifdef __A251__





//��������õĺ�
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


