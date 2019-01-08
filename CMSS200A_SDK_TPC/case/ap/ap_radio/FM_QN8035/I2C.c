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
 * Description : I2C��ʼ��
 *
 * Arguments   :

 * Returns     :

 * Notes       :
 *
 *******************************************************************************
 */
void near IIC_Init(void)
{
    //ʹ�����,����ʱ���ߺ�����������
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
    //Disable ʱ���ߺ������ߵ�����, ���
    //    Reset_GPIO(I2CDAT, SDA_OUTPORT, SDA_OUTPORT_NUM);
    //    Reset_GPIO(I2CDAT, SDA_INPORT, SDA_INPORT_NUM);
    //    Reset_GPIO(I2CSCL, SCL_OUTPORT, SCL_OUTPORT_NUM);
    //    Reset_GPIO(I2CSCL, SCL_INPORT, SCL_INPORT_NUM);
}

/************************************************************************
 * Description:  ģ��IIC  ����IIC ������ʱ
 * Input:   ��ʱֵ
 * Output: none
 * Note:
 *************************************************************************
 **/
void near IIC_Delay(uint16 delay)
{
    uint16 j;

    for (j = 0; j < delay; j++)
    {
        //��ʱ����
        //;
    }
}

/*
 *******************************************************************************
 *             IIC_Start
 *
 * Description :  ģ��IIC   Start  ����
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
 * Description :  ģ��IIC  Stop  ����
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
 * Description : ����ŵ�*buf��ַ�ﳤ��Ϊlength�����ݷ��ͳ�ȥ
 *
 * Arguments   : unsigned char *buf: ���ݴ�ŵ�ַ
 unsigned char length: ���ݳ���, ��byteΪ��λ
 *
 * Returns     : FALSE: �������ݽ��յ�nack
 TRUE: �������ݽ��յ�ack
 *
 * Notes       : ��������I2C �豸�������ݣ����������豸����Ӧ
 *
 ********************************************************************************
 */
uint8 I2C_Trans_Bytes(uint8 *buf, uint8 length)
{
    uint8 ret;

    IIC_Init();

    IIC_Start();
    //��������
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
        //ȫ�����ݷ������, �ɹ�
        ret = 1;
    }
    else
    {
        //δ�������, ʧ��
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
 * Description : ��ȡ����length
 *
 * Arguments   : unsigned char *buf: ���ݴ�ŵ�ַ
 unsigned char address: slave��ַ
 *               unsigned char length: ���ݳ���,��byteΪ��λ
 *
 * Returns     : FALSE: slave����Ӧ
 TRUE: �յ�����
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

    //д�豸��ַ
    IIC_WriteByte(0x20);

    if (!IIC_GetAck())
    {
        ret = 0;
        goto recev_ret;
    }

    IIC_WriteByte(Regaddress); //���Ĵ�����ַ
    if (!IIC_GetAck())
    {
        ret = 0;
        goto recev_ret;
    }

    IIC_Start(); //restart
    //���豸��ַ
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
 * Description : д1���ֽڵ����ݵ�slave
 *
 * Arguments   : 1���ֽ�����

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
        if (((dat << i) & 0x80) != 0) //����һ���ֽڣ��Ӹ�bit ����bit ���η���

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
 * Description : ��ȡ1���ֽڵ�����,����ack����nack��slave
 *
 * Arguments   : unsigned char ack: 1 ack; 0 nack

 *
 * Returns     : ����������
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
 * Description : ����ack����nack
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
 * Notes       :   ��ȡ���豸�˷��͹�������Ӧ�ź�Ack  or  NAck
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
