#include "ap_linein.h"
#include "actos.h"
#include "ap_common.h"

#pragma name(FUNCTION)
//从VRAM中读出的EQ,SRS参数表，通过EQ工具设置
EQ_VM_t g_eq_para;
const uint8 EQtemp[eq_max][10] =
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

void volume_callback(uint16 Vol)
{
    //实时改变音量的值
    //   g_comval.volume = (uint8) Vol * SOFTVOL_MAX / g_comval.VolumeMax;
    OutPutVolume(g_comval.volume);
}

/*===========================================
 functions: OutPutVolume(int vol)
 input:    int vol: 要输出的音量
 output:  void
 功能： 把音量值送到IO口，
 ============================================*/
void OutPutVolume(uint16 vol)
{
    set_dac_volume(vol);
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
uint8 callback_eq(eq_t eq_type, uint8 *buf)
{
    if ((eq_type == 0) && (buf == NULL))
    {
        g_decControlInfor.EQFlag = 0x00;
        decEffectDeal();
    }
    else
    {
        g_decControlInfor.EQFlag = 0x67;

        if (buf == NULL)
        {
            //从VRAM读，保存的参数及时生效
            VMRead(&g_eq_para, VM_PCSET_EQ, sizeof(EQ_VM_t));

            if (g_eq_para.magic != EQ_VRAM_MAGIC)
            {
                //使用代码默认EQ表
                memcpy(g_decControlInfor.EQVal, (void *) EQtemp[(uint8) eq_type],
                       sizeof(g_decControlInfor.EQVal));
            }
            else
            {
                //使用工具设置EQ表
                memcpy(g_decControlInfor.EQVal, (void *) &g_eq_para.left_eq_para[(uint8) ONE_EQPARA_BYTE * eq_type],
                       ONE_EQPARA_BYTE);
            }
        }
        else
        {
            memcpy(g_decControlInfor.EQVal, (void *) buf, sizeof(g_decControlInfor.EQVal));
        }

        if (AuCodecRun)
        {
            decEffectDeal();
        }
        g_decControlInfor.EQFlag = 0x87;
        if (buf == NULL)
        {
            if (g_eq_para.magic != EQ_VRAM_MAGIC)
            {
                //使用代码默认EQ表
                memcpy(g_decControlInfor.EQVal, (void *) EQtemp[(uint8) eq_type],
                       sizeof(g_decControlInfor.EQVal));
            }
            else
            {
                //使用工具设置EQ表
                memcpy(g_decControlInfor.EQVal, (void *) &g_eq_para.right_eq_para[(uint8) ONE_EQPARA_BYTE * eq_type],
                       ONE_EQPARA_BYTE);
            }
        }
        else
        {
            memcpy(g_decControlInfor.EQVal, (void *) buf, sizeof(g_decControlInfor.EQVal));
        }
        if (AuCodecRun)
        {
            decEffectDeal();
        }
    }
    return 0;
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
uint8 EQMode_callback(void)
{
    uint8 tmp_eqdata[12];
    memcpy(tmp_eqdata, (void *) EQtemp[(uint8) g_comval.eq_type], 10);
    callback_eq(g_comval.eq_type, (void *) tmp_eqdata);
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
            LEDClearScreen();
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
            g_comval.eq_type = eq_max - 1;
        }

    }
    EQMode_callback();
    need_draw = TRUE;
    return NULL;
}
#if 0
/********************************************************************************
 * Description : 处理音量键
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 *
 ********************************************************************************/
uint8 deal_vol_msg(uint8 key)
{
    if (key == Key_None)
    {
        if (CurUI_Type != Volume_UI)
        {
            LEDClearScreen();
            SetCurUI_Type(Volume_UI);
        }
        else
        {
            SetCurUI_Type(Linein_UI);
        }
    }
    else
    {
        if (CurUI_Type != Volume_UI)
        {
            LEDClearScreen();
            SetCurUI_Type(Volume_UI);
        }
        if (key == Key_Next)
        {
            if (g_comval.volume < g_comval.VolumeMax)
            {
                g_comval.volume++;
            }
        }
        else
        {
            if (g_comval.volume > 0)
            {
                g_comval.volume--;
            }
        }
        volume_callback(g_comval.volume);
    }
    need_draw = TRUE;
    return NULL;
}
#endif
