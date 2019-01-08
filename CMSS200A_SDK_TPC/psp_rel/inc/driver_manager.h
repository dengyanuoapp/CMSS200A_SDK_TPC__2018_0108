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
 * \brief    ��������
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/
#ifndef _DRIVER_MANAGER_H
#define _DRIVER_MANAGER_H

//-------------------------------------
//bank a ��Դ����  ע�⣺bank a���ܴ���128��bank
//-------------------------------------
//ϵͳ�ຯ����api���� 40  bank a ��Դ
#define SYS_BaseNoA      	0
//�洢�������̶�Ҫ���� 3 bank a ��Դ
#define STG_BaseNoA      	40
//�ļ�ϵͳ�����̶�Ҫ���� 18 bank a ��Դ
#define FS_BaseNoA      	43
//��ʾ�����̶�Ҫ���� 5 bank a ��Դ
#define UI_BaseNoA       	61
//���������̶�Ҫ���� 6 bank a ��Դ
#define KY_BaseNoA      	66
//mini�ļ�ϵͳ�����̶�Ҫ���� 5 bank a ��Դ
#define MFS_BaseNoA   		72
//����SD�������̶�Ҫ���� 3 bank a ��Դ
#define SDFF_BaseNoA     	77
//FM�����̶�Ҫ���� 5 bank a ��Դ
#define FM_BaseNoA     		80
//δ���� 43 bank a ��Դ
#define RESERVE_BaseNoA  	85

//-------------------------------------
//bank b ��Դ����  ע�⣺bank b���ܴ���124��bank
//-------------------------------------
//ϵͳ�ຯ����api���� 10  bank b ��Դ
#define SYS_BaseNoB 	   	0
//�洢�������̶�Ҫ���� 8 bank b ��Դ
#define STG_BaseNoB      	10
//�ļ�ϵͳ�����̶�Ҫ���� 35 bank b ��Դ
#define FS_BaseNoB      	18
//��ʾ�����̶�Ҫ���� 20 bank b ��Դ
#define UI_BaseNoB       	53
//���������̶�Ҫ���� 3 bank b ��Դ
#define KY_BaseNoB      	73
//mini�ļ�ϵͳ�����̶�Ҫ���� 10 bank b ��Դ
#define MFS_BaseNoB   		76
//����SD�������̶�Ҫ���� 3 bank b ��Դ
#define SDFF_BaseNoB     	86
//FM�����̶�Ҫ���� 5 bank b ��Դ
#define FM_BaseNoB     		89
//δ���� 30 bank b ��Դ
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
    unsigned char file_type;	//FILE_DRV  ��D��
    unsigned char drv_type;		//��������
    unsigned short text_addr;//��פ��������ڴ��еĵ�ַ
    unsigned short text_length;//��פ�δ���γ���
    unsigned short text2_addr;//��פ��������ڴ��еĵ�ַ
    unsigned short text2_length;//��פ�δ���γ���
    unsigned short data_addr;//��פ���ݶ����ڴ��еĵ�ַ
    unsigned short data_length;//��פ���ݶγ��ȣ�������BSS�Σ�
    unsigned long drv_init_entry;//��ʼ��������ַ
    unsigned long drv_exit_entry;//�˳�������ַ
    unsigned long drv_banka_file_offset;//Bank A banks�����������ļ��е���ʼ��ַ
    unsigned long drv_bankb_file_offset;//Bank B banks�����������ļ��е���ʼ��ַ
    //	unsigned long drv_bankc_file_offset;//Bank C banks�����������ļ��е���ʼ��ַ
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

