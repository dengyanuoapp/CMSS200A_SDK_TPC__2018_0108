#include <actos.h>
#include <basal.h>
#include <decC.h>
#include <tts_lib.h>

#pragma name(MW_BS_AD_CMD_TTS)

BYTE mp3check(void);

#pragma renamecode(MW_BS_AD_CMD_TTS)

bool pSdTtsSendCommand(BYTE cmd, BYTE music_type, void *param)
{
    play_status_t *pStatus;

    switch (cmd)
    {
    case MC_OPEN:
        //            m_deTTSOpenparam = (Open_param_t *)param;
        //            if (m_deTTSOpenparam != 0)
        //            {
        //                if (m_deTTSOpenparam->SoftVolumeMax != 0)
        //                {
        //                    g_decControlInfor.SoftVolumeMax = 40;
        //                }
        //                else
        //                {
        //                    g_decControlInfor.SoftVolumeMax = 0;
        //                }
        //                g_decControlInfor.FadeInTime = m_deTTSOpenparam->FadeInTime;
        //                g_decControlInfor.FadeOutTime = 0x03;//shortest
        //            }
        g_decControlInfor.SoftVolumeMax = 40;
        g_decControlInfor.FadeInTime = 0x1;
        g_decControlInfor.FadeOutTime = 0x03;
        break;

    case MC_LOAD:
        return TTSLoadCommand(music_type, param);

    case MC_PLAY:
        return TTSPlayCommand(param);

    case MC_STOP:
        return TTSStopCommand(param);

    case MC_CLOSE:
        if(m_de_TTS_Fp != NULL)
        {
            SD_FClose(m_de_TTS_Fp);
            m_de_TTS_Fp = NULL;
        }
        if(info_TTS_Fp != NULL)
        {
            SD_FClose(info_TTS_Fp);
            info_TTS_Fp = NULL;
        }
        break;

    case MC_SET_TTS:
        str_ptr = 0;
        str_buff = (char *)param;
        break;

    case MC_GETSTATUS:
        pStatus = (play_status_t *)param;
        //            memcpy(&(pStatus->total_time), &m_TTSTotaltimelength, sizeof(time_t));
        //            if((TTSplay_status.status == PLAYING_REACH_END) || (TTSplay_status.status == PLAYING_REACH_HEAD))
        //            {
        //                pStatus->status = TTSplay_status.status;
        //                pStatus->total_time = m_TTSTotaltimelength;
        //                break;
        //            }
        TTSplay_status.volume = g_decControlInfor.Volume;
        pStatus->volume = g_decControlInfor.Volume;

        pStatus->signal = TTSplay_status.signal;
        TTSplay_status.signal = 0;

        pStatus->status = TTSplay_status.status;
        if(g_decCurStatus.Status >= 0xf0)
        {
            TTSplay_status.status = PLAYING_ERROR;
            if(m_deTTSInitFlag)
            {
                m_deTTSInitFlag = 0;
                while(!decExit())
                {
                    ;//qac
                }
                TM_KillTimer(m_deTTScTimer_Sav);

                IRQ_Release(m_deTTSOldDspIntAddrSav, IRQ_DSP);
                IRQ_Release(m_deTTSOldDmaIntAddrSav, IRQ_DMA2);
            }
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

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

