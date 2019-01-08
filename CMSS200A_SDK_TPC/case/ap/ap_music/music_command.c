/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: sub functions 1 of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"

#pragma name(AP_SEND_COMMAND)

//==================================================================
bool musicpSendCommand_bank(uint8 cmd, void *param)
{
    bool result = FALSE;
    bool drm_flag;
    drm_flag = FALSE;
    if (cmd == MC_PLAY)
    {
        if (g_Openparam.typeerror == 0) //正常文件时进行下面操作
        {
#ifdef OPT_Sup_FadeIn_fun
            musicpSendCommand(MC_FADEINOUT, FADEIN_DEF | USE_FADEIN_TIME);
#endif //
        }
    }
    switch (g_currentMusicType)
    {
    case Music_MP3: //mp3
        result = mp3pSendCommand(cmd, param);

        break;

    case Music_WMA: //wma
    case Music_WMV: //wmv
        result = wmapSendCommand(cmd, param);
        break;
    case Music_WAV:
        result = wavpSendCommand(cmd, param);
        break;
    default:
        break;
    }
    if (cmd == MC_PLAY)
    {
        if (g_currentMusicType == Music_WAV)
        {
            g_comval.eq_type = none;
        }
        EQMode_callback(All_EQ_Set);
        if (g_music_vars.karaoke_flag != 0)
        {
            acc_vol_callback(g_music_vars.acc_vol);
            deal_caraok_msg();
        }
        if (result != 0)
        {
            PlayStatus = PlaySta;
        }
    }
    if (cmd == MC_STOP)
    {
        PlayStatus = StopSta;
    }
#if 0
#ifdef SuPPORTKARAOKE
    if(g_music_vars.Karaoke_mode != NOR_Play)
    {
        acc_vol_callback(g_music_vars.acc_vol);
        SpeechMute(g_music_vars.Karaoke_mode);
    }
#endif
#endif

    return result;
}

/*
 ********************************************************************************
 *             bool musicpSendCommand( uint8 cmd, uint16 param )
 *
 * Description : 根据不同的文件类型来选择发的命令（对MODULE）
 *
 * Arguments   : cmd 命令 param 参数
 *
 * Returns     : success/fail
 *
 * Notes       :
 *
 ********************************************************************************
 */
bool musicpSendCommand(uint8 cmd, void *param)
{
    switch (cmd)
    {
    case MC_GETTIME:
    case MC_GETSAPLE:
    case MC_GETSTATUS:
    case MC_GETAUDIOFMT:
    case MC_FADEINOUT:
        return wavpSendCommand(cmd, param);
    default:
        return musicpSendCommand_bank(cmd, param);
    }
    return FALSE;
}

void ClearTimePara(void)
{
    g_music_vars.MusicOpenMod = Nor_open;
    memset(&g_music_vars.BreakPTSave, 0x0, sizeof(breakPT_t));
    memset(&g_music_vars.BreakPTDisTime, 0x0, sizeof(time_t));
}
