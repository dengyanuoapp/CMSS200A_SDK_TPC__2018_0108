/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#include "ap_linein.h"
#include "actos.h"
#include "alarmdef.h"

#pragma name(AP_MAIN)
uint8 g_PAOpenFlag = 0;
int8 AuCodecRun = 0;
int16 SentenceDectTime;
uint8 SentenceDectEnergy;
int16 SentenceDectTime_bak;
int8 c_decEffectFlag;
int8 SetenceFlag = 0;
uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
uint32 m_decSpdifddrSav;
bool g_PAOpenFlag;
int8 last_SetenceFlag;
#ifdef LINEIN_AP
char report_buf[16]; //存诸播报数据的buffer
info_report_t info =
{   report_buf, 0, TRUE};
#endif

int16 main(int param)
{
    int16 result;
    result = VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
#ifdef LINEIN_AP
    result = report_cur_ap(EXTEND_LINEIN);
    if(result != NULL)
    {
        return result;
    }
#endif
    init_linein_channel();

    result = show_ui_func(param);

    exit_linein_func();
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    return result;
}

void init_linein_channel(void)
{
    //    SetPAVolume_LR(g_comval.volume);
    SentenceDectTime = 10000;
    SentenceDectEnergy = 0x0;
    AuCodecRun = 0;
    SetenceFlag = 0; // SetenceFlag =1 close PA
    last_SetenceFlag = SetenceFlag;
    callback_eq(g_comval.eq_type, NULL);
    decinitAnalog(AD_Aux);
    decstartAnalog(AD_Aux);
    volume_callback(g_comval.volume);
}

void exit_linein_func(void)
{
    decAnalogexit();
}

void linein_open_dac(void)
{
    dac_t dac_attr;
    dac_attr.dacselect = 0;
    dac_attr.rate = FS_44K1;
    EnableDAC(&dac_attr);
}

void linein_open_pa(void)
{
    pa_t pa_attr;

    if (g_PAOpenFlag == 0x00)
    {
        pa_attr.pa_in.dacin = 1;
        pa_attr.pa_in.fmin = 0;
        pa_attr.pa_in.linein = 0;
        pa_attr.pa_in.micin = 0;
        pa_attr.volume = 0;
        EnablePA(&pa_attr);
        g_PAOpenFlag = 0x01;
    }
}
#ifdef LINEIN_AP
uint8 report_cur_ap(uint8 report_msg)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS != 0)
    {
        report_buf[0] = ' ';
        report_buf[1] = report_msg;
        report_buf[2] = ' ';
        report_buf[3] = 0;
        check_report_state(&info);
        retval = wait_report_end(NULL);
    }
    return retval;
}
#endif

