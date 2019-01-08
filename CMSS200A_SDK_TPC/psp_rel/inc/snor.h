/*****************************************************************************
*                                 asdk 5102
*                            Module: Nor Flash Driver
*                 Copyright(c) 2001-2010 Actions Semiconductor,
*                            All Rights Reserved.
*
* History:
*      <author>       <time>             <version >           <desc>
*       husanxi     2010-3-30 16:35          1.0             build this file
*
* Description:spi nor驱动相关的宏定义
*
******************************************************************************/
#ifndef __SNOR_H__
#define __SNOR_H__

//#define         BREC_START_SECTOR           0x01                    //Brec 从哪个sector开始
//#define         LFI_START_SECTOR            0x41                    //LFI 从哪个sector开始
#define         SPINOR_ERASE_UNITS          0x80                    //SPI nor一次擦除单位(扇区数)

#define         VRAM_TOATL_USE_SCTRS        0x80                    //SPI Nor上为实现Vram所占用的总扇区数,最后一个扇区存放空间位图
#define         SPINOR_VRAM_MAX_SCTRS       0x20                    //最大写32个扇区Vram数据
#define			VRAM_AJUST_BUFFER			0x4400					//整理vram数据时的临时空间 4000～8000


#define		  NOR_TABLE_SECT			0x01

#define		  NOR_BUFFER				0x700


#define       NOR_FUNCTION_BASE         0xffd300


#define		  SDkernel_read				NOR_FUNCTION_BASE+0
#define       Snor_phy_read             NOR_FUNCTION_BASE+4
#define       Snor_phy_write            NOR_FUNCTION_BASE+8

#define		  Snor_byte_write			NOR_FUNCTION_BASE+12

#define       Snor_log_read             NOR_FUNCTION_BASE+16
#define       Snor_log_write            NOR_FUNCTION_BASE+20
#define       Snor_blk0_read            NOR_FUNCTION_BASE+24
#define       Snor_blk0_write           NOR_FUNCTION_BASE+28

#define       Snor_blck_erase           NOR_FUNCTION_BASE+32
#define       Snor_chip_erase           NOR_FUNCTION_BASE+36
#define       Snor_other_erase          NOR_FUNCTION_BASE+40

#define		  Snor_reset_reg			NOR_FUNCTION_BASE+44



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif  /* __SPI_NOR_H__ */

