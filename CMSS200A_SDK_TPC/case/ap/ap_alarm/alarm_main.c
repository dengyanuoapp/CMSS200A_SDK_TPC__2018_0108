/*
 *******************************************************************************
 *                ACTOS AP
 *        system setting ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: main.c,v 1.1 2008/10/12 09:40:36 stevenluo Exp $
 *******************************************************************************
 */
#include "actos.h"
#include "enhanced.h"
#include "ap_common.h"
#include "uhdrv.h"

#include "ui_alarm.h"

#pragma name(ALARM_MAIN)
const uint8 ExtAlarmstr1[3][4] =
{ "MP3", "WMA", "WAV" };//文件后缀数组
const uint8 ExtNext[4] =
{ "*  " };
uint8 key;
uint8 playing_flag = FALSE;//播放状态0-停止状态，1-暂停状态2-播放状态
uint16 playing_count;
Music_type_a g_currentMusicType;
Open_param_t g_Openparam;
uint8 play_times;//播放次数
uint8 g_TagInfoBuf[92];//用于放TAG信息的BUF
uint8 SearchAlarmFile(void);
extern uint8 play_alarm_music(uint8 type);
int16 main(int16 param)
{
    uint8 result;
    uint8 uhost_ok = FALSE;
    uint8 card_ok = FALSE;
    param = param;
    VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval));
    if (USBH_Check_Udisk())
    {
        if (DRV_ActiveDrive('U', MODE_SLOW) != 0)
        {
            result = SearchAlarmFile();
            if (result != 0)
            {
                g_comval.g_alarm.location.disk = 'U';
                uhost_ok = TRUE;
            }
        }
    }
    if (uhost_ok == FALSE)
    {
        if (DRV_DetectUD(1) == 0x20)
        {
            if (DRV_ActiveDrive('H', MODE_SLOW))
            {
                result = SearchAlarmFile();
                if (result != 0)
                {
                    g_comval.g_alarm.location.disk = 'H';
                    card_ok = TRUE;
                }

            }
        }
    }
    g_comval.g_alarm.volume = g_comval.volume;
    g_Openparam.filename = g_comval.g_alarm.location.filename;
    g_Openparam.BreakPTSave = NULL;
    if ((uhost_ok != 0) || (card_ok != 0))
    {
        memcpy(g_comval.g_alarm.location.filename, file_name, 12);
        g_currentMusicType = ALARMCheckMusicType(g_comval.g_alarm.location.filename);
        if (g_currentMusicType != Music_NON)
        {
            AlarmGetInfo();
            result = play_alarm_music(0);
        }
        else
        {
            strcpy(g_comval.g_alarm.location.filename, "alarm.mp3"); //使用内部alarm文件
            g_comval.g_alarm.location.disk = 'S';
            g_Openparam.typeerror = 0;
            g_Openparam.musicframeoffset = 0x0000;
            result = play_alarm_music(1);
        }
    }
    else
    {
        strcpy(g_comval.g_alarm.location.filename, "alarm.mp3"); //使用内部alarm文件
        g_comval.g_alarm.location.disk = 'S';
        g_Openparam.typeerror = 0;
        g_Openparam.musicframeoffset = 0x0000;
        g_Openparam.filename = g_comval.g_alarm.location.filename;
        result = play_alarm_music(1);
    }
    if (result == RESULT_MAIN)
    {
        return NULL;
    }
    else
    {
        return result;
    }
}

/********************************************************************************
 * Description :查找根目录下是否存入alarm文件夹，如果存在则看里面是否存入可播放歌曲。
 *
 * Arguments  :
 *
 * Returns     :
 *            无
 * Notes       :

 *
 ********************************************************************************/
uint8 SearchAlarmFile(void)
{
    uint8 result;
    uint8 NameBuf[50];
    uint8 name_count, i, tmp_char;
    uint8 dir_type = FS_DIR_DIR;
    if (FS_CD(":") == FALSE)
    {
        return 0;
    }
    while (1)
    {
        ClearWatchDog();
        if (FS_Dir(file_name, ExtNext, dir_type, 1) != 0)
        {
            //对于exfat文件系统，需要获取长名，然后再跟转换成短名进行比较
            if (file_name[11] == 0xfe)
            {
                NameBuf[0] = 0x20;
                name_count = FS_GetName(NameBuf, 24);
                if (name_count != 5)
                {
                    dir_type = FS_DIR_DIR_CONT;
                    continue;
                }
                memset(file_name, 0x0, 12);
                for (i = 0; i < name_count; i++)
                {
                    tmp_char = NameBuf[i * 2];
                    if ((tmp_char >= 0x80) || (NameBuf[i * 2 + 1] != 0))
                    {
                        break;
                    }
                    else if ((tmp_char >= 'a') && (tmp_char <= 'z'))
                    {
                        tmp_char -= 0x20;
                    }
                    file_name[i] = tmp_char;
                }
                if (i < name_count)
                {
                    continue;
                }
            }
            if (strncmp(file_name, "ALARM", 5) == 0)
            {
                if (FALSE == FS_CD(FS_CD_SON))
                {
                    result = 0;
                    break;
                }
                if (FS_Dir(file_name, (const int8 *) ExtAlarmstr1, FS_DIR_FILE, 3) == 0)
                {
                    result = 0;
                    break;
                }
                memcpy(g_comval.g_alarm.location.filename, file_name, 12);
                result = 1;
                break;
            }
            dir_type = FS_DIR_DIR_CONT;
        }
        else
        {
            result = 0;
            break;
        }
    }
    return result;
}

