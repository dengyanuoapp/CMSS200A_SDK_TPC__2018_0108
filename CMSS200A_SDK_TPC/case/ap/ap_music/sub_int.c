/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: timer ISR module of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"

#pragma name(AP_SUB_INT)
/*
 ********************************************************************************
 *             void stop_music_play(void)
 *
 * Description : 将播放状态切换到暂停状态
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.如果是停止状态下，则直接返回
 2.停止解码，关闭文件句柄
 ********************************************************************************
 */

void stop_music_play(uint8 bakup_flag)
{
    if (PlayStatus < PlaySta)
    {
        return;
    }
    FadeOut(TRUE);
    musicpSendCommand_bank(MC_STOP, NULL);
    if (bakup_flag == TRUE)
    {
        BackupCurrentTime();
        musicpSendCommand_bank(MC_BACKUP, NULL);
    }
    musicpSendCommand_bank(MC_CLOSE, NULL);
    if (bakup_flag == TRUE)
    {
        if (g_music_vars.cur_disk == disk_char[0])
        {
            memcpy(&g_music_vars.BreakPoit[0], &g_music_vars.BreakPTSave, sizeof(breakPT_t));
        }
        else
        {
            memcpy(&g_music_vars.BreakPoit[1], &g_music_vars.BreakPTSave, sizeof(breakPT_t));
        }
        VMWrite(&g_music_vars, VM_AP_MUSIC_CARD, sizeof(g_music_vars));
    }
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
}
/*显示当前播放时间*/
void ShowNowTime(void)
{
    itoa_2(g_music_vars.BreakPTDisTime.minute, temp_buffer);
    itoa_2(g_music_vars.BreakPTDisTime.second, &temp_buffer[2]);
    LEDPuts(NUMBER1, temp_buffer, 1);
    LEDDisplay(LED_COL, 0xff, 1);
}
/*
 ********************************************************************************
 *             void RealDisplay(void)
 *
 * Description : 将播放时间实现写入寄存器中，以备突然断电后，重启可以进行断点续播
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.主要是备份时，分，秒，用于重启后时间的显示

 ********************************************************************************
 */

void time_writetoreg(void)
{
    uint8 sfr_bak;
    romDI();
    //断点时间写入寄存器，以防硬件断电
    sfr_bak = SFR_BANK;
    SFR_BANK = 0x0c;
    RTCRDM19 = g_music_vars.BreakPTDisTime.hour;
    RTCRDM20 = g_music_vars.BreakPTDisTime.minute;
    RTCRDM21 = g_music_vars.BreakPTDisTime.second;
    SFR_BANK = sfr_bak;
    romEI();
}
/* 获取歌曲当前播放时间 */
void BackupCurrentTime(void)
{
    Full_time_t curtime_buf;

    if (apSendCommand(MC_GETTIME, g_currentMusicType, (void *) &curtime_buf) == 0) //直接调用apSendCommand防止切bank（AP_UI_PLY本身被切出去），在高比特率歌曲时防止有卡音现象
    {
        return;
    }
    g_music_vars.BreakPTDisTime.hour = curtime_buf.TimeHour;
    g_music_vars.BreakPTDisTime.minute = curtime_buf.TimeMinute;
    g_music_vars.BreakPTDisTime.second = curtime_buf.TimeSecond;
    time_writetoreg();
    return;
}
/*
 ********************************************************************************
 *             void deal_play_status(void)
 *
 * Description : 处理播放过程中的异常情况
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.如果非播放状态下，则直接返回
 *			   2.如果播放出错，则转到出错处理函数处理
 *			   3.如果播放到尾，则转到文件尾处理函数处理
 *			   4.如果播放到头，则转到文件头处理函数处理
 *			   5.正常情况下不断刷新播放时间
 ********************************************************************************
 */

uint8 deal_play_status(void)
{
    uint8 retval;
    music_play_status_t status_buf;
    if (PlayStatus < PlaySta)
    {
        return NULL;
    }
    apSendCommand(MC_GETSTATUS, g_currentMusicType, (void *) (&status_buf)); //直接调用apSendCommand防止切bank（AP_UI_PLY本身被切出去），在高比特率歌曲时防止有卡音现象
    if (status_buf.status == PLAYING_ERROR)
    {
        return deal_playing_err();
    }
    else if (status_buf.status == PLAYING_REACH_END)
    {
        retval = DealMusicFileEnd(0);
        if (retval != NULL)
        {
            return retval;
        }
    }
    //注，下面是为解决一种特殊情况而做出的特殊处理，在一般情况下可不分到头，到尾
    //     标志，统一用 PLAYING_REACH_END 标志就可以了，因为可通过 PlayStatus
    //     这个标志判断是快进还是快退，就可知道是到了文件头还是文件尾，但如果在
    //     快退到差不多到头时停止快退，这时已发停止命令，但由于模块层在接收这个
    //     命令后到真正停止快退要有一个时间差，如在这个时间差中到文件头，如还是
    //     用 PLAYING_REACH_END 标志那就分不清是到头还是到尾，所以要加一个标志
    //     PLAYING_REACH_HEAD 表示是到头。
    else if (status_buf.status == PLAYING_REACH_HEAD)
    {
        retval = deal_music_filehead();
        if (retval != NULL)
        {
            return retval;
        }
    }
    else
    {
        if (g_music_vars.BreakPTDisTime.second > 1)
        {
            prev_key_direc = Key_Next;
            err_music_count = 0;
        }
    }
    if (CurUI_Type == Music_UI)
    {
        BackupCurrentTime();
        ShowNowTime();
    }
    return retval;
}

