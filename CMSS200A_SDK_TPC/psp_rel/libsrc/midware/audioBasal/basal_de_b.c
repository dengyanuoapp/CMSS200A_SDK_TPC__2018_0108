/********************************************************************************
 *                              USDK 5102
 *                            Module: POWER MANAGER
 *                 Copyright(c) 2001-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      wuyueqian     2009-9-07 9:42     1.0             build this file
 ********************************************************************************/
/*!
 * file
 * brief
 * Author   wuyueqian
 * par
 *
 *      Copyright(c) 2001-2007 Actions Semiconductor, All Rights Reserved.
 *
 * version 1.0
 * date  2008/6/12
 *******************************************************************************/
#include "basal.h"
#include "decC.h"

#ifdef PC
extern void decAudioIntSub(void);
extern void decDMAIntSub(void);
extern void decTimerIntSub(void);
#endif

#pragma name(MW_BS_AD_CMD1)

//#define DECODE_BUFFER_ADDRESS_NORMAL 0xD80	//ԭ0x600��Ϊʹ��code ram
#define WRONG_DECODE_FILE_TYPE 0xf0

#define CAN_NOT_CALL_RESERVE_INTERFACE() while(1){;}

extern HANDLE m_deFileHandle;
extern Open_param_t *m_deOpenparam; //�������ַ

extern uint32 deBpointerSave;
extern uint32 deBreakPointSave;
extern decBreakPointInfor_t m_breakPointInfo;
extern uint32 deApointerSave;
extern uint32 deFilepages;
extern BYTE m_backupmode;
extern play_status_t play_status;
extern bool m_deABok_sav;
extern BYTE *m_deDataBuffer;
extern uint32 m_deOldDspIntAddrSav;
extern uint32 m_deOldDmaIntAddrSav;
extern time_t m_Totaltimelength;
extern uint8 m_decTimer_Sav;


bool CaltimeCommand(void *param);
bool RestoreCommand(void *param);
bool BackupCommand(void);
bool APointPlayCommand(void);
#pragma ROM(huge)
#ifndef PC
//#pragma ROM(large)
extern uint8 m_deRead_Data(uchar readFileDirection, DWORD position);
#pragma ROM(huge)
#else
extern uint8 m_deRead_Data(uchar readFileDirection, DWORD position);
#endif

#pragma renamecode(MW_BS_AD_CMD1)


/*************************************************************************
 * CaltimeCommand ͨ��AUDIO IP�����������ʱ��
 *����WMA WAV MP3������ͨ�������������ʱ��
 ****************************************************************************/

bool CaltimeCommand(void *param)
{

    param = param;
    if (play_status.status == PLAYING_ERROR)
    {
        return FALSE;
    }
    g_decControlInfor.PlayMode = PLAY_MODE_CALTIME; //  codec ����������������
    decInit();

    m_deOldDspIntAddrSav = IRQ_Intercept((uint32)(decAudioIntSub), IRQ_DSP);
    m_deOldDmaIntAddrSav = IRQ_Intercept((uint32)(decDMAIntSub), IRQ_DMA2);
    m_decTimer_Sav = TM_SetTimer((uint32) decTimerIntSub, 2);

    decstart();
    do
    {
        ClearWatchDog();
        if (g_decCurStatus.Status >= WRONG_DECODE_FILE_TYPE)
        {
            play_status.status = PLAYING_ERROR;
            break;
        }

    }
    while (g_decCurStatus.GetSongInfo != 0xff);  //while(0);

    memcpy(&m_Totaltimelength, &(g_decSongInfor.TotalPlayTime.TimeHour), sizeof(time_t));

    decExit();
    TM_KillTimer(m_decTimer_Sav);
    IRQ_Release(m_deOldDspIntAddrSav, IRQ_DSP);
    IRQ_Release(m_deOldDmaIntAddrSav, IRQ_DMA2);

    return TRUE;

}


BOOL apSendCommand_bank(BYTE cmd, void *param)
{
    switch(cmd)
    {
    case MC_SETPOWERCOUNT:
        g_decControlInfor.EnergyOutFlag = (uint8)param;
        break;

    case MC_GETPOWERVALUE:
        *((uint8 *)param) = g_decCurStatus.EnergyLevel;
        break;

    case MC_SETSOFTVOLUMEMAX:
        if (((uint8) param) != 0)
        {
            g_decControlInfor.SoftVolumeMax = 40;
        }
        else
        {
            g_decControlInfor.SoftVolumeMax = 0;
        }

        break;

    case MC_SET_CHANNEL:

        g_decControlInfor.ChannelSelect = (uint8) param;

        break;

    case MC_SETVOLUME:
        g_decControlInfor.Volume = (uint8)param;
        decSetVolume();
        break;

    case MC_SETA_B:
        switch((uint8)param)
        {
        case ClrABPoint://�������ʹ�ã�AP��signal = reach_Bʱ��⵽��������-1�ͷ�AB_FINISHED����
            if (play_status.status == PLAYING_WAIT_A)
            {
                APointPlayCommand();
            }
            play_status.status = PLAYING_PLAYING;
            g_decControlInfor.ABSetFlag = AB_CLEAR;
            play_status.signal = 0;
            m_deABok_sav = FALSE;//�˳�ABʱҪ��˱�־,�����´�ABֻ��һ��
            break;

        case SetAPoint://SET A
            g_decControlInfor.ABSetFlag = AB_SET_A;
            break;

        case SetBPoint://SET B
            g_decControlInfor.ABSetFlag = AB_SET_B;
            break;

        case ReturnApoint://����A�㲥��
            if(play_status.status == PLAYING_WAIT_A)
            {
                play_status.status = PLAYING_AB;
                APointPlayCommand();
            }
            break;
        case StopBPlay:
            decExit();
            play_status.status = PLAYING_WAIT_A;
            break;
        case ABFinished:
            m_deABok_sav = TRUE;
            break;

        default:
            break;
        }
        break;

    case MC_FADEINOUT:
        //                if(play_status.status == PLAYING_ERROR)
        //				{
        //					return FALSE;
        //				}
        if (((uint8)param & 0xf0) == FADEIN_DEF)
        {
            g_decControlInfor.FadeInTime = (uint8)param & 0x0f;//���õ����־
            return TRUE;
        }
        else if (((uint8)param & 0xf0) == FADEOUT_DEF)
        {
            g_decControlInfor.FadeOutTime = (uint8)param & 0x0f;//���õ�����־
            return TRUE;
        }
        else
        {

        }
        break;

    case MC_AUTO_AB:
        g_decControlInfor.SentDectFlag = (uint8)param;
        break;

    case MC_PAUSE:
        CAN_NOT_CALL_RESERVE_INTERFACE()

    case MC_RESTORE:
        CAN_NOT_CALL_RESERVE_INTERFACE()
        //return RestoreCommand();
    case MC_SETSPEED:
        CAN_NOT_CALL_RESERVE_INTERFACE()
        /*
         play_status.speed = (uint8)param;
         g_decControlInfor.PlaySpeed = (uint8)param;
         break;
         */

    case MC_SETCALLBACK:
        break;


    }
    return TRUE;
}



//other bank combine
/*

 bool BackupCommand(void)
 {
 m_deOpenparam->BreakPTSave->ApointSave = deApointerSave;
 m_deOpenparam->BreakPTSave->BpointSave = deBpointerSave;
 m_deOpenparam->BreakPTSave->breakPT = deBreakPointSave;
 if(m_backupmode == 0)//�ϵ���������
 {
 memcpy(&(m_deOpenparam->BreakPTSave->breakPointInfor), &g_decBreakPointInfor, sizeof(decBreakPointInfor_t));
 }
 else//AB ���������еĶϵ㲻���棬ֻ�Ǳ���A�㣬�Ժ�ϵ�������A�㿪ʼ
 {
 memcpy(&(m_deOpenparam->BreakPTSave->breakPointInfor), &m_breakPointInfo, sizeof(decBreakPointInfor_t));
 }
 return TRUE;
 }

 bool RestoreCommand(void)
 {
 deApointerSave = m_deOpenparam->BreakPTSave->ApointSave;
 deBpointerSave = m_deOpenparam->BreakPTSave->BpointSave;
 deBreakPointSave = m_deOpenparam->BreakPTSave->breakPT;
 memcpy(&m_breakPointInfo, &(m_deOpenparam->BreakPTSave->breakPointInfor), sizeof(decBreakPointInfor_t));
 return TRUE;
 }

 bool APointPlayCommand(void)
 {
 g_decControlInfor.PlayMode = PLAY_MODE_BREAK;
 memcpy(&g_decBreakPointInfor, &m_breakPointInfo, sizeof(decBreakPointInfor_t));
 FS_FSeek(0, FS_SEEK_FFROMSTART, m_deFileHandle);
 decInit();//����ʱ�䲻����
 decstart();
 return TRUE;
 }

 */
