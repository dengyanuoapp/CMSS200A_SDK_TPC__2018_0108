/*
 ********************************************************************************
 *                       GL5101---fmt driver
 *
 *                (c) Copyright 2009-2010, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : fmt_i2c_drv.msa
 * By     : alexliu
 * Version: 1> v1.00     first version     2009-1-17 16:19:49
 *
 ********************************************************************************
 * $Id: i2c_drv,v 1.0 2009-1-17 16:20:04 alexliu Exp $
 ********************************************************************************
 */
#include "ap_common.h"

/*TX MSK*/
#define TX_MSK_SYS_L        0x03ff
#define TX_MSK_SYS_H        0xfff8
#define TX_MSK_RF           0xf007
#define TX_MSK_SYS_PLL_PB   0xff03
#define TX_MSK_SYS_PLL_SB   0xf07f
#define TX_MSK_RFPLL_PB     0x807f
#define TX_MSK_RFPLL_SB_L   0x7fff
#define TX_MSK_RFPLL_SB_H   0xfff8
#define TX_MSK_PC           0xfff0
#define TX_MSK_POWC         0xfff0
#define TX_MSK_PILOT        0xfffc
#define TX_MSK_ST           0xfffb
#define TX_MSK_MUTE         0xfff7
#define TX_MSK_EMP          0xffef
#define TX_MSK_TIME         0xf3ff
#define TX_MSK_GAIN         0xfff8

#define REGPOS_RFPLL_PB     7
#define REGPOS_RFPLL_SB_H   1
#define REGPOS_RFPLL_SB_L   15

#define IIC_Delay_Time      5

#define IIC_SCL_High        GPIOGDAT |= 0x08;  //gpio_g3
#define IIC_SCL_Low         GPIOGDAT &= 0xf7;  //gpio_g3
#define IIC_SDA_High        GPIOGDAT |= 0x10;  //gpio_g4
#define IIC_SDA_Low         GPIOGDAT &= 0xef;  //gpio_g4
#define IIC_SCL_OUTPUT_EN        GPIOGOUTEN |= 0x08;    //gpio_g3
#define IIC_SCL_OUTPUT_DIS       GPIOGOUTEN &= 0xf7;    //gpio_g3
#define IIC_SCL_INPUT_EN         GPIOGINEN |= 0x08;     //gpio_g3
#define IIC_SCL_INPUT_DIS        GPIOGINEN &= 0xf7;     //gpio_g3
#define IIC_SDA_OUTPUT_EN        GPIOGOUTEN |= 0x10;    //gpio_g4
#define IIC_SDA_OUTPUT_DIS       GPIOGOUTEN &= 0xef;    //gpio_g4
#define IIC_SDA_INPUT_EN         GPIOGINEN |= 0x10;     //gpio_g4
#define IIC_SDA_INPUT_DIS        GPIOGINEN &= 0xef;     //gpio_g4
#define IIC_SDA_REG        GPIOGDAT
#define IIC_SDA_BIT        0xef

/*write_buffer[0]=0x5 value,write_buffer[1]=0x6 value,用于设置FM发射频率*/
/*write_buffer[2]=0x0a value,用于设置FM发射功率*/
/*write_buffer[3]=0x0c value,用于设置FM*/

#pragma name(FM_TX_DATA)

uint16 FM_TX_Write_BUFF[4] =
{ 0x9CA0, 0x8806, 0x0068, 0x0246 };

//void AS6300_hw_init(void);
void FM_TX_Write_Data(uint8 Reg, uint16 Reg_val);
uint16 FM_TX_Read_Data(uint8 Reg);
void FM_TX_Read_Freq(void);
uint8 IIC_ReadByte(uint8 ack);
uint8 IIC_WriteByte(uint8 c);
void IIC_Stop();
void IIC_Start();
void IIC_Delay(uint16 t);
void IIC_SendAck(bool k);

#pragma renamecode(AP_FM_TX)

void FM_TX_init(void) /*将gpio_c0/c1设置作为gpio，并且使能output,使用其他gpio模拟i2c，依此进行修改*/
{
    uint8 sfr_bak;

    //disable_interrupt();

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    IIC_SDA_High;
    IIC_SCL_High;

    IIC_SCL_OUTPUT_EN;
    IIC_SDA_OUTPUT_EN;
    IIC_SCL_INPUT_EN;
    IIC_SDA_INPUT_EN;

    MFPSEL6 &= 0x7f; //JTAG disable
    FM_TX_Write_Data(0, 0x5e00);
    FM_TX_Write_Data(1, 0x03b8);
    FM_TX_Write_Data(2, 0x27d3);
    FM_TX_Write_Data(3, 0x01c5);
    FM_TX_Write_Data(4, 0x4e03);
    FM_TX_Write_Data(5, 0x1da0);
    FM_TX_Write_Data(6, 0x8801);
    FM_TX_Write_Data(7, 0x592c);
    FM_TX_Write_Data(8, 0x0881);
    FM_TX_Write_Data(9, 0x0246);
    //FM_TX_Write_Data(10, 0x006f);
    FM_TX_Write_Data(10, 0x0068);
    FM_TX_Write_Data(11, 0x4018);
    //FM_TX_Write_Data(12, 0x0083);
    FM_TX_Write_Data(12, 0x1083);

    //FM_TX_Write_Data(13, 0x0601);
    FM_TX_Write_Data(13, 0x0600); //-6db
    FM_TX_Write_Data(14, 0xe6db);
    FM_TX_Write_Data(15, 0x0003);
    FM_TX_Write_Data(16, 0x0000);
    FM_TX_Write_Data(17, 0x0000);
    FM_TX_Write_Data(18, 0x0000);
    FM_TX_Write_Data(19, 0x0000);

    SFR_BANK = sfr_bak;
    //enable_interrupt(); //开中断

}

void FM_TX_Write_Data(uint8 Reg, uint16 Reg_val) /*/AS6300 send data according to Reg*/
{
    uint8 ack, addr;
    //disable_interrupt();
    //output8(0xee, input8(0xee)|0x10);

    addr = ((Reg) << 1) | 0x80;
    IIC_Start();
    ack = IIC_WriteByte(addr);
    /*如果ack=0，添加显示“硬件错误”*/

    ack |= IIC_WriteByte(Reg_val >> 8);
    /*如果ack=0，添加显示“硬件错误”*/

    ack |= IIC_WriteByte(Reg_val & 0xff);
    /*如果ack=0，添加显示“硬件错误”*/

    IIC_Stop();
    //IIC_SCL_Low;
    //output8(0xee, input8(0xee)&0xef);
    //enable_interrupt();
}

uint16 FM_TX_Read_Data(uint8 Reg) /*AS6300 read data according to Reg*/
{
    uint8 addr, value_l, value_h;
    uint16 temp;

    addr = ((Reg) << 1) | 0x81;
    IIC_Start();

    IIC_WriteByte(addr);
    value_h = IIC_ReadByte(1);
    value_l = IIC_ReadByte(1);
    IIC_Stop();
    temp = (uint16) (value_h << 8) + value_l;
    return temp;
}

void FM_TX_Set_Powc(uint8 Powc) /*set as6300 output power,从0～15级可选*/
{
    uint16 temp;

    temp = ((FM_TX_Write_BUFF[2] & TX_MSK_POWC) | Powc);
    FM_TX_Write_BUFF[2] = temp;
    FM_TX_Write_Data(10, FM_TX_Write_BUFF[2]);
}

void FM_TX_Set_Freq(uint16 frq)
{
    uint8 rfpll_pb;
    uint8 rfpll_sb;
    uint16 temp;
    uint8 sfr_bak;

    //disable_interrupt();

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    temp = frq + 2;
    rfpll_sb = temp % 15;
    rfpll_pb = (temp - rfpll_sb) / 15 - 1;
    FM_TX_Write_BUFF[0] = (rfpll_pb << REGPOS_RFPLL_PB) | (FM_TX_Write_BUFF[0] & TX_MSK_RFPLL_PB);

    FM_TX_Write_BUFF[0] = ((rfpll_sb) << REGPOS_RFPLL_SB_L) | (FM_TX_Write_BUFF[0] & TX_MSK_RFPLL_SB_L);
    FM_TX_Write_BUFF[1] = ((rfpll_sb) >> REGPOS_RFPLL_SB_H) | (FM_TX_Write_BUFF[1] & TX_MSK_RFPLL_SB_H);

    FM_TX_Write_Data(5, FM_TX_Write_BUFF[0]);
    FM_TX_Write_Data(6, FM_TX_Write_BUFF[1]);

    SFR_BANK = sfr_bak;

    //enable_interrupt(); //开中断
}

//void FM_TX_Read_Freq(void)
//{
//
//        uint16 temp1,temp2;
//        uint8 rfpll_p,rfpll_s;
//
//        temp1 = FM_TX_Read_Data(5);
//        temp2 = FM_TX_Read_Data(6);
//        rfpll_p = (temp1 >> REGPOS_RFPLL_PB);
//        rfpll_s = (FM_TX_Read_Data(5) >> REGPOS_RFPLL_SB_L) | ((FM_TX_Read_Data(6)&0x0007)<<REGPOS_RFPLL_SB_H);
//        FM_TX_Freq = 15*(rfpll_p+1)+rfpll_s+4;
//
//}


void IIC_Start(void) /*IIC start command*/
{
    //	      IIC_SCL_High;
    //	      IIC_Delay(IIC_Delay_Time);
    IIC_SDA_High; /*发送起始条件的数据信号*/
    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_High; /*起始条件建立时间大于4.7us,延时*/

    IIC_Delay(IIC_Delay_Time);
    IIC_SDA_Low; /*发送起始信号*//* 起始条件锁定时间大于4μs*/

    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_Low; /*钳住IIC总线，SCL为低电平，准备发送或接收数据 */
    IIC_Delay(IIC_Delay_Time);
}

void IIC_Stop(void) /*IIC stop command*/
{
    IIC_SDA_Low; /*发送结束条件的数据信号*/
    /*发送结束条件的时钟信号*/
    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_High; /*结束条件建立时间大于4μs*/
    IIC_Delay(IIC_Delay_Time);
    IIC_SDA_High; /*发送IIC总线结束信号*/
    IIC_Delay(IIC_Delay_Time);
}

void IIC_Delay(uint16 t)
{
    while (!t)
        t--;
}

uint8 IIC_WriteByte(uint8 c) /*IIC send a byte data, return ack signal(0:无应答，1:有应答)*/
{
    uint8 ack, i;

    for (i = 0; i < 8; i++)
    {
        if ((c << i) & 0x80)
        {
            IIC_SDA_High;
        }
        else
        {
            IIC_SDA_Low;
        }
        IIC_Delay(IIC_Delay_Time);
        IIC_SCL_High;
        IIC_Delay(IIC_Delay_Time);
        IIC_SCL_Low;
    }
    //	IIC_Delay(IIC_Delay_Time);
    //	IIC_SDA_High;

    IIC_SDA_INPUT_EN;
    IIC_SDA_OUTPUT_DIS;
    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_High; /*制造第9个i2c时钟*/
    IIC_Delay(IIC_Delay_Time);
    if (IIC_SDA_REG & IIC_SDA_BIT) /*判断是否接收到应答信号*/
    {
        ack = 0;
    }
    else
    {
        ack = 1;
    }
    IIC_SCL_Low;
    IIC_Delay(IIC_Delay_Time);

    //IIC_SDA_INPUT_DIS;
    IIC_SDA_OUTPUT_EN;

    return ack;

}

uint8 IIC_ReadByte(uint8 ack) /*IIC read a byte data, if ack = 1 ,send ack signal, if ack = 0, no send ack signal*/
{
    uint8 i, bits;

    bits = 0;

    IIC_SDA_High;
    IIC_SDA_INPUT_EN;
    IIC_SDA_OUTPUT_DIS;
    //IIC_SDA_Set_Input;
    for (i = 0; i < 8; i++)
    {
        IIC_SCL_Low;
        IIC_Delay(IIC_Delay_Time);
        IIC_SCL_High;
        IIC_Delay(IIC_Delay_Time);
        bits <<= 1;

        if (IIC_SDA_REG & IIC_SDA_BIT)
        {
            bits += 1;
        }
        IIC_Delay(IIC_Delay_Time);
    }
    IIC_SCL_Low;
    IIC_Delay(IIC_Delay_Time);
    if (ack == 1)
    {
        IIC_SendAck(0);
    }
    else
    {
        IIC_SendAck(1);
    }
    //IIC_SDA_INPUT_DIS;
    IIC_SDA_OUTPUT_EN;

    return bits;

}

void IIC_SendAck(bool k)
{
    if (k == 0)
    {
        IIC_SDA_Low;
    }
    else
    {
        IIC_SDA_High;
    }
    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_High;
    IIC_Delay(IIC_Delay_Time);
    IIC_SCL_Low;
    IIC_Delay(IIC_Delay_Time);
}
