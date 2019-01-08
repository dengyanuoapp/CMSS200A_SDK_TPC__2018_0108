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

#include "fm_drv.h"

#pragma name(FM_DRV_BANK)

/**********************************************************************
 Description: set to SNR based MPX control. Call this function before
 tune to one specific channel

 Parameters:
 None
 Return Value:
 None
 **********************************************************************/
void QNF_RXInit(void)
{
    QNF_SetRegBit(0x1B, 0x08, 0x00); //Let NFILT adjust freely
    QNF_SetRegBit(0x2C, 0x3F, 0x12); //When SNR<ccth31, ccfilt3 will work
    QNF_SetRegBit(0x1D, 0x40, 0x00);//Let ccfilter3 adjust freely
    QNF_SetRegBit(0x41, 0x0F, 0x0A);//Set a hcc index to trig ccfilter3's adjust
    QND_WriteReg(0x45, 0x50);//Set aud_thrd will affect ccfilter3's tap number
    QNF_SetRegBit(0x40, 0x70, 0x70); //snc/hcc/sm snr_rssi_sel; snc_start=0x40; hcc_start=0x30; sm_start=0x20
    QNF_SetRegBit(0x19, 0x80, 0x80); //Use SNR for ccfilter selection criterion
    QNF_SetRegBit(0x3E, 0x80, 0x80); //it is decided by programming this register
    QNF_SetRegBit(0x41, 0xE0, 0xC0);//DC notching High pass filter bandwidth; remove low freqency dc signals
    QNF_SetRegBit(0x42, 0x10, 0x10);//disable the vtune monitor
    QNF_SetRegBit(0x34, 0x7F, SMSTART_VAL); //set SNCSTART
    QNF_SetRegBit(0x35, 0x7F, SNCSTART_VAL); //set SNCSTART
    QNF_SetRegBit(0x36, 0x7F, HCCSTART_VAL); //set HCCSTART
}

/**********************************************************************
 void QND_TuneToCH(uint16 ch)
 **********************************************************************
 Description: Tune to the specific channel. call QND_SetSysMode() before
 call this function
 Parameters:
 ch
 Set the frequency (10kHz) to be tuned,
 eg: 101.30MHz will be set to 10130.
 Return Value:
 None
 **********************************************************************/
void QND_TuneToCH(uint16 ch)
{

    uint8 reg;
    QNF_RXInit();
    if ((ch == 8430) || (ch == 7290) || (ch == 6910))
    {
        QNF_SetRegBit(CCA, IMR, IMR);
    }
    else
    {
        QNF_SetRegBit(CCA, IMR, 0x00);
    }
    QNF_ConfigScan(ch, ch, 1);
    QNF_SetCh(ch);
    QNF_SetRegBit(0x00, 0x03, 0x03); //Enter CCA mode. This speed up the channel locking.
    //Auto tuning
    QND_WriteReg(0x4F, 0x80);
    reg = QND_ReadReg(0x4F);
    reg >>= 1;
    QND_WriteReg(0x4F, reg);
    TM_DelayMS(200);//avoid the noise which are "POP" and "sha sha" noise.
}

/*
 ********************************************************************************
 *             uint8 FMdrv_setfreq(uint16 Freq)
 *
 * Description : ����Ƶ��,����tune����
 *
 * Arguments   : �����õ�Ƶ��ֵ, ��100khz Ϊ��λ

 * Returns     : ����Ƶ���Ƿ�ɹ�,���ɹ�,����1
 *					  ����,����0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_setfreq(uint16 Freq)
{
    //QND_TuneToCH(Freq * 10);
    //FMdrv_mute(SetMUTE);
    QND_TuneToCH(Freq * 10);
    //FMdrv_mute(releaseMUTE);
    return 1;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_getfreq(uint16 *pfreq)
 *
 * Description : ��ȡ��ǰ���ŵ�̨��Ƶ��ֵ
 *
 * Arguments   : pfreq, ���淵��Ƶ��ֵ��ָ��

 * Returns     : �ɹ���ȡ, ����1
 *					  ����,����0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_getfreq(uint16 *pfreq)
{
    uint8 tCh;
    uint8 tStep;
    uint16 ch = 0;
    uint16 freq10k;

    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= CH_CH;
    ch = tStep;
    tCh = QND_ReadReg(CH);
    ch = (ch << 8) + tCh;
    freq10k = ChanToFreq(ch);

    //ת����100khzΪ��λ
    *pfreq = freq10k / 10;
    return 1;
}

/*
 ********************************************************************************
 *       uint8 FMdrv_search(uint16 Freq, uint8 SeekDirect)
 *
 * Description : FM��̨
 *
 * Arguments   :
 param1:  �����̨��ʽ,�����õ�Ƶ��ֵ,��100kHz Ϊ��λ
 param2:  ��������
 * Returns     :   ����ѵ��ĵ�̨Ϊ��̨,�򷵻�1;����,����0
 *
 * Notes       :   �������̨�����Զ���̨
 *
 ********************************************************************************
 */
uint8 FMdrv_search(uint16 Freq, uint8 SeekDirect)
{
    uint8 result;

    SeekDirect = SeekDirect;
    result = QND_RXValidCH(Freq * 10, QND_FSTEP_100KHZ);

    return result;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_setband(radio_band_e band)
 *
 * Description : FM��������
 *
 * Arguments   : ��ͨƵ��/�ձ�Ƶ��
 *
 * Returns     : �����óɹ�,�򷵻�1,����,����0
 *
 * Notes       : QN8035 û�в������üĴ���, ֱ������Ƶ��
 *
 ********************************************************************************
 */
uint8 FMdrv_setband(radio_band_e band)
{
    band = band;

    return 1;
}

/*
 ********************************************************************************
 *             uint8 QNF_GetSM(void)
 *
 * Description : ��ȡ��ǰ��̨��������Ϣ
 *
 * Arguments   : none
 *
 * Returns     : 0 --- ������,  1--- ������
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 QNF_GetSM(void)
{
    uint8 tdata;

    tdata = QND_ReadReg(STATUS1);
    tdata &= 0x01;

    return tdata;
}

/*
 ********************************************************************************
 *            uint8 FMdrv_seek(uint16 startfreq, uint16 endfreq, uint8 step)
 *
 * Description : ����FM Ӳ����̨����
 *
 * Arguments :
 param1: Ӳ����̨��ʽ����̨��ʼƵ��, ��100kHz Ϊ��λ
 param2: ���ý���Ƶ��,��100khzΪ��λ
 param3: step=0  ��̨����50khz
 step=1  ��̨����100khz
 step=2  ��̨����200khz
 * Returns :  �ɹ�����Ӳ����̨������1�����򷵻�0
 *
 * ��Ӳ����̨��ȫƵ�ε�seek(ȫ�Զ���̨), �̶��ӵ͵���
 ********************************************************************************
 */
uint8 FMdrv_seek(uint16 startfreq, uint16 endfreq, uint8 step)
{
    uint8 result;
    //uint8 temp;
    uint16 start, end;

    //Ĭ��Ӳ��seek  ��δ����
    hardseek_flag = 1;

    start = startfreq * 10;
    end = endfreq * 10;
    //������ʼƵ��,����Ƶ��Ͳ���:100khz
    QNF_ConfigScan(start, end, step);

    //������̨����
    //QND_RXSetTH();

    //CCA_CH_DIS=0, select CCA result
    result = QNF_SetRegBit(SYSTEM1, 0x01, 0x00);
    //set CHSC to enable CCA
    result = QNF_SetRegBit(SYSTEM1, 0x02, 0x02);
    //enter receive mode
    result = QNF_SetRegBit(SYSTEM1, 0x30, 0x10);

    if (result != 0)
    {
        //Ӳ��seek ��������δ���
        hardseek_flag = 0;
    }

    return result;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_breakseek(void)
 *
 * Description : Ӳ��seek �����У��ֶ�ֹͣseek ����
 *
 * Arguments :  NULL
 *
 * Returns : �ɹ�ֹͣ������1;����,����0
 *
 * Notes :
 *
 ********************************************************************************
 */
uint8 FMdrv_breakseek(void)
{
    uint8 result = 0;
    uint8 rdata;

    rdata = QND_ReadReg(SYSTEM1);

    if ((rdata & CHSC) != 0)
    {
        //�Դ���Ӳ��seek ������, CHSC=0 to disable CCA
        result = QNF_SetRegBit(SYSTEM1, 0x02, 0x00);
        if (result != 0)
        {
            //�ɹ��˳�seek
            hardseek_flag = 1;
            result = 1;
        }
    }
    //�Ѿ�������Ӳ��seek ���̣�ֱ�ӷ���ֹͣ�ɹ�
    else
    {
        result = 1;
    }
    return result;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_getstatus(fm_status_t *pBuf)
 *
 * Description : ��ȡ��ǰ��̨����Ϣ
 *
 * Arguments   :	  �����̨��Ϣ�Ľṹ��ָ��
 *
 * Returns     :		//�Ƿ��ȡ״̬�ɹ�,�����ȡ�ɹ�,�򷵻�ֵΪ1
 *					       ����,����0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_getstatus(fm_status_t *pBuf)
{
    //��ȡƵ����Ϣ
    FMdrv_getfreq((uint16 *) (&(pBuf->freq)));

    //��ȡ������״̬
    pBuf->stereo_status = (FM_Audio_e) QNF_GetSM();
    return 1;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_getseekflag(uint8* flag)
 *
 * Description : ��ȡӲ��seek �����Ƿ������־
 *
 * Arguments : ���������־��ָ��
 *
 * Returns : ����ȡ�ɹ�������1;����,����0
 *
 * Notes :   flag :  bit0  ����Ӳ��seek �Ƿ����    1:  �ѽ���   0:  δ����
 *                       bit1  ����Ӳ��seek �ҵ����Ƿ���Ч̨   1:  ��Ч̨
 ********************************************************************************
 */
uint8 FMdrv_getseekflag(uint8 *flag)
{
    uint8 result;
    uint8 rdata;
    uint8 *stc_flag;

    stc_flag = flag;
    rdata = QND_ReadReg(SYSTEM1);

    if ((rdata & CHSC) == 0)
    {
        //����seek���̽���
        hardseek_flag = 0x01;
        *stc_flag = 0x01;

        rdata = QND_ReadReg(STATUS1);
        if ((rdata & 0x08) == 0)
        {
            //�ҵ���Ч̨
            *stc_flag |= (0x01 << 1);
        }
    }
    //seek ����δ����
    else
    {
        *stc_flag = 0;
    }
    result = 1;

    return result;

}

/*
 ********************************************************************************
 *             uint8 FMdrv_getintensity(uint8 *pvalue)
 *
 * Description : ��ȡ��ǰ��̨�ź�ǿ��
 *
 * Arguments   : pvalue
 *
 * Returns     : ����ȡ�ɹ�,�򷵻�1,����,����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_getintensity(uint8 *pvalue)
{
    uint8 *ptr = pvalue;

    *ptr = QND_ReadReg(RSSISIG);
    return 1;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_mute(FM_MUTE_e mode)
 *
 * Description : FM��������
 *
 * Arguments   : �Ƿ���,0Ϊȡ������,1Ϊ����
 *
 * Returns     : �����þ������Ƴɹ�,�򷵻�1,����,����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_mute(FM_MUTE_e mode)
{
    uint8 result;

    if (mode != releaseMUTE)
    {
        result = QNF_SetRegBit(REG_DAC, 0x0B, 0x0B);

    }
    else
    {
        result = QNF_SetRegBit(REG_DAC, 0x0B, 0x00);
    }
    return result;
}

