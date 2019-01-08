/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: sub functions 3 of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"
#define MusicExtNum 6

#pragma name(GET_MUSICINFO)

const uint8 ExtMusicstr[MusicExtNum][4] =
{
    "MP1", "MP2", "MP3", "WMA", /*"ASF",*/
    "WMV", "WAV"
};

uint8 deal_change_music_msg(uint8 key)
{
    uint8 retval = NULL;
    if (PlayStatus > PlaySta)
    {
        musicpSendCommand_bank(MC_STEP, 0);
        PlayStatus = PlaySta;
        FF_flag = 0;
        return NULL;
    }
    stop_music_play(FALSE);
    if (key == Key_Next)
    {
        retval = get_next_prev_music(Music_Next, Manu_Sel);
    }
    else
    {
        retval = get_next_prev_music(Music_Prev, Manu_Sel);
    }
    need_draw = TRUE;
    return retval;
}

/*
 *******************************************************************************
 * Description : 换曲时要做的选曲，初始化动作
 * Arguments   :
 * Returns     : 真时表示可继续放，假时表示要停止放(播放状态)
 * Notes       :
 *******************************************************************************
 */
uint8 ChangeMusic(uint8 Direct, Music_sel_mod_t mod)
{
    bool result;
    uint8 i = 0;
    file_location_t location_tmp;

    ClearTimePara();

    //备份
    memcpy(&location_tmp, &CurUselocation, sizeof(file_location_t));

    /*播放时手动切换按键的情况*/
    if (mod == Manu_Sel)
    {
        switch (g_music_vars.repeat_mode)
        {
        case FSEL_ALL_REPEATONE:
            FSELSetMode(FSEL_ALL_REPEATALL);
            break;
        default:
            FSELSetMode(g_music_vars.repeat_mode);
            break;
        }
    }
    else
    {
        //正常播放的情况
        if (g_music_vars.repeat_mode == FSEL_ALL_REPEATONE)
        {
            return 2;
        }
        else
        {
            FSELSetMode(g_music_vars.repeat_mode);
        }
    }

    switch (Direct)
    {
    case Music_Next:
        result = FSELGetNext(CurUselocation.filename);
        break;

    case Music_Prev:
        result = FSELGetPrev(CurUselocation.filename);
        break;
    default:
        return TRUE;
    }

    if (!result)
    {
        memcpy(&CurUselocation, &location_tmp, sizeof(file_location_t));
        fselSetLocation(&CurUselocation);
        return FALSE;
    }
    return TRUE;
}
/*
 ******************************************************************************
 *             Music_type_t CheckMusicType(uint8   *filename)
 *
 * Description : 获取下一首或上一首歌曲
 *
 * Arguments   : direct:  文件名
 *				 mod:
 *
 * Returns     : Music_type_t 文件类型
 *
 * Notes       :
 *
 ********************************************************************************
 */

uint8 get_next_prev_music(uint8 direct, Music_sel_mod_t mod)
{
    uint8 g_result;
    /* 格式错误跳到下一曲继续播放*/
findfileagain:
    ClearWatchDog();
    write_poweroff_flag(0);
    g_result = ChangeMusic(direct, mod);
    if (g_result == FALSE)
    {
        return RESULT_UI_STOP;//回到停止界面
    }
    //单曲循环后自动继续循环播放时，不再获取文件信息
    if (g_result != 2)
    {
        get_music_location(FALSE);
    }
    else
    {
        PlayStatus = PauseSta;
    }
    need_draw = FALSE;
    if (g_Openparam.typeerror == 1)
    {
        if(FS_GetInfo() != 0)
        {
            return RESULT_UI_STOP;//回到停止界面
        }
        show_fileno_ui();
        ap_sleep(4);
        if (prev_PlayStatus != PlaySta)
        {
            return NULL;
        }
        else
        {
            if ((err_music_count++) > max_music_err)
            {
                err_music_count = 0;
                PlayStatus = StopSta;
                if (CurUselocation.disk == disk_char[0])
                {
                    //如果检测不到卡或者卡与u盘已经自动切换过一轮，则直接退出music应用
                    if((DRV_DetectUD(1) != 0x20) || (first_init == FALSE))
                    {
                        no_device_flag = TRUE;
                    }
                    else
                    {
                        CurUselocation.disk = disk_char[1];

                        first_init = FALSE;
                    }
                }
                else if (CurUselocation.disk == disk_char[1])
                {
                    //如果检测不到u盘或者卡与u盘已经自动切换过一轮，则直接退出music应用
                    if((USBH_Check_Udisk() == 0) || (first_init == FALSE))
                    {
                        no_device_flag = TRUE;
                    }
                    else
                    {
                        CurUselocation.disk = disk_char[0];

                        first_init = FALSE;
                    }
                }
                return NULL;
            }
            goto findfileagain;
        }

    }
    else if ((CurUI_Type == Music_UI) || (CurUI_Type == FileNo_UI) || (CurUI_Type == DIRNAME_UI))
    {
        show_ui_flag = TRUE;
        SetCurUI_Type(FileNo_UI);
        RefreshMainScreen();
#ifdef MUSIC_AP
        //有tts播报时，不增加延时显示文件序号
        if((g_TagInfoBuf[0] != 0x0) && (FF_flag == 0) && (prev_PlayStatus == PlaySta))
        {
            SetCurUI_Type(Music_UI);
            show_ui_flag = FALSE;
        }
#endif
    }
    get_fileinfo_flag = TRUE;
    return NULL;
}

/*
 ******************************************************************************
 *             Music_type_t CheckMusicType(uint8   *filename)
 *
 * Description : 检测当前是哪种类型的MUSIC文件
 *
 * Arguments   : uint8   *filename  文件名
 *
 * Returns     : Music_type_t 文件类型
 *
 * Notes       :
 *
 ********************************************************************************
 */
Music_type_t near CheckMusicType(uint8 *filename)
{
    uint8 cnt;
    uint8 ext[4];
    Music_type_t ret;
    memcpy(ext, filename + 8, 3);
    ext[0] = ((ext[0] >= 'a') && (ext[0] <= 'z')) ? (ext[0] - 'a' + 'A') : ext[0];
    ext[1] = ((ext[1] >= 'a') && (ext[1] <= 'z')) ? (ext[1] - 'a' + 'A') : ext[1];
    ext[2] = ((ext[2] >= 'a') && (ext[2] <= 'z')) ? (ext[2] - 'a' + 'A') : ext[2];
    ext[3] = 0;

    for (cnt = 0; cnt < Music_TypeMax; cnt++)
    {
        if (strcmp(ext, ExtMusicstr[cnt]) == 0)
        {
            ret = Music_NON + cnt + 1;
            if ((ret == Music_MP1) || (ret == Music_MP2))
            {
                return Music_MP3;
            }
            else
            {
                return ret;
            }
        }
    }
    return Music_NON;
}

/*
 ********************************************************************************
 *             uint16 GetInformation(void)
 *
 * Description : 读取歌曲信息,歌词
 *
 * Arguments   : void
 *
 * Returns     : int
 *
 * Notes       :
 *
 ********************************************************************************
 */
void GetInformation(void)
{
    uint8 first_check = TRUE;
    audio_format_t pFormat;
    ID3Info_t ID3;
    g_currentMusicType = CheckMusicType(CurUselocation.filename);

    memset(g_TagInfoBuf, 0, 92);
    ID3.TIT2_length = 30;
    ID3.TPE1_length = 30;
    ID3.TALB_length = 30;

    ID3.TIT2_buffer = &g_TagInfoBuf[0];
    ID3.TPE1_buffer = &g_TagInfoBuf[30];
    ID3.TALB_buffer = &g_TagInfoBuf[60];

    ID3.TENC_buffer = &g_TagInfoBuf[90];

    g_Openparam.typeerror = 0;
    switch (g_currentMusicType)
    {

    case Music_WMA:
WMA_check:
        g_Openparam.typeerror = wmaCheckType(NULL, &g_Openparam.musicframeoffset,
                                             &g_Openparam.musictotaltime, &pFormat, &ID3);
        //类型检查出错后，再用mp3格式检查一次，以防是后缀改动所致

        if (g_Openparam.typeerror != 0) //正常文件才操作
        {
            if (first_check == TRUE)
            {
                first_check = FALSE;
                goto MP3_check;
            }

#if 0
            /* 判断是否为MP3文件, 后缀改为了WMA */
            g_Openparam.typeerror = mp3CheckType(NULL, &g_Openparam.musicframeoffset, &g_Openparam.musictotaltime,
                                                 &pFormat);
            if (g_Openparam.typeerror == 0)
            {
                g_currentMusicType = Music_MP3;
            }
#endif
        }
        else
        {
            g_currentMusicType = Music_WMA;
        }

#if defined(AP_MUSIC_PEPORT_EN) && defined(AP_MUSIC_NAME)
        if(g_comval.SuppTTS != 0)
        {
            convert_music_id3_string(g_TagInfoBuf, 1);
        }
#endif
        break;

    case Music_WAV:
        g_Openparam.typeerror
            = wavCheckType(NULL, &g_Openparam.musicframeoffset, &g_Openparam.musictotaltime, &pFormat);
        break;

    case Music_MP3:
MP3_check:
        g_Openparam.typeerror = mp3CheckType(NULL, &g_Openparam.musicframeoffset,
                                             &g_Openparam.musictotaltime, &pFormat);
        //类型检查出错后，再用WMA格式检查一次，以防是后缀改动所致

        if (g_Openparam.typeerror != 0)
        {
            if (first_check == TRUE)
            {
                first_check = FALSE;
                goto WMA_check;
            }

#if 0
            /* 判断是否为WMA文件, 后缀改为了MP3 */
            g_Openparam.typeerror = wmaCheckType(NULL, &g_Openparam.musicframeoffset, &g_Openparam.musictotaltime,
                                                 &pFormat, &ID3);
            if (g_Openparam.typeerror == 0)
            {
                g_currentMusicType = Music_WMA;
            }
#endif
        }
        else
        {
            g_currentMusicType = Music_MP3;
        }
#if defined(AP_MUSIC_PEPORT_EN) && defined(AP_MUSIC_NAME)
        if(g_comval.SuppTTS != 0)
        {
            if ((g_Openparam.typeerror == 0) && (mp3GetID3Info(NULL, &ID3, LAN_ID_SCHINESE)))
            {
                convert_music_id3_string(g_TagInfoBuf, 0);
            }
        }
#endif
        break;
    default:
        break;
    }

#if defined(AP_MUSIC_PEPORT_EN) && defined(AP_MUSIC_NAME)
    if((g_comval.SuppTTS != 0) && (g_TagInfoBuf[0] == 0))
    {
        memcpy(g_TagInfoBuf, CurUselocation.filename, 12);
        convert_music_name_string(g_TagInfoBuf);
    }
#endif
}

