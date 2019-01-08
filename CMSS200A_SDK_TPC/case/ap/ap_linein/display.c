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

#pragma name(DISPLAY)
const uint8 linein_str[5] =
{ "LINE" };
const uint8 eq_str[eq_max][3] =
{ "E1", "E2", "E3", "E4", "E5", "E6", "E7" , "E8"};

//void show_volume_ui(void);
void show_linein_ui(void);

void show_eq_ui(void)
{
    memset(temp_buffer, 0xff, 0x04);
    memcpy(temp_buffer, &eq_str[g_comval.eq_type], 0x02);
    LEDPuts(NUMBER2, temp_buffer, 1);
}
#if 0
/********************************************************************************
 * Description :显示当前的状态信息
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :
 *
 ********************************************************************************/
void show_volume_ui(void)
{
    memset(temp_buffer, 0xff, 0x04);
    itoa2(g_comval.volume, temp_buffer);
    LEDPuts(NUMBER2, temp_buffer, 1);
}
#endif
void show_linein_ui(void)
{
    LEDClearScreen();
    LEDPuts(NUMBER1, linein_str, 1);
    LEDDisplay(LED_AUX, 0xff, 1);
    /* stop or play icon */
    if (mute_flag)
    {
        LEDDisplay(LED_PLAY, 0xff, 0);
        LEDDisplay(LED_PAUSE, 0xff, 1);
    }
    else
    {
        LEDDisplay(LED_PLAY, 0xff, 1);
        LEDDisplay(LED_PAUSE, 0xff, 0);
    }
}

void ui_display(void)
{
    switch (CurUI_Type)
    {
    case Volume_UI:
        show_volume_ui();
        break;
    case EQ_UI:
        show_eq_ui();
        break;
    case Linein_UI:
        show_linein_ui();
        break;
    case STANDBY_UI:
        //        ShowTimerSetting_UI();
        break;
    default:
        break;
    }
}
