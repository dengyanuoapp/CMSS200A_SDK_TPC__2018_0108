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
#include <basal.h>
#include <decC.h>
#include <tts_lib.h>

#pragma name(MW_BS_AD_CMD1_TTS)

#define DECODE_BUFFER_ADDRESS_NORMAL 0x0d80//0x1800
#define WRONG_DECODE_FILE_TYPE 0xf0

#pragma renamecode(MW_BS_AD_CMD1_TTS)

/*************************************************************************
 * SdLoadCommand 初始化codec用的全局变量g_decInitPara，
 * 包括codec的buffer、dma中断的hook函数、歌曲文件的第一帧数据地址等等
 ****************************************************************************/
bool TTSLoadCommand(BYTE music_type, void *param)
{
    param = param;

    m_deTTSDataBuffer = (BYTE *) DECODE_BUFFER_ADDRESS_NORMAL;

    g_decReadFilePara.ReadFileHandleHook = (DWORD) m_deTTSRead_Data;
    g_decReadFilePara.ReadFileBuffAddr = (DWORD) (m_deTTSDataBuffer);
    g_decControlInfor.SongType = music_type;

    //if(m_deTTSOpenparam != NULL){
    //g_decSongInfor.FileHDRLen = m_deTTSOpenparam->musicframeoffset;
    //}
    g_decSongInfor.FileHDRLen = 0;

    TTSplay_status.speed = 0x00;

    m_de_TTS_Fp = SD_FOpen(tts_lib, 0);
    info_TTS_Fp = SD_FOpen(tts_info, 0);

    if((m_de_TTS_Fp == NULL) || (info_TTS_Fp == NULL))
    {
        TTSplay_status.status = PLAYING_ERROR;
        return FALSE;
    }
    /* 初始化TTS的头 */
    SD_FSeek(info_TTS_Fp, SEEK_SET, 0);
    SD_FRead(info_TTS_Fp, &tts_info_head, sizeof(tts_header_t));
    fix_head_data(&tts_info_head);

    TTSplay_status.status = PLAYING_STOP;

    g_decReadFilePara.FileLen = m_de_TTS_Fp->endaddr - m_de_TTS_Fp->startaddr;
    return TRUE;
}


#if 0
/*************************************************************************
 * SdStopCommand 停止播放
 *1 g_decReadFilePara.ReadFilePages在dma hook中被赋值 = deSdBreakPointSave
 ****************************************************************************/

bool TTSStopCommand(void *param)
{
    param = param;
    if(m_deTTSInitFlag)
    {
        m_deTTSInitFlag = 0;
        while (!decExit())
        {
            ; //qac
        }
        TM_KillTimer(m_deTTScTimer_Sav);
        IRQ_Release(m_deTTSOldDspIntAddrSav, IRQ_DSP);
        IRQ_Release(m_deTTSOldDmaIntAddrSav, IRQ_DMA2);

        TTSplay_status.status = PLAYING_STOP;
    }
    return TRUE;
}
#endif

/*************************************************************************
 * SdPlayCommand 播放
 *
 ****************************************************************************/
bool TTSPlayCommand(void *param)
{
    param = param;

    if (TTSplay_status.status == PLAYING_ERROR)
    {
        return FALSE;
    }
    TTSplay_status.status = PLAYING_PLAYING;//默认为播放状态

    //要从头开始放
    g_decControlInfor.PlayMode = PLAY_MODE_NORMAL;
    //SD_FSeek(m_de_TTS_Fp, 0, 0);

    //初始化读取数据变量
    deTTSSecCount = 0;
    deTTSSecOffset = 0;
    prev_next_flag = 1;

    m_deTTSInitFlag = 0x1;
    tts_lib_flag = *(uint8 *)param;

    decInit();
    m_deTTSOldDspIntAddrSav = IRQ_Intercept((uint32) decAudioIntSub, IRQ_DSP);
    m_deTTSOldDmaIntAddrSav = IRQ_Intercept((uint32) decDMAIntSub, IRQ_DMA2);
    m_deTTScTimer_Sav = TM_SetTimer((uint32) decTimerIntSub, 2);
    decstart();

    return TRUE;
}

