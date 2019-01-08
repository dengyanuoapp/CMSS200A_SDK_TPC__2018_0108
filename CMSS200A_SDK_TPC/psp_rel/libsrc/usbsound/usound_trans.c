/********************************************************************************
 *                              AS260A
 *                            Module: USB Sound
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2012-10-23 16:40     1.0                build this file
 ********************************************************************************/
/*!
 * \file     usound_trans.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               数据传输代码
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#include "usound_intern.h"
#include "usound_extern.h"
#pragma name(USC_TRANS)
#pragma renamecode(?PR?USC_TRANS)

/******************************************************************************/
/*!
 * \par  Description:
 *     access usb fifo data
 * \param[in]    ep_select:endp NO.、address:buff address、data_lenth:data length
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void usb_fifo_access(uint8 ep_select, uint8 *address, uint16 data_lenth)
{
    uint16 i;

    for (i = 0; i < data_lenth; i++)
    {
        if (EP1IN == ep_select)
        {
            //address += 2; //上传左声道数据
            //i += 2;
            Fifo1dat = *address;
            address++;
            Fifo1dat = *address;
            address++;
            i++;
            address += 2; //上传右声道数据
            i += 2;
        }
        else if (EP2OUT == ep_select)
        {
            *address = Fifo2dat;
            address++;
        }
        else
        {
            return;
        }
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     set dma reload mode to transfer data
 * \param[in]    none
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void dma_reload_from_adc(void)
{
    uint8 sfr_temp = SFR_BANK;

    SFR_BANK = BANK_DMA_USB2;
    DMAnCTL0 &= ~(3 << 4);
    DMAnCTL0 |= 1 << 4; //16bit width
    //DMAnCTL0 |= 0x40; //separated
    DMAnCTL0 &= ~0x40;
    DMAnCTL1 = 0x05;
    DMAnDADDR0H = 0x00;
    DMAnDADDR0M = 0x40;
    DMAnDADDR0L = 0x00;
    //DMAnDADDR1H = 0x00;
    //DMAnDADDR1M = 0x43;
    //DMAnDADDR1L = 0x80;
    DMAnIP |= 0x03; //clear pending bit
    DMAnIE |= 0x03;
    DMAnFrameLenH = (uint8)((RECORD_BUFF_SIZE/2)>>8);
    DMAnFrameLenL = (RECORD_BUFF_SIZE/2) & 0xFF;
    DMAnCTL0 |= 0x02; //enable reload mode
    DMAnCTL0 |= 0x01; //start dma
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     set dma reload mode to transfer data
 * \param[in]    none
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void dma_reload_to_dac(void)
{
    uint8 sfr_temp = SFR_BANK;

    SFR_BANK = BANK_DMA_USB1;
    DMAnCTL0 &= ~(3 << 4);
    if (DOWNLOAD_BIT_DEPTH_24== gud_param.bit_depth)
    {
        DMAnCTL0 |= (0x1<<5);
    }
    else
    {
        DMAnCTL0 |= (0x1<<4);
    }
    DMAnCTL1 = 0x50;
    DMAnSADDR0H = 0x00;
    DMAnSADDR0M = 0x49;//0x30;
    DMAnSADDR0L = 0x00;
    DMAnIP |= 0x03; //clear pending bit
    DMAnIE |= 0x03; //enable interrupt
    if(DOWNLOAD_BIT_DEPTH_24==gud_param.bit_depth)
    {
        DMAnFrameLenH = (uint8)((PLAY_BUFF_SIZE/3)>>8);
        DMAnFrameLenL = (PLAY_BUFF_SIZE/3) & 0xFF;
    }
    else
    {
        DMAnFrameLenH = (uint8)((PLAY_BUFF_SIZE/2)>>8);
        DMAnFrameLenL = (PLAY_BUFF_SIZE/2) & 0xFF;
    }
    
    DMAnCTL0 |= 0x02; //enable reload mode
    DMAnCTL0 |= 0x01; //start dma
    SFR_BANK = sfr_temp;
}
