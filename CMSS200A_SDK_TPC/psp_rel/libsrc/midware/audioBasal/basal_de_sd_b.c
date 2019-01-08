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

#pragma name(MW_BS_AD_CMD1_SD)

#define DECODE_BUFFER_ADDRESS_NORMAL 0x0d80//0x1800
#define WRONG_DECODE_FILE_TYPE 0xf0


extern SD_FILE *m_deSd_Fp;
extern Open_param_t *m_deSdOpenparam;//�������ַ

//extern uint32 deSdBpointerSave;
extern uint32 deSdBreakPointSave;
//extern decBreakPointInfor_t m_SdbreakPointInfo;
//extern uint32 deSdApointerSave;
extern uint32 deSdFilepages;
//extern BYTE m_Sdbackupmode;
extern play_status_t Sdplay_status;
//extern bool m_deSdABok_sav;
extern BYTE *m_deSdDataBuffer;
extern uint32 m_deSdOldDspIntAddrSav;
extern uint32 m_deSdOldDmaIntAddrSav;
extern time_t m_SdTotaltimelength;
extern uint8 m_deSdcTimer_Sav;

bool SdLoadCommand(BYTE music_type, void *param);
bool SdStopCommand(void *param);
bool SdPlayCommand(void *param);
bool SdCaltimeCommand(void *param);
bool SdRestoreCommand(void);
bool SdBackupCommand(void);
//bool APointPlayCommand(void);

#ifndef PC
//#pragma ROM(large)
extern uint8 m_deSdRead_Data(uchar readFileDirection, DWORD position);
//#pragma ROM(huge)
#else
extern uint8 m_deSdRead_Data(uchar readFileDirection, DWORD position);
#endif

#pragma renamecode(MW_BS_AD_CMD1_SD)

/*************************************************************************
 * SdLoadCommand ��ʼ��codec�õ�ȫ�ֱ���g_decInitPara��
 * ����codec��buffer��dma�жϵ�hook�����������ļ��ĵ�һ֡���ݵ�ַ�ȵ�
 ****************************************************************************/
bool SdLoadCommand(BYTE music_type, void *param)
{
    //    BankSwitch(BYTE2_OF(m_deSdRead_Data));
    //    BankSwitch(BYTE2_OF(decAudioIntSub));

    m_deSdDataBuffer = (BYTE *) DECODE_BUFFER_ADDRESS_NORMAL;

    g_decReadFilePara.ReadFileHandleHook = (DWORD) m_deSdRead_Data;
    g_decReadFilePara.ReadFileBuffAddr = (DWORD) (m_deSdDataBuffer);
    g_decControlInfor.SongType = music_type;

    if (m_deSdOpenparam != NULL)
    {
        g_decSongInfor.FileHDRLen = m_deSdOpenparam->musicframeoffset;
        memcpy(&m_SdTotaltimelength, &(m_deSdOpenparam->musictotaltime), sizeof(time_t));
    }
    else
    {
        g_decSongInfor.FileHDRLen = 512;
    }
    Sdplay_status.speed = 0x00;


    Sdplay_status.status = PLAYING_STOP;

    m_SdTotaltimelength.second = 0;
    m_SdTotaltimelength.minute = 0;
    m_SdTotaltimelength.hour = 0;

    if (m_deSdOpenparam != NULL)
    {
        m_deSd_Fp = SD_FOpen(m_deSdOpenparam->filename, FS_OPEN_NORMAL);
    }
    else
    {
        m_deSd_Fp = SD_FOpen((uint8 *) param, FS_OPEN_NORMAL);
    }
    if (m_deSd_Fp == NULL)
    {
        Sdplay_status.status = PLAYING_ERROR;
        return FALSE;
    }
    {
        /* uint32 EndPageLen;//���һ��PAGE���ֽ���
         EndPageLen = (uint32)FS_GetUnalignedLen(m_deSd_Fp);//ȡ���һ��PAGE��BYTE��
         deSdFilepages = (uint32)FS_GetFileLen(m_deSd_Fp);
         g_decReadFilePara.FileLen = (((uint32)deSdFilepages)<<9); //* 512;

         //�����ļ�β������PAGE������,һҳһҳ����
         if (EndPageLen != 0)
         {
             g_decReadFilePara.FileLen = g_decReadFilePara.FileLen - 512 + EndPageLen;
         }		 */
    }
    g_decReadFilePara.FileLen = m_deSd_Fp->endaddr - m_deSd_Fp->startaddr;
    deSdFilepages = (g_decReadFilePara.FileLen) >> 9;
    if (g_decReadFilePara.FileLen == 0)
    {
        Sdplay_status.status = PLAYING_ERROR;
        return FALSE;
    }
    return TRUE;

}

/*************************************************************************
 * SdStopCommand ֹͣ����
 *1 g_decReadFilePara.ReadFilePages��dma hook�б���ֵ = deSdBreakPointSave
 ****************************************************************************/

bool SdStopCommand(void *param)
{
    param = param;
    while (!decExit())
    {
        ; //qac
    }
    TM_KillTimer(m_deSdcTimer_Sav);
    IRQ_Release(m_deSdOldDspIntAddrSav, IRQ_DSP);
    IRQ_Release(m_deSdOldDmaIntAddrSav, IRQ_DMA2);

    Sdplay_status.status = PLAYING_STOP;
    return TRUE;
}

/*************************************************************************
 * SdPlayCommand ����
 *
 ****************************************************************************/
bool SdPlayCommand(void *param)
{
    if (Sdplay_status.status == PLAYING_ERROR)
    {
        return FALSE;
    }
    if (Sdplay_status.status == PLAYING_PAUSE)
    {
        decContinue();
        Sdplay_status.status = PLAYING_PLAYING;
        return TRUE;
    }

    Sdplay_status.status = PLAYING_PLAYING;//Ĭ��Ϊ����״̬
    if ((uint8) param == Nor_Break_open)
    {
        SdRestoreCommand();
        if (deSdBreakPointSave < deSdFilepages)
        {
            g_decControlInfor.PlayMode = PLAY_MODE_BREAK;
        }
        SD_FSeek( m_deSd_Fp, 0, 0);
    }
    else
    {
        //Ҫ��ͷ��ʼ��
        g_decControlInfor.PlayMode = PLAY_MODE_NORMAL;
        SD_FSeek(m_deSd_Fp, 0, 0);
    }
    if (m_deSdOpenparam == NULL)
    {
        g_decControlInfor.PlayMode = PLAY_MODE_AB;//���������ַΪ�����ʾΪ�����ԱȲ���
    }

    decInit();
    m_deSdOldDspIntAddrSav = IRQ_Intercept((uint32) decAudioIntSub, IRQ_DSP);
    m_deSdOldDmaIntAddrSav = IRQ_Intercept((uint32) decDMAIntSub, IRQ_DMA2);
    m_deSdcTimer_Sav = TM_SetTimer((uint32) decTimerIntSub, 2);
    decstart();
    return TRUE;
}

