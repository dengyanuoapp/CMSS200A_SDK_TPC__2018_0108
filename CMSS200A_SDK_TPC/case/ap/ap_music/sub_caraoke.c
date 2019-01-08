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

int8 MicVolEar = 0x05; //�����������ʱ��Mic��Ĭ������0~7
//int8 far MicVolSpe = 0x03;              //��������ȵ�ʱ��Mic��Ĭ������0~7
int8 DECGainEar = 0x40; //����״̬��Ĭ��DEC����Q2.6��ʽ
//int8 far DECGainSpe = 0x80;              //����״̬��Ĭ��DEC����Q2.6��ʽ
const uint8 KALAEQtemp[4][10] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
    5, 3, 0, -1, 0, 4, 5, 6, 0, 0, // 2
    0, 0, 0, -1, -1, -2, -4, 6, 0, 0, // 3
    5, 2, 0, 0, 0, 0, 0, 6, 0, 0 // 4
};
const uint16 CH_strint[] =
{ LEARNMOD, SINGMOD };//�л�����OKģʽ
const uint16 karaoke_EQ[] =
{ SOUEFF1, SOUEFF2, SOUEFF3, SOUEFF4 };
const uint8 adc_param[10] =
{ 0, 0x0c, 0x30, 0, 0, 0, 0, 0, 0, 0x80 };

/*
 *****************************************************************************************************************
 *             void openMicRoute(char micgain)
 *
 * Description : ��MICͨ·��������micgain�趨MIC����
 *
 * Arguments   : micgain = 0~7��20db~40db��
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
 * Description : �ر�MICͨ·
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
 * Description : �ص����������ÿ���OK�����ȼ�
 *
 * Arguments   :int8 Vol  �����ȼ�(-1 ~ 1)
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
 * Description : �򿪻�ر�����������·
 *
 * Arguments   : flag = 0 �ر�����������·
 *               flag = 1 ������������·
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
    if (flag == 0) //ѧϰģʽ, �ر���������
    {
        //DAC_CTL3 &= 0xbf;
        sCaraokeDisable();
    }
    else //�ݳ�ģʽ, ����������
    {
        //DAC_CTL3 |= 0x40;
        /* ��ȡ��ǰ���������� */
        musicpSendCommand(MC_GETSAPLE, (void *) (&g_Openparam.current_samplerate));
        sCaraokeEnable(g_Openparam.current_samplerate, 1);
    }
    //SFR_BANK = sfr_bak;
}
/*
 *****************************************************************************************************************
 *             void setDECGain(int8 DecGain)
 *
 * Description : ����DEC�����棬�ڿ���OK״̬�µ��ڰ�������
 *
 * Arguments   : DecGain  ,USE Q4.20��ʽ
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
 * Description : ����OKģʽ�µģţ�
 *
 * Arguments   :  int8 eqmode  :�ṩ0~6��7��Ƶ�ι�EQ����
 *                int8 eqval ��ǰƵ�ε�EQ��ֵ
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
 * Description : ����OKģʽ�µģţ�
 *
 * Arguments   :  BYTE eqmode  :�ṩ0~3��4��Ƶ�ι�EQģʽ���� ���ù̶���EQֵ
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
 * Description : ��MIC��·��������VOLMODE�趨Ĭ�ϵ�MIC������DEC����
 *
 * Arguments   : volmode������volmode�жϵ�ǰ�Ƕ���ģʽ��������ģʽ���Ӷ����ò�ͬ��mic��dec����
 *               volmode = 0,���ߴ���2���ر�micin����dec Vol���óɲ����зŴ����С
 *               volmode = 1����ʾ����OKģʽ
 *
 * Returns     : void
 *
 * Notes       :
 *
 *****************************************************************************************************************
 */
void SetVol(int8 volmode)
{
    if (volmode == 0) //�ǿ���OKģʽ���������Ÿ���
    {
        CloseMicRoute(); //�ر�MICͨ��
        acc_vol_callback(0);
    }
    else if (volmode == 1)
    {
        acc_vol_callback(g_music_vars.acc_vol);
        OpenMicRoute(0x02); //��Micͨ�����趨������ΪĬ������
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
 * Description : ����������ģ�飬Micinģ�飬����EQ���óɿ���OKģʽ
 *
 * Arguments   : volmode������volmode�жϵ�ǰ�Ƕ���ģʽ��������ģʽ���Ӷ����ò�ͬ��mic��dec����
 *               volmode = 0,�ر�micin����dec Vol���óɲ����зŴ����С
 *               volmode = 1����ʾ����ģʽ
 *               volmode = 2����ʾ����ģʽ
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
    SetVol(1); //���ݶ�����������ѡ��Ĭ��mic������Ĭ��dec����
    SetCaraokeEqMode(0); //�趨����OK EQ
}

/*
 *****************************************************************************************************************
 *             void CaraokeDisable()
 *
 * Description :�˳�����OKģʽ
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
        /* ��ȡ��ǰ���������� */
        musicpSendCommand(MC_GETSAPLE, (void *) (&g_Openparam.current_samplerate));
        sCaraokeEnable(g_Openparam.current_samplerate, 1);
    }
    else
    {
        sCaraokeDisable();
    }
}
/********************************************************************************
 * Description :����mic���ع���
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
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

