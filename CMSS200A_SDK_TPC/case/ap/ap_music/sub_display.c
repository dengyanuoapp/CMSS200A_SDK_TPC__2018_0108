/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: display module of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"
#pragma name(AP_SUB_DIS)
const uint8 eq_str[eq_max][3] =
{ "E1", "E2", "E3", "E4", "E5", "E6", "E7" , "E8"};
const uint8 repeat_str[4][4] =
{ "ALL", "ONE", "RAN", "FOL" };
const uint8 repeat_val1[MAX_REPEAT_MODE] =
{ FSEL_ALL_REPEATALL, FSEL_ALL_REPEATONE, FSEL_ALL_RANDOM, FSEL_DIR_REPEATALL };

void show_disk_icon(uint8 on_off)
{
    if (CurUselocation.disk == disk_char[1])
    {
        LEDDisplay(LED_USB, 0xff, 0);
        LEDDisplay(LED_SD, 0xff, on_off);
    }
    else
    {
        LEDDisplay(LED_SD, 0xff, 0);
        LEDDisplay(LED_USB, 0xff, on_off);
    }
}
void show_status_icon(uint8 on_off)
{
    if (PlayStatus >= PlaySta)
    {
        LEDDisplay(LED_PAUSE, 0xff, 0);
        LEDDisplay(LED_PLAY, 0xff, on_off);
    }
    else
    {
        LEDDisplay(LED_PLAY, 0xff, 0);
        LEDDisplay(LED_PAUSE, 0xff, on_off);
    }
}
void show_col_icon(uint8 on_off)
{
    LEDDisplay(LED_COL, 0xff, on_off);
}
void show_play_ui(void)
{
    show_disk_icon(1);
    show_status_icon(1);
    show_col_icon(1);
    ShowNowTime();
}
void show_fileno_ui(void)
{
    LEDClearScreen();
    show_disk_icon(1);
    itoa4(g_musicsequence, temp_buffer);
    LEDPuts(NUMBER1, temp_buffer, 1);
    show_count = 0;
}
void show_dirno_ui(void)
{
    show_disk_icon(1);
    itoa4(dir_no, temp_buffer);
    LEDPuts(NUMBER1, temp_buffer, 1);
    show_count = 0;
}
#if 0
void show_volume_ui(void)
{
    memset(temp_buffer, 0xff, 0x04);
    itoa2(g_comval.volume, temp_buffer);
    LEDPuts(NUMBER2, temp_buffer, 1);
}
#endif
void show_eq_ui(void)
{
    memset(temp_buffer, 0xff, 0x04);
    memcpy(temp_buffer, &eq_str[g_comval.eq_type], 0x02);
    LEDPuts(NUMBER2, temp_buffer, 1);
}
void show_repeat_ui(void)
{
    uint8 repeat_mode;
    uint8 i;
    memset(temp_buffer, 0xff, 0x04);
    for (i = 0; i < 4; i++)
    {
        if (g_music_vars.repeat_mode == repeat_val1[i])
        {
            repeat_mode = i;
            break;
        }
    }
    if (i >= MAX_REPEAT_MODE)
    {
        repeat_mode = 0;
    }
    memcpy(temp_buffer, &repeat_str[repeat_mode], 0x03);
    LEDPuts(NUMBER2, temp_buffer, 1);
}
void show_digital_ui(void)
{
    uint8 tmp_buf[4];
    memset(tmp_buf, 0x0, 0x04);
    memcpy(&tmp_buf[4 - DigitalCount], DigitalBuf, DigitalCount);
    LEDPuts(NUMBER1, tmp_buf, 1);
}
/*
 ********************************************************************************
 *           void RefreshMainScreen( void )
 *
 * Description : 主显示的刷新函数,只有有需要时,才调用此函数
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */
void RefreshMainScreen(void)
{
    LEDClearScreen();
    switch (CurUI_Type)
    {
    case Music_UI:
        show_play_ui();
        break;
    case Volume_UI:
        show_volume_ui();
        break;
    case FileNo_UI:
        show_fileno_ui();
        break;
    case Digital_UI:
        show_digital_ui();
        break;
    case EQ_UI:
        show_eq_ui();
        break;
    case Repeat_UI:
        show_repeat_ui();
        break;
    case DIRNAME_UI:
        show_dirno_ui();
        break;
    case STANDBY_UI:
        //        ShowTimerSetting_UI();
        break;
    default:
        break;
    }
}

