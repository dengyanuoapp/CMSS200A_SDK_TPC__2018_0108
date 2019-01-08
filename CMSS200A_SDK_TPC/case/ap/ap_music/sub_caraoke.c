/*
 *******************************************************************************
 *                ACTOS AP
 *        voice player stop program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#include "ap_music.h"
//#include "AppDef.h"

#pragma name(AP_KALACBK)

int8 MicVolEar = 0x05; //输出到耳机的时候Mic的默认音量0~7
//int8 far MicVolSpe = 0x03;              //输出到喇叭的时候Mic的默认音量0~7
int8 DECGainEar = 0x40; //耳机状态下默认DEC增益Q2.6格式
//int8 far DECGainSpe = 0x80;              //喇叭状态下默认DEC增益Q2.6格式
const uint8 KALAEQtemp[4][10] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
    5, 3, 0, -1, 0, 4, 5, 6, 0, 0, // 2
    0, 0, 0, -1, -1, -2, -4, 6, 0, 0, // 3
    5, 2, 0, 0, 0, 0, 0, 6, 0, 0 // 4
};
const uint16 CH_strint[] =
{ LEARNMOD, SINGMOD };//切换卡拉OK模式
const uint16 karaoke_EQ[] =
{ SOUEFF1, SOUEFF2, SOUEFF3, SOUEFF4 };
const uint8 adc_param[10] =
{ 0, 0x0c, 0x30, 0, 0, 0, 0, 0, 0, 0x80 };

/*
 *****************************************************************************************************************
 *             void openMicRoute(char micgain)
 *
 * Description : 打开MIC通路，并根据micgain设定MIC增益
 *
 * Arguments   : micgain = 0~7（20db~40db）
 *
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void OpenMicRoute(int16 gain)
{
    uint8 cur_sample = 48;
    adc_t adcparam;
    if (PlayStatus != PlaySta)
    {
        return;
    }
    EnableAIN(AD_MIC);
    TM_DelayMS(20);
    memcpy(&adcparam, adc_param, sizeof(adc_t));
    musicpSendCommand(MC_GETSAPLE, (void *) &cur_sample);
    adcparam.rate = cur_sample;
    EnableADC(&adcparam);
    SetAINGain(0, gain);
}
/*
 *****************************************************************************************************************
 *             void CloseMicRoute(char micgain)
 *
 * Description : 关闭MIC通路
 *
 * Arguments   : NULL
 *
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void CloseMicRoute(void)
{
    DisableAIN();
    DisableADC();
}
/*
 ********************************************************************************
 *             void acc_vol_callback(int8 CNT)
 *
 * Description : 回调函数，设置卡拉OK伴音等级
 *
 * Arguments   :int8 Vol  伴音等级(-1 ~ 1)
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void acc_vol_callback(int16 vol)
{
    uint8 acc_vol;
    if (vol == 0)
    {
        acc_vol = 0x10;
    }
    else if (vol == 1)
    {
        acc_vol = 0x30;
    }
    else
    {
        acc_vol = 0x08;
    }
    setDECGain(acc_vol);
}
/*
 *****************************************************************************************************************
 *             void SpeechMute(bool flag)
 *
 * Description : 打开或关闭人声消除电路
 *
 * Arguments   : flag = 0 关闭人声消除电路
 *               flag = 1 打开人声消除电路
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void SpeechMute(uint16 flag)
{
    //uint8 sfr_bak;
    //sfr_bak = SFR_BANK;
    //SFR_BANK = BANK_AUIP;
    if (flag == 0) //学习模式, 关闭人声消除
    {
        //DAC_CTL3 &= 0xbf;
        sCaraokeDisable();
    }
    else //演唱模式, 打开人声消除
    {
        //DAC_CTL3 |= 0x40;
        /* 获取当前歌曲采样率 */
        musicpSendCommand(MC_GETSAPLE, (void *) (&g_Openparam.current_samplerate));
        sCaraokeEnable(g_Openparam.current_samplerate, 1);
    }
    //SFR_BANK = sfr_bak;
}
/*
 *****************************************************************************************************************
 *             void setDECGain(int8 DecGain)
 *
 * Description : 设置DEC的增益，在卡拉OK状态下调节伴奏音量
 *
 * Arguments   : DecGain  ,USE Q4.20格式
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void setDECGain(uint8 DecGain)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_AUIP;

    GlobalGainH = DecGain;
    GlobalGainM = 0x00;
    GlobalGainL = 0x00;

    SFR_BANK = sfr_bak;
}
/*
 *****************************************************************************************************************
 *             void SetCaraokeEq(int8 eqmode,int8 eqval)
 *
 * Description : 卡拉OK模式下的ＥＱ
 *
 * Arguments   :  int8 eqmode  :提供0~6共7个频段供EQ设置
 *                int8 eqval 当前频段的EQ数值
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
/*void SetCaraokeEq(int8 eqmode,int8 eqval)
 {
 BYTE eqvaltemp;
 if(eqval < 25)
 {
 eqvaltemp = 64 + eqval;
 }
 else
 {
 eqvaltemp = eqval;
 }
 if(eqmode!=0xff);
 {
 UserEQTable[eqmode] = eqvaltemp;
 }
 //    if(eqmode!=0x55)
 //    {
 pSendCommand(MC_SETUSEREQ, 0, (int8) &DISEQTable);
 //    }
 //    else
 //    {
 //        pSendCommand(MC_SETUSEREQ, 0, (int8) &UserEQTable);
 //    }
 acc_vol_callback(g_music_vars.acc_vol);
 } */
/*
 *****************************************************************************************************************
 *             void SetCaraokeEq(BYTE eqmode,BYTE eqval)
 *
 * Description : 卡拉OK模式下的ＥＱ
 *
 * Arguments   :  BYTE eqmode  :提供0~3共4个频段供EQ模式设置 设置固定的EQ值
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */

void SetCaraokeEqMode(uint16 eqmode)
{
    uint8 temp[12];
    memset(temp, 0, 12);
    if (eqmode == 0)
    {
        musicpSendCommand(MC_SETEQ, 0);
    }
    else
    {
        memcpy(temp, (void *) KALAEQtemp[eqmode], 10);
        musicpSendCommand(MC_SETUSEREQ, (void *) temp);
        musicpSendCommand(MC_SETEQ, 0x07);
    }
    //acc_vol_callback(g_music_vars.acc_vol);
}
/*
 *****************************************************************************************************************
 *             void SetVol(int8 volmode)
 *
 * Description : 打开MIC电路，并根据VOLMODE设定默认的MIC音量和DEC音量
 *
 * Arguments   : volmode，根据volmode判断当前是耳机模式还是喇叭模式，从而设置不同的mic，dec参数
 *               volmode = 0,或者大于2，关闭micin，将dec Vol设置成不进行放大或缩小
 *               volmode = 1，表示拉卡OK模式
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void SetVol(int8 volmode)
{
    if (volmode == 0) //非卡拉OK模式，正常播放歌曲
    {
        CloseMicRoute(); //关闭MIC通道
        acc_vol_callback(0);
    }
    else if (volmode == 1)
    {
        acc_vol_callback(g_music_vars.acc_vol);
        OpenMicRoute(0x02); //打开Mic通道并设定其增益为默认增益
    }
    else
    {
        CloseMicRoute();
        acc_vol_callback(0);
    }
}

/*
 *****************************************************************************************************************
 *             void CaraokeEnable(int8 volmode)
 *
 * Description : 打开人声消除模块，Micin模块，并将EQ设置成卡拉OK模式
 *
 * Arguments   : volmode，根据volmode判断当前是耳机模式还是喇叭模式，从而设置不同的mic，dec参数
 *               volmode = 0,关闭micin，将dec Vol设置成不进行放大或缩小
 *               volmode = 1，表示喇叭模式
 *               volmode = 2，表示耳机模式
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void CaraokeEnable(int8 volmode)
{
    SpeechMute(volmode);
    SetVol(1); //根据耳机或者喇叭选择默认mic音量，默认dec音量
    SetCaraokeEqMode(0); //设定卡拉OK EQ
}

/*
 *****************************************************************************************************************
 *             void CaraokeDisable()
 *
 * Description :退出卡拉OK模式
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void CaraokeDisable(void)
{
    SetVol(0);
    SpeechMute(0);
}
void deal_caraok_msg(void)
{
    if (g_music_vars.karaoke_flag == 1)
    {
        /* 获取当前歌曲采样率 */
        musicpSendCommand(MC_GETSAPLE, (void *) (&g_Openparam.current_samplerate));
        sCaraokeEnable(g_Openparam.current_samplerate, 1);
    }
    else
    {
        sCaraokeDisable();
    }
}
/********************************************************************************
 * Description :处理mic开关功能
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :

 *
 ********************************************************************************/
void deal_mic_msg(void)
{
    micin_status = 0;
    check_plug_device_msg(&micin_status, MIC_IN);
    if(micin_status == 0)
    {
        return;
    }
    if (mic_on_flag == 1)
    {
        OpenMicRoute(MIC_GAIN_VAL);
    }
    else
    {
        CloseMicRoute();
    }
}

