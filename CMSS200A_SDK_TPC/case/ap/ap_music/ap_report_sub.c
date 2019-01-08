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

uint8 deal_device_msg1(uint8 key)
{
    uint8 retval;
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
        //Ϊ�˷�ֹ��⵽������Ϣ�Ժ������ϸ��ε��������©������
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
        //Ϊ�˷�ֹ��⵽������Ϣ�Ժ������ϸ��ε��������©������
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
    //�л�����ʱ���ȱ���ϵ���Ϣ

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
            //��ǿ���ٰβ忨�ĳ�����������������򲥷��쳣������
            if (result == RESULT_UI_STOP)
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
 * Description :����ID3�͸�����Ϊ�˽�Լ�ռ�д�ɺ���
 *
 * input : NULL
 *
 * output: param ������Ϣֵ
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
