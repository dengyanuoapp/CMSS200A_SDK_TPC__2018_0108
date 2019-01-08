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

extern uint8 fselError;
#pragma name(FILE_SELECTOR)

uint8 cmp_location(void)
{
    uint8 i;
    for (i = 0; i < 2; i++)
    {
        if (CurUselocation.disk == disk_char[i])
        {
            if (CurUselocation.ClusterNo != g_music_vars.location[i].ClusterNo)
            {
                return 0;
            }
            else if (CurUselocation.DirEntry != g_music_vars.location[i].DirEntry)
            {
                return 0;
            }
            else if (CurUselocation.filename[11] != 0xfe)
            {
                if (memcmp(&CurUselocation.filename, g_music_vars.location[i].filename, 12) != 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}
void write_poweroff_flag(uint8 flag)
{
    uint8 sfr_bak;
    g_power_flag = flag;
    sfr_bak = SFR_BANK;
    SFR_BANK = 0x0c;
    RTCRDM22 = g_power_flag;
    SFR_BANK = sfr_bak;
}
void read_breakpoint(void)
{
    uint8 sfr_bak;
    if (g_power_flag != 0)
    {
        if (cmp_location() == 0)
        {
            return;
        }
        g_music_vars.MusicOpenMod = Hard_Break_open;
        //Ӳ���ϵ���������ȡ�ϵ�ʱ������ʾ
        sfr_bak = SFR_BANK;
        SFR_BANK = 0x0c;
        g_music_vars.BreakPTDisTime.hour = RTCRDM19;
        g_music_vars.BreakPTDisTime.minute = RTCRDM20;
        g_music_vars.BreakPTDisTime.second = RTCRDM21;
        SFR_BANK = sfr_bak;
    }
    else
    {
        if (cmp_location() == 0)
        {
            ClearTimePara();
            return;
        }
        g_music_vars.MusicOpenMod = Nor_Break_open;
    }
    if((g_music_vars.BreakPTDisTime.hour == 0) && (g_music_vars.BreakPTDisTime.minute == 0) && (g_music_vars.BreakPTDisTime.second == 0))
    {
        g_music_vars.MusicOpenMod = Nor_open;
    }
}
/******************************************************************************
 ** ����: ��ʼ��C,U,H�̣�
 ** �ӿ�: static int InitFileSelector_sub(char disk)
 ** ����:
 ** �������: char disk,����Ҫ��ʼ�����̷���Ŀǰ֧�����֣�C�����̣�H���忨�̣�U�����U�̻��������
 **           ÿ��ֻ��ʼ��һ���̡�
 ** �������: 1���ɹ���0��ʧ��
 ** ʹ��˵��:�˺���ֻ�����ļ���InitFileSelector��StartFileSelector�������á�
 ********************************************************************************/
uint16 InitFileSelector_sub(char disk)
{
file_init:
    read_VM();
    if (FSELInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, g_music_vars.fselmod, &CurUselocation, scan_flag) != 0)
    {
        CurUselocation.disk = disk;
        scan_flag = FALSE;
        g_musicamount = FSELGetTotal();
        return 1;
    }
    else if (auto_switch_device_flag != 0)
    {
        auto_switch_device_flag = 0;
        if (disk == disk_char[0])
        {
            if (DRV_DetectUD(1) == 0x20)
            {
                disk = disk_char[1];
                goto file_init;
            }
        }
        else
        {
            if (USBH_Check_Udisk())
            {
                disk = disk_char[0];
                goto file_init;
            }
        }
    }
    return 0;
}

/******************************************************************************
 ** ����:  �ļ�ѡ������ʼ����������ʾ
 ** �ӿ�:  int InitFileSelector(void)
 ** ����:  ���ļ�ѡ������ʼ������Ч�����ϣ����Ҳ������򱨴��˳�
 ** �������: void
 ** �������: 0: û����Ч����,��ʼ��ʧ�ܣ�Ҫ���˳�AP
 1: ����ɳ�ʼ���ļ�ѡ��������
 ** ʹ��˵��:
 ********************************************************************************/
uint16 InitFileSelector(void)
{
    if (InitFileSelector_sub(CurUselocation.disk) != 0)
    {
        return 1;
    }
    return 0;
}

/******************************************************************************
 ** ����:
 ** �ӿ�:
 ** ����:
 ** �������:
 ** �������:
 ** ʹ��˵��: //ע���⺯�����ڳ�ʼ�����ļ�ѡ������set ��Ŀ¼����õ�
 ********************************************************************************/
uint16 StartFileSelector(void)
{
    if (InitFileSelector_sub(CurUselocation.disk) != 0)
    {
        if (FSELGetNext(CurUselocation.filename) != 0)
        {
            return 1;
        }
    }

    return 0;

}
/******************************************************************************
 ** ����:
 ** �ӿ�: int UpdateFileSelector(void)
 ** ����: �����ļ�ѡ����
 ** �������: void
 ** �������:
 0: ���²��ɹ���Ҫ�˳�AP
 1: ���³ɹ����ѿ���
 ** ʹ��˵��:
 ********************************************************************************/
uint16 UpdateFileSelector(void)
{
    uint8 result;
set_locastion:
    result = FSELSetLocation(&CurUselocation);

    if (result == 1)
    {
        if (CurUselocation.filename[0] != '\0')
        {
            return 1;
        }
    }
    if (fselError != FSEL_ERR_DISK)
    {
        /* ��ͷ��ʼ���� */
        ClearTimePara();
        result = StartFileSelector();
    }
    scan_flag = TRUE;
    if (result == 0)
    {
        if (auto_switch_device_flag == 0)
        {
            return 0;
        }
        if (CurUselocation.disk == disk_char[0])
        {
            if (DRV_DetectUD(1) == 0x20)
            {
                CurUselocation.disk = disk_char[1];
                goto set_locastion;
            }
        }
        else
        {
            if (USBH_Check_Udisk())
            {
                CurUselocation.disk = disk_char[0];
                goto set_locastion;
            }
        }
    }
    return result;
}

uint8 deal_playing_err(void)
{
    uint8 retval = NULL;
    uint8 first_check = TRUE;
check_disk:
    if ((CurUselocation.disk == disk_char[1]) && (DRV_DetectUD(1) != 0x20))
    {
        stop_music_play(TRUE);
        CurUselocation.disk = disk_char[0];
        if (USBH_Check_Udisk() == 0)
        {
            no_device_flag = TRUE;
        }
        reinit_file_info();
    }
    else if ((CurUselocation.disk == disk_char[0]) && (!USBH_Check_Udisk()))
    {
        stop_music_play(TRUE);
        CurUselocation.disk = disk_char[1];
        if (DRV_DetectUD(1) != 0x20)
        {
            no_device_flag = TRUE;
        }
        reinit_file_info();
    }
    else
    {
        //first_checkΪfalse,˵�����̻��ڣ����ܴ����Ѿ��𻵣����л�����ʾʱ�ӽ���
        if (first_check == FALSE)
        {
            no_device_flag = TRUE;
        }
        else
        {
            stop_music_play(FALSE);
            if ((err_music_count++) > max_music_err)
            {
                err_music_count = 0;
                PlayStatus = StopSta;
                return NULL;
            }
            if (prev_key_direc == Key_Next)
            {
                retval = get_next_prev_music(Music_Next, Auto_Sel);
            }
            else
            {
                retval = get_next_prev_music(Music_Prev, Auto_Sel);
            }
            //��ǿ���ٰβ忨�ĳ�����������������򲥷��쳣������
            if (retval == RESULT_UI_STOP)
            {
                //���ٰβ忨��u�̺����û�����ü��л����̣���Ҫ���³�ʼ������
                if (CurUselocation.disk == disk_char[1] && (DRV_DetectUD(1) == 0x20))
                {
                }
                else if ((CurUselocation.disk == disk_char[0]) && (USBH_Check_Udisk() != 0))
                {
                }
                else
                {

                    //������Ҫ��Ϊ�˷�ֹ����u�̰γ��Ժ�û�м�ʱ��⵽
                    //����u�̰γ���Ϣ���Ӷ�������ȡ���̶����ִ��������
                    first_check = FALSE;
                    goto check_disk;
                }
                reinit_file_info();
            }
            need_draw = TRUE;
        }

    }
    return NULL;
}

