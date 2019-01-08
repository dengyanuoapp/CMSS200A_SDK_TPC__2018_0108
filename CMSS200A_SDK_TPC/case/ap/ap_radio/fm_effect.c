#include "ap_radio.h"
#include "actos.h"
#include "ap_common.h"
#include "decC.h"       //设置EQ时使用
#pragma name(FM_EFFECT)

const uint8 EQtemp[8][10] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //none
    5, 3, -1, -2, 0, 1, 3, 6, 0, 0, //rock
    0, 3, 4, 3, 0, -1, -2, 6, 0, 0, //Pop
    6, 4, 2, 0, 1, 3, 5, 6, 0, 0, //Classic
    0, 0, 1, 2, 0, 2, 4, 6, 0, 0, //Soft
    2, 1, 0, -1, -2, 0, 3, 6, 0, 0, //Jazz
    6, 5, 3, 2, 1, 0, 0, 6, 0, 0,//Dbb
    6, 6, 5, 4, 6, 1, 0, 6, 1, 0 //user
};


void set_eq_data(eq_t eq_type, void *dat)
{
    //memcpy(g_decControlInfor.EQVal, dat, sizeof(g_decControlInfor.EQVal));
    if ((eq_type == 0) && (dat == NULL))
    {
        g_decControlInfor.EQFlag = 0x00;
        if (AuCodecRun)
        {
            deal_Effectcoff();
        }
    }
    else
    {
        g_decControlInfor.EQFlag = 0x67;
        if (dat == NULL)
        {
            //从VRAM读，保存的参数及时生效
            VMRead(&g_eq_para, VM_PCSET_EQ, sizeof(EQ_VM_t));
            if (g_eq_para.magic != EQ_VRAM_MAGIC)
            {
                //使用代码默认EQ表
                memcpy(g_decControlInfor.EQVal, (void *) EQtemp[(uint8) g_comval.eq_type],
                       sizeof(g_decControlInfor.EQVal));
            }
            else
            {
                //使用工具设置EQ表
                memcpy(g_decControlInfor.EQVal, (void *) &g_eq_para.left_eq_para[(uint8) ONE_EQPARA_BYTE
                        * g_comval.eq_type], ONE_EQPARA_BYTE);
            }
        }
        else
        {
            memcpy(g_decControlInfor.EQVal, (void *) dat, sizeof(g_decControlInfor.EQVal));
        }

        if (AuCodecRun)
        {
            decEffectDeal();
        }

        g_decControlInfor.EQFlag = 0x87;

        if (dat == NULL)
        {
            if (g_eq_para.magic != EQ_VRAM_MAGIC)
            {
                //使用代码默认EQ表
                memcpy(g_decControlInfor.EQVal, (void *) EQtemp[(uint8) g_comval.eq_type],
                       sizeof(g_decControlInfor.EQVal));
            }
            else
            {
                //使用工具设置EQ表
                memcpy(g_decControlInfor.EQVal, (void *) &g_eq_para.right_eq_para[(uint8) ONE_EQPARA_BYTE
                        * g_comval.eq_type], ONE_EQPARA_BYTE);
            }
        }
        else
        {
            memcpy(g_decControlInfor.EQVal, (void *) dat, sizeof(g_decControlInfor.EQVal));
        }

        if (AuCodecRun)
        {
            decEffectDeal();
        }
    }
}
/********************************************************************************
 * Description : Callback eq function
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 *
 ********************************************************************************/
uint8 EQMode_callback(eq_t eq_type)
{
    uint8 tmp_eqdata[12];
    memcpy(tmp_eqdata, (void *) EQtemp[(uint8) g_comval.eq_type], 10);
    set_eq_data(eq_type, NULL);
    return 0;
}

/*
 ********************************************************************************
 *             void deal_eq_msg(void)
 *
 * Description : 处理EQ模式调节消息
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       :
 *
 *
 *
 ********************************************************************************
 */

uint8 deal_eq_msg(uint8 key)
{
    if (key == Key_None)
    {
        if (CurUI_Type != EQ_UI)
        {
            SetCurUI_Type(EQ_UI);
            return NULL;
        }
        key = Key_Next;
    }

    if (key == Key_Next)
    {
        if (g_comval.eq_type < (eq_max - 1))
        {
            g_comval.eq_type++;
        }
        else
        {
            g_comval.eq_type = 0;
        }
    }
    else
    {
        if (g_comval.eq_type > 0)
        {
            g_comval.eq_type--;
        }
        else
        {
            g_comval.eq_type = (eq_max - 1);
        }

    }
    EQMode_callback(g_comval.eq_type);
    need_draw = TRUE;
    return NULL;
}

void exit_fmin_channel(void)
{
    decAnalogexit();
}

void init_fmin_channel(void)
{
    SentenceDectTime = 10000;
    SentenceDectEnergy = 0;
    AuCodecRun = 0;
    SetenceFlag = 0; // SetenceFlag =1 close PA
    last_SetenceFlag = SetenceFlag;
    //EQMode_callback(g_comval.eq_type);
    set_eq_data(g_comval.eq_type, NULL);

    decinitAnalog(AD_FM); //demo layout error
    //TM_DelayMS(50); //延时，防止PAPA 声
    decstartAnalog(AD_FM);
    SetAINGain(7, 0 );
    volume_callback(g_comval.volume);

    //SetPAVolume(g_comval.volume);
}

/*
 ********************************************************************************
 *             void    volume_callback(uint16 Vol)
 *
 * Description : 音量设置的CALLBACK函数
 *
 * Arguments   :    当前需设置的音量值
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void volume_callback(uint16 Vol)
{
    //实时改变音量的值
    g_comval.vol_display = (uint8) Vol;
    if (g_comval.VolumeMax != 0)
    {
        g_comval.volume = g_comval.vol_display; // g_comval.vol_display * SOFTVOL_MAX / g_comval.VolumeMax;
    }

#ifdef use_external_pa
    set_external_pavol(PA2_0_UI, g_comval.pavol2_0);
    set_external_pavol(PA0_1_UI, g_comval.pavol0_1);
    set_external_pavol(PABASS_UI, g_comval.pabass);
    set_external_pavol(PAFLT_UI, g_comval.paflt);
#else
    fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
    if (g_comval.volume == 0)
    {
        fmSendCommand(FM_MUTE, SetMUTE, NULL);
    }
    else
    {
        fmSendCommand(FM_MUTE, releaseMUTE, NULL);
    }
#endif
}
