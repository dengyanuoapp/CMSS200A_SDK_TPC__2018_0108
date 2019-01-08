#include <actos.h>
#include <basal.h>
#include <decC.h>

#pragma name(MW_BS_AD_CMD_SD)


extern SD_FILE *m_deSd_Fp;
extern Open_param_t *m_deSdOpenparam;
extern play_status_t Sdplay_status;
extern uint32 m_deSdOldDspIntAddrSav;
extern time_t m_SdTotaltimelength;
extern uint8 m_deSdcTimer_Sav;
extern uint32 m_deSdOldDmaIntAddrSav;

extern bool SdLoadCommand(BYTE music_type, void *param);
extern bool SdStopCommand(void *param);
extern bool SdPlayCommand(void *param);
extern bool SdBackupCommand(void);
extern bool SdRestoreCommand(void);
extern BYTE mp3check(void);

#define CAN_NOT_CALL_RESERVE_INTERFACE() while(1){;}

#pragma renamecode(MW_BS_AD_CMD_SD)

#ifndef PC
BOOL apSdSendCommand(BYTE cmd, BYTE music_type, void *param)
#else
BOOL apSendCommand(BYTE cmd, BYTE music_type, void *param)
#endif
{
    play_status_t *pStatus;
    audio_format_t *pFormat;

#ifdef PC
    if(DealCommandPC(cmd, music_type, param) == FALSE)
        return FALSE;
#endif
    switch (cmd)
    {
    case MC_OPEN:
        m_deSdOpenparam = (Open_param_t *) param;
        if (m_deSdOpenparam != 0)
        {
            if (m_deSdOpenparam->typeerror != 0)
            {
                Sdplay_status.status = PLAYING_ERROR;
                m_SdTotaltimelength.second = 0;
                m_SdTotaltimelength.minute = 0;
                m_SdTotaltimelength.hour = 0;
                return FALSE;
            }
            if (m_deSdOpenparam->SoftVolumeMax != 0)
            {
                g_decControlInfor.SoftVolumeMax = 40;
            }
            else
            {
                g_decControlInfor.SoftVolumeMax = 0;
            }
            g_decControlInfor.FadeInTime = m_deSdOpenparam->FadeInTime;
            g_decControlInfor.FadeOutTime = 0x03;//shortest
        }
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
    case MC_LOAD:
        return SdLoadCommand(music_type, param);

    case MC_PLAY:
        return SdPlayCommand(param);

    case MC_PAUSE:
        CAN_NOT_CALL_RESERVE_INTERFACE()

    case MC_STOP:
        return SdStopCommand(param);

    case MC_STEP:
        if(((int8)param) < 0)
        {
            g_decControlInfor.FFRevFlag = FAST_BACKWARD;
            g_decControlInfor.FFRevSpeed = (uint8)(0 - ((int8)param));
        }
        else if(((int8)param) > 0)
        {
            g_decControlInfor.FFRevFlag = FAST_FORWARD;
            g_decControlInfor.FFRevSpeed = ((uint8)param);
        }
        else
        {
            g_decControlInfor.FFRevFlag = STEP_NORMAL;
            g_decControlInfor.FFRevSpeed = 0;
        }
        decFFRevDeal();
        break;

    case MC_FADEINOUT:
        //                if(Sdplay_status.status == PLAYING_ERROR)
        //				{
        //					return FALSE;
        //				}
        if (((uint8)param & 0xf0) == FADEIN_DEF)
        {
            g_decControlInfor.FadeInTime = (uint8)param & 0x0f;//设置淡入标志
            return TRUE;
        }
        else if (((uint8)param & 0xf0) == FADEOUT_DEF)
        {
            g_decControlInfor.FadeOutTime = (uint8)param & 0x0f;//设置淡出标志
            return TRUE;
        }
        else
        {

        }
        break;

    case MC_CLOSE:
        if(m_deSd_Fp != NULL)
        {
            SD_FClose(m_deSd_Fp);
            m_deSd_Fp = 0;
        }
        break;

    case MC_SETVOLUME:
        g_decControlInfor.Volume = (uint8)param;
        decSetVolume();
        break;


    case MC_SETUSEREQ:
        memcpy(g_decControlInfor.EQVal, (const void *)param, sizeof(g_decControlInfor.EQVal));
        break;

    case MC_SETEQ:

        g_decControlInfor.EQFlag = (uint8)param;
        if(Sdplay_status.status == PLAYING_PLAYING)
        {
            decEffectDeal();
        }
        break;

    case MC_AUTO_AB:
        g_decControlInfor.SentDectFlag = (uint8)param;
        break;

    case MC_BACKUP:
        return SdBackupCommand();
    case MC_RESTORE:
        CAN_NOT_CALL_RESERVE_INTERFACE()
    case MC_SETSPEED:
        CAN_NOT_CALL_RESERVE_INTERFACE()

    case MC_SETCALLBACK:
        break;

    case MC_GETTIME:
        memcpy((Full_time_t *)param, &g_decCurStatus.NowTime, sizeof(DWORD));
        break;

    case MC_GETSAPLE:
        *((uint8 *)param) = g_decSongInfor.SampleRate;
        break;

    case MC_GETSTATUS:
        pStatus = (play_status_t *)param;
        memcpy(&(pStatus->total_time), &m_SdTotaltimelength, sizeof(time_t));

        if((Sdplay_status.status == PLAYING_REACH_END) || (Sdplay_status.status == PLAYING_REACH_HEAD))
        {
            pStatus->status = Sdplay_status.status;
            break;
        }

        Sdplay_status.volume = g_decControlInfor.Volume;
        pStatus->volume = g_decControlInfor.Volume;

        pStatus->signal = Sdplay_status.signal;//
        Sdplay_status.signal = 0;

        pStatus->status = Sdplay_status.status;
        if(Sdplay_status.status != PLAYING_REACH_END)
        {
            if(g_decCurStatus.Status >= 0xf0)
            {
                Sdplay_status.status = PLAYING_ERROR;
                while(!decExit())
                {
                    ;//qac
                }
                TM_KillTimer(m_deSdcTimer_Sav);
                IRQ_Release(m_deSdOldDspIntAddrSav, IRQ_DSP);
                IRQ_Release(m_deSdOldDmaIntAddrSav, IRQ_DMA2);

            }
        }

        break;

    case MC_GETAUDIOFMT:
        if(Sdplay_status.status == PLAYING_ERROR)
        {
            return FALSE;
        }
        pFormat = (audio_format_t *)param;
        memcpy(pFormat->bit_rate, ((uint8 *)(&(g_decSongInfor.BitRate))) + 1, (sizeof(DWORD) - 1));
        pFormat->bit_rate[0] += 0x30;
        pFormat->bit_rate[1] += 0x30;
        pFormat->bit_rate[2] += 0x30;
        pFormat->bit_rate[3] = 0;
        pFormat->sample_rate = g_decSongInfor.SampleRate;
        pFormat->channel = g_decSongInfor.Channel;
        break;

    case MC_SETPOWERCOUNT:
        g_decControlInfor.EnergyOutFlag = (uint8)param;
        break;
    case MC_GETPOWERVALUE:
        *((uint8 *)param) = g_decCurStatus.EnergyLevel;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
