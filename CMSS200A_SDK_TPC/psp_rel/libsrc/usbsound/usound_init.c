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
 * \file     usound_init.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               usb、AUDIO等模块初始化
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
#include "gl5115_bit.h"

#define USE_DAC_CH0
#define USE_ADC_LEFT_CHANNEL

#pragma name(USC_INIT)
#pragma renamecode(?PR?USC_INIT)

void clear_fir_rds_ram(void)
{
    uint8  i;
    uint8  SFR_BANK_bak;
    uint8* pFirRdsRamL = (uint8*)0x9640;
    uint8* pFirRdsRamH = (uint8*)0x9740;
    uint8* tempH = pFirRdsRamH;
    uint8* ptempL = pFirRdsRamL;

    SFR_BANK_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    // enable fir_rds_ram clock
    MEMCLKENCTL0_FIR_RDS_RAMClkEn = 1;
    // set fir_rds_ram clock to cpu
    MEMCLKSELCTL1 &= (~0xc);
    for (i=0; i<0xc0; i++) {
        *pFirRdsRamL++ = *pFirRdsRamH++ = 0;
    }

    // select fir_rds_ram as asrc clock
    MEMCLKSELCTL1 = (MEMCLKSELCTL1&(~0xc))|0x4;
    SFR_BANK = SFR_BANK_bak;
}

void clear_fir_cs_ram(void)
{
    uint16 i;
    uint8  SFR_BANK_bak;
    uint8* pFirCsRamL = (uint8*)0x4700;
    uint8* pFirCsRamM = (uint8*)0x4f00;
    uint8* pFirCsRamH = (uint8*)0x5700;
    SFR_BANK_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    // enable fir_cs_ram clock
    MEMCLKENCTL0_FIR_CS_RAMClkEn = 1;
    // set fir_rds_ram clock to cpu
    MEMCLKSELCTL0 &= 0xcf;
    for(i=0;i<0x100;i++) {
        *pFirCsRamL++ = *pFirCsRamM++ = *pFirCsRamH++ =0;
    }
    // set fir_cs_ram clock to asrc
    MEMCLKSELCTL0 |= 0x10;
    SFR_BANK = SFR_BANK_bak;
}

void src(uint8 SrcType)
{
    uint32 i = 0;
    uint8 SFR_BANK_bak;

    SFR_BANK_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    // reset asrc controller
    MRCR3_ASRCReset = 0;
    // enable asrc controller clock
    CLKENCTL2_ASRCCLKEN = 1;
#if 1 //#ifdef USE_PLL
    ADC_DAC_CLK_CTL_ASRCCLKSEL = 1;
#else
    ADC_DAC_CLK_CTL_ASRCCLKSEL = 0;
#endif
     
    SFR_BANK = 0x13;
    // disable DAC Channel 0 SR Fine Tune
    DAC_CH0_SRFT_CTL0_DA0SRFTEN = 0;
    // disable DAC Channel 1 SR Fine Tune
    DAC_CH1_SRFT_CTL0_DA1SRFTEN = 0;
    // config asrc mode : half empty/ half full is from auip pcmram
    DAC_CH0_SRFT_CTL0 &= (~0x18);
    //DAC_CH1_SRFT_CTL1 &= (~0x18);
    DAC_CH1_SRFT_CTL0 &= (~0x18);
#ifdef USE_DAC_CH0
    clear_fir_rds_ram();
    DAC_CH0_SR_CTL = (DAC_CH0_SR_CTL&0xf8)|SrcType;

    //DAC_CH0_SRFT_CTL1 = 0x24; //48M配置不会断音
    //DAC_CH0_SRFT_CTL2 = 0x00;
    DAC_CH0_SRFT_CTL1 = 0x20;  //52M配置不会断音
    DAC_CH0_SRFT_CTL2 = 0x00;
   
    DAC_CH0_SRFT_CTL3 = 0x20;
    DAC_CH0_SRFT_CTL4 = 0x80;
    DAC_CH0_SR_GAIN = 0x9b;//0xab; //+0db gain // +6dB gain
	// enable DAC Channel 0 SR Fine Tune
    DAC_CH0_SRFT_CTL0_DA0SRFTEN = 1;
#else
    clear_fir_cs_ram();
    DAC_CH1_SR_CTL = (DAC_CH1_SR_CTL&0xf8)|SrcType;
    DAC_CH1_SRFT_CTL1 = 0x1f;
    DAC_CH1_SRFT_CTL2 = 0x80;
    DAC_CH1_SRFT_CTL3 = 0x20;
    DAC_CH1_SRFT_CTL4 = 0x80;
    DAC_CH1_SR_GAIN = 0xab; // +6dB gain
    // enable DAC Channel 1 SR Fine Tune
    DAC_CH1_SRFT_CTL0_DA1SRFTEN = 1;
#endif

    SFR_BANK = BANK_CMU_RMU;
    // enable asrc module
    MRCR3_ASRCReset = 1;
    SFR_BANK = SFR_BANK_bak;
}

void set_dac_samplerate(uint8 nSampleRateIndex)
{
    switch(nSampleRateIndex)
    {   
//        case SAMPLE_48K:
        case SAMPLE_96K:
            src(K_SRC_1_2);
            break;
        default:
            break;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     ADC、DAC init.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void ADC_DAC_init(void)
{
    uint8 sfr_temp = SFR_BANK;

	//vcc set to 3.1v
	SFR_BANK = BANK_PMU;
	VOUT_CTL &= ~(0x07<<4);
	VOUT_CTL |= 0x05<<4;

    SFR_BANK = BANK_CMU_RMU;
    MCU_PLL_CTL = 0xad;//0xaf;//0xac;//0xad;//0xae;//0xaf;

    CLKENCTL0 |= 0x30;	// enable DAC & ADC clock
	//CLKENCTL2 = 0x0;
    //CLKENCTL2 |= 0x20; //enable dma clock
    MRCR2 &= 0x0f;
    MRCR2 |= 0x30;	// set DAC & ADC to normal mode
    MRCR3 &= 0xfe; //reset ASRC

    ADC_DAC_CLK_CTL &= 0x88;
    if(SAMPLE_48K == gud_param.record_sample_rate)
    {
        ADC_DAC_CLK_CTL |= 0x10;//0x12;//0x10; //48k //0x40; //16k
        g_record_packet_size = 96;
    }
    else
    {
        ADC_DAC_CLK_CTL |= 0x40; //48k //0x40; //16k
        g_record_packet_size = 32;
    }
    
    AUDIO_PLL_CTL &= 0xb0;
    if (gud_param.play_sample_rate == SAMPLE_44K1)
    {
       AUDIO_PLL_CTL |= 0x05; //0x0d; //44.4k or 48k download
    }
    else
    {
       AUDIO_PLL_CTL |= 0x0d;//0x0d;//0x09; //0x0d; //44.4k or 48k download
    }
    AUDIO_PLL_CTL |= 0x01;

    SFR_BANK = BANK_AUIP;

    DAC_VOLUME0 = 0x9b;
    DAC_VOLUME1 = 0x9b;

#ifdef USE_DAC_CH0
    //DAC_CH0_FIFO_CTL0 &= 0xee; //to dac
    //DAC_CH0_FIFO_CTL0 &= (~0xe);
    DAC_CH0_FIFO_CTL0 &= 0xe0;
    DAC_CH0_FIFO_CTL0 |= 0x06; //from dma
    DAC_CH0_FIFO_CTL1 |= 0x04; //enable drq
#else
    DAC_CH1_FIFO_CTL0 &= 0xee; //to dac
    DAC_CH1_FIFO_CTL0 &= (~0xe);
    DAC_CH1_FIFO_CTL0 |= 0x06; //from dma
    DAC_CH1_FIFO_CTL1 |= 0x04; //enable drq
#endif

    //init adc
    AINOP_CTL = (AINOP_CTL & 0xf8) | 0x01; //aux and mic
    ADC_GAIN0 &= 0xe7; //ain0op disable
    ADC_GAIN1 |= 0x30;
    AINOP_CTL &= 0xf7; //ain op1 gain boost disable
    ADC_GAIN0 = (ADC_GAIN0 & 0xf8) | 0x02; //set ain op0 gain 0db
    ADC_GAIN1 = (ADC_GAIN1 & 0xf0) | 0x02; //0x07; //set ain op1 gain 0db
    ADC_GAIN1 &= 0x3f; //disable mic mix left to right and right to left
	ADC_GAIN1 |= 0x80;
    AINOP_CTL &= 0xcf;//0xe7; //disable op0 and op1 zero-cross detect enable

    ADC_CTL0 = 0x00;
    ADC_CTL0 |= 0x03;
    AINOP_CTL |= 0xc0; //0xc8; //only disable OP1GB bit

    ADC_FIFOCTL1 = 0x10;
    ADC_FIFOCTL0 = 0x48;
    ADC_FIFOCTL0 |= 0x04;

#ifdef __USE_ADC_SRFT__
    SFR_BANK = BANK_DAC_ADC;
    ADC_SRFT_CTL0 |= 0x08;
    ADC_SRFT_CTL1 = 0x20; //+/- 0.4%
    ADC_SRFT_CTL2 = 0x20;
    ADC_SRFT_CTL3 = 0x1f;
    ADC_SRFT_CTL4 = 0xdf;

    SFR_BANK = BANK_CMU_RMU; //enable aa_ram cs_ram clock
    MEMCLKENCTL0 |= 0x30;
    MEMCLKSELCTL0 &= 0xf3;

    for (i = 0; i < 0x40; i++) //init aa_ram
    {
        *((uint8 *) (0x9600 + i)) = 0;
        *((uint8 *) (0x9700 + i)) = 0;
        *((uint8 *) (0x9800 + i)) = 0;
    }
    MEMCLKSELCTL0 |= 0x04;

    SFR_BANK = BANK_DAC_ADC;
    ADC_SRFT_CTL0 = (ADC_SRFT_CTL0 & 0xf9) | 0x02;
    ADC_SRFT_CTL0 |= 0x01;

    for (i = 0; i < 0xff; i++)
    {
        ;
    }

    SFR_BANK = BANK_CMU_RMU;
    MRCR3 |= 0x01;
#endif

    SFR_BANK = BANK_AUIP;
    I2S_CTL1 = 0x00;
    DAC_ANALOG0 |= 0x10;
    PA_APCTL |= 0x02;
    DAC_ANALOG0 &= 0xf7;
    DDV_CTL0 |= 0x10;	 //全能直驱
//	DDV_CTL0 = 0xf7;
    pa_volume_bak = PA_VOLUME;
    PA_VOLUME = 0x06;
    // dac l&r enable, dac not mute
    DAC_ANALOG0 |= 0xc1;
    // OSR 32X
    DAC_CTL &= 0x3f;
    // disable Zero Cross Detection
    DAC_CH0_FIFO_CTL0 &= 0x2f;
#ifdef USE_DAC_CH0
    // dac en, dac if en, dac ch0 mix en
    DAC_CTL |= 0x13;
    // en dac fifo
    DAC_CH0_FIFO_CTL0 |= 0x01;
#else
    // dac en, dac if en, dac ch1 mix en
    DAC_CTL |= 0x23;
    // en dac fifo
    DAC_CH1_FIFO_CTL0 |= 0x01;
#endif

    set_dac_samplerate(gud_param.play_sample_rate);

    SFR_BANK = sfr_temp;
}

/*
 ********************************************************************************
 *                           OTG_PHY_Setting
 * Description : USB PHY设置
 * Arguments   : d: address; e: value.
 * Returns     : none
 * Notes       : FPGA version, Just for GL6005 PHY setting.
 ********************************************************************************
 */

/*
OTG_PHY_Setting(0xe7, 0x0b);
OTG_PHY_Setting(0xe7, 0x1f);
OTG_PHY_Setting(0xe0, 0xa3);
OTG_PHY_Setting(0xe3, 0x9b);




void OTG_PHY_Setting(unsigned char addr, unsigned char value)
{
    unsigned char tmp;

    VDSTAT = value; //set Value

    tmp = addr;
    VDCTRL = tmp & 0x0f; //set low addr
    usdelay(5);
    // mdelay(1);
    VDCTRL = tmp | 0x10;
    usdelay(5);
    // mdelay(1);

    tmp = (addr >> 4);
    VDCTRL = tmp & 0x0f; //set high addr
    usdelay(5);
    // mdelay(1);
    VDCTRL = tmp | 0x10;
    usdelay(5);
    // mdelay(1);

}*/

void OTG_PHY_Setting(unsigned char addr, unsigned char value)
{
    unsigned char tmp;

    VDSTAT = value; //set Value

    tmp = addr;
    VDCTRL = tmp & 0x0f; //set low addr
    VDCTRL = tmp | 0x10;

    tmp = (addr >> 4);
    VDCTRL = tmp & 0x0f; //set high addr
    VDCTRL = tmp | 0x10;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     usb module deinit.
 * \param[in]    none.
 * \param[out]   none.
 * \return       1:sucess, 0:fail

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
uint8 usound_init(usound_param *pud_param)
{
    uint8 ret_var;
    uint8 sfr_temp = SFR_BANK;

    SFR_BANK = BANK_CMU_RMU;
    MRCR2 &= 0xf3;
    muram_addr = MemBankCtl & 0x08; //save MURAM_ADDR
    MemBankCtl |= 0x08;
    MEMCLKENCTL0 |= 0xcb;//0x8b; //enable pcmram、fir_rds_ram、uram 、fir_mpx_ram clock
    MEMCLKSELCTL0 &= 0x3c; //0xfc; //link fir_mpx_ram to cpu clock
    MEMCLKSELCTL1 &= 0x70;
    MEMCLKSELCTL1 |= 0x87;//0x80;//0x03;//0x84;//0x87;//0x8c;//link fir_rds_ram、uram to uram_clock,  link pcmram to cpu clock
    CLKENCTL1 |= 0x40; //enable usb clock
    CLKENCTL2 |= 0x20; //enable dma clock

    MRCR2 = MRCR2 | 0x01; //reset DMA
    MRCR2 = MRCR2 | 0x04; //reset usb module
    mdelay(1);

    SFR_BANK = BANK_USB;
    USB_Efuse_Ref &= 0xbf; //usb operation mode
    USB_PHYCTRL &= 0x3f;
    mdelay(1);

    SFR_BANK = BANK_PMU;
    MULTI_USED |= 0x80; //enable UVDD

    SFR_BANK = BANK_USB;
    DPDMCTRL |= 0x1f; //enable status detect and 500k pullup
    mdelay(1);
    IDVBUSCTRL = 0x1f;

    SFR_BANK = BANK_CMU_RMU;
    MRCR2 = MRCR2 | 0x08; //reset usb module

    SFR_BANK = BANK_USB;
    while (0x01 == (USBSTATUS & 0x01))
    {
        ; /*do nothing*/
    }
    BKDOOR = BKDOOR | 0x80;

    IE0 &= 0x7f;
    if ((DPDMCTRL & 0x40) != 0x00)
    {
        /* enable USB INT */
        AIE = AIE | 0x01;

        //phy set
        OTG_PHY_Setting(0xe7, 0x0b);
        OTG_PHY_Setting(0xe7, 0x1f);
        OTG_PHY_Setting(0xe0, 0xa3);
        
       /* OTG_PHY_Setting(0xe7, 0x4b);
        OTG_PHY_Setting(0xe7, 0x5f);
        OTG_PHY_Setting(0xe0, 0xb3);
        
        OTG_PHY_Setting(0xe3, 0x9b);
        printh(0x8899);*/
        mdelay(1);

        /* Enable USB setup, connect/disconnect interrupt.  */
        Usbirq_hcusbirq = 0xff;
        Usbien_hcusbien = 0x81;

        Ep03tokirq = 0xa4;
        Ep03tokien = 0xa4; //Enable IN1?OUT2?IN3 TOKEN
        //Ep03tokien = 0x24;  //endpoint 3 ???????????

        /* Enable USB external Interrupt request.  */
        USBEIRQ = 0x80;
        USBEIRQ = 0x08;

        UsbCTRL_STUS = 0; //usb connection
        ret_var = 1;
    }
    else
    {
        ret_var = 0;
    }

    memcpy(&gud_param,pud_param,sizeof(gud_param));
    descriptor_init(pud_param);
    ADC_DAC_init();
    usb_isr = IRQ_Intercept((uint32) usound_isr, IRQ_USB);
//#ifdef _USB_RECORD_
    dma_adc_isr = IRQ_Intercept((uint32) dma_adc_half_isr, IRQ_DMA1);
//#endif
    dma_dac_isr = IRQ_Intercept((uint32) dma_dac_half_isr, IRQ_DMA0);
    AIE |= 0x44;
    IE0 = IE0 | (uint8) 0x80;
    SFR_BANK = sfr_temp;
	vol_change_flag=0;
    return ret_var;
}
