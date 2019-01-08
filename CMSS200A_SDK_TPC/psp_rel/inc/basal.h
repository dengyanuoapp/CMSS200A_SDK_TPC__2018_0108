#ifndef _BASAL_H
#define _BASAL_H

#ifdef __C251__
#include <typeext.h>
#include <mmcmd.h>
#include <time.h>
#include <midware_BS_EH.h>

#define TYPE_MP3    0
#define TYPE_WMA    1
#define TYPE_PCM    2

#define PLAYING_STOP            0x00
#define PLAYING_PLAYING         0x01
#define PLAYING_PAUSE           0x02
#define PLAYING_AB              0x03
#define PLAYING_ABPAUSE         0x04
#define PLAYING_SEARCHING       0x05
#define PLAYING_REACH_END    0x06
#define PLAYING_REACH_A        0x07
#define PLAYING_REACH_B        0x08
#define PLAYING_SEACH_FAIL    0x09
#define PLAYING_SEACH_SUCC    0x0A
#define PLAYING_WAIT_A     0x0B
#define PLAYING_WAIT_B    0x0D

#define PLAYING_ERROR        0x0C
#define PLAYING_REACH_HEAD   0x0E
#define PLAYING_DRMSONG      0x0f

#define RECORDING_STOP          0x00
#define RECORDING_RECORDING     0x01
#define RECORDING_PAUSE         0x02
#define RECORDING_WAITING       0x03
#define RECORDING_EXCEPTION     0x04
#define RECORDING_OUT_SPACE    0x05
#define RECORDING_DISK_ERROR    0x06
#define RECORDING_VOR_RECORDING    0x07
#define RECORDING_VOR_WAITING    0x08

#define SIGNAL_NON 0x00
#define SIGNAL_REACH_V 0x01
#define SIGNAL_REACH_B 0x02
//#define SIGNAL_CLEAR_B 0x03


#define FADENON_DEF 0
#define FADEIN_DEF  10
#define FADEOUT_DEF 20
#define FADEOUT2_DEF 30

#define BANK_ENABLE  0x01
#define BANK_DISABLE 0xfe

#define _GPIO_TEST

#define READ_SFR_CODE      0xFF0102
#define WRITE_SFR_CODE     0xFF0100


#define SFR_RTCRDM0                                     0xbc      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM1                                     0xbd      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM2                                     0xbf      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM3                                     0xc1      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM4                                     0xc2      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM5                                     0xc3      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM6                                     0xc4      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM7                                     0xc5      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM8                                     0xc6      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM9                                     0xc7      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM10                                    0xc8      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM11                                    0xc9      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM12                                    0xca      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM13                                    0xcb      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM14                                    0xcc      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM15                                    0xcd      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM16                                    0xce      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM17                                    0xcf      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM18                                    0xd2      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM19                                    0xd3      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM20                                    0xd4      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM21                                    0xd5      // 0x0c 	RTC Random access Register
#define SFR_RTCRDM22                                    0xd6      // 0x0c 	RTC Random access Register

typedef void (*WriteSFR)(uint8 sfr_bank, uint8 sfr_reg, uint8 value);
typedef uint8 (*ReadSFR)(uint8 sfr_bank, uint8 sfr_reg);

typedef struct
{
    char mute: 1;
    char unmute: 1;
    char fm: 1;
    char linein: 1;
    char mic: 1;
} mix_t;

/*
 typedef struct
 {
 char mic:1;
 char fm:1;
 char linein:1;
 char dac:1;
 char reserved:4;
 }input_t;
 */
typedef uint8 input_t;
#define ENC_MIC 0x00
#define ENC_FM 0x01
#define ENC_LINEIN 0x01
#define ENC_MIX 0x03
typedef struct
{
    input_t input;
    BOOL mix;
} resource_t;

typedef struct
{
    BYTE MS;
    BYTE TimeHour;
    BYTE TimeMinute;
    BYTE TimeSecond;
} Full_time_t;

typedef struct
{
    uchar status;
    uchar gain;
    uchar avrDelay;
    uint16 avrLevel;
    resource_t source;
    time_t total_time;

} mp3_rec_status_t;
typedef mp3_rec_status_t wav_rec_status_t;

typedef enum
{
    none,
    rock,
    pop,
    classic,
    soft,
    jazz,
    dbb,
    //SRS,
    user,
    eq_max
} eq_t;

typedef struct
{
    uchar status;//解码状态，0正常，非0异常
    uchar signal; //信号量，0一般状态，2到达B点，3清楚B点
    uchar volume;//音量
    time_t total_time;//歌曲总时间
    eq_t eq;//当前EQ
    signed char speed;//AK211X硬件不支持，保留
} mp3_play_status_t;

typedef mp3_play_status_t wma_play_status_t;
typedef mp3_play_status_t music_play_status_t;
typedef mp3_play_status_t ogg_play_status_t;
typedef mp3_play_status_t ape_play_status_t;
typedef mp3_play_status_t play_status_t;

typedef struct
{
    BYTE switchflag;//开关如下各项(为1的位表示对应项打开，为0时表示关闭)
    int RemainPlayCnt;//余下可播放的次数
    int originalPlayCnt;//原始可播放次数

    date_t BeginDate;//开始播放日期
    time_t BeginTime;

    date_t ExpDate;//截止播放日期(2004-12-30)
    time_t ExpTime;

    int RemainTime;//(第一次用过之后还能用的小时数，对同一首歌是固定值)
    int RemainStoreTime;//余下可存储的时长    (天数)//现在保留此接口，但暂时不加
    char DealOnClkBck;//0: 时钟回调时删除 1：时钟回调失效
} licensinf_t;

typedef enum
{
    INF_NORMALWMA, //普通的WMA [0]
    INF_DRMHVLICENSE, //是DRM有license [1]
    INF_DRMNOLICENSE //是DRM但无license [2]
} drm_mod_e;

typedef struct
{
    drm_mod_e drmmode;
    licensinf_t drminf;
} wma_inf_t;

typedef enum
{
    Nor_open, //从头开始放
    Nor_Break_open, //从上次断点个开始放
    ABPlay_open, //回复到上次的AB播放
    Hard_Break_open //硬件断点续播
} Open_mod_t;

typedef enum
{
    ClrABPoint, //清除AB点,回到普通播放
    SetAPoint, //设定A点
    SetBPoint, //设定B点
    ReturnApoint, //间隔完回到A点
    StopBPlay, //停止B点播放
    ABFinished //AB次数完
} SetABMode_t;

typedef struct
{
    uint32 BreakPointAddr; // 断点对应的文件位置，单位：btye
    uint32 PacketCnt; // 断点对应的Packet数目
    uint32 PacketSize; // 断点对应的Packet大小
    uint8 SynHdrData1;//断点对应的同步信息1
    uint8 SynHdrData2;//断点对应的同步信息2
    uint16 CurBitrate; // 断点对应的比特率
} decBreakPointInfor_t;

typedef struct
{
    uint32 breakPT;//断点位置
    uint32 ApointSave;//A点位置
    uint32 BpointSave;//B点位置
    decBreakPointInfor_t breakPointInfor;
    //BYTE breakPTTime[15];//MP3断点时间
} breakPT_t;

typedef struct
{
    uint8 *filename; //文件名地址
    breakPT_t *BreakPTSave; //断点参数地址
    BYTE typeerror; //异常文件格式 0： 正常文件；1：异常文件，主要用于异常文件的预先判断
    uint32 musicframeoffset; //mp3文件第一帧起始位置
    time_t musictotaltime; //mp3总时间
    uint8 FadeInTime; //设置淡入时间，0为不支持淡入
    uint8 SoftVolumeMax; //设置数字音量最大级数，0为采用硬件音量
    uint8 current_samplerate;             //当前歌曲的采样率信息, khz, 卡拉ok用, 单独发命令获取
} Open_param_t;

typedef struct
{
    uint8 bits;//采样比特数，保留 //char
    uint8 sample_rate;//采样率索引   //char
    uint8 channel;//通道数  //char
    uint8 bit_rate[4];//比特率  //char
} audio_format_t;

typedef struct
{
    uchar state;
    uchar volume;
    uchar vor_time;
    time_t total_time;
    signed char speed;
} wav_play_status_t;

typedef struct
{
    uchar status; // (1:playing ; 2:pause ; 3(searching).
    uint freq; //当前电台的频率。单位为1k (最高位恒为1)
    uchar station; //当前电台对应的电台表项号。
    uchar station_count; //电台表中的保存电台数。
    uchar mode; //0 (US-Europe) /1 (Japan) ;?
    uchar volume; //音量。
    uchar stereo_status;
    uchar vol_display;
} fm_play_status_t;

typedef struct
{
    bool existflag;
    BYTE playmode;
    int apeMusicCurNum;
    int apeMusicMaxNum;
    time_t curPartStartTime;
    time_t curPartEndTime;
    time_t curTime;
    char curMusicName[31];
    char curArtistName[31];
} cuefileinfo;

typedef enum
{
    WRITE_WRONG = -1,
    WRITE_NONE = 0,
    WRITE_ONE = 1,
    WRITE_HEAD = 8
} retWavrProceed_e;

/*
 typedef enum
 {
 _FS_8K=0,       //8k sample rate
 _FS_32K=6        //32k sample rate
 }samplerate_t;

 typedef enum
 {
 _BT_8K=0,
 _BT_32K=3,
 _BT_64K=7,
 _BT_128K=11,
 _BT_256K=16
 }bitrate_t;

 typedef enum
 {
 ENCODE_WAV_ADPCM=0,
 ENCODE_WAV_PCM
 }encmode_t;

 typedef enum
 {
 SINGLE_CH = 1,
 DUAL_CH   = 2
 }channelnum_t;

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
typedef enum
{
    _FS_8K = 0, //8k sample rate
    _FS_11K025, //11.025k sample rate
    _FS_12K, //12k sample rate
    _FS_16K, //16k sample rate
    _FS_22K05, //22.05k  sample rate
    _FS_24K, //24k sample rate
    _FS_32K, //32k sample rate
    _FS_44K1, //44.1k sample rate
    _FS_48K, //48k sample rate
    _FS_96K //96k sample rate
} samplerate_t;

typedef enum
{
    _BT_8K = 0,
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
} bitrate_t;

typedef enum
{
    STEREO = 0,
    JOINTSTEREO,
    DUALCHANNEL,
    MONO,
    NOISE
} encmode_t;

typedef enum
{
    ENCODE_WAV_ADPCM = 0,
    ENCODE_WAV_PCM
} wav_encmode_t;

typedef enum
{
    SINGLE_CH = 1,
    DUAL_CH = 2
} channelnum_t;

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
    DB_N7P5 = 0,
    DB_N6P0 = 1,
    DB_N4P5 = 2,
    DB_N3P0 = 3,
    DB_N1P5 = 4,
    DB_0P0 = 5,
    DB_1P5 = 6,
    DB_3P0 = 7
} gain_t;
BOOL apSendCommand(BYTE cmd, BYTE music_type, void *param);
BOOL apSDSendCommand(BYTE cmd, BYTE music_type, void *param);

#define mp3SDSendCommand(cmd,param) apSDSendCommand((cmd),TYPE_MP3,(void *)(param))
#define mp3pSendCommand(cmd,param) apSendCommand((cmd),TYPE_MP3,(void *)(param))
#define wmapSendCommand(cmd,param) apSendCommand((cmd),TYPE_WMA,(void *)(param))
#define wavpSendCommand(cmd,param) apSendCommand((cmd),TYPE_PCM,(void *)(param))

retWavrProceed_e rProceed(void);
BOOL rSendCommand(BYTE cmd, BYTE music_type, void *param);
#define mp3rSendCommand(cmd,param) rSendCommand((cmd),TYPE_MP3,(param))
#define mp3rProceed rProceed
#define wavrSendCommand(cmd,param) rSendCommand((cmd),TYPE_PCM,(param))
#define wavrProceed rProceed
#define wmarSendCommand(cmd,param) rSendCommand((cmd),TYPE_WMA,(param))
#define wmarProceed rProceed
#define recordSendCommand(cmd,param) rSendCommand((cmd),TYPE_PCM,(param))
#define recordrProceed rProceed

#endif
#endif

