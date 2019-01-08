
/*author by wuyueqian*/
#include "sdfs.h"

#ifndef _BANK_MANAGER_H_
#define _BANK_MANAGER_H_

#define SUPPORT_AP_LEVEL 3
#define AP_HEAD_MAX_SIZE 0x200
#define APINO_ExecAP      		0x00+API_APM_Base
//#define APINO_BankSwitch      	0x01+API_APM_Base
//#define API_BankFLSUH 			0x0000+API_APM_BaseNoB
#ifdef __C251__
typedef struct
{
    unsigned char     file_type;          //'P'
    unsigned char     ap_type;            //0:ap_system, 1:ap_user
    unsigned char     major_version;      //3, for ACTOS 3.0
    unsigned char     minor_version;      //0
    unsigned char     magic[4];           //'W', 'G', 0x19, 0x97
    unsigned long     text_offset;		    //代码段文件内偏移
    unsigned short    text_length;				//代码段长度
    unsigned short    text_addr;          //代码段在内存中的地址
    unsigned long     data_offset;					//数据段文件内偏移
    unsigned short    data_length;				//数据段长度（不包括BSS段）
    unsigned short    data_addr;          //数据段在内存中的地址
    unsigned short    bss_length;						//Bss段长度
    unsigned short    bss_addr;						//Bss起始地址
    //	unsigned long     control_bank_offset;  //can save space for nor_base
    //	unsigned long     ui_bank_offset;		//because has the bank_Info, so need't it
    //	unsigned long     basal_bank_offset;
    //	unsigned long     codec_bank_offset;
    //	unsigned long     enhanced1_bank_offset;
    //	unsigned long     enhanced2_bank_offset;
    unsigned long     entry;              //ap 入口

} ap_head_t;

typedef struct
{
    unsigned long     bank_offset;
    unsigned short    bank_length;
    unsigned short    bank_addr;
} ap_bank_head_t;

/*
typedef struct
{
    unsigned char   isvalid;
    unsigned char   reserve[3];
    unsigned int    startaddr;
    unsigned int   endaddr;
    unsigned int   rwpointer;
} sd_file_t;
*/

typedef struct
{
    SD_FILE 		  ap_handle;
    //	unsigned short    text_length;
    //    unsigned short    text_addr;
    //	unsigned long     text_offset;
} ap_tcb_t;

#define ExecAP(strName,Param) 		API_ExecAP(APINO_ExecAP,strName,Param)
#define BankSwitch(epc) 			API_BankSwitch(APINO_BankSwitch,epc)

//切换到 BankNo, 返回旧的 bank 号码
int16 API_BankSwitch(uint8 cmd, uint32 epc);
//执行 strName ap, 返回ap的返回值
int16 API_ExecAP(uint8 cmd, int8 *strName, int16 Param);
//void BankFlush(uint8 cmd,uint8 gindex);


typedef int16 (*main_func_t)(int16);
#endif/*__C251__*/

#ifdef __A251__

mAPM_ExecAP     macro
mov      R11, #APINO_ExecAP
ECALL	 RSTSYSAPI
endm
mAPM_BankSwitch macro
mov      R11, #APINO_BankSwitch
ECALL	 RSTSYSAPI1
endm
//mAPM_MIDBankSwitch macro
//        mov      R10,#API_MID_BankSwitch
//		LCALL	 RSTFastAPI
//        endm
#endif /*__A251__*/

#endif
