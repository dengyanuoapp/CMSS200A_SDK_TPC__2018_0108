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

uint8 FM_Slevel; //��̨����

//uint16 Freq_RF;
/* Ӳ��seek ״̬��ʶ*/
uint8 hardseek_flag;

/*
 ********************************************************************************
 *             uint8 QNF_SetCh(uint16 freq)
 *
 * Description : ����Ƶ��,����tune����
 *
 * Arguments   : �����õ�Ƶ��ֵ, ��10khz Ϊ��λ

 * Returns     : ����Ƶ���Ƿ�ɹ�,���ɹ�,����1
 *					  ����,����0
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
 * Description : ��ָ���Ĵ�����ָ��bit����Ϊָ����ֵ
 *
 * Arguments   : RegAddr -- �Ĵ�����ַ
 *               bitMask -- �����õ�bitλ
 *               data_val -- ����ֵ
 *
 * Returns     : �ɹ�,�򷵻�ֵΪ1
 *			    ����,����0
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
 * Description : ɨ��ǰ������ʼƵ��, ����Ƶ�ʼ�����
 *
 * Arguments   : start --- ��ʼƵ��, 10khzΪ��λ
 *               stop --- ����Ƶ��, 10khzΪ��λ
 *               step --- ����    1: 100khz    2: 200khz   0: 50khz
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

    //���2bit���ֲ���,ch[9],ch[8]
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
 * Description : �ж����õ�Ƶ���Ƿ�����Ч��̨
 *
 * Arguments :  freq:Ҫд��Ƶ��, 10khz Ϊ��λ
 step:����100KHZ
 * Returns :	�Ƿ���Ч��̨�ı�־
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
 * Description : д�Ĵ���
 *
 * Arguments :  RegAddr:Ҫд�ļĴ���
 *              Data: Ҫд��ļĴ���ֵ
 * Returns :	�ɹ�/ʧ��
 *
 * Notes :   ����ģ��ĳ��ָ���Ĵ�����ֵ
 *
 ********************************************************************************
 */
uint8 QND_WriteReg(uint8 RegAddr, uint8 Data)
{
    uint8 i, result;
    uint8 WriteBuffer[3];
    //д�����豸��ַ
    WriteBuffer[0] = 0x20;
    WriteBuffer[1] = RegAddr;
    WriteBuffer[2] = Data;

    //disable_interrupt(); //��ֹ�ж�
    romDI();
    for (i = 0; i < 5; i++)
    {
        ClearWatchDog();
        result = I2C_Trans_Bytes(WriteBuffer, 3);
        if (result != 0)
        {
            //enable_interrupt(); //���ж�
            romEI();
            return 1;
        }
    }
    //enable_interrupt(); //���ж�
    romEI();
    return 0;
}

/*
 ********************************************************************************
 *           uint8 QND_ReadReg(uint8 RegAddr)
 *
 * Description : ���Ĵ�����ֵ
 *
 * Arguments :  RegAddr:Ҫ���ļĴ���
 *
 * Returns :	��ȡ���ļĴ�����ֵ
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

    //disable_interrupt(); //��ֹ�ж�
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

    //enable_interrupt(); //���ж�
    romEI();
    return Data;
}
/*
 ********************************************************************************
 *           uint16 ChanToFreq(uint16 chan)
 *
 * Description : �������ļĴ�����ֵ̨ת����ʵ�ʵ�Ƶ��
 *
 * Arguments :  chan:�����ĵ�ֵ̨
 *
 * Returns :	   ת��������ʵ��Ƶ��ֵ, ��10khz Ϊ��λ
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
 * Description : ��Ƶ��ת����д�Ĵ�����ֵ
 *
 * Arguments :  freq:Ҫд���Ƶ��, ��10khz Ϊ��λ
 *
 * Returns :	   ת��������Ҫд��Ĵ����ĵ�ֵ̨Channel Index
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
