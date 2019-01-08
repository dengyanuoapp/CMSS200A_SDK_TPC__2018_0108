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
 * \file     addr_map.h
 * \brief    地址信息
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/
#ifndef __ADDR_MAP_H__
#define __ADDR_MAP_H__

#define RAM_BANK_A_ADDR 0x1e00

#define RAM_BANK_A_SIZE 0x200

#define BNKA_DATA_SRAM_ADDR	0x500
#define RAM_BANK_B_ADDR 0x1600
#define RAM_BANK_B_SIZE 0x800
#define BNKB_DATA_SRAM_ADDR	0x580

//#define SRAM_BANK_C_ADDR 0x1400
//#define SRAM_BANK_C_SIZE 0x200

#define SRAM_SIZE_MASK 0xffff
#define RDATA_ADDR_BASE 0x0
#define RCODE_ADDR_BASE 0xff0000
#define MAX_BANK_SIZE 0x4000

#define KERNEL_BANK_A 0x0000
#define KERNEL_BANK_A_PAGE_INDEX 0x6
#define KERNEL_BANK_A_PAGE_MASK_M 0xc0	/*high is 0xff*/
#define KERNEL_BANK_A_MAX_SIZE 0x4000	/*16K*/
#define KERNEL_BANK_A_REDIRECTADDR_M 0x0 /*the lowest 16k*/

#define KERNEL_BANK_B 0x2000
#define KERNEL_BANK_B_PAGE_INDEX 0x7
#define KERNEL_BANK_B_PAGE_MASK_M 0xc0  /*high is 0xff*/
#define KERNEL_BANK_B_MAX_SIZE 0x4000   /*16K*/
#define KERNEL_BANK_B_REDIRECTADDR_M 0x0 /*the lowest 16k*/


//#define KERNEL_BANK_C 0x37
//#define KERNEL_BANK_C_PAGE_INDEX 0x3     /*muti-use codec*/
//#define KERNEL_BANK_C_PAGE_MASK_M 0xc0   /*high is 0xff*/
//#define KERNEL_BANK_C_MAX_SIZE 0x4000    /*16K*/
//#define KERNEL_BANK_C_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/


#define AP_BANK_CONTROL 0x00//0b1000
#define AP_BANK_CONTROL_PAGE_INDEX 0
#define AP_BANK_CONTROL_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_CONTROL_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_CONTROL_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/


#define AP_BANK_UI      0x02//0b1001
#define AP_BANK_UI_PAGE_INDEX      1
#define AP_BANK_UI_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_UI_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_UI_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/

#define AP_BANK_BASAL     0x04//0b10100
#define AP_BANK_BASAL_PAGE_INDEX     2
#define AP_BANK_BASAL_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_BASAL_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_BASAL_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/

#define AP_BANK_CODEC     0x05//0b10101
#define AP_BANK_CODEC_PAGE_INDEX     3
#define AP_BANK_CODEC_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_CODEC_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_CODEC_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/

#define AP_BANK_ENHANCED_1      0x06//0b10110
#define AP_BANK_ENHANCED_1_PAGE_INDEX      4
#define AP_BANK_ENHANCED_1_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_ENHANCED_1_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_ENHANCED_1_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/

#define AP_BANK_ENHANCED_2      0x07//0b10111
#define AP_BANK_ENHANCED_2_PAGE_INDEX      5
#define AP_BANK_ENHANCED_2_PAGE_MASK_M 0xc0   /*high is 0xff*/
#define AP_BANK_ENHANCED_2_MAX_SIZE 0x4000    /*16K*/
#define AP_BANK_ENHANCED_2_REDIRECTADDR_M 0x00 /*the first 16k, it is 0x40 if the second 16k*/

#endif
