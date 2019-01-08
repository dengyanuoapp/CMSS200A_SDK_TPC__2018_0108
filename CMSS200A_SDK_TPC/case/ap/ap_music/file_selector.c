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
        //硬件断电后进来，获取断点时间再显示
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
 ** 名字: 初始化C,U,H盘，
 ** 接口: static int InitFileSelector_sub(char disk)
 ** 描述:
 ** 输入参数: char disk,输入要初始化的盘符。目前支持三种：C：主盘，H：插卡盘，U：外插U盘或读卡器。
 **           每次只初始化一个盘。
 ** 输出参数: 1：成功。0：失败
 ** 使用说明:此函数只被本文件的InitFileSelector，StartFileSelector函数调用。
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
 ** 名字:  文件选择器初始化及报错显示
 ** 接口:  int InitFileSelector(void)
 ** 描述:  把文件选择器初始化到有效的盘上，如找不到，则报错退出
 ** 输入参数: void
 ** 输出参数: 0: 没有有效的盘,初始化失败，要求退出AP
 1: 已完成初始化文件选择器动作
 ** 使用说明:
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
 ** 名字:
 ** 接口:
 ** 描述:
 ** 输入参数:
 ** 输出参数:
 ** 使用说明: //注，这函数是在初始化完文件选择器或set 完目录后调用的
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
 ** 名字:
 ** 接口: int UpdateFileSelector(void)
 ** 描述: 更新文件选择器
 ** 输入参数: void
 ** 输出参数:
 0: 更新不成功，要退出AP
 1: 更新成功，已可用
 ** 使用说明:
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
        /* 从头开始播放 */
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
        //first_check为false,说明磁盘还在，可能磁盘已经损坏，则切换到显示时钟界面
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
            //加强快速拔插卡的出错处理，以免出现死机或播放异常的问题
            if (retval == RESULT_UI_STOP)
            {
                //快速拔插卡或u盘后，如果没有来得及切换磁盘，需要重新初始化磁盘
                if (CurUselocation.disk == disk_char[1] && (DRV_DetectUD(1) == 0x20))
                {
                }
                else if ((CurUselocation.disk == disk_char[0]) && (USBH_Check_Udisk() != 0))
                {
                }
                else
                {

                    //这里主要是为了防止卡或u盘拔出以后，没有及时检测到
                    //卡或u盘拔出消息，从而继续读取磁盘而出现错误的问题
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

