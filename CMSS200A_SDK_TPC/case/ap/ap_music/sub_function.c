/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: sub functions 4 of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"
#pragma name(SUB_FUNCTION)
const uint8 repeat_val[MAX_REPEAT_MODE] =
{ FSEL_ALL_REPEATALL, FSEL_ALL_REPEATONE, FSEL_ALL_RANDOM, FSEL_DIR_REPEATALL };
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


/****************************code ************************************************/
/*
 ********************************************************************************
 *             void volume_callback(void)
 *
 * Description : 调整音量的大小
 *
 *
 * Arguments   : Vol:当前的音量值
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */

void volume_callback(uint16 Vol)
{
    //    uint8 sfr_bak;
    //    g_comval.vol_display = (uint8) Vol;
    //    g_comval.volume = (uint8) Vol * SOFTVOL_MAX / g_comval.VolumeMax;
    g_comval.volume = Vol;
    set_dac_volume(g_comval.volume);
    //    musicpSendCommand(MC_SETVOLUME, (void*) g_comval.volume);

}

/*
 ********************************************************************************
 *             void SelectCurMusic(void)
 *
 * Description : 重选当前的歌曲播放
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       : 主要是快退到文件头时，需要先关闭播放的歌曲，然后再打开播放
 *
 ********************************************************************************
 */

void SelectCurMusic(void)
{
    musicpSendCommand(MC_STOP, NULL);
    musicpSendCommand(MC_CLOSE, NULL);
    ClearTimePara();
    if (g_Openparam.typeerror == 0)
    {
        musicpSendCommand(MC_OPEN, (void *) &g_Openparam);
        musicpSendCommand(MC_LOAD, NULL);
        musicpSendCommand(MC_PLAY, NULL);
    }
}
/*
 ********************************************************************************
 *             void DisInFBHead(void)
 *
 * Description : 处理文件到头的情况
 *
 *
 * Arguments   :  DisMod:暂时保留
 *
 * Returns     : void
 *
 * Notes       : 当快退到头时，只有当按键抬起后才重新播放
 *
 ********************************************************************************
 */

void DisInFBHead(uint8 DisMod)
{
    DisMod = DisMod;
    memset(temp_buffer, 0x0, 9);
    LEDPuts(NUMBER1, temp_buffer, 1);
    LEDDisplay(LED_COL, 0xff, 1);
    //等待长按键的抬起
    while (ap_get_message() != AP_KEY_LONGUP)
    {
        ;
    }
    SelectCurMusic();
}

/*
 ********************************************************************************
 *             void DealMusicFileEnd(void)
 *
 * Description : 处理文件播放到尾的情况
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       :1.处理快退到头的情况
 *			   2.处理快进到文件尾的情况
 *			   3.处理正常播放到文件尾的情况
 *
 ********************************************************************************
 */

uint16 DealMusicFileEnd(uint8 DisMod)
{
    need_draw = TRUE;
    if (PlayStatus == FBPlaySta)
    {
        DisInFBHead(DisMod);//到头显示
    }
    else if (PlayStatus == FFPlaySta)
    {
        while (ap_get_message() != AP_KEY_LONGUP/* (AP_KEY_NEXT | AP_KEY_UP)*/)
        {
            ;
        }
        stop_music_play(FALSE);
        return get_next_prev_music(Music_Next, Manu_Sel);
    }
    else
    {
        stop_music_play(FALSE);
        return get_next_prev_music(Music_Next, Auto_Sel);
    }
    return NULL;
}
uint8 deal_music_filehead(void)
{
    uint8 retval = NULL;
    if (PlayStatus == PlaySta)
    {
        DisInFBHead(1);//到头显示
    }
    else
    {
        retval = DealMusicFileEnd(1);
        ClearTimePara();
    }
    return retval;
}
/*
 ********************************************************************************
 *             void FadeOut(void)
 *
 * Description : 处理淡出效果
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

void FadeOut(bool DisMod)
{
    /*淡出处理*/
    DisMod = DisMod;
#ifdef OPT_Sup_FadeOut_fun
    music_play_status_t status_buf;

    while (1)
    {
        bool result = musicpSendCommand(MC_FADEINOUT, FADEOUT_DEF | USE_FADEOUT_TIME);
        if (result != 0)
        {
            break;
        }

        musicpSendCommand(MC_GETSTATUS, (void *) (&status_buf));
        if ((status_buf.status == PLAYING_REACH_END) || (status_buf.status == PLAYING_REACH_HEAD) || (status_buf.status
                == PLAYING_ERROR))
        {
            break;
        }
        if ((status_buf.status == PLAYING_WAIT_A) || (status_buf.signal == SIGNAL_REACH_B) || (status_buf.signal
                == SIGNAL_REACH_V))
        {
            musicpSendCommand(MC_SETA_B, ClrABPoint);
            break;
        }
    }
#endif
}
/*
 ********************************************************************************
 *             void deal_long_keyup(void)
 *
 * Description : 处理长按键抬起的情况
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       : 1.在播放时间界面时，主要是修复快进退为正常播放的情况
 *			    2.在文件序号或目录序号显示界面时，主要停止快速切换文件或目录，转而播放切换到的文件
 *
 *
 ********************************************************************************
 */

void deal_long_keyup(void)
{
    switch (CurUI_Type)
    {
    case Music_UI:
        if (PlayStatus > PlaySta)
        {
            musicpSendCommand_bank(MC_STEP, 0);
            PlayStatus = PlaySta;
        }
        break;
    case FileNo_UI:
    case DIRNAME_UI:
        if ((PlayStatus < PlaySta) && (FF_flag != 0))
        {
            if (prev_PlayStatus == PlaySta)
            {
                get_fileinfo_flag = TRUE;
            }
        }
        break;
    default:
        break;
    }
    FF_flag = 0;
}
/**************************************************************************
 * Description:  EQ 参数设置函数
 *
 * Input:  mode   EQ 设置模式
 *
 * Output:  null
 *
 * note:
 ***************************************************************************
 ***/
void EQMode_callback(EQ_set_mod_t mode)
{
    uint8 eq_left[12], eq_right[12];

    mode = mode;
    memset(eq_left, 0, 12);
    memset(eq_right, 0, 12);

    //从VRAM读，保存的参数及时生效
    VMRead(&g_eq_para, VM_PCSET_EQ, sizeof(g_eq_para));
    if (g_eq_para.magic != EQ_VRAM_MAGIC)
    {
        memcpy(eq_left, (void *) EQtemp[(uint8) g_comval.eq_type], 10);
        memcpy(eq_right, (void *) EQtemp[(uint8) g_comval.eq_type], 10);
    }
    else
    {
        memcpy(eq_left, (void *) &g_eq_para.left_eq_para[(uint8) ONE_EQPARA_BYTE * g_comval.eq_type], 7);
        memcpy(eq_right, (void *)&g_eq_para.right_eq_para[(uint8) ONE_EQPARA_BYTE * g_comval.eq_type], 7);
    }
    if (g_comval.eq_type == 0)
    {
        musicpSendCommand(MC_SETUSEREQ, (void *) eq_left);
        musicpSendCommand(MC_SETEQ, 0);
    }
    else
    {
        musicpSendCommand(MC_SETUSEREQ, (void *) eq_left);
        musicpSendCommand(MC_SETEQ, 0x67);
        musicpSendCommand(MC_SETUSEREQ, (void *) eq_right);
        musicpSendCommand(MC_SETEQ, 0x87);
    }
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
            g_comval.eq_type = eq_max - 1;
        }

    }
    if (PlayStatus == PlaySta)
    {
        EQMode_callback(All_EQ_Set);
    }
    need_draw = TRUE;
    return NULL;
}
/*
 ********************************************************************************
 *             void deal_mute_msg(void)
 *
 * Description : 开关静音模式
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

void deal_mute_msg(void)
{
    if (mute_flag == 1)
    {
        set_dac_volume(0);
    }
    else
    {
        set_dac_volume(g_comval.volume);
    }
}
/*
 ********************************************************************************
 *             deal_repeat_msg(uint8 key)
 *
 * Description : 处理循环播放的模式
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

uint8 deal_repeat_msg(uint8 key)
{
    uint8 repeat_mode;
    uint8 i;
    if (key == Key_None)
    {
        if (CurUI_Type != Repeat_UI)
        {
            SetCurUI_Type(Repeat_UI);
            return NULL;
        }
        key = Key_Next;
    }
    for (i = 0; i < MAX_REPEAT_MODE; i++)
    {
        if (g_music_vars.repeat_mode == repeat_val[i])
        {
            repeat_mode = i;
            break;
        }
    }
    if (i >= MAX_REPEAT_MODE)
    {
        repeat_mode = 0;
    }
    if (key == Key_Next)
    {
        if (repeat_mode < (MAX_REPEAT_MODE - 1))
        {
            repeat_mode++;
        }
        else
        {
            repeat_mode = 0;
        }
    }
    else
    {
        if (repeat_mode > 0)
        {
            repeat_mode--;
        }
        else
        {
            repeat_mode = MAX_REPEAT_MODE;
        }
    }
    g_music_vars.repeat_mode = repeat_val[repeat_mode];
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
            SetCurUI_Type(Volume_UI);
        }
        else
        {
            SetCurUI_Type(Music_UI);
        }
    }
    else
    {
        if (CurUI_Type != Volume_UI)
        {
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
void reinit_file_info(void)
{
    PlayStatus = StopSta;
    scan_flag = TRUE;
    if (no_device_flag == FALSE)
    {
        LEDClearScreen();
        LEDPuts(NUMBER1, load_str, 1);
check_uhost:
        if (CurUselocation.disk == disk_char[0] && (USBH_Check_Udisk() == 0x0))
        {
            CurUselocation.disk = disk_char[1];
            if(first_init != TRUE)
            {
                no_device_flag = TRUE;
                goto reinit;
            }
            first_init = FALSE;
            goto check_card;
        }
check_card:
        if (CurUselocation.disk == disk_char[1] && DRV_DetectUD(1) != 0x20)
        {
            CurUselocation.disk = disk_char[0];
            if(first_init != TRUE)
            {
                no_device_flag = TRUE;
                goto reinit;
            }
            first_init = FALSE;
            goto check_uhost;
        }
    }
reinit:
    write_poweroff_flag(0);
    if (no_device_flag == FALSE)
    {
        if(InitFileSelector() != 0)
        {
            get_fileinfo_flag = TRUE;
            need_draw = TRUE;
            Read_VMflag = TRUE;
            auto_switch_device_flag = TRUE;
            return ;
        }
        else
        {
            LEDPuts(NUMBER1, err_str, 1);
            ap_sleep(4);
            if(first_init == TRUE)
            {
                //对已坏掉的卡或u盘进行特别处理
                DRV_UnInstall(DRV_STG);
                first_init = FALSE;
                if(CurUselocation.disk == disk_char[1])
                {
                    CurUselocation.disk = disk_char[0];
                    goto check_uhost;
                }
                else
                {
                    CurUselocation.disk = disk_char[1];
                    goto check_card;
                }
            }
        }

    }
    need_draw = FALSE;
    get_fileinfo_flag = FALSE;
    no_device_flag = TRUE;
}

/*
 ********************************************************************************
 *             void deal_device_msg(void)
 *
 * Description : 处理设备拔插消息
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

uint8 deal_device_msg(uint8 key)
{
    uint8 retval;
    //    while (ap_get_message() != NULL)
    //        ;
    no_device_flag = FALSE;
    if (key == AP_MSG_SD_OUT)//卡拔出
    {
        if (CurUselocation.disk == disk_char[0])//如果播放的是u盘，则不处理卡拔出消息
        {
            return NULL;
        }
        else if (USBH_Check_Udisk())//如果播放的是卡，并且存在u盘，则切换到u盘播放
        {
            CurUselocation.disk = disk_char[0];
        }
        else//如果没有u盘，则将no_device_flag置起来
        {
            no_device_flag = TRUE;
        }
    }
    else if (key == AP_MSG_UH_OUT)//uhost 拔出
    {
        if (CurUselocation.disk == disk_char[1])//如果播放的是卡盘，则不处理uhost拔出消息
        {
            return NULL;
        }
        else if (DRV_DetectUD(1) == 0x20)//如果播放的是uhost，并且存在卡，则切换到卡盘播放
        {
            CurUselocation.disk = disk_char[1];
        }
        else//如果没有卡盘，则将no_device_flag置起来
        {
            no_device_flag = TRUE;
        }
    }
    else if (key == AP_MSG_SD_IN)//卡插入
    {
        first_init = TRUE;
        //如果原先的盘符是卡盘，并且不是停止状态，则说明是误检，不做任何处理
        //否则直接切换到卡盘
        if ((CurUselocation.disk == disk_char[1]) && (PlayStatus == PlaySta))
        {
            return NULL;
        }
        CurUselocation.disk = disk_char[1];
    }
    else
    {
        first_init = TRUE;
        //如果原先的盘符是u盘，并且不是停止状态，则说明是误检，不做任何处理
        //否则直接切换到u盘
        if ((CurUselocation.disk == disk_char[0]) && (PlayStatus == PlaySta))
        {
            return NULL;
        }
        CurUselocation.disk = disk_char[0];
    }
    //切换磁盘时，先保存断点信息
    stop_music_play(TRUE);
    reinit_file_info();
#ifdef MUSIC_AP
    if(no_device_flag == TRUE)
    {
        return 1;
    }
    PlayStatus = StopSta;
    prev_PlayStatus = PlayStatus;
    if(g_comval.SuppTTS != 0)
    {
        retval = report_disk_info();
        if(retval > 1)
        {
            return retval;
        }
    }

#else

    retval = retval;
#endif
    return 1;
}
/*
 ********************************************************************************
 *             void deal_folder_longmsg(void)
 *
 * Description : 处理长按folder key功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 deal_folder_longmsg(uint8 key)
{
    stop_music_play(FALSE);
    if (CurUI_Type != DIRNAME_UI)
    {
        SetCurUI_Type(DIRNAME_UI);
        LEDDisplay(LED_COL, 0xff, 0);
        LEDDisplay(LED_PAUSE, 0xff, 0);
        itoa4(dir_no, temp_buffer);
        LEDPuts(NUMBER1, temp_buffer, 1);
        show_count = 0;
    }
    if (key == Key_Next)
    {
        FF_flag = Key_Next;
    }
    else
    {
        FF_flag = Key_Prev;
    }
    need_draw = FALSE;
    return NULL;
}
/*
 ********************************************************************************
 *             void deal_next_prev_longmsg(void)
 *
 * Description : 处理不同场景下长按next,prev key的功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.在播放显示播放时间界面实现快进退
 *			   2.在播放显示文件序号界面实现快速切换歌曲
 *			   3.在暂停显示播放时间界面实现快速切换歌曲
 *			   4.在显示音量调节界面实现继续调节音量功能
 *
 ********************************************************************************
 */

uint8 deal_next_prev_longmsg(uint8 key)
{
    uint8 retval = NULL;
    switch (CurUI_Type)
    {
    case Music_UI:
        if (PlayStatus == PlaySta)
        {
            if (key == Key_Next)
            {
                PlayStatus = FFPlaySta;
                musicpSendCommand_bank(MC_STEP, 4);
            }
            else
            {
                PlayStatus = FBPlaySta;
                musicpSendCommand_bank(MC_STEP, -4);
            }
        }
        else if ((PlayStatus < PlaySta) && (FF_flag == 0))
        {

            if (key == Key_Next)
            {
                FF_flag = Key_Next;
            }
            else
            {
                FF_flag = Key_Prev;
            }
            prev_key_direc = key;
            SetCurUI_Type(FileNo_UI);
            LEDDisplay(LED_COL, 0xff, 0);
            LEDDisplay(LED_PAUSE, 0xff, 0);
            itoa4(g_musicsequence, temp_buffer);
            LEDPuts(NUMBER1, temp_buffer, 1);
            show_count = 0;
            need_draw = FALSE;
        }
        break;
    case Volume_UI:
        deal_vol_msg(key, Music_UI);
        break;
    case FileNo_UI:
        if (FF_flag == 0)
        {
            if (key == Key_Next)
            {
                FF_flag = Key_Next;
            }
            else
            {
                FF_flag = Key_Prev;
            }
            stop_music_play(FALSE);
            need_draw = FALSE;
        }
        break;
    default:
        break;

    }
    return NULL;
}
/*
 ********************************************************************************
 *             void deal_digital_key(void)
 *
 * Description : 处理数字键输入的功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.主要是将输入的数字键值转换为相对应的数字，然后存储到buf中
 *
 ********************************************************************************
 */

uint8 deal_digital_key(uint8 key)
{
    uint8 key_val;
    if ((DigitalCount >= 4) || (DigitalCount == 0))
    {
        SetCurUI_Type(Digital_UI);
    }
    key_val = (key & ~AP_KEY_UP) - AP_KEY_NUM0;//将数字按键消息转换成数字
    DigitalBuf[DigitalCount] = key_val;
    DigitalCount++;
    need_draw = TRUE;
    return NULL;
}
/*
 ********************************************************************************
 *             void SelMusicByDigital(void)
 *
 * Description : 实现数字点歌功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :如果输入的数字超过磁盘的音乐文件总数或者为0，则不做任何响应
 *			  否则根据输入的数字找到对应的歌曲进行播放
 ********************************************************************************
 */

void SelMusicByDigital(void)
{
    uint16 realNum = 0;
    uint8 filename[12];
    realNum = DigitalSwitchToRealNum();
    //如果当前的数字小于或文件总数时，则播放选取的数字对应的歌曲
    //否则什么都不做，直接返回播放界面。
    if ((realNum <= g_musicamount) && (realNum > 0) && (realNum != g_musicsequence))
    {
        stop_music_play(FALSE);
        ClearTimePara();
        fselGetFileByNoInDisk(filename, realNum);
        get_music_location(FALSE);
        get_fileinfo_flag = TRUE;
    }
    else
    {
        DigitalCount = 0;
        memset(DigitalBuf, 0, 4);
    }
    SetCurUI_Type(Music_UI);
}
/*
 ********************************************************************************
 *             void deal_folder_key(void)
 *
 * Description : 处理短按folder 键功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.主要实现上下切换目录的功能
 *
 ********************************************************************************
 */
uint8 deal_folder_key(uint8 key)
{
    uint8 retval = NULL;
    uint8 filename[16];
    stop_music_play(FALSE);
    err_music_count = 0;
    if (key == Key_Next)
    {
        retval = fselGetNextFolderFile(filename);
    }
    else
    {
        retval = fselGetPrevFolderFile(filename);
    }
    get_music_location(FALSE);
    dir_no = fselGetDirNo(filename, 8);
    if (CurUI_Type != DIRNAME_UI)
    {
        SetCurUI_Type(DIRNAME_UI);
    }
    ClearTimePara();
    if (prev_PlayStatus == PlaySta)
    {
        get_fileinfo_flag = TRUE;
    }
    return NULL;
}
/*
 ********************************************************************************
 *             void deal_mode_msg(void)
 *
 * Description : 处理不同场景下短按mode 键的功能
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.在数字输入界面，实现数字点歌功能
 *			   2.在其他界面实现一键切换功能
 ********************************************************************************
 */

uint8 deal_mode_msg(void)
{
    uint8 retval;
    stop_music_play(TRUE);
    g_music_vars.MusicOpenMod = Nor_open;
    PlayStatus = StopSta;
    prev_PlayStatus = PlayStatus;
    first_init = TRUE;
    if (CurUselocation.disk == disk_char[0])
    {
        if (DRV_DetectUD(1) == 0x20)
        {
            no_device_flag = FALSE;
        }
        else
        {
            return RESULT_MAIN;
        }
        CurUselocation.disk = disk_char[1];
        g_music_vars.cur_disk = CurUselocation.disk;
#ifdef MUSIC_AP
        retval = report_disk_info();
        if(retval != 0)
        {
            return retval;
        }

#else

        retval = retval;
#endif

        if (no_device_flag == FALSE)
        {
            LEDClearScreen();
            LEDPuts(NUMBER1, load_str, 1);
        }
        write_poweroff_flag(0);
        if (no_device_flag == FALSE)
        {
            scan_flag = TRUE;
            if(InitFileSelector() != 0)
            {
                get_fileinfo_flag = TRUE;
                need_draw = TRUE;
                Read_VMflag = TRUE;
                return 1;
            }
            else
            {
                LEDPuts(NUMBER1, err_str, 1);
                ap_sleep(4);
            }
        }

    }
    return RESULT_MUSIC_UPLAY;
}

