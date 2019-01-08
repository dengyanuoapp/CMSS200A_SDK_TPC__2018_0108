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
 * Description : 设置频点,进行tune操作
 *
 * Arguments   : 需设置的频点值, 以100khz 为单位

 * Returns     : 设置频点是否成功,若成功,返回1
 *					  否则,返回0
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
 * Description : 获取当前播放电台的频率值
 *
 * Arguments   : pfreq, 保存返回频率值的指针

 * Returns     : 成功获取, 返回1
 *					  否则,返回0
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

    //转成以100khz为单位
    *pfreq = freq10k / 10;
    return 1;
}

/*
 ********************************************************************************
 *       uint8 FMdrv_search(uint16 Freq, uint8 SeekDirect)
 *
 * Description : FM搜台
 *
 * Arguments   :
 param1:  软件搜台方式,需设置的频率值,以100kHz 为单位
 param2:  搜索方向
 * Returns     :   如果搜到的电台为真台,则返回1;否则,返回0
 *
 * Notes       :   用软件搜台做半自动搜台
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
 * Description : FM波段设置
 *
 * Arguments   : 普通频段/日本频段
 *
 * Returns     : 若设置成功,则返回1,否则,返回0
 *
 * Notes       : QN8035 没有波段设置寄存器, 直接设置频率
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
 * Description : 获取当前电台立体声信息
 *
 * Arguments   : none
 *
 * Returns     : 0 --- 立体声,  1--- 单声道
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
 * Description : 启动FM 硬件搜台过程
 *
 * Arguments :
 param1: 硬件搜台方式，搜台起始频点, 以100kHz 为单位
 param2: 设置结束频点,以100khz为单位
 param3: step=0  搜台步长50khz
 step=1  搜台步长100khz
 step=2  搜台步长200khz
 * Returns :  成功启动硬件搜台，返回1；否则返回0
 *
 * 用硬件搜台做全频段的seek(全自动搜台), 固定从低到高
 ********************************************************************************
 */
uint8 FMdrv_seek(uint16 startfreq, uint16 endfreq, uint8 step)
{
    uint8 result;
    //uint8 temp;
    uint16 start, end;

    //默认硬件seek  尚未启动
    hardseek_flag = 1;

    start = startfreq * 10;
    end = endfreq * 10;
    //设置起始频点,结束频点和步长:100khz
    QNF_ConfigScan(start, end, step);

    //设置搜台门限
    //QND_RXSetTH();

    //CCA_CH_DIS=0, select CCA result
    result = QNF_SetRegBit(SYSTEM1, 0x01, 0x00);
    //set CHSC to enable CCA
    result = QNF_SetRegBit(SYSTEM1, 0x02, 0x02);
    //enter receive mode
    result = QNF_SetRegBit(SYSTEM1, 0x30, 0x10);

    if (result != 0)
    {
        //硬件seek 已启动，未完成
        hardseek_flag = 0;
    }

    return result;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_breakseek(void)
 *
 * Description : 硬件seek 过程中，手动停止seek 过程
 *
 * Arguments :  NULL
 *
 * Returns : 成功停止，返回1;否则,返回0
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
        //仍处于硬件seek 过程中, CHSC=0 to disable CCA
        result = QNF_SetRegBit(SYSTEM1, 0x02, 0x00);
        if (result != 0)
        {
            //成功退出seek
            hardseek_flag = 1;
            result = 1;
        }
    }
    //已经不处于硬件seek 过程，直接返回停止成功
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
 * Description : 获取当前电台的信息
 *
 * Arguments   :	  保存电台信息的结构体指针
 *
 * Returns     :		//是否读取状态成功,如果读取成功,则返回值为1
 *					       否则,返回0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FMdrv_getstatus(fm_status_t *pBuf)
{
    //获取频率信息
    FMdrv_getfreq((uint16 *) (&(pBuf->freq)));

    //获取立体声状态
    pBuf->stereo_status = (FM_Audio_e) QNF_GetSM();
    return 1;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_getseekflag(uint8* flag)
 *
 * Description : 获取硬件seek 过程是否结束标志
 *
 * Arguments : 保存结束标志的指针
 *
 * Returns : 若获取成功，返回1;否则,返回0
 *
 * Notes :   flag :  bit0  此轮硬件seek 是否结束    1:  已结束   0:  未结束
 *                       bit1  此轮硬件seek 找到的是否有效台   1:  有效台
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
        //本轮seek过程结束
        hardseek_flag = 0x01;
        *stc_flag = 0x01;

        rdata = QND_ReadReg(STATUS1);
        if ((rdata & 0x08) == 0)
        {
            //找到有效台
            *stc_flag |= (0x01 << 1);
        }
    }
    //seek 过程未结束
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
 * Description : 获取当前电台信号强度
 *
 * Arguments   : pvalue
 *
 * Returns     : 若获取成功,则返回1,否则,返回0
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
 * Description : FM静音设置
 *
 * Arguments   : 是否静音,0为取消静音,1为静音
 *
 * Returns     : 若设置静音控制成功,则返回1,否则,返回0
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

