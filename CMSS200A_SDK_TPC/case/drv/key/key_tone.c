#include "actos.h"
#include "irq.h"
#include "key.h"

#define MAX_KEY_TONE_TIME 100
#pragma name(BAC_KT)
const uint8 Tonedate[2][2] = {0x26, 0x40, 0xda, 0xbf};
//const uint8 Tonedate[2][2] = {0x7F, 0x7F, 0x7F, 0x7F};
const uint8 Fout_44K_256fs[8] = {0, 0, 44, 0, 22, 0, 11, 0}; //i2S输出时是256fs
const uint8 Fout_48K_256fs[8] = {96, 0, 48, 32, 24, 16, 12, 8};
const uint8 Fout_44K_512fs[8] = {44, 0, 22, 0, 11, 0, 0, 0}; //i2s加ADC或只有ADC输出时是512fs
const uint8 Fout_48K_512fs[8] = {48, 32, 24, 16, 12, 8, 0, 0};

void Delay10ms(void); //24Mhz，延时500ms
extern void _nop_(void);
void ADC_DAC_loop(void);
/*
 **************************************************************
 *  void sKY_Beep(char value)           output BeepTone                 *
 **************************************************************
 */
bool sKY_Beep(uint8 api_no, uint8 value)
{
    api_no = api_no;
#if 0
    if(value == 1)
    {
        switch_blue_light(1);
        switch_red_light(0);
    }
    else
    {
        switch_blue_light(0);
        switch_red_light(1);
    }
#endif
    value = value;
    //romDI();
    ADC_DAC_loop();
    //romEI();
    return TRUE;
}
void config_sr_fine_tune(uint8 value)
{
    uint8 SFR_BANK_bak;
    uint8 i;
    SFR_BANK_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    // reset asrc module
    MRCR3_ASRCReset = 0;
    // enable asrc module clock
    CLKENCTL2_ASRCCLKEN = 1;
    // select asrc clock source
    ADC_DAC_CLK_CTL_ASRCCLKSEL = 1;
    // set fir_rds_ram clk
    MEMCLKENCTL0_FIR_RDS_RAMClkEn = 1;
    // select fir_rds_ram as cpu clock
    MEMCLKSELCTL1 &= (~0xc);

    for (i = 0; i < 0xC0; i++) //fir_rds_ram_l
    {
        *((uint8 *) (0x9640 + i)) = 0;
        *((uint8 *) (0x9740 + i)) = 0;
    }
    // select fir_rds_ram as asrc clock
    MEMCLKSELCTL1 = (MEMCLKSELCTL1 & (~0xc)) | 0x4;
    SFR_BANK = 0x13;
    // disable DAC Channel 0 SR Fine Tune
    DAC_CH0_SRFT_CTL0 = 0;

    DAC_CH0_SR_CTL = (DAC_CH0_SR_CTL & 0xf8);

    DAC_CH0_SR_CTL = value;

    SFR_BANK = BANK_CMU_RMU;
    // enable asrc module
    MRCR3_ASRCReset = 1;
    SFR_BANK = SFR_BANK_bak;
}
void ADC_DAC_loop(void)
{
    uint16 i, j, k;
    uint8 sfr_temp = SFR_BANK;
    uint8 temp, tonedata1, tonedata2;
    //    uint8 ADCSR;
    uint8 cur_samplerate, data_length;
	uint16 data_length1;
#if 0
    SFR_BANK = BANK_CMU_RMU;

            {
                goto exit;
                //直接退出
            }
        }
        //200ms延迟后，仍等不到kt finish，直接退出
        break; //退出
    }

exit:
    SetPAVolume(paVolBak);
    SFR_BANK = sfr_bak;
#endif
    SFR_BANK = BANK_DAC_ADC;
    DAC_CH1_FIFO_CTL0 |= 0x20;
    DAC_CH0_FIFO_CTL0 |= 0x20;
    DAC_CH0_FIFO_CTL0 &= 0xF1;
    DAC_CH0_FIFO_CTL0 |= 0x01;
    DAC_CTL |= 0x11;
    //    config_sr_fine_tune(srcnum);
    SFR_BANK = BANK_CMU_RMU;
    cur_samplerate = AUDIO_PLL_CTL & 0x0e;
    temp = (ADC_DAC_CLK_CTL & 0x07);
    //	ADCSR=ADC_DAC_CLK_CTL & 0x70;

    if(cur_samplerate <= 0x06) //44.1k
    {

        //	   data_length=44;//14;//22;//14;
        data_length = Fout_44K_512fs[0];
        data_length1 = 32*MAX_KEY_TONE_TIME;
    }
    else                        //48k
    {
        //	   data_length=44;//16;//24;//16;
        data_length = Fout_48K_512fs[0];
        data_length1 = 32*MAX_KEY_TONE_TIME;
    }
    SFR_BANK = BANK_DAC_ADC;
    for(j = 0; j < data_length1; j++)
    {
        k = 0;
        for (i = 0; i < data_length; i++)
        {
            if(i >= (data_length / 2))
            {
                k = 1;
            }
            tonedata1 = Tonedate[k][0];
            tonedata2 = Tonedate[k][1];
            while ((DAC_CH0_FIFO_CTL1 & 0x08) == 0x08)
            {
                ;
            }
            //左声道
            DAC_CH0_PCML = 0x00;
            DAC_CH0_PCMM = tonedata1;
            DAC_CH0_PCMH = tonedata2;
            while ((DAC_CH0_FIFO_CTL1 & 0x08) == 0x08)
            {
                ;
            }
            //右声道
            DAC_CH0_PCML = 0x00;
            DAC_CH0_PCMM = tonedata1;
            DAC_CH0_PCMH = tonedata2;
        }
		ClearWatchDog();
    }
#if 1
    for (i = 0; i < data_length * 12; i++)
    {
        i++;
        while ((DAC_CH0_FIFO_CTL1 & 0x08) == 0x08)
        {

            ;//ClearWatchDog();
        }
        DAC_CH0_PCML = 0x00;
        DAC_CH0_PCMM = 0;

        DAC_CH0_PCMH = 0;

        while ((DAC_CH0_FIFO_CTL1 & 0x08) == 0x08)
        {
            ;//ClearWatchDog();
        }
        DAC_CH0_PCML = 0x00;
        DAC_CH0_PCMM = 0;

        DAC_CH0_PCMH = 0;
    }
#endif
    SFR_BANK = BANK_DAC_ADC;
    DAC_CTL &= 0xEF;
    DAC_CH0_FIFO_CTL0 &= 0xFE;
    DAC_CH0_FIFO_CTL0 |= 0x20;
    SFR_BANK = BANK_CMU_RMU;
    // reset asrc module
    MRCR3_ASRCReset = 0;
    SFR_BANK = sfr_temp;
}

void Delay10ms(void)
{
    uint8 i, j;
    for (i = 0; i < 255; i++)
    {
        for (j = 0; j < 255; j++)
        {
            _nop_();
            _nop_();
        }
    }
}
