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
 * Description : ���������Ĵ�С
 *
 *
 * Arguments   : Vol:��ǰ������ֵ
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
 * Description : ��ѡ��ǰ�ĸ�������
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       : ��Ҫ�ǿ��˵��ļ�ͷʱ����Ҫ�ȹرղ��ŵĸ�����Ȼ���ٴ򿪲���
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
 * Description : �����ļ���ͷ�����
 *
 *
 * Arguments   :  DisMod:��ʱ����
 *
 * Returns     : void
 *
 * Notes       : �����˵�ͷʱ��ֻ�е�����̧�������²���
 *
 ********************************************************************************
 */

void DisInFBHead(uint8 DisMod)
{
    DisMod = DisMod;
    memset(temp_buffer, 0x0, 9);
    LEDPuts(NUMBER1, temp_buffer, 1);
    LEDDisplay(LED_COL, 0xff, 1);
    //�ȴ���������̧��
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
 * Description : �����ļ����ŵ�β�����
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       :1.������˵�ͷ�����
 *			   2.���������ļ�β�����
 *			   3.�����������ŵ��ļ�β�����
 *
 ********************************************************************************
 */

uint16 DealMusicFileEnd(uint8 DisMod)
{
    need_draw = TRUE;
    if (PlayStatus == FBPlaySta)
    {
        DisInFBHead(DisMod);//��ͷ��ʾ
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
        DisInFBHead(1);//��ͷ��ʾ
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
 * Description : ������Ч��
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
    /*��������*/
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
 * Description : ��������̧������
 *
 *
 * Arguments   :
 *
 * Returns     : void
 *
 * Notes       : 1.�ڲ���ʱ�����ʱ����Ҫ���޸������Ϊ�������ŵ����
 *			    2.���ļ���Ż�Ŀ¼�����ʾ����ʱ����Ҫֹͣ�����л��ļ���Ŀ¼��ת�������л������ļ�
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
 * Description:  EQ �������ú���
 *
 * Input:  mode   EQ ����ģʽ
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

    //��VRAM��������Ĳ�����ʱ��Ч
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
 * Description : ����EQģʽ������Ϣ
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
 * Description : ���ؾ���ģʽ
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
 * Description : ����ѭ�����ŵ�ģʽ
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
 * Description : ����������
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
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
                //���ѻ����Ŀ���u�̽����ر���
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
 * Description : �����豸�β���Ϣ
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
    if (key == AP_MSG_SD_OUT)//���γ�
    {
        if (CurUselocation.disk == disk_char[0])//������ŵ���u�̣��򲻴����γ���Ϣ
        {
            return NULL;
        }
        else if (USBH_Check_Udisk())//������ŵ��ǿ������Ҵ���u�̣����л���u�̲���
        {
            CurUselocation.disk = disk_char[0];
        }
        else//���û��u�̣���no_device_flag������
        {
            no_device_flag = TRUE;
        }
    }
    else if (key == AP_MSG_UH_OUT)//uhost �γ�
    {
        if (CurUselocation.disk == disk_char[1])//������ŵ��ǿ��̣��򲻴���uhost�γ���Ϣ
        {
            return NULL;
        }
        else if (DRV_DetectUD(1) == 0x20)//������ŵ���uhost�����Ҵ��ڿ������л������̲���
        {
            CurUselocation.disk = disk_char[1];
        }
        else//���û�п��̣���no_device_flag������
        {
            no_device_flag = TRUE;
        }
    }
    else if (key == AP_MSG_SD_IN)//������
    {
        first_init = TRUE;
        //���ԭ�ȵ��̷��ǿ��̣����Ҳ���ֹͣ״̬����˵������죬�����κδ���
        //����ֱ���л�������
        if ((CurUselocation.disk == disk_char[1]) && (PlayStatus == PlaySta))
        {
            return NULL;
        }
        CurUselocation.disk = disk_char[1];
    }
    else
    {
        first_init = TRUE;
        //���ԭ�ȵ��̷���u�̣����Ҳ���ֹͣ״̬����˵������죬�����κδ���
        //����ֱ���л���u��
        if ((CurUselocation.disk == disk_char[0]) && (PlayStatus == PlaySta))
        {
            return NULL;
        }
        CurUselocation.disk = disk_char[0];
    }
    //�л�����ʱ���ȱ���ϵ���Ϣ
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
 * Description : ������folder key����
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
 * Description : ����ͬ�����³���next,prev key�Ĺ���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.�ڲ�����ʾ����ʱ�����ʵ�ֿ����
 *			   2.�ڲ�����ʾ�ļ���Ž���ʵ�ֿ����л�����
 *			   3.����ͣ��ʾ����ʱ�����ʵ�ֿ����л�����
 *			   4.����ʾ�������ڽ���ʵ�ּ���������������
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
 * Description : �������ּ�����Ĺ���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.��Ҫ�ǽ���������ּ�ֵת��Ϊ���Ӧ�����֣�Ȼ��洢��buf��
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
    key_val = (key & ~AP_KEY_UP) - AP_KEY_NUM0;//�����ְ�����Ϣת��������
    DigitalBuf[DigitalCount] = key_val;
    DigitalCount++;
    need_draw = TRUE;
    return NULL;
}
/*
 ********************************************************************************
 *             void SelMusicByDigital(void)
 *
 * Description : ʵ�����ֵ�蹦��
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :�����������ֳ������̵������ļ���������Ϊ0�������κ���Ӧ
 *			  �����������������ҵ���Ӧ�ĸ������в���
 ********************************************************************************
 */

void SelMusicByDigital(void)
{
    uint16 realNum = 0;
    uint8 filename[12];
    realNum = DigitalSwitchToRealNum();
    //�����ǰ������С�ڻ��ļ�����ʱ���򲥷�ѡȡ�����ֶ�Ӧ�ĸ���
    //����ʲô��������ֱ�ӷ��ز��Ž��档
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
 * Description : ����̰�folder ������
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.��Ҫʵ�������л�Ŀ¼�Ĺ���
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
 * Description : ����ͬ�����¶̰�mode ���Ĺ���
 *
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :1.������������棬ʵ�����ֵ�蹦��
 *			   2.����������ʵ��һ���л�����
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

