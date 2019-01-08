/********************************************************************************
 *                              USDK(GL5110)
 *                            Module: KERNEL
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>           <version >             <desc>
 *      wuyueqian     2011-9-10 09:00     1.0             build this file
 ********************************************************************************/
/*!
 * \file     driver_manager.h
 * \brief    驱动管理
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/
#ifndef _DRIVER_MANAGER_H
#define _DRIVER_MANAGER_H

//-------------------------------------
//bank a 资源分配  注意：bank a不能大于128个bank
//-------------------------------------
//系统类函数的api分配 40  bank a 资源
#define SYS_BaseNoA      	0
//存储类驱动固定要分配 3 bank a 资源
#define STG_BaseNoA      	40
//文件系统驱动固定要分配 18 bank a 资源
#define FS_BaseNoA      	43
//显示驱动固定要分配 5 bank a 资源
#define UI_BaseNoA       	61
//键盘驱动固定要分配 6 bank a 资源
#define KY_BaseNoA      	66
//mini文件系统驱动固定要分配 5 bank a 资源
#define MFS_BaseNoA   		72
//快速SD卡驱动固定要分配 3 bank a 资源
#define SDFF_BaseNoA     	77
//FM驱动固定要分配 5 bank a 资源
#define FM_BaseNoA     		80
//未分配 43 bank a 资源
#define RESERVE_BaseNoA  	85

//-------------------------------------
//bank b 资源分配  注意：bank b不能大于124个bank
//-------------------------------------
//系统类函数的api分配 10  bank b 资源
#define SYS_BaseNoB 	   	0
//存储类驱动固定要分配 8 bank b 资源
#define STG_BaseNoB      	10
//文件系统驱动固定要分配 35 bank b 资源
#define FS_BaseNoB      	18
//显示驱动固定要分配 20 bank b 资源
#define UI_BaseNoB       	53
//键盘驱动固定要分配 3 bank b 资源
#define KY_BaseNoB      	73
//mini文件系统驱动固定要分配 10 bank b 资源
#define MFS_BaseNoB   		76
//快速SD卡驱动固定要分配 3 bank b 资源
#define SDFF_BaseNoB     	86
//FM驱动固定要分配 5 bank b 资源
#define FM_BaseNoB     		89
//未分配 30 bank b 资源
#define RESERVE_BaseNoB 	94

typedef struct
{
    unsigned char	group_sys;
    unsigned char	group_stg;
    unsigned char	group_fat;
    unsigned char	group_ui;
    unsigned char	group_key;
    unsigned char	group_minifs;
    unsigned char	group_reserve;
} drv_bank;

typedef struct
{
    unsigned char file_type;	//FILE_DRV  ‘D’
    unsigned char drv_type;		//驱动类型
    unsigned short text_addr;//常驻代码段在内存中的地址
    unsigned short text_length;//常驻段代码段长度
    unsigned short text2_addr;//常驻代码段在内存中的地址
    unsigned short text2_length;//常驻段代码段长度
    unsigned short data_addr;//常驻数据段在内存中的地址
    unsigned short data_length;//常驻数据段长度（不包括BSS段）
    unsigned long drv_init_entry;//初始化函数地址
    unsigned long drv_exit_entry;//退出函数地址
    unsigned long drv_banka_file_offset;//Bank A banks在驱动程序文件中的起始地址
    unsigned long drv_bankb_file_offset;//Bank B banks在驱动程序文件中的起始地址
    //	unsigned long drv_bankc_file_offset;//Bank C banks在驱动程序文件中的起始地址
    unsigned long drv_op_entry;//entry
} drv_head_t;

typedef struct
{
    uint32 bank_a_offset;
    uint32 bank_b_offset;
    uint32 exit_entry;
    uint32 drv_op_entry;
} drv_bank_info_t;

typedef struct
{
    uint16  bank_a_offset;
    uint16  bank_b_offset;
    uint32 sys_op_entry_0;
    uint32 sys_op_entry_1;
    uint32 sys_op_entry_2;
} sys_bank_info_t;

#define MAX_DRV_GROUP_TOTAL 8
#define API_TOTAL_ENTRY (intMAX_DRV_GROUP_TOTAL+1+1)//sys+libc
#define DRV_GROUP_SYS 0xAA
typedef enum
{
    DRV_GROUP_STG = 0,
    DRV_GROUP_FS,
    DRV_GROUP_UI,
    DRV_GROUP_KEY,
    DRV_GROUP_MFS,
    DRV_GROUP_SDFF,
    DRV_GROUP_FM,
    DRV_GROUP_RESERVE = 7
} drv_type_t;

typedef uint8 (*drv_entry_i)(uint8 cmd, uint8 mode);
typedef uint8 (*drv_exit_i)(uint8 cmd);

//#define APINO_DATA_SAVE  		0x0000+API_BANK_Base
//#define APINO_DATA_RESTORE    	0x0001+API_BANK_Base

#define	SaveBankData(val)	  	API_SaveBankData(APINO_DATA_SAVE,val)
#define	RestoreBankData(val) 	API_RestoreBankData(APINO_DATA_RESTORE,val)

void API_SaveBankData(uint8 cmd, void *val);
void API_RestoreBankData(uint8 cmd, void *val);

#endif /*_DRIVER_H */

