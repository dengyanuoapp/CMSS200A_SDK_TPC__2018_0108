/*******************************************************************************
 *                              AS260A
 *                            Module: IIC driver
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     mikeyang     2011-11-03 15:00     1.0             build this file
 *******************************************************************************/
#include "I2C.h"

#pragma name(FM_DRV_IIC)

void near Set_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num);
void near Reset_GPIO(uint8 port, uint8 GPIO_port, uint8 GPIO_port_num);
void near IIC_WriteByte(uint8 dat);
uint8 near IIC_ReadByte(void);
uint8 near IIC_GetAck(void);
void near IIC_SendAck(uint8 ack);
void near IIC_Delay(uint16 delay);

void near SetSDA_1_hold(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SDA_DATA |= (0x01 << SDA_PORT_NUM);

    SFR_BANK = sfr_bak;

    IIC_Delay(5);

}

void near SetSDA_0_hold(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SDA_DATA &= ~(0x01 << SDA_PORT_NUM);

    SFR_BANK = sfr_bak;
    IIC_Delay(5);
}

void near SetCLK_1_hold(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SCL_DATA |= (0x01 << SCL_PORT_NUM);

    SFR_BANK = sfr_bak;
    IIC_Delay(5);
}

void near SetCLK_0_hold(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SCL_DATA &= ~(0x01 << SCL_PORT_NUM);

    SFR_BANK = sfr_bak;
    IIC_Delay(5);
}

void near SetSDAOut(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SDA_OUT_EN |= (0x01 << SDA_PORT_NUM);
    SFR_SDA_IN_EN &= ~(0x01 << SDA_PORT_NUM);

    SFR_BANK = sfr_bak;
}

void near SetSDAIn(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SDA_IN_EN |= (0x01 << SDA_PORT_NUM);
    SFR_SDA_OUT_EN &= ~(0x01 << SDA_PORT_NUM);

    SFR_BANK = sfr_bak;
}

void near SetCLKOut(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SCL_OUT_EN |= (0x01 << SCL_PORT_NUM);
    SFR_SCL_IN_EN &= ~(0x01 << SCL_PORT_NUM);

    SFR_BANK = sfr_bak;
}

BOOL near GetSDAIn(void)
{
    uint8 ret;
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    ret = (SFR_SDA_DATA & (0x01 << SDA_PORT_NUM)) ? 1 : 0;
    //ret = (SFR_SDA_DATA & (0x01 << SDA_PORT_NUM))

    SFR_BANK = sfr_bak;
    return ret;
}

/*
 *******************************************************************************
 *             void IIC_Init(void)
 *
 * Description : I2C初始化
 *
 * Arguments   :

 * Returns     :

 * Notes       :
 *
 *******************************************************************************
 */
void near IIC_Init(void)
{
    //使能输出,并将时钟线和数据线拉高
    SetSDAOut();
    SetCLKOut();
    SetSDA_1_hold();
    SetCLK_1_hold();
}

void near IIC_Exit(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;

    SFR_SCL_IN_EN &= ~(0x01 << SCL_PORT_NUM);
    SFR_SCL_OUT_EN &= ~(0x01 << SCL_PORT_NUM);

    SFR_SDA_IN_EN &= ~(0x01 << SDA_PORT_NUM);
    SFR_SDA_OUT_EN &= ~(0x01 << SDA_PORT_NUM);

    SFR_BANK = sfr_bak;
    //Disable 时钟线和数据线的输入, 输出
    //    Reset_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
    //    Reset_GPIO(I2CDAT, SDA_INPORT, SDA_INPORT_NUM);
    //    Reset_GPIO(I2CSCL, SCL_OUTPORT, SCL_OUTPORT_NUM);
    //    Reset_GPIO(I2CSCL, SCL_INPORT, SCL_INPORT_NUM);
}

/************************************************************************
 * Description:  模拟IIC  总线IIC 操作延时
 * Input:   延时值
 * Output: none
 * Note:
 *************************************************************************
 **/
void near IIC_Delay(uint16 delay)
{
    uint16 j;

    for (j = 0; j < delay; j++)
    {
        //延时函数
        //;
    }
}

/*
 *******************************************************************************
 *             IIC_Start
 *
 * Description :  模拟IIC   Start  流程
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 *******************************************************************************
 */
/*
 ____
 SDA:    |_______

 ______
 SCL:      |_____
 */

void near IIC_Start(void) /*IIC start command*/
{
    //data pull up
    //Set_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM);
    SetSDA_1_hold();
    //clk pull up
    //Set_GPIO(I2CSCL,SCL_DATAPORT, SCL_DATAPORT_NUM);
    SetCLK_1_hold();
    //data pull down
    //Reset_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM);
    SetSDA_0_hold();
    //clk pull down
    //Reset_GPIO(I2CSCL,SCL_DATAPORT, SCL_DATAPORT_NUM);
    SetCLK_0_hold();
}

/*
 *******************************************************************************
 *             IIC_Stop
 *
 * Description :  模拟IIC  Stop  流程
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 *******************************************************************************
 */
/*
 _______
 SDA: _______|
 ________
 SCL: ______|
 */

void near IIC_Stop(void) /*IIC stop command*/
{
    //data pull down
    //Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);
    SetSDA_0_hold();
    //clk  pull up
    //Set_GPIO(I2CSCL,SCL_DATAPORT, SCL_DATAPORT_NUM);
    SetCLK_1_hold();
    //data pull up
    //Set_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM);
    SetSDA_1_hold();
}

//=============================================================================================
/*
 ******************************************************************************
 *             uint8 I2C_Trans_Bytes(uint8 *buf, uint8 length)
 *
 * Description : 将存放到*buf地址里长度为length的数据发送出去
 *
 * Arguments   : unsigned char *buf: 数据存放地址
 unsigned char length: 数据长度, 以byte为单位
 *
 * Returns     : FALSE: 发送数据接收到nack
 TRUE: 发送数据接收到ack
 *
 * Notes       : 由主控向I2C 设备发送数据，接收来自设备的响应
 *
 ********************************************************************************
 */
uint8 I2C_Trans_Bytes(uint8 *buf, uint8 length)
{
    uint8 ret;

    IIC_Init();

    IIC_Start();
    //发送数据
    do
    {
        IIC_WriteByte(*buf);
        buf++;
        if (!IIC_GetAck())
        {
            break;
        }
        --length;
    }
    while (length != 0);

    if (length == 0)
    {
        //全部数据发送完成, 成功
        ret = 1;
    }
    else
    {
        //未发送完成, 失败
        ret = 0;
    }
    IIC_Stop();

    IIC_Exit();
    return ret;
}

//=============================================================================================
/*
 ******************************************************************************
 *             uint8 I2C_Recev_Bytes(uint8 *buf, uint8 address, uint8 length)
 *
 * Description : 读取长度length
 *
 * Arguments   : unsigned char *buf: 数据存放地址
 unsigned char address: slave地址
 *               unsigned char length: 数据长度,以byte为单位
 *
 * Returns     : FALSE: slave不响应
 TRUE: 收到数据
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 I2C_Recev_Bytes(uint8 *buf, uint8 Regaddress, uint8 length)
{
    uint8 ret = 0;
    uint8 *p;

    p = buf;

    IIC_Init();
    IIC_Start();

    //写设备地址
    IIC_WriteByte(0x20);

    if (!IIC_GetAck())
    {
        ret = 0;
        goto recev_ret;
    }

    IIC_WriteByte(Regaddress); //读寄存器地址
    if (!IIC_GetAck())
    {
        ret = 0;
        goto recev_ret;
    }

    IIC_Start(); //restart
    //读设备地址
    IIC_WriteByte(0x21);

    if (!IIC_GetAck())
    {
        ret = 0;
        goto recev_ret;
    }

    while (length != 0)
    {
        if (length == 1) //the last byte, sent nack
        {
            *p = IIC_ReadByte();
            IIC_SendAck(1);
        }
        else
        {
            *p = IIC_ReadByte();
            IIC_SendAck(0);
        }
        p++;
        length--;
    }
    ret = 1;
recev_ret:
    IIC_Stop();
    IIC_Exit();
    return ret;
}

/*
 *******************************************************************************
 *             void IIC_WriteByte(uint8 dat)
 *
 * Description : 写1个字节的数据到slave
 *
 * Arguments   : 1个字节数据

 *
 * Returns     : 1 ack; 0 nack
 *
 * Notes       :
 *
 *******************************************************************************
 */
void near IIC_WriteByte(uint8 dat)
{
    uint8 i;
    for (i = 0; i < 8; i++)
    {
        if (((dat << i) & 0x80) != 0) //对于一个字节，从高bit 到低bit 依次发送

        {
            //Set_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM);		   //data  pull up
            SetSDA_1_hold();

        }
        else
        {
            //Reset_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM); 	     //data pull down
            SetSDA_0_hold();
        }
        //Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);	       //clk pull up
        //Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);	       //clk pull down
        SetCLK_1_hold();
        SetCLK_0_hold();
    }
}

/*
 *******************************************************************************
 *             uint8 IIC_ReadByte(void)
 *
 * Description : 读取1个字节的数据,并回ack或者nack给slave
 *
 * Arguments   : unsigned char ack: 1 ack; 0 nack

 *
 * Returns     : 读到的数据
 *
 * Notes       :
 *
 *******************************************************************************
 */
uint8 near IIC_ReadByte(void)
{
    uint8 t = 8, dat = 0;

    //Reset_GPIO(I2CDAT,SDA_OUTPORT, SDA_OUTPORT_NUM); 	     //disable data pin output
    SetSDAIn();

    IIC_Delay(IIC_DELAY_TIME);

    do
    {
        //Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);	       //clk pull up
        SetCLK_1_hold();

        dat <<= 1;

        dat |= GetSDAIn();

        //Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);	       //clk pull down
        SetCLK_0_hold();

        --t;
    }
    while (t != 0);

    //Set_GPIO(I2CDAT,SDA_OUTPORT, SDA_OUTPORT_NUM); 	     //enable data pin output
    SetSDAOut();
    //IIC_Delay(IIC_DELAY_TIME);
    return dat;
}

/*
 *******************************************************************************
 *             void IIC_SendAck(uint8 ack)
 *
 * Description : 发送ack还是nack
 *
 * Arguments   : unsigned char ack: 1 noack; 0 ack

 *
 * Returns     :
 *
 * Notes       :
 *
 *******************************************************************************
 */
void near IIC_SendAck(uint8 ack)
{
    if (ack != 0)
    {
        //Set_GPIO(I2CDAT,SDA_DATAPORT, SDA_DATAPORT_NUM);          //nack
        SetSDA_1_hold();
    }
    else
    {
        //Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);          //ack
        SetSDA_0_hold();
    }
    //Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);          //clk pull up
    //Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);          //clk pull down
    SetCLK_1_hold();
    SetCLK_0_hold();
}

/*
 *******************************************************************************
 *             uint8 IIC_GetAck(void)
 *
 * Description : Return Ack From the slave device, one is Ack, but zero NoAck.
 *
 * Arguments   : unsigned char ack: 1 noack; 0 ack

 *
 * Returns     :
 *
 * Notes       :   获取从设备端发送过来的响应信号Ack  or  NAck
 *
 *******************************************************************************
 */
uint8 near IIC_GetAck(void)
{
    uint8 ack, read_val;

    //Reset_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);        //disable data output,   input always enable
    SetSDA_1_hold();

    SetSDAIn();

    //IIC_Delay(IIC_DELAY_TIME);
    //Set_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);          //clk pull up
    SetCLK_1_hold();

    //ack = !GetSDAIn();
    read_val = GetSDAIn();
    if(read_val != 0)
    {
        ack = 0;
    }
    else
    {
        ack = 1;
    }

    //Set_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);

    //IIC_Delay(IIC_DELAY_TIME);
    //Reset_GPIO(I2CSCL, SCL_DATAPORT, SCL_DATAPORT_NUM);          //clk pull down
    //Reset_GPIO(I2CDAT, SDA_DATAPORT, SDA_DATAPORT_NUM);          //data pull down
    SetCLK_0_hold();
    //SetSDA_0_hold();
    SetSDAOut();
    return ack;
}
