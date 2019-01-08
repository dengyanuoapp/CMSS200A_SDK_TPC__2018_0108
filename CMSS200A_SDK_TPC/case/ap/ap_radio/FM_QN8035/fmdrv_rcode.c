/*******************************************************************************
 *                              AS260A
 *                            Module: fm drvier for qn8035
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     mikeyang     2011-11-03 15:00     1.0             build this file
 *******************************************************************************/

#include  "fm_drv.h"

#pragma name(FM_DRV_RCODE)

//uint16 DelayTime;

uint8 qnd_R16;
uint8 qnd_R17;
uint8 qnd_R46;

uint8 FM_Slevel; //搜台门限

//uint16 Freq_RF;
/* 硬件seek 状态标识*/
uint8 hardseek_flag;

/*
 ********************************************************************************
 *             uint8 QNF_SetCh(uint16 freq)
 *
 * Description : 设置频点,进行tune操作
 *
 * Arguments   : 需设置的频点值, 以10khz 为单位

 * Returns     : 设置频点是否成功,若成功,返回1
 *					  否则,返回0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 QNF_SetCh(uint16 freq)
{
    // calculate ch parameter used for register setting
    uint8 tStep;
    uint8 tCh;
    uint16 f;
#if 0
    uint16 pll_dlt;
    if ((freq == 8540) || (freq == 8550) || (freq == 8560))
    {
        pll_dlt = (uint16) qnd_R16 >> 3;
        pll_dlt |= (uint16) qnd_R17 << 5;
        pll_dlt -= 1039;
        QND_WriteReg(0x16, (uint8) pll_dlt << 3);
        QND_WriteReg(0x17, (uint8) (pll_dlt >> 5));
        if (freq == 8540)
        {
            QND_WriteReg(0x46, 0x9D);
        }
        else if (freq == 8550)
        {
            QND_WriteReg(0x46, 0x69);
        }
        else
        {
            QND_WriteReg(0x46, 0x34);
        }
        freq = 8570;
    }
    else
#endif
    {
        QND_WriteReg(0x16, qnd_R16);
        QND_WriteReg(0x17, qnd_R17);
        QND_WriteReg(0x46, qnd_R46);
    }

    f = FreqToChan(freq);
    // set to reg: CH
    tCh = (uint8) f;
    QND_WriteReg(CH, tCh);
    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= ~CH_CH;
    tStep |= ((uint8) (f >> 8) & CH_CH);
    QND_WriteReg(CH_STEP, tStep);
    return 1;
}

/*
 ********************************************************************************
 *        uint8 QNF_SetRegBit(uint8 RegAddr, uint8 bitMask, uint8 data_val)
 *
 * Description : 将指定寄存器的指定bit设置为指定的值
 *
 * Arguments   : RegAddr -- 寄存器地址
 *               bitMask -- 需设置的bit位
 *               data_val -- 设置值
 *
 * Returns     : 成功,则返回值为1
 *			    否则,返回0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 QNF_SetRegBit(uint8 RegAddr, uint8 bitMask, uint8 data_val)
{
    uint8 result;
    result = QND_ReadReg(RegAddr);
    result &= (uint8) (~bitMask);
    result |= (data_val & bitMask);
    return QND_WriteReg(RegAddr, result);
}
/*
 ********************************************************************************
 *          void QNF_ConfigScan(uint16 start, uint16 stop, uint8 step)
 *
 * Description : 扫描前配置起始频率, 结束频率及步长
 *
 * Arguments   : start --- 起始频率, 10khz为单位
 *               stop --- 结束频率, 10khz为单位
 *               step --- 步长    1: 100khz    2: 200khz   0: 50khz
 * Returns     : none
 *
 * Notes       :
 *
 ********************************************************************************
 */
void QNF_ConfigScan(uint16 start, uint16 stop, uint8 step)
{
    uint8 tStep = 0;
    uint8 tS;
    uint16 fStart;
    uint16 fStop;

    //最低2bit保持不变,ch[9],ch[8]
    //tStep = QND_ReadReg(CH_STEP);
    //tStep &= CH_CH;

    fStart = FreqToChan(start);
    fStop = FreqToChan(stop);
    // set to reg: CH_START
    tS = (uint8) (fStart & 0xff);
    QND_WriteReg(CH_START, tS);
    tStep |= ((uint8) (fStart >> 6) & CH_CH_START);
    // set to reg: CH_STOP
    tS = (uint8) (fStop & 0xff);
    QND_WriteReg(CH_STOP, tS);
    tStep |= ((uint8) (fStop >> 4) & CH_CH_STOP);
    // set to reg: CH_STEP
    tStep |= step << 6;
    QND_WriteReg(CH_STEP, tStep);
    return;
}
/*
 ********************************************************************************
 *         uint8 QND_RXValidCH(uint16 freq, uint8 step)
 *
 * Description : 判断设置的频率是否是有效电台
 *
 * Arguments :  freq:要写的频率, 10khz 为单位
 step:步进100KHZ
 * Returns :	是否有效电台的标志
 *
 * Notes :
 *
 ********************************************************************************
 */
uint8 QND_RXValidCH(uint16 freq, uint8 step)
{
    uint8 regValue;
    uint8 timeOut = 200;
    uint8 isValidChannelFlag = 0;
#ifdef PILOT_CCA
    uint8 snr;
    uint8 readCnt = 0, stereoCount = 0;
#endif
    QNF_ConfigScan(freq, freq, step);

    //QNF_SetRegBit(CCA, 0x3F, 0);  //setting RSSI threshold for CCA

    //enter CCA mode,channel index is decided by internal CCA
    QNF_SetRegBit(SYSTEM1, 0x03, 0x02);
    do
    {
        regValue = QND_ReadReg(SYSTEM1);
        TM_DelayMS(5); //delay 5ms
        timeOut--;
    }
    while ((regValue & CHSC) && timeOut);  //when seeking a channel or time out,be quited the loop
    //read out the rxcca_fail flag of RXCCA status
    regValue = QND_ReadReg(STATUS1) & 0x08;
    if (regValue != 0)
    {
        isValidChannelFlag = 0;
    }
    else
    {
        isValidChannelFlag = 1;
    }
#ifdef PILOT_CCA       //special search  handle ways
    if(isValidChannelFlag != 0)
    {
        snr = QND_ReadReg(SNR);
        TM_DelayMS(60);
        if(snr <= 25)
        {
            isValidChannelFlag = 0;
            for(readCnt = 0; readCnt < 10; readCnt++)
            {
                TM_DelayMS(2);
                stereoCount += ((QND_ReadReg(STATUS1) & 0x01) ? 0 : 1);
                if(stereoCount >= 3)
                {
                    isValidChannelFlag = 1;
                    break;
                }
            }
        }
    }
#endif
    return isValidChannelFlag;
}

/*
 ********************************************************************************
 *          uint8 QND_WriteReg(uint8 RegAddr, uint8 Data)
 *
 * Description : 写寄存器
 *
 * Arguments :  RegAddr:要写的寄存器
 *              Data: 要写入的寄存器值
 * Returns :	成功/失败
 *
 * Notes :   设置模组某个指定寄存器的值
 *
 ********************************************************************************
 */
uint8 QND_WriteReg(uint8 RegAddr, uint8 Data)
{
    uint8 i, result;
    uint8 WriteBuffer[3];
    //写操作设备地址
    WriteBuffer[0] = 0x20;
    WriteBuffer[1] = RegAddr;
    WriteBuffer[2] = Data;

    //disable_interrupt(); //禁止中断
    romDI();
    for (i = 0; i < 5; i++)
    {
        ClearWatchDog();
        result = I2C_Trans_Bytes(WriteBuffer, 3);
        if (result != 0)
        {
            //enable_interrupt(); //开中断
            romEI();
            return 1;
        }
    }
    //enable_interrupt(); //开中断
    romEI();
    return 0;
}

/*
 ********************************************************************************
 *           uint8 QND_ReadReg(uint8 RegAddr)
 *
 * Description : 读寄存器的值
 *
 * Arguments :  RegAddr:要读的寄存器
 *
 * Returns :	读取到的寄存器的值
 *
 * Notes :
 *
 ********************************************************************************
 */
uint8 QND_ReadReg(uint8 RegAddr)
{
    uint8 Data, ret;
    uint8 buf[4];
    uint8 i;

    //disable_interrupt(); //禁止中断
    romDI();
    ClearWatchDog();

    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            ret = I2C_Recev_Bytes(&buf[i], RegAddr, 1);
            if (ret == 0)
            {
                //read fail
                buf[i] = 0;
            }
        }
        if ((buf[0] == buf[1]) && (buf[1] == buf[2]) && (buf[2] == buf[3]))
        {
            break;
        }
    }
    Data = buf[0];

    //enable_interrupt(); //开中断
    romEI();
    return Data;
}
/*
 ********************************************************************************
 *           uint16 ChanToFreq(uint16 chan)
 *
 * Description : 将读出的寄存器电台值转换成实际的频点
 *
 * Arguments :  chan:读出的电台值
 *
 * Returns :	   转换出来的实际频点值, 以10khz 为单位
 *
 * Notes :
 *
 ********************************************************************************
 */
uint16 ChanToFreq(uint16 chan)
{
    uint16 Freq;
    Freq = chan * 5;
    Freq += 6000;
    return Freq;

}

/*
 ********************************************************************************
 *           uint16 FreqToChan(uint16 freq)
 *
 * Description : 将频点转换成写寄存器的值
 *
 * Arguments :  freq:要写入的频点, 以10khz 为单位
 *
 * Returns :	   转换出来的要写入寄存器的电台值Channel Index
 *
 * Notes :
 *
 ********************************************************************************
 */
uint16 FreqToChan(uint16 freq)
{
    uint16 Chan;
    Chan = freq - 6000;
    Chan /= 5;
    return Chan;
}
