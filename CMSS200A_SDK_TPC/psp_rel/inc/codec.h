/*
*******************************************************************************
*                ACTOS AP
*        codec decleare file, ap donot use this file, use basal.h
*
*        (c) Copyright, Actions Co,Ld.
*             All Right Reserved
*
*******************************************************************************
*/

#ifndef CODEC_H
#define CODEC_H

//常量定义

//录音文件的格式, riff 的 wFormatTags
#define    WAVE_FORMAT_ACTIONS_PCM        0x81
#define    WAVE_FORMAT_ACTIONS_ADPCM            0x82
#define    WAVE_FORMAT_ACTIONS_AG4                0x83
#define    WAVE_FORMAT_ACTIONS_AG8                0x84

//状态定义
#define    CODEC_STATUS_OK    0        //正常
#define    CODEC_STATUS_ERROR    1        //错误
#define    CODEC_STATUS_READ    2        //codec需要读数据
#define    CODEC_STATUS_WRITE    3        //codec需要写数据
#define    CODEC_STATUS_REACHB    4        //播放到达b点
#define    CODEC_STATUS_MAXLEN    5
#define    CODEC_STATUS_VORON        6
#define    CODEC_STATUS_VOROFF        7
#define    CODEC_STATUS_FORMAT_ERR        0xf0    //文件格式错误

#define    CODEC_STATUS_SETA            0x33    //codec计算好A点的偏移
#define    CODEC_STATUS_SETB            0x77    //codec计算好B点的偏移

#define    CODEC_STATUS_READ_BACKWD         0x22
#define    CODEC_STATUS_READ_FORWD          0x11

#define    CODEC_STATUS_WRITE_TAIL        0x00
#define    CODEC_STATUS_WRITE_HEAD        0xFF

//命令定义
#define    CODEC_CMD_NORMAL        0x00
#define    CODEC_CMD_FF            0x55
#define    CODEC_CMD_RW            0xAA

#define    CODEC_CMD_SETA          0x22
#define    CODEC_CMD_SETB          0xAA
#define    CODEC_CMD_BACKA    0x88

#define CODEC_CMD_PLAY_NORMAL    0x00
#define CODEC_CMD_PLAY_RESTORE    0x01

#ifdef PC
#define __C251__
#endif


#ifdef __C251__
#ifndef PC
#include <typeext.h>
#include <time.h>
#include <basal.h>
#else
#include "typeext.h"
#include "time.h"
#include "basal.h"
#endif

//#define CODEC_TEST

#define PLAY_MODE_NORMAL 0
#define PLAY_MODE_BREAK 1
#define PLAY_MODE_CALTIME 2
#define PLAY_MODE_AB 3

#define FAST_FORWARD 0x55
#define FAST_BACKWARD 0xaa
#define STEP_NORMAL 0x00

#define AB_CLEAR 0x00
#define AB_SET_A 0x22
#define AB_SET_B 0xaa

#define DOUBLE_CHANNEL 0
#define LEFT_CHANNEL 1
#define RIGHT_CHANNEL 2

//数据搬运函数入口指针
typedef struct
{
    WORD read;    //!=NULL, using interrupt mode
    WORD write; //==NULL, using pooling mode
    WORD Fread;
} codec_hook_t;

//ad,ag,wma decoder use
typedef struct
{
    //    WORD ms;
    BYTE second;
    BYTE minute;
    BYTE hour;
} codec_time_t;

typedef struct
{
    BYTE Hms;
    BYTE second;
    BYTE minute;
} decInitTime_t;
typedef struct
{
    WORD    BitsreamBuff1; //comment,1page buffer, 512 bytes/page
    WORD    BitsreamBuff2; //End address of page buffer
    BYTE    SongType; //song type 0:mp3  1:wma  02:adpcm/pcm
    BYTE    PlayMode; //00:normal decode, 01:break point play,02:only compute time info,03:play from begin to end
    long    FileLen; //file length
    WORD    HookRead; //读文件函数
} decInitPara_t;

typedef struct
{
    BYTE            SampleRate; //采样率，kHZ
    BYTE            Channel; //通道数，1：单通道，2：双通道
    DWORD           BitRate;  //比特率，最高byte为0，后3btye依次为，百、十、个位数，单位：kbps，可实时改变
    Full_time_t     TotalPlayTime;  //播放总时间，最高byte为0，后3btye依次为，时、分、秒
    DWORD           FileHDRLen;//文件头长度，单位：btye，由basal提供
} decSongInfor_t;

typedef struct
{
    BYTE            Status;//实时解码状态，0：正常，非0：异常
    BYTE            GetSongInfo; // 0x00:not yet; 0xff: get song info successfully
    BYTE            EnergyLevel;//当前能量级别，0-63级别
    BYTE            SetenceFlag;//当前断句标志，0：无断句，非0：检测到断句
    DWORD           NowTime;//当前播放时间，高byte为0，后3btye依次为，时、分、秒
} decCurStatus_t;

typedef struct
{
    BYTE        SongType; //歌曲类型, 0: mp3, 1: wma, 2: wav
    BYTE        PlayMode; //播放模式， 0: 正常播放，1：断点续播， 2：计算歌曲信息
    BYTE        Volume;     // 当前音量级别，硬音量则跟PA级别一致，软音量则待定
    BYTE        FFRevFlag; // 快进退标志，0：无快进退，0x55：快进，0xaa：快退
    BYTE        FFRevSpeed; // 快进退速度，0-5级别，推荐值为4
    BYTE        ABSetFlag; // AB play flag 0x22:set a  0xaa:set b 0xff:deal AB play process
    BYTE        PlaySpeed; // play speed control
    BYTE        SoftVolumeMax; // 音量调节模式，0：硬音量，非0：软音量的最大级别，软音量启动
    BYTE        EQFlag;//EQ/SRS类型，0：无音效，0x07：自定义EQ，0x4f：自定义SRS，其他值待定
    BYTE        EQVal[11];// EQ/SRS参数，前7个为EQ频带增益参数或SRS模式参数，第8个为音效切换标志，后3个为增益控制（防溢出）
    BYTE        EnergyOutFlag;//能量输出标志，0：不输出，非0：输出
    BYTE        SentDectFlag;//断句检测标志，0：不检测，非0：检测
    BYTE        TimeLimit; // 断句检测时间，0-31级别
    BYTE        EnergyLimit; // 断句检测能量门槛，0-63级别
    BYTE        FadeInTime;//淡入时间，0：不淡入，1: 0.1s， 2：0.2s，3：0.4s，4：0.8s，5：1.6s
    BYTE        FadeOutTime; // 淡入时间，0：不淡入，1: 0.1s， 2：0.2s，3：0.4s，4：0.8s，5：1.6s
    BYTE        ChannelSelect;//通道选择，0：双通道，1：左声道，2：右声道
    BYTE        CntrReserve;//Reserve
} decControlInfor_t;



typedef struct
{
    DWORD       ReadFileHandleHook;//文件读接口，basal提供给codec调用的读数据接口，类型为函数指针 R11, 0x11: 向文件头方向读一页，0x22，向文件尾方向读一页，0x33，读文件中任意位置
    DWORD		ReadFileBuffAddr; //文件数据缓冲区首地址，类型为指针
    DWORD       FileLen;//文件长度，单位：byte
    DWORD       ReadFileCurAddr;//当前页首字节在文件中位置，单位：btye
} decReadFilePara_t;

typedef struct
{
    uint8 TSHDEn;
    uint8 InputMode;
    uint8 OutputMode;
    uint8 LimiterEn;
    uint8 SpeakerSize;
    uint8 TruBassEn;
    uint8 DefinitionLEn;
    uint8 DefinitionREn;
    uint8 FocusLEn;
    uint8 FocusREn;
    uint8 HighBitRate;
    uint8 SRS3DHeadphone;
    uint8 SRS3Dmode;
    uint8 SRS3DEn;
    uint8 SRSTruBassProcessMode;
    uint8 SurroundEn;
    uint8 InputGain;
    uint8 OutputGain;
    uint8 BypassGain;
    uint8 SurroundLevel;
    uint8 LimiterControl;
    uint8 SpaceControl;
    uint8 CenterControl;
    uint8 TruBassControl;
    uint8 DefinitionControl;
    uint8 FOCUSControl;
    uint8 DialogClarityEn;
    uint8 DialogClarityChannelFOCUSControl;
} decEffectInfor_t;

typedef struct
{
    BYTE        SongType;// 保留
    BYTE        FsIndex;//采样率Index，跟76一致
    BYTE        BitDepth;//采样比特数，Reserve
    BYTE        BTIndex;//比特率Index，跟76一致
    BYTE        EncodeMode;//编码模式，Wav录音时, 0: adpcm, 1:pcm
    BYTE        ChannelNum;//通道数，1：单通道，2：双通道
    input_t     InputSel;//输入源，跟76一致
    BYTE        InputGain;//输入源增益，跟76一致，推荐值5
    BYTE        ADCGain;//ADC增益，0-3, 推荐值3
    BYTE        DSPGain;//DSP增益，0-1, 推荐值0
    BYTE        DelayTime;//录音延时设定，单位：20ms
    BYTE        EnergyOut;//能量输出标志，0：不输出，非0：输出
    BYTE        AVRFlag;//静音检测标志，0：不启动，1：声控，2：分曲
    BYTE        AVRDelay;//静音检测时间，0-5秒，推荐值3
    uint16      AVRLevel;//静音检测门槛，推荐值700
    int16       LeftOffset;//左通道DC Offset修正值，Reserve
    int16       RightOffset;//右通道DC Offset修正值，Reserve

} encControlInfor_t;


typedef struct
{
    DWORD        WriteFileHandle;//文件写接口，basal层提供实现，输入参数请使用c_encWriteFileBuffAddr，c_encWriteFileCurSize
    DWORD        FileLen;//已写长度，单位：byte
    WORD        WriteFileBuffAddr;////编码数据buffer地址
    WORD        WriteFileCurSize;//buffer大小，单位：byte
} encWriteFilePara_t;



typedef struct
{
    BYTE        Status;//实时解码状态，0：正常，非0：异常
    BYTE        AVRStatus;//静音检测状态，0：正常，1：声控停止，2：分曲
    WORD        EnergyLevel;//能量级别
    Full_time_t CurTime;//当前录音时间，高byte为0，后3btye依次为，时、分、秒
} encStatus_t;



typedef struct
{
    BYTE  seek_to_head;        //是写数据,还是些文件头.
    char  write_length;    //需要写盘的长度(page)
    void *write_addr;        //需要写盘的地址
} write_param_t;

typedef struct
{
    uchar sec;
    uchar minute;
    uchar hour;
} enc_time_t;
//文件格式状态/控制
typedef struct
{
    BYTE type;            //和riff格式的 wFormatTags 同
    char bits;                //采样点的精度, 16,18 bit
    char sample_rate;        //采样率, 8/12/16/24/32/48
    char channel;            //1(mono)/2(stereo)/3(Joint Stero)/4(Dual channel)
    int bit_rate;            //8/16/24/32/40/48/56/64/80/96/112/128/144/160/192/224/256/320 //kbps, 0 表示可变位率
    enc_time_t total_time;        //歌曲总时间,录音时表示可录音总时长
} audio_status_control_t;



//播放控制
typedef char *buffer_de_t;     //decoder 只需要一个buffer
typedef struct
{
    char volume;            //音量,0~31, 0xff表示 mute
    char ff_rw;            //<0表示rewind;>0表示forward;=0表示正常播放
    char seta_b;            //设置A-B 播放
    char speed;            //-8~8, 0为不变速
    eq_t eq;                // None/ROCK/POP/CLASSIC/ SOFT/ JAZZ/DBB/USER
    buffer_de_t buffer;        //放音使用的buffer, 空间由module层给出
} play_control_t;

//录音状态
typedef struct
{
    char status;            //当前的状态, max length/dsp error/需要写盘vor on/vor off
    codec_time_t current_time;    //实时报告录音时间
    write_param_t  write_param;    //写盘参数
} rec_status_t;

//录音控制
typedef struct
{
    char     *bit_stream;        //编码后的数据
    char        *pcm_stream;        //编码前的数据,即adc的buffer
} buffer_en_t;
typedef struct
{
    uchar gain;            //增益0(-7.5db)/1/…/7(3db)
    uchar vor_time;        // VOR时长. 0(off)/1/2/3…/10s(max)
    resource_t source;        //录音源
    char noise_dec;        //是否需要降噪功能
    char noise_level;
    char agc;                //是否使用agc(自动增益控制)
    buffer_en_t buffer;        //录音使用的buffer, 空间由module层给出
    char VoxPara;
} rec_control_t;

//<><><><><><><><><><><><>><><><><><><><><><>><><><><>><><><>><><>>>><><><><><><><>
//mp3 encoder
//<><><><><><><><><><><><>><><><><><><><><><>><><><><>><><><>><><>>>><><><><><><><>
/*
typedef enum
{
        _FS_8K=0,       //8k sample rate
        _FS_11K025,      //11.025k sample rate
        _FS_12K,        //12k sample rate
        _FS_16K,        //16k sample rate
        _FS_22K05,      //22.05k  sample rate
        _FS_24K,        //24k sample rate
        _FS_32K,        //32k sample rate
        _FS_44K1,       //44.1k sample rate
        _FS_48K,        //48k sample rate
        _FS_96K         //96k sample rate
}samplerate_t;

typedef enum
{
    _BT_8K=0,
    _BT_16K,
    _BT_24K,
    _BT_32K,
    _BT_40K,
    _BT_48K,
    _BT_56K,
    _BT_64K,
    _BT_80K,
    _BT_96K,
    _BT_112K,
    _BT_128K,
    _BT_144K,
    _BT_160K,
    _BT_192K,
    _BT_224K,
    _BT_256K,
    _BT_320K,
    _BT_384K
}bitrate_t;

typedef enum
{
    STEREO=0,
    JOINTSTEREO,
    DUALCHANNEL,
    MONO,
    NOISE
}encmode_t;

typedef enum
{
    SINGLE_CH = 1,
    DUAL_CH   = 2
}channelnum_t;

//typedef enum
//{
   // MICIN=0,
   // LINEIN=1,
   // FMIN=2,
   // PAIN=3,
   // DACIN=4
//}inputainsel_t;

typedef enum
{
   DB_N7P5=0,
   DB_N6P0=1,
   DB_N4P5=2,
   DB_N3P0=3,
   DB_N1P5=4,
   DB_0P0=5,
   DB_1P5=6,
   DB_3P0=7
}gain_t;
*/
typedef   struct
{
    char status;                    //当前的状态, max length/dsp error/需要写盘vor on/vor off
    codec_time_t  current_time;    //实时报告录音时间
} mp3recstatus_t;

typedef struct
{
    byte             VorTime;
    input_t          SourceInput;      //input_t is defined in the basel.h
    samplerate_t     FsIndex;
    bitrate_t        BTIndex;
    encmode_t        EncodeMode;
    channelnum_t     ChannelNum;
    buffer_en_t      Buffer;
    byte             RecMode;
    byte 			AVRLevel;			//avr func add by mzh 2007.1.18
    byte   			AVRDelay;
} mp3recinitpara_t;

#endif
#endif    /* CODEC_H*/



