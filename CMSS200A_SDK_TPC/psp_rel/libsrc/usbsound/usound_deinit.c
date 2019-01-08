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
 * \file     usound_deinit.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               usb及Audio模块卸载
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
#pragma name(USC_DEINIT)
#pragma renamecode(?PR?USC_DEINIT)

/******************************************************************************/
/*!
 * \par  Description:
 *     ADC、DAC deinit.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void ADC_DAC_deinit(void)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_DAC_ADC; //disable asrc
    DAC_CH1_SRFT_CTL0 &= 0xfe;
    DAC_CH0_SRFT_CTL0 &= 0xfe;
    ADC_SRFT_CTL0 &= 0xfe;

    SFR_BANK = BANK_CMU_RMU;
    MRCR3 &= 0xfe;

    SFR_BANK = BANK_DMA_USB1; //stop dma
    DMAnCTL0 &= 0xfc;
    SFR_BANK = BANK_DMA_USB2;
    DMAnCTL0 &= 0xfc;

    SFR_BANK = BANK_AUIP; //diable fifo
    PA_VOLUME = 0x06; //pa_volume_bak;
    DAC_CH1_FIFO_CTL0 &= 0xfe;
    ADC_FIFOCTL0 &= 0xfb;
    DAC_ANALOG0 |= 0x01;

    //SFR_BANK = BANK_CMU_RMU;
    //AUDIO_PLL_CTL &= 0xfe; //disable AUDIO_PLL
}

/******************************************************************************/
/*!
 * \par  Description:
 *     usb exit .
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void usound_exit(void)
{
    uint8 sfr_temp = SFR_BANK;
    IE0 &= 0x7f;
    AIE &= 0xba;

    ADC_DAC_deinit();
    SFR_BANK = BANK_USB;
    USBEIRQ = 0xc0;
    Usbirq_hcusbirq = 0xff;
    Usbien_hcusbien = 0x00;
    Ep03tokirq = 0xff;
    Ep03tokien = 0x00;

    SFR_BANK = BANK_CMU_RMU;
    MEMCLKSELCTL1 &= 0x70;//link pcmram、fir_rds_ram、uram to cpu_clock
    MemBankCtl |= muram_addr;

    SFR_BANK = BANK_USB;
    /* software disconnect bit.  */
    UsbCTRL_STUS |= 0x40;
    /* disable software Vbus.  */
    IDVBUSCTRL = 0x08;
    BKDOOR = BKDOOR & 0x7f;
    IE0 |= 0x80;

    SFR_BANK = BANK_CMU_RMU;
    MRCR2 = MRCR2 | 0x04; //reset usb module before
    mdelay(1);
    MRCR2 = MRCR2 | 0x08; //reset usb module
    SFR_BANK = BANK_USB;
    while ((USBSTATUS & 0x01) == 0x01)
    {
        ; /*do nothing*/
    }
    USB_PHYCTRL |= 0xc0; //phy disable
    DPDMCTRL = 0x10; // 恢复uhost检测
    MULTI_USED &= (~0x80); //disable UVDD

    SFR_BANK = BANK_CMU_RMU;
    CLKENCTL1 &= (~0x40); //disable usb clock
    IRQ_Release(usb_isr, IRQ_USB);
//#ifdef _USB_RECORD_
    IRQ_Release(dma_adc_isr, IRQ_DMA1);
//#endif
    IRQ_Release(dma_dac_isr, IRQ_DMA0);
    return;
}
