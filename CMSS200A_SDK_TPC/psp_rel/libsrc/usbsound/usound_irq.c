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
 * \file     usound_irq.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               usb中断服务程序
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

#define INVALID_INT        0x00
#define SETUP_INT          0x01
#define OUT2TOKEN_INT      0x02
#define IN1TOKEN_INT       0x03
#define IN3TOKEN_INT       0x04
#define DISCONNECT_INT     0x05

#pragma name(USC_IRQ)
#pragma renamecode(?PR?USC_IRQ)

/******************************************************************************/
/*!
 * \par  Description:
 *     interrupt parse
 * \param[in]    none.
 * \param[out]   none.
 * \return       interrupt type

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static uint8 parse_uisr_source(void)
{
    if (0 != (Usbirq_hcusbirq & 0x01))
    {
        Usbirq_hcusbirq |= 0x01;
        return SETUP_INT;
    }
    else if (0 != (Usbirq_hcusbirq & 0x80))
    {
        Usbirq_hcusbirq |= 0x80;
        return DISCONNECT_INT;
    }
    else if (0 != (Ep03tokirq & 0x04))
    {
        Ep03tokirq |= 0x04;
        return OUT2TOKEN_INT;
    }
    else if (0 != (Ep03tokirq & 0x20))
    {
        Ep03tokirq |= 0x20;
        return IN1TOKEN_INT;
    }
    else if (0 != (Ep03tokirq & 0x80))
    {
        Ep03tokirq |= 0x80;
        return IN3TOKEN_INT;
    }
    else
    {
        return INVALID_INT;
    }
}

#if 0
void adjust_audio_pll(int16 data_count, int16 high_threshold, int16 low_threshold)
{
    uint8 pll_select_low = 0, pll_select_high = 0, sfr_temp = 0;
    if (gud_param.play_sample_rate == SAMPLE_44K1)
    {
        pll_select_low = 0;
        pll_select_high = 0x06;
    }
    else
    {
        pll_select_low = 0x08;
        pll_select_high = 0x0e;
    }
    
    SFR_BANK = BANK_CMU_RMU;
    if (data_count >= high_threshold)
    {
        AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | pll_select_high; //0x06; //0x0e; //adjust faster
    }
    else if (data_count <= low_threshold)
    {
        AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | pll_select_low; //0x00; //0x08; //adjust slower
    }

}
#endif
/******************************************************************************/
/*!
 * \par  Description:
 *     interrupt service
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void dma_adc_half_isr(void)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_DMA_USB2;
    if (0 != (0x03 & DMAnIP))
    {
        DMAnIP |= 0x03;
        record_data_count += RECORD_BUFF_HALF;

        if ((record_data_count > RECORD_HIGH_OVERFLOW) || (record_data_count <= g_record_packet_size))
        {
            DMAnCTL0 &= 0xfe;
            record_data_count = 0;
            start_record_flag = 0;
            rd_record_buff_ptr = 0;
        }

#ifdef __USE_ADC_SRFT__
        SFR_BANK = BANK_DAC_ADC;
        if (record_data_count >= RECORD_HIGH_THRESHOLD)
        {
            ADC_SRFT_CTL0 = (ADC_SRFT_CTL0 & 0xf9) | 0x02; //adjust slower
        }
        else if (record_data_count <= RECORD_LOW_THRESHOLD)
        {
            ADC_SRFT_CTL0 = (ADC_SRFT_CTL0 & 0xf9) | 0x04; //adjust faster
        }
#else
        if (start_play_flag == 0) //如果没播放，就调，优先保证播放不断音
        {
            
            //adjust_audio_pll(record_data_count, RECORD_HIGH_THRESHOLD, RECORD_LOW_THRESHOLD);
            SFR_BANK = BANK_CMU_RMU;
            if (record_data_count >= RECORD_HIGH_THRESHOLD)
            {
                AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | 0x08; //adjust slower
            }
            else if (record_data_count <= RECORD_LOW_THRESHOLD)
            {
                AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | 0x0e; //adjust faster
            }
            else
            {
                ;
            }
        }
#endif
    }
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     interrupt service
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void dma_dac_half_isr(void)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_DMA_USB1;
    if (0 != (0x03 & DMAnIP))
    {
        DMAnIP |= 0x03;
        play_data_count -= PLAY_BUFF_HALF;

        if ((play_data_count < PLAY_LOW_OVERFLOW) || (play_data_count > PLAY_HIGH_OVERFLOW))
        {
            DMAnCTL0 &= 0xfe; //
            start_play_flag = 0; //
            play_data_count = 0;
            wr_play_buff_ptr = 0;
        }

        SFR_BANK = BANK_CMU_RMU;
        if (play_data_count >= PLAY_HIGH_THRESHOLD)
        {
            AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | 0x0e; //adjust faster
        }
        else if (play_data_count <= PLAY_LOW_THRESHOLD)
        {
            AUDIO_PLL_CTL = (AUDIO_PLL_CTL & 0xf1) | 0x08; //adjust slower
        }
        else
        {
            ;
        }
        
        //adjust_audio_pll(play_data_count, PLAY_HIGH_THRESHOLD, PLAY_LOW_THRESHOLD);
    }
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     iso out token process.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void iso_out_token_isr(void)
{
    uint16 data_lenth, dma_lenth,condition;
    uint32 dma_address;
    uint8 sfr_temp = SFR_BANK;

    SFR_BANK = BANK_USB;
    while (1)
    {
        if (0x00 == (Out2cs_hcin2cs & 0x02))
        {
            break;
        }
        if (0x01 == (Out2cs_hcin2cs & 0x01))
        {
            break;
        }

        if (USB_IS_CONNECT != is_usb_connect())
        {
            break;
        }
    }
    if (DOWNLOAD_BIT_DEPTH_24== gud_param.bit_depth)
    {
        condition = PLAY_BUFF_SIZE / 3; //0x1400;//0x800; //PLAY_BUFF_HALF)//0x554)//PLAY_BUFF_HALF)
    } 
    else
    {
        condition= PLAY_BUFF_HALF;
    }
    if (play_data_count >= condition)
    {
        usb_state |= PLAY_RUNNING;
        if (start_play_flag == 0)
        {
            dma_reload_to_dac();
            start_play_flag = 1;
        }
    }

    SFR_BANK = BANK_USB;
    if (DOWNLOAD_BIT_DEPTH_24== gud_param.bit_depth)
    {
        data_lenth = (Out2bcl_hcin2bcl+(Out2bch_hcin2bch<<8));
    }
    else
    {
        //data_lenth = (Out2bcl_hcin2bcl+(Out2bch_hcin2bch<<8));
        data_lenth = Out2bcl_hcin2bcl;
    }
    if (data_lenth == 0)
    {
        goto exit_iso_out;
    }

    play_data_count += data_lenth;

    if (wr_play_buff_ptr + data_lenth > PLAY_BUFF_SIZE)
    {
        dma_address = PLAY_BUFF_START + wr_play_buff_ptr;
        dma_lenth = PLAY_BUFF_SIZE - wr_play_buff_ptr;
        if (0 != dma_lenth)
        {
            usb_fifo_access(EP2OUT, (uint8 *) dma_address, dma_lenth);
        }
        dma_address = PLAY_BUFF_START;
        dma_lenth = data_lenth - dma_lenth;
        usb_fifo_access(EP2OUT, (uint8 *) dma_address, dma_lenth);
        wr_play_buff_ptr = dma_lenth;
    }
    else
    {
        dma_address = PLAY_BUFF_START + wr_play_buff_ptr;
        dma_lenth = data_lenth;
        usb_fifo_access(EP2OUT, (uint8 *) dma_address, dma_lenth);
        wr_play_buff_ptr += data_lenth;
    }

exit_iso_out:
    Out2cs_hcin2cs |= 0x02;
    SFR_BANK = sfr_temp;
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     iso in token process.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void iso_in_token_isr(void)
{
    uint32 data_lenth, dma_lenth, dma_address;
    uint8 sfr_temp = SFR_BANK;

    if (0 == start_record_flag)
    {
        dma_reload_from_adc();
        start_record_flag = 1;
    }

    if (record_data_count < g_record_packet_size)
    {
        data_lenth = record_data_count;
    }
    else
    {
        data_lenth = g_record_packet_size;
    }
    data_lenth <<= 1;
    record_data_count -= data_lenth;

    SFR_BANK = BANK_USB;
    if (rd_record_buff_ptr + data_lenth > RECORD_BUFF_SIZE)
    {
        dma_address = RECORD_BUFF_START + rd_record_buff_ptr;
        dma_lenth = RECORD_BUFF_SIZE - rd_record_buff_ptr;
        if (0 != dma_lenth)
        {
            usb_fifo_access(EP1IN, (uint8 *) dma_address, dma_lenth);
        }
        dma_address = RECORD_BUFF_START;
        dma_lenth = data_lenth - dma_lenth;
        usb_fifo_access(EP1IN, (uint8 *) dma_address, dma_lenth);
        rd_record_buff_ptr = dma_lenth;
    }
    else
    {
        dma_address = RECORD_BUFF_START + rd_record_buff_ptr;
        dma_lenth = data_lenth;
        usb_fifo_access(EP1IN, (uint8 *) dma_address, dma_lenth);
        rd_record_buff_ptr += data_lenth;
    }

    In1cs_hcout1cs |= 0x02;
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     interrupt transfer in token process.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void hid_in_token_isr(void)
{
    if (hid_send_flag != 0)
    {
        hid_send_flag--;
        while (0 != (0x02 & In3cs_hcout3cs))
        {
            if (USB_IS_CONNECT != is_usb_connect())
            {
                break;
            }
        }
        Fifo3dat = hid_report;
        hid_report = 0;
        In3cs_hcout3cs |= 0x02;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     usb interrupt service func.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void usound_isr(void)
{
    uint8 int_type;
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    USBEIRQ |= 0x80;
    int_type = parse_uisr_source();
    switch (int_type)
    {
    case SETUP_INT:
        deal_setup();
        break;
    case OUT2TOKEN_INT:
        iso_out_token_isr();
        break;
    case IN1TOKEN_INT:
#ifdef _USB_RECORD_
        iso_in_token_isr();
#endif
        break;
    case IN3TOKEN_INT:
        hid_in_token_isr();
        break;
    case DISCONNECT_INT:
        USBEIRQ = 0x80; //关中断，ap将退出
        usb_state |= USOUND_PLUG_OUT;
        break;
    default:
        Usbirq_hcusbirq = 0xff;
        break;
    }
    SFR_BANK = sfr_temp;
    return;
}

