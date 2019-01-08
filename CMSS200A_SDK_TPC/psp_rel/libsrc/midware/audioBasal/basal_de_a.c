#include <actos.h>
#include <basal.h>
#include <decC.h>

#pragma name(MW_BS_AD_CMD)

extern HANDLE m_deFileHandle;
extern Open_param_t *m_deOpenparam;
extern play_status_t play_status;
extern bool m_deABok_sav;
extern uint32 m_deOldDspIntAddrSav;
extern time_t m_Totaltimelength;
extern uint8 m_decTimer_Sav;
extern uint32 m_deOldDmaIntAddrSav;

extern uint32 deBpointerSave;
extern uint32 deBreakPointSave;
extern decBreakPointInfor_t m_breakPointInfo;
extern uint32 deApointerSave;
extern uint32 deFilepages;
extern BYTE m_backupmode;
extern BYTE *m_deDataBuffer;

//const signed char VPSTranTable[17]={-32,-28,-24,-20,-16,-12,-8,-4,0,8,16,24,32,40,48,56,64};

bool LoadCommand(BYTE music_type, void *param);
bool StopCommand(void *param);
bool PlayCommand(void *param);
void save_kill_timer();

extern bool CaltimeCommand(void *param);
extern bool BackupCommand(void);
extern bool RestoreCommand(void *param);
extern bool APointPlayCommand(void);
extern BYTE mp3check(void);

extern BOOL apSendCommand_bank(BYTE cmd, void *param);
extern uint8 m_deRead_Data(uchar readFileDirection, DWORD position);

//#define CAN_NOT_CALL_RESERVE_INTERFACE() while(1){;}

#define DECODE_BUFFER_ADDRESS_NORMAL 0xD80	//原0x600改为使用code ram

#pragma renamecode(MW_BS_AD_CMD)

#ifndef PC
BOOL apSendCommand(BYTE cmd, BYTE music_type, void *param)
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
        m_deOpenparam = (Open_param_t *) param;
        if (m_deOpenparam != 0)
        {
            if (m_deOpenparam->typeerror != 0)
            {
                play_status.status = PLAYING_ERROR;
                m_Totaltimelength.second = 0;
                m_Totaltimelength.minute = 0;
                m_Totaltimelength.hour = 0;
                return FALSE;
            }
            if (m_deOpenparam->SoftVolumeMax != 0)
            {
                g_decControlInfor.SoftVolumeMax = 40;
            }
            else
            {
                g_decControlInfor.SoftVolumeMax = 0;
            }
            g_decControlInfor.FadeInTime = m_deOpenparam->FadeInTime;
            g_decControlInfor.FadeOutTime = 0x03;//shortest
        }
        break;

    case MC_CALTIME:
        return CaltimeCommand(param);

    case MC_LOAD:
        return LoadCommand(music_type, param);

    case MC_PLAY:
        return PlayCommand(param);

    case MC_STOP:
        return StopCommand(param);

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

    case MC_CLOSE:
        if(m_deFileHandle != NULL)
        {
            FS_FClose(m_deFileHandle);
            m_deFileHandle = 0;
        }
        break;

    case MC_SETUSEREQ:
        memcpy(g_decControlInfor.EQVal, (const void *)param, sizeof(g_decControlInfor.EQVal));
        break;

    case MC_SETEQ:

        g_decControlInfor.EQFlag = (uint8)param;
        if(play_status.status == PLAYING_PLAYING)
        {
            decEffectDeal();
        }
        break;

    case MC_BACKUP:
        return BackupCommand();

    case MC_GETTIME:
        memcpy((Full_time_t *)param, &g_decCurStatus.NowTime, sizeof(DWORD));
        break;

    case MC_GETSAPLE:
        *((uint8 *)param) = g_decSongInfor.SampleRate;
        break;

    case MC_GETSTATUS:
        pStatus = (play_status_t *)param;
        memcpy(&(pStatus->total_time), &m_Totaltimelength, sizeof(time_t));

        if((play_status.status == PLAYING_REACH_END) || (play_status.status == PLAYING_REACH_HEAD))
        {
            pStatus->status = play_status.status;
            break;
        }

        play_status.volume = g_decControlInfor.Volume;
        pStatus->volume = g_decControlInfor.Volume;

        pStatus->signal = play_status.signal;//
        play_status.signal = 0;
        play_status.eq = g_decControlInfor.EQFlag;

        pStatus->status = play_status.status;
        if(play_status.status != PLAYING_REACH_END)
        {
            if((g_decCurStatus.Status >= 0xf0) && (g_decCurStatus.Status != 0xf4))
            {
                play_status.status = PLAYING_ERROR;
                while(!decExit())
                {
                    ;//qac
                }
                //                TM_KillTimer(m_decTimer_Sav);
                save_kill_timer();
                IRQ_Release(m_deOldDspIntAddrSav, IRQ_DSP);
                IRQ_Release(m_deOldDmaIntAddrSav, IRQ_DMA2);

            }
        }

        break;

    case MC_GETAUDIOFMT:
        if(play_status.status == PLAYING_ERROR)
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

    default:
        return  apSendCommand_bank(cmd, param);
        //return FALSE;
    }
    return TRUE;
}


/*************************************************************************
 * LoadCommand 初始化codec用的全局变量g_decInitPara，
 * 包括codec的buffer、dma中断的hook函数、歌曲文件的第一帧数据地址等等
 ****************************************************************************/
bool LoadCommand(BYTE music_type, void *param)
{
    //    BankSwitch(BYTE2_OF(m_deRead_Data));
    //    BankSwitch(BYTE2_OF(decAudioIntSub));

    m_deDataBuffer = (BYTE *) DECODE_BUFFER_ADDRESS_NORMAL;

    g_decReadFilePara.ReadFileHandleHook = (DWORD) m_deRead_Data;
    g_decReadFilePara.ReadFileBuffAddr = (DWORD) (m_deDataBuffer);
    g_decControlInfor.SongType = music_type;

    if (m_deOpenparam != NULL)
    {
        g_decSongInfor.FileHDRLen = m_deOpenparam->musicframeoffset;
        memcpy(&m_Totaltimelength, &(m_deOpenparam->musictotaltime), sizeof(time_t));
    }
    else
    {
        g_decSongInfor.FileHDRLen = 512;
    }

    //play_status.speed=0x00;
    m_deABok_sav = FALSE;

    //g_decControlInfor.SoftVolumeMax = 41;	 //由上层去设定，否则每次调用会修改之前设置过的值
    //g_decControlInfor.Volume = 23;

    g_decControlInfor.ChannelSelect = 0;//双通道

    g_decControlInfor.TimeLimit = 50; //auto ab play
    g_decControlInfor.EnergyLimit = 8; //auto ab play

    play_status.status = PLAYING_STOP;

    m_backupmode = 0;

    if (m_deOpenparam != NULL)
    {
        m_deFileHandle = FS_FOpen(m_deOpenparam->filename, FS_OPEN_NOSEEKBACK);
    }
    else
    {
        m_deFileHandle = FS_FOpen((uint8 *) param, FS_OPEN_NOSEEKBACK);
    }

    if (m_deFileHandle == NULL)
    {
        play_status.status = PLAYING_ERROR;
        return FALSE;
    }

    {
        uint32 EndPageLen;//最后一个PAGE的字节数
        EndPageLen = (uint32)FS_GetUnalignedLen(m_deFileHandle);//取最后一个PAGE的BYTE数
        deFilepages = (uint32)FS_GetFileLen(m_deFileHandle);
        g_decReadFilePara.FileLen = (((uint32)deFilepages) << 9); //* 512;

        //处理文件尾不是整PAGE的数据,一页一页传送
        if (EndPageLen != 0)
        {
            g_decReadFilePara.FileLen = g_decReadFilePara.FileLen - 512 + EndPageLen;
        }
    }

    if (g_decReadFilePara.FileLen == 0)
    {
        play_status.status = PLAYING_ERROR;
        return FALSE;
    }

    return TRUE;

}


/*************************************************************************
 * StopCommand 停止播放
 *1 g_decReadFilePara.ReadFilePages在dma hook中被赋值 = deBreakPointSave
 ****************************************************************************/

bool StopCommand(void *param)
{
    m_backupmode = (uint8) param;
    while (!decExit())
    {
        ; //qac
    }
    //g_decBreakPointInfor.BreakPointAddr = g_decReadFilePara.ReadFileCurAddr;//保存文件断点信息
    //    TM_KillTimer(m_decTimer_Sav);
    save_kill_timer();
    IRQ_Release(m_deOldDspIntAddrSav, IRQ_DSP);
    IRQ_Release(m_deOldDmaIntAddrSav, IRQ_DMA2);

    play_status.status = PLAYING_STOP;
    return TRUE;
}

/*************************************************************************
 * PlayCommand 播放
 *
 ****************************************************************************/
bool PlayCommand(void *param)
{
    if (play_status.status == PLAYING_ERROR)
    {
        return FALSE;
    }
    if (play_status.status == PLAYING_PAUSE)
    {
        decContinue();
        play_status.status = PLAYING_PLAYING;
        return TRUE;
    }

    play_status.status = PLAYING_PLAYING;//默认为播放状态
    if ((uint8) param == ABPlay_open)
    {
        RestoreCommand(param);
        //如果deApointerSave = deFilepages表示最后一页，不用再改
        if ((deApointerSave < deFilepages))
        {
            play_status.status = PLAYING_AB;
            g_decControlInfor.ABSetFlag = AB_FLAG_DEAL_AB_PROCESS;//为了进入RD 读文件时就检测该断点是否超过B点了
            memcpy(&g_decBreakPointInfor, &m_breakPointInfo, sizeof(decBreakPointInfor_t));
            g_decControlInfor.PlayMode = PLAY_MODE_BREAK;
        }
        //回到文件头后，codec会根据deApointerSave来seek，并读取数据
        FS_FSeek(0, 0, m_deFileHandle);
    }
    else if (((uint8) param == Nor_Break_open) || ((uint8) param == Hard_Break_open))
    {
        RestoreCommand(param);
        if (deBreakPointSave < deFilepages)
        {
            g_decControlInfor.PlayMode = PLAY_MODE_BREAK;
            memcpy(&g_decBreakPointInfor, &m_breakPointInfo, sizeof(decBreakPointInfor_t));
        }
        FS_FSeek(0, 0, m_deFileHandle);
    }
    else
    {
        //要从头开始放
        g_decControlInfor.PlayMode = PLAY_MODE_NORMAL;
        FS_FSeek(0, 0, m_deFileHandle);
    }

    if (m_deOpenparam == NULL)
    {
        g_decControlInfor.PlayMode = PLAY_MODE_AB;//若参数表地址为空则表示为跟读对比播放
    }

    decInit();

    m_deOldDspIntAddrSav = IRQ_Intercept((uint32)(decAudioIntSub), IRQ_DSP);
    m_deOldDmaIntAddrSav = IRQ_Intercept((uint32)(decDMAIntSub), IRQ_DMA2);
    m_decTimer_Sav = TM_SetTimer((uint32) decTimerIntSub, 2);
    decstart();
    return TRUE;
}

void save_kill_timer()
{
    if(m_decTimer_Sav != 0)
    {
        TM_KillTimer(m_decTimer_Sav);
        m_decTimer_Sav = 0;
    }
}