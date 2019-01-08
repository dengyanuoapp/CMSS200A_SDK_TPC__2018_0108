/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: play status module
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"
#define IntCntValDef 10

extern bool g_light_flag;
uint8 fast_switch_music(void);

#pragma name(AP_UI_PLY)
/*
 ********************************************************************************
 *             uint16 ui_play( void )
 *
 * Description : ��ʾʱ�䣬����, AB��
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 ui_play(void)
{
    uint8 retval;
    FF_flag = 0;
    while (ap_get_message() != NULL)
        ;
#ifdef MUSIC_AP
    if(g_comval.SuppTTS != 0)
    {
        retval = report_disk_info();
        if(retval > 1)
        {
            return retval;
        }
    }
#endif
    while (1)
    {
        ClearWatchDog();
        retval = get_file_info();
        if (retval != NULL)
        {
            return retval;
        }
        if (no_device_flag == TRUE)
        {
            return RESULT_MAIN;
        }
        if (need_draw != 0)
        {
            RefreshMainScreen();
            need_draw = FALSE;
        }
        //����EQ���ߵ���EQ����
        deal_pc_eqsrs(0);
        retval = deal_key_msg();
        if (retval != NULL)
        {
            return retval;
        }
        retval = deal_play_status();
        if (retval != NULL)
        {
            return retval;
        }
        retval = fast_switch_music();
        if (retval != NULL)
        {
            return retval;
        }
        retval = fast_switch_dir();
        if (retval != NULL)
        {
            return retval;
        }
    }/* end of while(1) */
    return 0;
}
/*
 ********************************************************************************
 *             void save_dir_info(void)
 *
 * Description : ���汸�ݵ�ǰ��·����Ϣ����Ҫ����exfat�ļ�ϵͳ������fat16/32
 *				�����κ�����
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */

void save_dir_info(void)
{
    uint8 dir_buf[74];
    memset(dir_buf, 0x0, 74);
    FS_GetCurDir_layer(dir_buf);
    if (CurUselocation.disk == disk_char[0])
    {
        VMWrite(dir_buf, VM_EXFAT_INFO_UH, sizeof(dir_buf));
    }
    else
    {
        VMWrite(dir_buf, VM_EXFAT_INFO_CARD, sizeof(dir_buf));
    }
}
/*
 ********************************************************************************
 *             void get_music_location(void)
 *
 * Description : ��ȡ��ǰ������location��Ϣ����ǰ���̵��ļ���������ǰ���ļ����;
 *				��ȡ�ļ���ID3��Ϣ���Լ�����Ϣ���浽vram�С�
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */

void get_music_location(uint8 flag)
{
    stop_music_play(TRUE);
    FSELGetLocation(&CurUselocation);
    save_dir_info();
    if (flag != 0)
    {
        read_breakpoint();
    }
    //    g_musicamount = FSELGetTotal();
    g_musicsequence = FSELGetNo();
    GetInformation(); //ȡ�ļ���Ϣ
    g_music_vars.cur_disk = CurUselocation.disk;
    CurUselocation.fselTotal = g_musicamount;
    CurUselocation.fselFileNo = g_musicsequence;
    if (CurUselocation.disk == disk_char[0])
    {
        memcpy(&g_music_vars.location[0], &CurUselocation, sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPoit[0], &g_music_vars.BreakPTSave, sizeof(breakPT_t));
    }
    else
    {
        memcpy(&g_music_vars.location[1], &CurUselocation, sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPoit[1], &g_music_vars.BreakPTSave, sizeof(breakPT_t));
    }
    VMWrite(&g_music_vars, VM_AP_MUSIC_CARD, sizeof(g_music_vars));
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (max_music_err >= g_musicamount)
    {
        max_music_err = g_musicamount;
    }
    PlayStatus = PauseSta; //��ͣʱ����ȥ��ȡ��ǰʱ�䣬ʹ��֮ǰ����Ķϵ�ʱ��
    need_draw = TRUE;
}
/*
 ********************************************************************************
 *             void fast_switch_music(void)
 *
 * Description : �����л��ļ����л������л�ȡ�ļ���Ϣ�����򿪲����ļ�
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

uint8 fast_switch_music(void)
{
    uint8 retval = NULL;
    if ((FF_flag == 0) || (CurUI_Type != FileNo_UI))
    {
        return retval;
    }
    if(holdkey_flag == FALSE)
    {
        deal_long_keyup();
        return retval;
    }
    err_music_count = 0;
get_next_continue:
    retval = deal_change_music_msg(FF_flag);
    if ((retval == RESULT_UI_STOP) && (g_music_vars.repeat_mode == FSEL_ALL_RANDOM))
    {
        goto get_next_continue;
    }
    get_fileinfo_flag = FALSE;
    itoa4(g_musicsequence, temp_buffer);
    LEDPuts(NUMBER1, temp_buffer, 1);
    return retval;
}
/*
 ********************************************************************************
 *             void fast_switch_dir(void)
 *
 * Description : �����л�Ŀ¼���л�������ֻ��ȡ�ļ���Ϣ�����򿪲����ļ�
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

uint8 fast_switch_dir(void)
{
    uint8 retval = NULL;
    if ((FF_flag == 0) || (CurUI_Type != DIRNAME_UI))
    {
        return retval;
    }
    if(holdkey_flag == FALSE)
    {
        deal_long_keyup();
        return retval;
    }
    err_music_count = 0;
    retval = deal_folder_key(FF_flag);
    itoa4(dir_no, temp_buffer);
    LEDPuts(NUMBER1, temp_buffer, 1);
    get_fileinfo_flag = FALSE;
    return retval;
}
/*
 ********************************************************************************
 *             void deal_next_prev_msg(void)
 *
 * Description : ����ͬ�����¶̰�next,prev key�Ĺ���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.����������ʵ�ֵ�����������
 *			   2.��EQ���ڽ���ʵ��EQ�л�����
 *			   3.��ѭ��ģʽ����ʵ���л�ѭ����ʽ����
 *			   4.�������Զ��ػ�����ʵ�ֵ����Զ��ػ�ʱ��繦��
 *			   5.���������ʵ���л���������
 *
 ********************************************************************************
 */

uint8 deal_next_prev_msg(uint8 key)
{
    uint8 retval = NULL;
    FF_flag = 0;
    switch (CurUI_Type)
    {
    case Volume_UI:
        retval = deal_vol_msg(key, Music_UI);
        break;
    case EQ_UI:
        retval = deal_eq_msg(key);
        break;
    case Repeat_UI:
        retval = deal_repeat_msg(key);
        break;
    case STANDBY_UI:
        set_autosleep_time(key);
        break;
    default:
        prev_key_direc = key;
        err_music_count = 0;
get_next_continue:
        retval = deal_change_music_msg(key);
        if ((retval == RESULT_UI_STOP) && (g_music_vars.repeat_mode == FSEL_ALL_RANDOM))
        {
            goto get_next_continue;
        }
        if (prev_PlayStatus != PlaySta)
        {
            get_fileinfo_flag = FALSE;
        }
        break;
    }
    return retval;
}
/*
 ********************************************************************************
 *             void deal_play_key(void)
 *
 * Description : ����ͬ�����¶̰�play key�Ĺ���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.�ڷǲ���ʱ����ʾ�������лز���ʱ����ʾ����
 *			   2.�ڲ���ʱ����ʾ������ͣ״̬�£��л�������״̬
 *			   3.�ڲ���ʱ����ʾ���沥��״̬�£��л�����ͣ״̬
 *
 ********************************************************************************
 */

uint8 deal_play_key(void)
{
    if (CurUI_Type == Digital_UI)
    {
        SelMusicByDigital();
        return NULL;
    }
    if (CurUI_Type != Music_UI)
    {
        SetCurUI_Type(Music_UI);
        return NULL;
    }
    if (PlayStatus != PlaySta)
    {
        if (g_Openparam.typeerror == 1)
        {
            get_next_prev_music(Music_Next, Auto_Sel);
        }
        get_fileinfo_flag = TRUE;
    }
    else
    {
        stop_music_play(TRUE);
        g_music_vars.MusicOpenMod = Nor_Break_open;
        PlayStatus = PauseSta;
        prev_PlayStatus = StopSta;
    }
    need_draw = TRUE;
    return NULL;//�˻ص�ֹͣ״̬
}

/*
 ********************************************************************************
 *             void deal_key_msg(void)
 *
 * Description : ����ϵͳ��Ϣ�Ͱ�����Ϣ
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.�����ܵ���ϵͳ��Ϣ�Ͱ�����Ϣת��Ϊʵ�ʹ���ʵ��
 *
 ********************************************************************************
 */

uint8 deal_key_msg(void)
{
    uint8 key;
    uint8 result = NULL;
    key = ap_get_message(); //����ֵ
    switch (key)
    {
    case AP_MSG_RTC:
#if 0
        if (PlayStatus == PlaySta)
        {
            result = check_plug_device_msg(&micin_status, MIC_IN);
            if ((result == AP_MSG_MIC_IN) && (mic_on_flag == 0))
            {
                mic_on_flag = 1;
                OpenMicRoute(MIC_GAIN_VAL);
            }
            else if ((result == AP_MSG_MIC_OUT) && (mic_on_flag == 1))
            {
                mic_on_flag = 0;
                CloseMicRoute();
            }
            result = NULL;
        }
#endif
        if (show_ui_flag != 0)
        {
            show_count++;
            if (show_count >= 4)
            {
                show_ui_flag = 0;
                show_count = 0;
                SetCurUI_Type(Music_UI);
            }
        }
        break;
    case AP_MSG_WAIT_TIMEOUT:
        if (CurUI_Type == Digital_UI)
        {
            SelMusicByDigital();
            return NULL;
        }
        else if (CurUI_Type != Music_UI)
        {
            SetCurUI_Type(Music_UI);
        }
        break;
    case AP_KEY_NEXT | AP_KEY_LONG: //���
        key = Key_Next;
    case AP_KEY_PREV | AP_KEY_LONG: //����
        if(FF_flag == 0)
        {
            result = deal_next_prev_longmsg(key);
        }
        break;

    case AP_KEY_NEXT | AP_KEY_UP:
        key = Key_Next;
    case AP_KEY_PREV | AP_KEY_UP:
        result = deal_next_prev_msg(key);
        break;
    case AP_KEY_PLAY | AP_KEY_UP:
        result = deal_play_key();
        break;
    case AP_KEY_MODE | AP_KEY_UP:
        result = deal_mode_msg();
        if(result == 1)
        {
            result = NULL;
        }
        break;
    case AP_KEY_LONGUP:
        deal_long_keyup();
        break;
    case AP_KEY_CLOCK | AP_KEY_UP:
        result = set_clock_ui(1);
        break;
    case AP_KEY_CLOCK | AP_KEY_LONG:
#ifdef MUSIC_AP
        longkey_flag = TRUE;
        result = deal_clock_report();
        if(result == 1)
        {
            result = 0;
        }
#endif
        break;
    case AP_KEY_VOL | AP_KEY_UP:
        key = Key_None;
        goto dealvolkey;
    case AP_KEY_VOLADD | AP_KEY_UP:
    case AP_KEY_VOLADD | AP_KEY_LONG:
        key = Key_Next;
    case AP_KEY_VOLSUB | AP_KEY_UP:
    case AP_KEY_VOLSUB | AP_KEY_LONG:
dealvolkey:
        result = deal_vol_msg(key, Music_UI);
        break;
    case AP_KEY_VOL | AP_KEY_LONG:
#ifdef MUSIC_AP
        longkey_flag = TRUE;
        result = deal_play_report();
#endif
        break;
    case AP_KEY_EQ | AP_KEY_UP:
        result = deal_eq_msg(Key_None);
        break;
    case AP_KEY_LOOP | AP_KEY_UP:
        result = deal_repeat_msg(Key_None);
        break;
    case AP_MSG_SD_IN:
    case AP_MSG_SD_OUT:
    case AP_MSG_UH_IN:
    case AP_MSG_UH_OUT:
        deal_device_msg(key);
        break;
    case AP_KEY_FOLDERADD | AP_KEY_LONG:
        key = Key_Next;
    case AP_KEY_FOLDERSUB | AP_KEY_LONG:
        result = deal_folder_longmsg(key);
        break;
    case AP_KEY_FOLDERADD | AP_KEY_UP:
        key = Key_Next;
    case AP_KEY_FOLDERSUB | AP_KEY_UP:
        result = deal_folder_key(key);
        break;
    case AP_KEY_KARAOKE | AP_KEY_UP: /* ��Ϊ����OK���ܵĿ�ݿ��ؼ� */
        g_music_vars.karaoke_flag = !g_music_vars.karaoke_flag;
        deal_caraok_msg();
        break;
    case AP_KEY_MICIN | AP_KEY_UP:
        //        mic_on_flag = !mic_on_flag;
        //        deal_mic_msg();
        break;
    case AP_KEY_MUTE | AP_KEY_UP:
        mute_flag = !mute_flag;
        deal_mute_msg();
        break;
    case AP_KEY_AUTOSLEEP | AP_KEY_UP:
        set_autosleep_time(Key_None);
        break;
    default:
        if ((key >= (AP_KEY_NUM0 | AP_KEY_UP)) && (key <= (AP_KEY_NUM9 | AP_KEY_UP)))
        {
            result = deal_digital_key(key);
        }
        else
        {
            result = ap_handle_hotkey(key);
        }
        break;
    }
    return (uint16) result;
}
void reinit_file_location(void)
{
    if (CurUselocation.disk == disk_char[0])
    {
        memset(&g_music_vars.location[0], 0x0, sizeof(file_location_t));
        memset(&g_music_vars.BreakPoit[0], 0x0, sizeof(breakPT_t));
        strcpy(g_music_vars.location[0].path, FS_CD_ROOT);
        g_music_vars.location[0].disk = disk_char[0];
        memcpy(&CurUselocation, &g_music_vars.location[0], sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPTSave, &g_music_vars.BreakPoit[0], sizeof(breakPT_t));
    }
    else
    {
        memset(&g_music_vars.location[1], 0x0, sizeof(file_location_t));
        memset(&g_music_vars.BreakPoit[1], 0x0, sizeof(breakPT_t));
        strcpy(g_music_vars.location[1].path, FS_CD_ROOT);
        g_music_vars.location[1].disk = disk_char[1];
        memcpy(&CurUselocation, &g_music_vars.location[1], sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPTSave, &g_music_vars.BreakPoit[1], sizeof(breakPT_t));
    }
    get_fileinfo_flag = TRUE;
    PlayStatus = StopSta;
}
/*
 ********************************************************************************
 *             void get_file_info(void)
 *
 * Description : ��ȡ�ļ���Ϣ��ת������״̬
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.����get_fileinfo_flag�ж��Ƿ���Ҫ��ȡ�ļ�ϵͳ��ת������״̬
 *			   2.�������ͣ״̬�����ٸ����ļ�ϵͳ����ȡ�ϵ���Ϣ
 *			   3.���ļ�Ȼ��ʼ����
 *
 ********************************************************************************
 */

uint8 get_file_info(void)
{
    uint8 retval = NULL;
    uint8 reinit_flag = FALSE;
    uint16 tmp_musicsequence;
get_info_continue:
    if ((get_fileinfo_flag == 0) || (no_device_flag == TRUE))
    {
        return NULL;
    }
    get_fileinfo_flag = FALSE;
    if (Read_VMflag)
    {
        //        read_VM();
        Read_VMflag = FALSE;
        //������³�ʼ���Ժ�VRAM������ļ�������ʵ�ʼ�����ļ�������ͬ��
        //˵���Դ����й�������������ϵ���Ϣ����ͷ��ʼ����
        if(g_musicamount != CurUselocation.fselTotal)
        {
            reinit_file_location();
            goto get_info_continue;
        }
        tmp_musicsequence = CurUselocation.fselFileNo;
        reinit_flag = TRUE;
    }
    /*��ͣ״̬��ȥ�����ļ���Ϣ���ӿ���ͣ�ٶ�*/
    if (PlayStatus != PauseSta)
    {
        if (UpdateFileSelector() == 0)
        {
            need_draw = FALSE;
            no_device_flag = TRUE;
            return NULL;
        }
        //        read_breakpoint();
        auto_switch_device_flag = 0;
        get_music_location(TRUE);
        //������³�ʼ���Ժ�VRAM������ļ������ʵ�ʼ�����ļ���Ų�ͬ��
        //˵���Դ����й�������������ϵ���Ϣ����ͷ��ʼ����
        if((reinit_flag == TRUE) && (tmp_musicsequence != CurUselocation.fselFileNo))
        {
            reinit_file_location();
            reinit_flag = FALSE;
            goto get_info_continue;
        }
        reinit_flag = FALSE;
        g_music_vars.karaoke_flag = 0;
        show_ui_flag = TRUE;
        prev_PlayStatus = PlaySta;
        SetCurUI_Type(FileNo_UI);

    }
    else
    {
        show_ui_flag = FALSE;
    }
#if 0
    if((g_musicamount == 0) || (g_musicamount < g_musicsequence))
    {
        reinit_file_location();
        goto get_info_continue;
    }
#endif
    if (g_Openparam.typeerror == 1)
    {
        err_music_count++;
        retval = get_next_prev_music(Music_Next, Auto_Sel);
        get_fileinfo_flag = FALSE;
        //���ڻ�ȡ��һ��ʱ�ο���u�̵����
        if (retval == RESULT_UI_STOP)
        {
            if (CurUselocation.disk == disk_char[1] && (DRV_DetectUD(1) == 0x20))
            {
            }
            else if ((CurUselocation.disk == disk_char[0]) && (USBH_Check_Udisk() != 0))
            {
            }
            else
            {
                return RESULT_MAIN;
            }
            reinit_file_info();
            need_draw = TRUE;
            goto get_info_continue;
        }
    }
#ifdef MUSIC_AP
    if(show_ui_flag == TRUE)
    {
        show_fileno_ui();
    }
    if(g_Openparam.typeerror == 1)
    {
        retval = 0;
    }
    else
    {
        retval = deal_play_report();
    }
    if((retval == 1)||(Read_VMflag==TRUE))
    {
        need_draw = TRUE;
        retval = 0;
        goto get_info_continue;
    }
    else if(retval != 0)
    {
        return retval;
    }
    //�����TTS����������ʱ��ʾ�ļ����
    if((show_ui_flag == TRUE) && (g_TagInfoBuf[0] != 0x0))
    {
        show_ui_flag = FALSE;
        SetCurUI_Type(Music_UI);
    }
#endif
    retval = switch_play_status(1);
    //���ڽ�������аο���u�̵����
    if (retval != 0)
    {
        if (CurUselocation.disk == disk_char[1] && (DRV_DetectUD(1) == 0x20))
        {
            reinit_file_info();
            retval = NULL;
        }
        else if ((CurUselocation.disk == disk_char[0]) && (USBH_Check_Udisk() != 0))
        {
            reinit_file_info();
            retval = NULL;
        }
        else
        {
            retval = deal_playing_err();
        }
        if (retval == NULL)
        {
            need_draw = TRUE;
            goto get_info_continue;
        }
    }
    return retval;

}
void get_check_sum(void)
{
    uint8 check_sum;
    uint8 i;
    if (CurUselocation.filename[11] != 0xfe)
    {
        return;
    }
    check_sum = FS_GetCheckSum();
    if (g_music_vars.MusicOpenMod != Nor_open)
    {
        return;
    }
    for (i = 0; i < 2; i++)
    {
        if (CurUselocation.disk == disk_char[i])
        {
            if (check_sum != g_music_vars.check_sum[i])
            {
                g_music_vars.check_sum[i] = check_sum;
                ClearTimePara();
            }
            else
            {
                break;
            }
        }
    }
}
/*
 ********************************************************************************
 *             void switch_play_status(void)
 *
 * Description : ���ļ��л�������״̬
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.���֧������,���Ȳ��Ÿ�����ID3��Ϣ��û��ID3ʱ���Ÿ����ļ���
 *
 *
 *
 *
 ********************************************************************************
 */

uint8 switch_play_status(uint8 report_flag)
{
    report_flag = report_flag;
    if ((no_device_flag == TRUE) || (g_Openparam.typeerror == 1))
    {
        return RESULT_MAIN;
    }
    stop_music_play(FALSE);
    write_poweroff_flag(1);
    musicpSendCommand_bank(MC_OPEN, (void *) &g_Openparam);
    //����exfat�ļ�ϵͳ����Ҫ��ȡ�ļ�����У��ͣ��Ա�ϵ�̲�ʱ
    //�����ж��Ƿ�Ϊͬһ���ļ�
    get_check_sum();
    if (!musicpSendCommand_bank(MC_LOAD, NULL))
    {
        return 1;
    }
    musicpSendCommand_bank(MC_SETPOWERCOUNT, 5);

    if (!musicpSendCommand_bank(MC_PLAY, (void *) (g_music_vars.MusicOpenMod)))
    {
        musicpSendCommand_bank(MC_STOP, NULL);
        return 1;
    }
    volume_callback(g_comval.volume);
    prev_PlayStatus = PlaySta;
    return 0;
}

