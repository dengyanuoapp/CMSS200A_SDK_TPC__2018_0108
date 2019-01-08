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
 * Description : ������״̬�л�����ͣ״̬
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.�����ֹͣ״̬�£���ֱ�ӷ���
 2.ֹͣ���룬�ر��ļ����
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
/*��ʾ��ǰ����ʱ��*/
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
 * Description : ������ʱ��ʵ��д��Ĵ����У��Ա�ͻȻ�ϵ���������Խ��жϵ�����
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.��Ҫ�Ǳ���ʱ���֣��룬����������ʱ�����ʾ

 ********************************************************************************
 */

void time_writetoreg(void)
{
    uint8 sfr_bak;
    romDI();
    //�ϵ�ʱ��д��Ĵ������Է�Ӳ���ϵ�
    sfr_bak = SFR_BANK;
    SFR_BANK = 0x0c;
    RTCRDM19 = g_music_vars.BreakPTDisTime.hour;
    RTCRDM20 = g_music_vars.BreakPTDisTime.minute;
    RTCRDM21 = g_music_vars.BreakPTDisTime.second;
    SFR_BANK = sfr_bak;
    romEI();
}
/* ��ȡ������ǰ����ʱ�� */
void BackupCurrentTime(void)
{
    Full_time_t curtime_buf;

    if (apSendCommand(MC_GETTIME, g_currentMusicType, (void *) &curtime_buf) == 0) //ֱ�ӵ���apSendCommand��ֹ��bank��AP_UI_PLY�����г�ȥ�����ڸ߱����ʸ���ʱ��ֹ�п�������
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
 * Description : �����Ź����е��쳣���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.����ǲ���״̬�£���ֱ�ӷ���
 *			   2.������ų�����ת��������������
 *			   3.������ŵ�β����ת���ļ�β����������
 *			   4.������ŵ�ͷ����ת���ļ�ͷ����������
 *			   5.��������²���ˢ�²���ʱ��
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
    apSendCommand(MC_GETSTATUS, g_currentMusicType, (void *) (&status_buf)); //ֱ�ӵ���apSendCommand��ֹ��bank��AP_UI_PLY�����г�ȥ�����ڸ߱����ʸ���ʱ��ֹ�п�������
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
    //ע��������Ϊ���һ��������������������⴦����һ������¿ɲ��ֵ�ͷ����β
    //     ��־��ͳһ�� PLAYING_REACH_END ��־�Ϳ����ˣ���Ϊ��ͨ�� PlayStatus
    //     �����־�ж��ǿ�����ǿ��ˣ��Ϳ�֪���ǵ����ļ�ͷ�����ļ�β���������
    //     ���˵���ൽͷʱֹͣ���ˣ���ʱ�ѷ�ֹͣ���������ģ����ڽ������
    //     ���������ֹͣ����Ҫ��һ��ʱ���������ʱ����е��ļ�ͷ���绹��
    //     �� PLAYING_REACH_END ��־�Ǿͷֲ����ǵ�ͷ���ǵ�β������Ҫ��һ����־
    //     PLAYING_REACH_HEAD ��ʾ�ǵ�ͷ��
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

