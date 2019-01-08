/********************************************************************************************
 Airoha Technology FM module driver(I2C DRIVER)  for ATJ2091
 last update 2007/09/19
 Version:V0.1
 ********************************************************************************************/

/***********************I2C CONTROL*********************/
//SDA output/input enable port-GPIOC1
#include "ap_radio.h"

#pragma name(FM_IIC_DRV)

//#define     SDA_OUTPORT         (uint8*)GPIOBOUTEN
//#define     SDA_OUTPORT_NUM      0x05          //B5 output enalbe
//#define     SDA_INPORT           (uint8*)GPIOBINEN
//#define     SDA_INPORT_NUM       0x05

//#define     SCL_OUTPORT          (uint8*)GPIOBOUTEN
//#define     SCL_OUTPORT_NUM      0x04           //B4 output enable
//#define     SCL_INPORT           (uint8*)GPIOBINEN
//#define     SCL_INPORT_NUM       0x04

//data port
//#define     SDA_DATAPORT            (uint8*)GPIOBDAT
//#define     SDA_DATAPORT_NUM        0x05          //B5 outpu data


//#define     SCL_DATAPORT            (uint8*)GPIOBDAT
//#define     SCL_DATAPORT_NUM        0x04         //B4 output data

#define     I2CDAT				0x01
#define     I2CSCL				0x02
#define     SDA_OUTPORT          0x00
#define     SDA_OUTPORT_NUM      0x01          //c1 output enalbe
#define     SDA_INPORT           0x01
#define     SDA_INPORT_NUM       0x01

#define     SCL_OUTPORT          0x00
#define     SCL_OUTPORT_NUM      0x07           //b7 output enable
#define     SCL_INPORT           0x01
#define     SCL_INPORT_NUM       0x07

//data port
#define     SDA_DATAPORT            0x02
#define     SDA_DATAPORT_NUM        0x01          //c1 output data
#define     SCL_DATAPORT            0x02
#define     SCL_DATAPORT_NUM        0x07         //b7 output data
extern uint8 ReadBuffer[4];
extern uint8 WriteBuffer[64];
extern uint8 Set_Mute;
void Set_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num);
void Reset_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num);

uint8 WriteOneFrame(uint8 count);
void FM_Wait50ms(void);
uint8 ReadOneFrame(void);
void I2C_Wait5us(void);
uint8 I2C_TransByte(uint8 param1, uint8 param2);
uint8 I2C_ReceByte(uint8 param);
void I2C_SendByte(uint8 param);
void SetMfpToNand(void);

void I2C_Init(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    LOSC2_CTL &= 0xf9;
    LOSC2_CTL |= 0x60;

    SFR_BANK = sfr_bak;
    Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    Set_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
    Set_GPIO(I2CSCL, SCL_OUTPORT, SCL_OUTPORT_NUM);
    Set_GPIO(I2CSCL, SCL_INPORT, SCL_INPORT_NUM);
    Set_GPIO(I2CDAT, SDA_INPORT, SDA_INPORT_NUM);
}

/*void I2C_Exit(void)
 {

 Reset_GPIO(SDA_OUTPORT, SDA_OUTPORT_NUM);
 // I2C_Wait5us();
 Reset_GPIO(SCL_OUTPORT, SCL_OUTPORT_NUM);
 }  */
void I2C_Start(void)
{
    Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    I2C_Wait5us();
    Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);

}
void I2C_Stop(void)
{
    Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    I2C_Wait5us();
    Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
}

uint8 WriteOneFrame(uint8 count)
{
    uint8 i, result;
    disable_interrupt(); //禁止中断
    for (i = 0; i < 2; i++)
    {
        I2C_Start();
        result = I2C_TransByte(0x20, 0x00);
        if (result == 0)
        {
            break;
        }
        else
        {
            I2C_Stop();
        }
    }
    if (result == 0)
    {

        for (i = 0; i < count; i++)
        {
            result = I2C_TransByte(WriteBuffer[i], 0x00);
            if (result != 0)
            {
                goto error;
            }
        }
        I2C_Stop();
        enable_interrupt(); //开中断
        return TRUE;
    }
error:
    I2C_Stop();
    enable_interrupt(); //开中断
    return FALSE;
}

uint8 ReadOneFrame(void)
{
    uint8 i, result;
    I2C_Start();
    result = I2C_TransByte(0x21, 0x01);
    if (result == 0)
    {
        for (i = 0; i < 4; i++)
        {
            ReadBuffer[i] = I2C_ReceByte(0x00);
        }
        //ReadBuffer[4]=I2C_ReceByte(0x01);
        Set_Mute = I2C_ReceByte(0x01);
        I2C_Stop();
        return TRUE;
    }
    I2C_Stop();
    return FALSE;
}

uint8 I2C_TransByte(uint8 param1, uint8 param2)
{
    uint8 IOStatus;
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    I2C_SendByte(param1);
    Reset_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
    Set_GPIO(I2CDAT, SDA_INPORT, SDA_INPORT_NUM);
    I2C_Wait5us();
    Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    I2C_Wait5us();

    IOStatus = GPIOCDAT & (0x01 << SDA_DATAPORT_NUM);

    if (IOStatus != 0)
    {
        Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        Set_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
        I2C_Wait5us();
        Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        SFR_BANK = sfr_bak;
        return TRUE;
    }
    else
    {
        if (param2 != 1)
        {

            //   Reset_GPIO(SDA_INPORT,SDA_INPORT_NUM);
            Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
            Set_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
            I2C_Wait5us();
            Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
            Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        }
        else
        {
            I2C_Wait5us();
            Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        }
        SFR_BANK = sfr_bak;
        return FALSE;
    }
}
uint8 I2C_ReceByte(uint8 param)
{
    uint8 IOStatus1, IOStatus2;
    uint8 sfr_bak;
    uint8 result = 0;
    uint8 temp = 0;
    uint8 i;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    Reset_GPIO(I2CSCL, SCL_OUTPORT, SCL_OUTPORT_NUM);
    Set_GPIO(I2CSCL, SCL_INPORT, SCL_INPORT_NUM);
    do
    {

        IOStatus1 = GPIOBDAT & (0x01 << SCL_DATAPORT_NUM);
    }
    while (IOStatus1 == 0);
    //	Reset_GPIO(SCL_INPORT,SCL_INPORT_NUM);			//保持GPIOB5 输入一直使能，防止出现中间状态，导致NAND会影响SCL的时序
    Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    Set_GPIO(I2CSCL, SCL_OUTPORT, SCL_OUTPORT_NUM);
    for (i = 0; i < 8; i++)
    {
        Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        I2C_Wait5us();
        result <<= 1;
        IOStatus2 = GPIOCDAT & (0x01 << SDA_DATAPORT_NUM);
        if (IOStatus2 != 0)
        {
            result += 0x01;
        }

        Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        I2C_Wait5us();
    }

    // Reset_GPIO(SDA_INPORT,SDA_INPORT_NUM);
    temp = (uint8) (result | 0xfe);
    if (temp == 0xff)
    {

        Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    }
    else
    {

        Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    }
    Set_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
    Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    if (param == 1)
    {
        Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        I2C_Wait5us();
        Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        I2C_Wait5us();
        Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    }
    else
    {

        Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        I2C_Wait5us();

        Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        I2C_Wait5us();

        Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        Reset_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);

        Set_GPIO(I2CDAT, SDA_INPORT, SDA_INPORT_NUM);
    }
    SFR_BANK = sfr_bak;
    return result;
}
void I2C_SendByte(uint8 param)
{
    uint8 i;
    uint8 SendData;
    SendData = param;
    for (i = 0; i < 8; i++)
    {

        if (((SendData << i) & 0x80) != 0)
        {
            Set_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        }
        else
        {
            Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
        }
        I2C_Wait5us();
        Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
        I2C_Wait5us();
        Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);
    }
}
uint8 ReadStatus(void)
{
    uint8 result;
    uint8 temp = 0;
    uint8 count = 0;
#ifndef PC
    FM_Wait50ms();
    disable_interrupt(); //禁止中断
    do
    {
        ClearWatchDog();
        result = ReadOneFrame();
        if (result != 0)
        {
#ifdef HARDWARE_SEEK
            temp = ReadBuffer[0] | 0xbf;
#else
            temp = ReadBuffer[3] | 0x7f;
#endif
        }
        count++;
        if (count > 20)
        {
            enable_interrupt(); //开中断
            return 0;
        }
    }
    while (temp != 0xff);
    enable_interrupt(); //开中断
    return 1;
#else
    return 1;
#endif
}
void I2C_Wait5us(void)
{
    uint8 i;
    for (i = 30; i > 0; i--)
    {
        ; //FM短暂延时
    }

}

void FM_Wait50ms(void)
{
    uint16 j, k;
    for (j = 0; j < 50; j++)
    {
        ClearWatchDog();
        for (k = 0; k < 2000; k++)
        {
            ; //FM延时
        }
    }
}

void Set_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num)
{
    uint8 sfr_bak;

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    MFPSEL6 &= ~(1 << 7);
    if (port == I2CDAT)
    {
        switch (GPIO_port)
        {
        case 0:
            GPIOCOUTEN |= (0x01 << GPIO_port_num);
            break;
        case 1:
            GPIOCINEN |= (0x01 << GPIO_port_num);
            break;
        case 2:
            GPIOCDAT |= (0x01 << GPIO_port_num);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (GPIO_port)
        {
        case 0:
            GPIOBOUTEN |= (0x01 << GPIO_port_num);
            break;
        case 1:
            GPIOBINEN |= (0x01 << GPIO_port_num);
            break;
        case 2:
            GPIOBDAT |= (0x01 << GPIO_port_num);
            break;
        default:
            break;
        }
    }
    SFR_BANK = sfr_bak;
}

void Reset_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    MFPSEL6 &= ~(1 << 7);
    if (port == I2CDAT)
    {
        switch (GPIO_port)
        {

        case 0:
            GPIOCOUTEN &= ~(0x01 << GPIO_port_num);
            break;
        case 1:
            GPIOCINEN &= ~(0x01 << GPIO_port_num);
            break;
        case 2:
            GPIOCDAT &= ~(0x01 << GPIO_port_num);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (GPIO_port)
        {
        case 0:
            GPIOBOUTEN &= ~(0x01 << GPIO_port_num);
            break;
        case 1:
            GPIOBINEN &= ~(0x01 << GPIO_port_num);
            break;
        case 2:
            GPIOBDAT &= ~(0x01 << GPIO_port_num);
            break;
        default:
            break;
        }

    }
    SFR_BANK = sfr_bak;
}

void SetMfpToNand(void)
{

    /*uint8 sfr_bak;
     sfr_bak = SFR_BANK;
     SFR_BANK = BANK_GPIO;
     GPIOBOUTEN &= ~(0x01 << SDA_OUTPORT_NUM);
     GPIOBOUTEN &= ~(0x01 << SCL_OUTPORT_NUM);
     GPIOBINEN &= ~(0x01 << SDA_INPORT_NUM);
     GPIOBINEN &= ~(0x01 << SCL_INPORT_NUM);
     SFR_BANK = sfr_bak;*/
}
