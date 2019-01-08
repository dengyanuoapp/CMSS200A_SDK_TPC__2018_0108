#include "drv_key.h"

#pragma name(BAC_IE0)
bool key_init(uint8 api_no)
{
    uint8 sfr_bak;
    api_no = api_no;
    sfr_bak = SFR_BANK;

    SFR_BANK = BANK_GPIO;
    AD_Select1 |= 0x01;/*gpio_g2 mapping to LRADC1(rem_con)*/
    SFR_BANK = BANK_PMU;
    PMUADC_CTL |= 0xa0; //enable LRADC1,ADC FREQUENCY 128Hz
    _nop_();
    delay(); //enable LRADC1后需要等待最长10ms才能读出正确的数据，否则可能是全0
    SYSTEM_CTL &= 0x7f; //设置play按键时间=2S

	adjust_key_sequence();
	
    TM_SetTimer((uint32) key_scan, 2); //设置RTC中断函数入口
    //   TM_SetClock((int) (uint16) key_scan, 1);

    //ctc set
    SFR_BANK = BANK_RTC;
    CTCCNTH = 0xb7; //0x07;
    CTCCNTL = 0x1a; //0x52;
    CTCCTL = 0xc0; //0xc8;       /256  hosc

    IRQ_Intercept((uint32) ctc_isr, IRQ_CTC); //设置CTC中断函数入口
    CTCCTL |= 1 << 7; //enable ctc
    IE0 |= 1 << 1; //enable IE.1

    //init ir key
    //init_ir();
    //    ADC_DAC_init();
    init_ADSelect0();
    init_bat_gpio();
    return 1;
}

void delay(void)
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
void key_exit(uint8 api_no)
{
    api_no = api_no;
    TM_KillTimer(0x02); //关闭RTC
    IE0 &= ~(1 << 1); //disable IE.1
    IRQ_Release((uint16) ctc_isr, IRQ_CTC);
    IRQ_Release((uint16) IR_Interrupt, IRQ_UART);
}
void ADC_DAC_init(void)
{

    uint16 i;
    uint8 sfr_temp = SFR_BANK;

    SFR_BANK = BANK_CMU_RMU;
	CLKENCTL0 = 0x0;
    CLKENCTL0 |= 0x20;
 //   CLKENCTL2 |= 0x40;
    MRCR2 &= 0xEF;
    MRCR2 |= 0x10;
    MRCR3 &= 0xFE;

    SFR_BANK = 0x06; //initial MFP_GPIO
    GPIOEINEN = 0x00;
    GPIOEOUTEN = 0x00;

    SFR_BANK = BANK_CMU_RMU; //set i2s_sr
    ADC_DAC_CLK_CTL &= 0xf8;
    //ADC_DAC_CLK_CTL |= 0x10;
    ADC_DAC_CLK_CTL |= 0x02;
    AUDIO_PLL_CTL &= 0xf1;
    AUDIO_PLL_CTL |= 0x09;

    SFR_BANK = BANK_SPDIF;
    I2S_CTL1 |= 0x20;

    SFR_BANK = BANK_AUIP;
    //init dac
    DAC_CH0_FIFO_CTL0 |= 0x10;
    DAC_CTL = 0x41;
    DAC_CH0_FIFO_CTL0 |= 0x01;
    DAC_CTL |= 0x10;

    SFR_BANK = BANK_CMU_RMU; //
    MEMCLKENCTL0 |= 0x02;
    MEMCLKSELCTL1 &= 0xf3;
    for (i = 0; i < 0xC0; i++) //fir_rds_ram_l
    {
        *((uint8 *) (0x9640 + i)) = 0;
        *((uint8 *) (0x9740 + i)) = 0;
    }

    MEMCLKSELCTL1 |= 0x04;
    //set SRC
    SFR_BANK = BANK_DAC_ADC;
    DAC_CH0_SR_CTL &= 0xf8;
    DAC_CH0_SR_CTL |= 0;
    SFR_BANK = BANK_CMU_RMU;
    MRCR3 |= 0x01;
    SFR_BANK = sfr_temp;

}
//初始红外遥控器按键相关的寄存器
void init_ir(void)
{
#if 0
    uint8 sfr_bak;
    /*IRC config*/
    SFR_BANK = BANK_CMU_RMU;
    //IRC controller reset
    MRCR1 &= 0xFB;
    MRCR1 |= 0x04;
    CLKENCTL1 |= 0x20;

    SFR_BANK = BANK_GPIO;
    //数字红外指外接红外头
    //模拟红外指外接二极管
    //set GPIO_c5 as IR_RX
    GPIOCOUTEN &= 0xDF;
    GPIOCINEN &= 0xDF;
    GPIODDAT &= 0xDF;
    MFP_CTL5 |= 0x10;

    SFR_BANK = BANK_IR;
    //enable irc ,select nec decode and enable ir_irq
    IR_CTL = 0xb0;
    IR_LUC = 0x0; //IRC low user code
    IR_HUC = 0xbf; //IRC high user code
    IE0 |= 0x10; //enable IRC interrupt
    SFR_BANK = sfr_bak;
    //注册IR中断
    IRQ_Intercept((uint32) IR_Interrupt, IRQ_UART);
#endif
}
void init_LRADCkey(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    AD_Select1 |= 0x01;
    SFR_BANK = sfr_bak;
}
void init_bat_gpio(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    //使能bat检测,将GPIO A6切换到LCD_seg2模式
    BAT_CHECK_GPIO_INPUT &= ~ (1 << BAT_CHECK_GPIO_NUM);
    MFP_CTL0 &= 0xEF;
    MFP_CTL0 |= 0x08;
    //使能red light开关GPIO 口
    RED_LIGHT_GPIO_OUTPUT |= (1 << RED_LIGHT_GPIO_NUM);
    RED_LIGHT_GPIO_DAT &= ~(1 << RED_LIGHT_GPIO_NUM);
#if 0
    //使能blue light开关GPIO 口
    BLUE_LIGHT_GPIO_OUTPUT |= (1 << BLUE_LIGHT_GPIO_NUM);
    BLUE_LIGHT_GPIO_DAT &= ~(1 << BLUE_LIGHT_GPIO_NUM);
#endif
    SFR_BANK = sfr_bak;
}
void switch_red_light(uint8 flag)
{
#if 0
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    if(flag != 0)
    {
        RED_LIGHT_GPIO_DAT |= (1 << RED_LIGHT_GPIO_NUM);
    }
    else
    {
        RED_LIGHT_GPIO_DAT &= ~(1 << RED_LIGHT_GPIO_NUM);
    }
    SFR_BANK = sfr_bak;
#endif
}
void switch_blue_light(uint8 flag)
{
#if 0
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    if(flag != 0)
    {
        BLUE_LIGHT_GPIO_DAT |= (1 << BLUE_LIGHT_GPIO_NUM);
    }
    else
    {
        BLUE_LIGHT_GPIO_DAT &= ~(1 << BLUE_LIGHT_GPIO_NUM);
    }
    SFR_BANK = sfr_bak;
#else
    flag = flag;
#endif
}
void drv_switch_pilot_light(uint8 api_no, uint8 light_type, uint8 flag)
{
#if 0
    api_no = api_no;
    switch(light_type)
    {
    case 0:
        switch_red_light(flag);
        break;
    case 1:
        switch_blue_light(flag);
        break;
    default:
        break;
    }
#endif
}
//将GPIOC6切换到AVCC1模式，以便给mic端供电
//将GPIOC7切换到LRADC5,用于作为LRADC按键使用
void init_ADSelect0(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    AD_Select &= 0x3F;
    AD_Select |= 0x40;//GPIOC6 is used as AVCC1(analog function)
//    AD_Select |= 0x10;//GPIOC7 is used as LRADC5(analog function)
//    AD_Select |= 0x01;//GPIOA6 is used as LRADC4(analog function)
	
    SFR_BANK = sfr_bak;
}
