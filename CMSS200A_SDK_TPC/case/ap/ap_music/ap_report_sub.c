/*
 *******************************************************************************
 *                ACTOS AP
 *        This program demostrates how to build an AP
 *
 *        (c) Copyright Actions Co,Ld.
 *
 *******************************************************************************
 */
#include "ap_music.h"

#pragma name(COM_REPORT_SUB)
#ifdef MUSIC_AP
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

uint8 deal_device_msg1(uint8 key)
{
    uint8 retval;
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
        //为了防止检测到插入消息以后，又马上给拔掉，则出现漏检的情况
        if (DRV_DetectUD(1) == 0x20)
        {
            CurUselocation.disk = disk_char[1];
        }
        else if (USBH_Check_Udisk() != 0x0)
        {
            CurUselocation.disk = disk_char[0];
        }
        else
        {
            no_device_flag = TRUE;
        }
    }
    else
    {
        //为了防止检测到插入消息以后，又马上给拔掉，则出现漏检的情况
        if (USBH_Check_Udisk() != 0x0)
        {
            CurUselocation.disk = disk_char[0];
        }
        else if (DRV_DetectUD(1) == 0x20)
        {
            CurUselocation.disk = disk_char[1];
        }
        else
        {
            no_device_flag = TRUE;
        }
    }
    if(no_device_flag == TRUE)
    {
        return RESULT_MAIN;
    }
    //切换磁盘时，先保存断点信息

    stop_music_play(TRUE);

    PlayStatus = StopSta;
    prev_PlayStatus = PlayStatus;
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
#endif

    return 1;
}

uint8 deal_report_result(uint8 retval)
{
    uint8 result;
    //    while(ap_get_message()!=NULL);
    if(retval == RESULT_MAIN)
    {
        result = deal_mode_msg();
        if(result == 1)
        {
            result = NULL;
        }
        return result;
    }
    else if((retval == AP_MSG_SD_IN) || (retval == AP_MSG_UH_IN) || (retval == AP_MSG_SD_OUT) || (retval == AP_MSG_UH_OUT) )
    {
        return deal_device_msg1(retval);
    }
    else if(retval != NULL)
    {
        return retval;
    }
    else
    {
        if (g_Openparam.typeerror == 1)
        {
            result = get_next_prev_music(Music_Next, Auto_Sel);
            //加强快速拔插卡的出错处理，以免出现死机或播放异常的问题
            if (result == RESULT_UI_STOP)
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
                    //                first_check = FALSE;
                    //                goto check_disk;
                    no_device_flag = TRUE;
                    PlayStatus = StopSta;
                    return NULL;
                }
            }
        }
        if(prev_PlayStatus == PlaySta)
        {
            get_fileinfo_flag = FALSE;
            g_music_vars.MusicOpenMod = Nor_Break_open;
            return switch_play_status(0);
        }
        return NULL;
    }
}
uint8 report_disk_info(void)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS)
    {
        retval = report_fix_info((CurUselocation.disk == 'H') ? EXTEND_SDCARD : EXTEND_UDISK,
                                 FALSE, (void *)&CurUselocation.disk);
        return deal_report_result(retval);
    }
    return retval;
}
/**
 **************************************************************
 * Description :播报ID3和歌名，为了节约空间写成函数
 *
 * input : NULL
 *
 * output: param 返回消息值
 *
 * Notes :

 *****************************************************************
 */
uint8 deal_play_report(void)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS == 0)
    {
        return NULL;
    }
    prev_PlayStatus = PlayStatus;
    stop_music_play(TRUE);
    PlayStatus = PauseSta;
    if(PlayStatus != PlaySta)
    {
        while(ap_get_message() != NULL);
        retval = report_music_tag(g_TagInfoBuf, CurUselocation.disk);
    }
    return deal_report_result(retval);

}
uint8 deal_clock_report(void)
{
    uint8 retval;
    if(g_comval.SuppTTS == 0)
    {
        return NULL;
    }
    prev_PlayStatus = PlayStatus;
    stop_music_play(TRUE);
    PlayStatus = PauseSta;
    retval = report_time_info(g_music_vars.cur_disk, FALSE);
    return deal_report_result(retval);
}
#endif
