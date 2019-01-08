/********************************************************************************
 *                              USDK 5102
 *                            Module: POWER MANAGER
 *                 Copyright(c) 2001-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *    wuyueqian     2009-9-07 9:42     1.0             build this file
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
#include <actos.h>
#include <basal.h>
#include <encC.h>
#include <codec.h>
#include <stringH.h>

#pragma name(MW_BS_AD_EN)

#define ENABLE_PITCH

#define RECORDING_OPEN    0x20
#define RECORDING_LOAD    0x21

#define READ_ADDRESS        0x0201
#define VOR_TIME_OUT      0x01
#define RECORD_BUFFER_ADDRESS 0x0d80//0xc000
#define RECORD_BUFFER_SIZE 512

#define WRITE_NONE_PAGE 0
#define WRITE_PAGE 1
//#define WRITE_HEAD 2
#define ONE_PAGE_BITNUM 9
#define INPUT_MIC 0x08
#define INPUT_LINEIN 0x40
#define INPUT_FM 0x02
#define INPUT_DAC 0x01
#define ADPCM 0
#define PCM 1
#define AVR_TURN_OFF 0
#define AVR_TURN_ON_VOICE 1
#define AVR_TURN_ON_SONG 2

#define AK211X_SUPPORT_WAV_ONLY

BYTE far m_wavRecStatus;// = RECORDING_WAITING;      //当前状态
handle far m_wavRecFileHandle;// = NULL;    //打开语音文件的句柄
uint8 far m_wavRecOldTimerNum; //timer isr 使用的timer号码
uint16 far m_wavRecOldDspIsr; //保存旧的hip isr
uint16 far m_wavRecOldDMAIsr; //保存旧的ADC isr

char far m_wavRecFileName[12];

uint32 far pitch_write_page = 0;
uint8 far pitch_write_flag;

void encode_setAudioFormat(void *param);
bool encode_getNewFileHandle(void *param);
void encode_initAudioFormat(BYTE music_type);
void encode_getAudioFormat(void *param);
void ch_bit_rate(char *tmp);
int8 parse_bit_rate(uint8 *ch);
void setRecordBufferClock(void);

int16  RunEffectCH(int16 *pSoundInOut);

#pragma renamecode(MW_BS_AD_EN)

#ifdef ENABLE_PITCH
void swappage(uint8 *dat)
{
    uint16 i;
    uint8 tmp;

    for(i = 0; i < 512; i += 2)
    {
        tmp = dat[i];
        dat[i] = dat[i + 1];
        dat[i + 1] = tmp;
    }
}
#endif

/*******************************************************************************
 * Function:       Wav2rProceed
 * Description:    wav 录音编码写入文件
 * Input：
 * Output：        0:没有写, 1:写一个扇区, 8:已写了WAV头文件
 *                 -1:出错,用wavrSendCommand(MC_GETSTATUS,mp3_rec_status_t *)获取详细信息
 * Other：         一次写一个扇区(512 bytes),
 *******************************************************************************/
#ifdef PC
retWavrProceed_e srProceed(void)
#else
retWavrProceed_e rProceed(void)
#endif
//retWavrProceed_e srProceed(void)
{
    uchar status;
    BOOL result;
    uint8 flag;
    status = encGetBitstream();//查询enc的数据编码
    if (WRITE_PAGE == status)
    {
#ifdef ENABLE_PITCH
        swappage((uint8 *)g_encWriteFilePara.WriteFileBuffAddr);
        flag = RunEffectCH((int16 *)g_encWriteFilePara.WriteFileBuffAddr);
        //flag = 1;
        if(flag != pitch_write_flag)
        {
            pitch_write_flag = flag;
        }
        swappage((uint8 *)g_encWriteFilePara.WriteFileBuffAddr);
        if(flag)
        {
            result = FS_FWrite((const char *) g_encWriteFilePara.WriteFileBuffAddr,
                               (char)(g_encWriteFilePara.WriteFileCurSize >> ONE_PAGE_BITNUM),    //风险，从16位变8位
                               m_wavRecFileHandle);
            pitch_write_page++;
        }
        else
        {
            result = 1;
        }
#else
        result = FS_FWrite((const char *) g_encWriteFilePara.WriteFileBuffAddr,
                           (char)(g_encWriteFilePara.WriteFileCurSize >> ONE_PAGE_BITNUM),    //风险，从16位变8位
                           m_wavRecFileHandle);
#endif
        if (result == 1)
        {
            g_encWriteFilePara.FileLen += g_encWriteFilePara.WriteFileCurSize;
            return WRITE_PAGE;
        }
        else
        {
            encStop();
            encExit();
            IRQ_Release(m_wavRecOldDspIsr, IRQ_DSP);
            //IRQ_Release(m_wavRecOldDMAIsr, IRQ_DMA2);

            if(m_wavRecOldTimerNum)
            {
                TM_KillTimer(m_wavRecOldTimerNum);
                m_wavRecOldTimerNum = 0;
            }
            FS_FClose(m_wavRecFileHandle);
            m_wavRecStatus = RECORDING_DISK_ERROR;
            //文件已关闭
            m_wavRecFileHandle = NULL;
            return WRITE_WRONG;
        }
    }
    else if (status == 0x02)// 是文件头写入WAV文件
    {
        encExit();
        IRQ_Release(m_wavRecOldDspIsr, IRQ_DSP);
        //		IRQ_Release(m_wavRecOldDMAIsr, IRQ_DMA2);
        if(m_wavRecOldTimerNum)
        {
            TM_KillTimer(m_wavRecOldTimerNum);
            m_wavRecOldTimerNum = 0;
        }
        result = FS_FClose(m_wavRecFileHandle);
        if (!result)
        {
            m_wavRecStatus = RECORDING_DISK_ERROR;
            return WRITE_WRONG;
        }
        //	if(g_encControlInfor.SongType!=0)
        //	{
        m_wavRecFileHandle = FS_FOpen(m_wavRecFileName, 0x02);
        if (NULL == m_wavRecFileHandle)
        {
            return WRITE_NONE;
        }
#ifndef PC
#ifdef ENABLE_PITCH
        {
            uint8 *buf = (char *) g_encWriteFilePara.WriteFileBuffAddr;
            uint32 filesize = (pitch_write_page + 1) * 512;
            filesize -= 8;
            buf[4] = filesize & 0xff;
            buf[5] = (filesize >> 8) & 0xff;
            buf[6] = (filesize >> 16) & 0xff;
            buf[7] = (filesize >> 24) & 0xff;

            filesize = pitch_write_page * 512;
            buf[508] = filesize & 0xff;
            buf[509] = (filesize >> 8) & 0xff;
            buf[510] = (filesize >> 16) & 0xff;
            buf[511] = (filesize >> 24) & 0xff;
        }
#endif
        //write head
        result = FS_FWrite((const char *) g_encWriteFilePara.WriteFileBuffAddr,
                           (char)(g_encWriteFilePara.WriteFileCurSize >> ONE_PAGE_BITNUM),
                           m_wavRecFileHandle);
#endif
        m_wavRecStatus = RECORDING_STOP;
        if (result == 1)
        {
            return WRITE_HEAD;
        }
        else
        {
            m_wavRecStatus = RECORDING_DISK_ERROR;
            return WRITE_WRONG;
        }
        //	}
        //	else
        //	{
        //已经成功关闭文件
        //	   m_wavRecFileHandle = NULL;
        //	   m_wavRecStatus = RECORDING_STOP;
        //	   return WRITE_HEAD;
        //	}
    }
    else
    {
        return WRITE_NONE;
    }
}
/*******************************************************************************
 * Function:       wavrSendCommand
 * Description:    wav encode modual entry
 * Input：
 * Output：
 * Other：         // 发送MC_STOP之后,需要等到wav2rProceed()返回8才可以发送MC_CLOSE
 *******************************************************************************/
#ifdef PC
BOOL srSendCommand(BYTE cmd, BYTE music_type, void *param)
#else
BOOL rSendCommand(BYTE cmd, BYTE music_type, void *param)
#endif
//BOOL srSendCommand(BYTE cmd, BYTE music_type, void* param)
{
    mp3_rec_status_t *wav_rec_status; // AP 层
    time_t *pTime;
    BOOL result;

    switch (cmd)
    {
    case MC_OPEN:
        encode_initAudioFormat(music_type);
        break;

    case MC_CLOSE:
        if(m_wavRecOldTimerNum)
        {
            TM_KillTimer(m_wavRecOldTimerNum);
            m_wavRecOldTimerNum = 0;
        }
        if ((m_wavRecStatus == RECORDING_STOP))// && (m_wavRecFileHandle != NULL))
        {
            if (NULL != m_wavRecFileHandle)
            {
#ifdef PC
                WriteWavDataPC(m_wavRecFileHandle);
#endif
                FS_FClose(m_wavRecFileHandle);
                m_wavRecFileHandle = NULL;
            }
            else
            {
                m_wavRecStatus = RECORDING_EXCEPTION;
                return FALSE;
            }
        }
        else
        {
            m_wavRecStatus = RECORDING_EXCEPTION;
            return FALSE;
        }

        break;

    case MC_LOAD:
        result = encode_getNewFileHandle(param);
        if (!result)
        {
            return FALSE;
        }
        memset((char *)g_encWriteFilePara.WriteFileBuffAddr, 0xff, 512);
        FS_FWrite((const char *) g_encWriteFilePara.WriteFileBuffAddr, 1, m_wavRecFileHandle);
        g_encWriteFilePara.FileLen += 512;
        encInit();
        m_wavRecStatus = RECORDING_WAITING;
        break;

    case MC_RECORD:
        if ((m_wavRecStatus == RECORDING_WAITING) && (m_wavRecFileHandle != NULL))
        {
            m_wavRecOldTimerNum = TM_SetTimer((uint32) encTimerIntSub, 2);
            if (m_wavRecOldTimerNum == 0)
            {
                m_wavRecStatus = RECORDING_DISK_ERROR;
                return FALSE;
            }
            //m_wavRecOldDspIsr = IRQ_Intercept((uint16) encAudioIntSub, IRQ_DSP);
            //m_wavRecOldDMAIsr = IRQ_Intercept((int)encDMAIntSub, IRQ_DMA2);

            encStart();
        }
        else
        {
            // 继续录音
            if (m_wavRecStatus == RECORDING_PAUSE)
            {
                encContinue();
            }
            else
            {
                m_wavRecStatus = RECORDING_EXCEPTION;
                return FALSE;
            }
        }
        m_wavRecStatus = RECORDING_RECORDING;
        break;

    case MC_PAUSE:
        if (m_wavRecStatus == RECORDING_RECORDING)
        {
            encPause();
        }
        else
        {
            m_wavRecStatus = RECORDING_EXCEPTION;
            return FALSE;
        }
        m_wavRecStatus = RECORDING_PAUSE;
        break;

    case MC_STOP:
        encStop();//codec prepare the head data for writing
        break;

    case MC_SETGAIN:
        g_encControlInfor.InputGain = (BYTE) param;
        break;

    case MC_SETGAIN_DSP:
        if ((((BYTE) param) < 2))// && (((BYTE) param) >= 0))
        {
            g_encControlInfor.DSPGain = (BYTE) param;
        }
        break;
    case MC_SETGAIN_ADC:
        if ((((BYTE) param) < 4))// && (((BYTE) param) >= 0))
        {
            g_encControlInfor.ADCGain = (BYTE) param;
        }
        break;

    case MC_SET_RECORD_DELAY:
        g_encControlInfor.DelayTime = (BYTE) param;

    case MC_SETAVRFLAG:
        if ((((BYTE) param) < 3))// && (((BYTE) param) >= 0))
        {
            g_encControlInfor.AVRFlag = (BYTE) param;
        }
        break;
    case MC_SETAVRLEVEL:
        g_encControlInfor.AVRLevel = (WORD) param;
        break;
    case MC_SETAVRDELAY:
        g_encControlInfor.AVRDelay = (BYTE) param;
        break;

    case MC_SET_SONG_TYPE:
        g_encControlInfor.SongType = (BYTE) param;
        break;

    case MC_RECODEMODE:
        g_encControlInfor.EncodeMode = (BYTE) param;
        break;

    case MC_SETSOURCE:
        g_encControlInfor.InputSel = ((resource_t *) param)->input;
        //memcpy(&(g_encControlInfor.InputSel), &(((resource_t *)param)->input), sizeof(input_t));
        break;

    case MC_SETAUDIOFMT:
        encode_setAudioFormat(param);
        break;

    case MC_GETTIME:
        pTime = (time_t *) param;
        //memcpy(pTime, &(g_encStatus.CurTime.TimeHour), sizeof(time_t));
        {
            uint32 total_second = (uint32)pitch_write_page * 512 / (256000 / 8);
            uint32 hour, second, minute;

            hour = total_second / 3600;
            minute = (total_second - hour * 3600) / 60;
            second = total_second - hour * 3600 - minute * 60;

            pTime->hour = hour;
            pTime->minute = minute;
            pTime->second = second;
        }
        break;

    case MC_GETSTATUS:
        wav_rec_status = (mp3_rec_status_t *) param;
        if (g_encStatus.Status != 0x0)
        {
            wav_rec_status->status = g_encStatus.Status;
        }
        else
        {
            wav_rec_status->status = m_wavRecStatus;
        }
        wav_rec_status->gain = g_encControlInfor.InputGain;
        wav_rec_status->avrDelay = g_encControlInfor.AVRDelay;
        wav_rec_status->avrLevel = g_encControlInfor.AVRLevel;
        memcpy(&(wav_rec_status->source.input), &(g_encControlInfor.InputSel), sizeof(input_t));
        memcpy(&(wav_rec_status->total_time.hour), &(g_encStatus.CurTime.TimeHour), sizeof(time_t));
        break;

    case MC_GETAUDIOFMT:
        encode_getAudioFormat(param);
        break;

    default:
        return FALSE;
        //break;
    }//switch(cmd)

    return TRUE;
}

void setRecordBufferClock(void)
{
    //    uint8 sfr_save;
    //    sfr_save = SFR_BANK;
    //    SFR_BANK = BANK_CMU_RMU;
    //    MCSR7 &= 0xE3;
    //    MCSR7 |= 0x08;
    //    SFR_BANK = sfr_save;
}

/*******************************************************************************
 * Function:       encode_initAudioFormat
 * Description:    初始化录音参数
 * Input：
 * Output：
 * Other：         对应 MC_OPEN
 *******************************************************************************/

void encode_initAudioFormat(BYTE music_type)
{
    g_encControlInfor.InputSel = ENC_MIC;

    //music_type = music_type;
    g_encControlInfor.SongType = music_type;
    g_encControlInfor.FsIndex = _FS_8K;
    g_encControlInfor.ChannelNum = SINGLE_CH;//default is 32kbps
    //g_encControlInfor.BitDepth = 16;
    g_encControlInfor.BTIndex = 1;
    g_encControlInfor.EncodeMode = (BYTE) ADPCM;
    g_encControlInfor.InputGain = 5;
    g_encControlInfor.ADCGain = 2;
    g_encControlInfor.DSPGain = 0;
    g_encControlInfor.DelayTime = 15;//20*15 =300MS
    g_encControlInfor.EnergyOut = 0;//disable
    g_encControlInfor.AVRFlag = AVR_TURN_OFF;//disable
    g_encControlInfor.AVRDelay = 3;
    g_encControlInfor.AVRLevel = 700;
    //g_encControlInfor.LeftOffset = reserved;
    //g_encControlInfor.RightOffset = reserved;

    //setRecordBufferClock();
    g_encWriteFilePara.WriteFileBuffAddr = RECORD_BUFFER_ADDRESS;
    g_encWriteFilePara.WriteFileCurSize = RECORD_BUFFER_SIZE;
    g_encWriteFilePara.FileLen = 0;
    //g_encWriteFilePara.WriteFileHandle 在MC_LOAD中初始化

    m_wavRecStatus = RECORDING_WAITING;
    pitch_write_page = 0;
    pitch_write_flag = FALSE;
}

/*******************************************************************************
 * Function:       encode_getNewFileHandle
 * Description:    检查文件是否存在,不存在,创建该文件.
 * Input：
 * Output：
 * Other：         对应 MC_LOAD
 *******************************************************************************/
BOOL encode_getNewFileHandle(void *param)
{
    memcpy(m_wavRecFileName, (char *) param, 11);
    m_wavRecFileName[11] = '\0';

    m_wavRecFileHandle = FS_FOpen(m_wavRecFileName, 0);
    if ((m_wavRecFileHandle != NULL) || (m_wavRecStatus != RECORDING_WAITING))
    {
        m_wavRecStatus = RECORDING_EXCEPTION;
        m_wavRecFileHandle = NULL;
        return FALSE;
    }
    //recording_waiting and m_wavRecFileHandle == null
    m_wavRecFileHandle = FS_FCreate(m_wavRecFileName);
    if (m_wavRecFileHandle == NULL)
    {
        m_wavRecStatus = RECORDING_DISK_ERROR;
        return FALSE;
    }
    g_encWriteFilePara.WriteFileHandle = m_wavRecFileHandle;
    return TRUE;
}

/*******************************************************************************
 * Function:       parse_bit_rate
 * Description:    转换 AP层的 BIT RATE 到 CODE层的BIT RATE
 * Input：
 * Output：
 * Other：         //
 *******************************************************************************/
int8 parse_bit_rate(uint8 *ch)
{
    int tmp;
    tmp = (*(ch) - '0') * 100 + (*(ch + 1) - '0') * 10 + (*(ch + 2) - '0');
    switch (tmp)
    {
    case 64:
        return 0;

    case 128:
        return 1;

    case 256:
        return 2;

    default:
        return -1;
    }
}

/*******************************************************************************
 * Function:       encode_setAudioFormat
 * Description:    设置 CODE 层的  g_encInitPara
 * Input：
 * Output：
 * Other：         对应 MC_SETAUDIOFMT
 *******************************************************************************/
void encode_setAudioFormat(void *param)
{
    audio_format_t *m_audio_fmt;
    int8 result;
    m_audio_fmt = (audio_format_t *) param;

    //#ifndef AK211X_SUPPORT_WAV_ONLY
#if 1
    //int8 result;
    result = parse_bit_rate(m_audio_fmt->bit_rate);
    if(result >= 0)
    {
        //if(g_encControlInfor.SongType==0)
        {
            g_encControlInfor.BTIndex = result;
        }
    }

    g_encControlInfor.FsIndex = m_audio_fmt->sample_rate;
    g_encControlInfor.ChannelNum = m_audio_fmt->channel;

#else
    g_encControlInfor.FsIndex = m_audio_fmt->sample_rate;
    g_encControlInfor.ChannelNum = m_audio_fmt->channel;
#endif
}
/*******************************************************************************
 * Function:       ch_bit_rate
 * Description:    转换 CODE 层的 BIT RATE 到 AP 层的BIT RATE
 * Input：
 * Output：
 * Other：         //
 *******************************************************************************/
void ch_bit_rate(char *tmp)
{
    *tmp = ' ';
    *(tmp + 1) = ' ';
    *(tmp + 3) = 0;
    switch (g_encControlInfor.BTIndex)
    {
    case _BT_8K:
        *(tmp + 2) = '8';
        break;

    case _BT_16K:
        *(tmp + 1) = '1';
        *(tmp + 2) = '6';
        break;
    case _BT_24K:
        *(tmp + 1) = '2';
        *(tmp + 2) = '4';
        break;
    case _BT_32K:
        *(tmp + 1) = '3';
        *(tmp + 2) = '2';
        break;
    case _BT_40K:
        *(tmp + 1) = '4';
        *(tmp + 2) = '0';
        break;
    case _BT_48K:
        *(tmp + 1) = '4';
        *(tmp + 2) = '8';
        break;
    case _BT_56K:
        *(tmp + 1) = '5';
        *(tmp + 2) = '6';
        break;

    case _BT_64K:
        *(tmp + 1) = '6';
        *(tmp + 2) = '4';
        break;

    case _BT_80K:
        *(tmp + 1) = '8';
        *(tmp + 2) = '0';
        break;

    case _BT_96K:
        *(tmp + 1) = '9';
        *(tmp + 2) = '6';
        break;

    case _BT_112K:
        *tmp = '1';
        *(tmp + 1) = '1';
        *(tmp + 2) = '2';
        break;

    case _BT_128K:
        *tmp = '1';
        *(tmp + 1) = '2';
        *(tmp + 2) = '8';
        break;

    case _BT_144K:
        *tmp = '1';
        *(tmp + 1) = '4';
        *(tmp + 2) = '4';
        break;

    case _BT_160K:
        *tmp = '1';
        *(tmp + 1) = '6';
        *(tmp + 2) = '0';
        break;

    case _BT_192K:
        *tmp = '1';
        *(tmp + 1) = '9';
        *(tmp + 2) = '2';
        break;

    case _BT_224K:
        *tmp = '2';
        *(tmp + 1) = '2';
        *(tmp + 2) = '4';
        break;

    case _BT_256K:
        *tmp = '2';
        *(tmp + 1) = '5';
        *(tmp + 2) = '6';
        break;

    case _BT_320K:
        *tmp = '3';
        *(tmp + 1) = '2';
        *(tmp + 2) = '0';
        break;
    default:
        break;
    }
}

/*******************************************************************************
 * Function:       encode_getAudioFormat
 * Description:    获取 CODE 层 audio_format_t
 * Input：
 * Output：
 * Other：         对应 MC_GETAUDIOFMT
 *******************************************************************************/
void encode_getAudioFormat(void *param)
{
    audio_format_t *m_audio_fmt;

    m_audio_fmt = (audio_format_t *) param;
    m_audio_fmt->sample_rate = g_encControlInfor.FsIndex;
    m_audio_fmt->channel = g_encControlInfor.ChannelNum;
    ch_bit_rate(m_audio_fmt->bit_rate); //语法
}
