/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *       History Record:
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(DISPLAY)

void show_eq_ui(void);
void RefreshMainScreen(void)
{
    char buf[5];
    char i;
    LEDClearScreen();
branch_reflash:
    switch (CurUI_Type)
    {
    case FMFreq_UI: //显示频点
        /* stop or play icon */
        LEDDisplay(LED_PLAY, 0xff, !mute_flag);
        LEDDisplay(LED_PAUSE, 0xff, mute_flag);

        LEDDisplay(LED_FM, 0xff, 1);
        itoa4(g_FMStatus.freq, buf);
        for (i = 0; i < 4; i++)
        {
            buf[i] -= '0';
        }

        if (g_FMStatus.freq >= 1000)
        {
            LEDDisplay(NUMBER1, buf[0], 1);
        }
        LEDDisplay(NUMBER2, buf[1], 1);
        LEDDisplay(NUMBER3, buf[2], 1);
        LEDDisplay(NUMBER4, buf[3], 1);
        break;

    case FMChannel_UI://显示电台号
        if (!g_FMStatus.station)
        {
            CurUI_Type = FMFreq_UI;
            goto branch_reflash;
            break;
        }
        LEDDisplay(NUMBER1, NUM_C, 1);
        LEDDisplay(NUMBER2, NUM_H, 1);
        itoa2(g_FMStatus.station, buf);
        for (i = 0; i < 2; i++)
        {
            buf[i] -= '0';
        }
        LEDDisplay(NUMBER3, buf[0], 1);
        LEDDisplay(NUMBER4, buf[1], 1);
        break;

    case Volume_UI:
        show_volume_ui();
        //        itoa2(g_comval.volume, buf);
        //        LEDDisplay(NUMBER2, buf[0] - '0', 1);
        //        LEDDisplay(NUMBER3, buf[1] - '0', 1);
        break;

    case FMInputFreq_UI:
        for (i = 0; i < 4; i++)
        {
            LEDDisplay(NUMBER1 + i, DigitalBuf[i], 1);
        }
        break;
    case EQ_UI:
        show_eq_ui();
        break;

    default:
        break;
    }
}
void show_eq_ui(void)
{
    //memset(temp_buffer, 0xff, 0x04);
    //memcpy(temp_buffer, eq_str[g_comval.eq_type], 0x02);
    //LEDPuts(NUMBER2, temp_buffer, 1);
    LEDDisplay(NUMBER2, NUM_E, 1);
    LEDDisplay(NUMBER3, (g_comval.eq_type + 1), 1);

}

