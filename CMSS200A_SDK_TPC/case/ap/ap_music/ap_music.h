/*
 ******************************************************************************* *                               AS211A
 *                              AS211A
 *                              Module: music ap
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: the head file of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-14 11:00     1.0             build this file
 *******************************************************************************
 */
#ifndef _AP_MUSIC_H
#define _AP_MUSIC_H
#include "ap_common.h"//�ؼ�ͷ�ļ�
#include "mmcmd.h"//ģ�������������
#include "ap_option.h"//AP���ܿ���ѡ��
#include "basal.h"//basal module ͷ�ļ�
#include "uhdrv.h"
#include "ap_report.h"
#include "eqtool.h"

#define CARD_KERNEL 1
//#define USE_CARDBASE

#define RESULT_CURR_UI NULL//�ڵ�ǰ����
#define RESULT_LAST_UI RESULT_USER1+1//����һ����
#define RESULT_UI_STOP RESULT_USER1+2//�� STOP ����
//�ڶ��ַ�ʽ�ص� STOP ����,���ַ�ʽ����ֹͣ����
#define RESULT_UI_STOP2 RESULT_USER1+3
#define RESULT_UI_PLAY  RESULT_USER1+4//�� PLAY ����
#define RESULT_LIST         RESULT_USER1+5  //�б�����Ϣ
#define RESULT_LIST_ERR     RESULT_USER1+6  //�б����ڻ���������
#define RESULT_PLAYLIST_PLAY     RESULT_USER1+7  //�����б��е��ļ���Ϣ
#define RESULT_USERLIST     RESULT_USER1+8

/* ��ʾ��Ϣ��Y���� */
#define MSG_Y_POSITION     24

/*�л������������ַ�ʽ*/
#define Music_Reset_Head 0
#define Music_Reset_End 1
#define Music_Cur   2//ѡ��ǰ
#define Music_Next  3//ѡ��һ��
#define Music_Prev  4//ѡ��һ��
/*֧�ֵ�ѭ��ģʽ��*/
#define REPEATMODE_NUM      5

#define MAX_REPEAT_MODE  4

/*���뵭��ʱ���趨*/
#define FADE_0MS         0
#define FADE_100MS     1
#define FADE_200MS     2
#define FADE_400MS     3
#define FADE_25MS       4
#define FADE_50MS       5

/* ʵ��ʹ�õĵ���͵���ʱ�� */
#define USE_FADEIN_TIME      FADE_100MS
#define USE_FADEOUT_TIME     FADE_100MS

/* �����л���ʼ������ʱ�� */
#define BEGINE_SCROLL_TIME    1

//#define POSITION_LRCBG     (0)//�����ʾ����ͼ����
//#define POSITIONY_LRCBG     (0)
//
//#define POSITION_LRCTOP     (6)//�����ʾ�ı�����
//#define POSITIONY_LRCTOP     90//(93)
//#define POSITION_LRCMID     (0)//�����ʾ�ı�����
//#define POSITIONY_LRCMID    (74)//(57)
//#define POSITION_LRCBOT     (6)//�����ʾ�ı�����
//#define POSITIONY_LRCBOT    20// (21)
#define SOFTVOLUMEMAX     40  //�������������
//#define MENU_GROUP         5  //һ����ʾ�Ĳ˵���

#define MAX_ERR_MUSIC_NUM  30

#define MIC_GAIN_VAL         15
typedef enum
{
    NormalPlay, //��ͨģʽ����ʾ(0)
    WaitSetAPointForAB, //AB�ŵĴ���A��(1)
    WaitSetBPointForAB, //AB�ŵĴ���B��(2)
    ABLoopPlay
    //ABѭ������(3)
} Replay_status_t;

typedef enum
{
    StopSta, //ֹͣ(0)
    PauseSta, //��ͣ(1)
    PlaySta, //��ͨģʽ����ʾ(2)
    FFPlaySta, //���״̬(3)
    FBPlaySta
    //����״̬(4)
} Play_status_t;

typedef enum
{
    LoopNor, //��ͨѭ��
    LoopOne, //ѭ���ŵ�ǰ��
    LoopOneDir, //ѭ���ŵ�ǰĿ¼
    LoopDir, //ѭ����Ŀ¼
    LoopAll, //ѭ������
    RandomMod, //�������
    Intro, //�������
    FolderShift,
    MaxRepeatMod
} Repeat_Mode_t;

typedef enum
{
    Music_NON = 0, Music_MP1, Music_MP2, Music_MP3, Music_WMA,
    //  Music_ASF,
    Music_WMV,
    Music_WAV,
    Music_TypeMax
} Music_type_t;

typedef enum
{
    StudyMod, SingMod, NOR_Play
} Karaoke_status_t;

typedef enum
{
    Manu_Sel, //�ֶ��л���һ��
    Auto_Sel
    //�Զ��л���һ��
} Music_sel_mod_t;

typedef enum
{
    Left_EQ_Set, Right_EQ_Set, All_EQ_Set = 0xff
} EQ_set_mod_t;

//AP_MUSICʹ�õ�ϵͳ����
typedef struct
{
    uint8 Wow;
    uint8 Bass;
    uint8 Srs;
    uint8 Center; //����
    uint8 Definition; //Ƶ��ͻ��
    uint8 Speakersize;
    uint8 Limiter;
    uint8 GainSetting;
    uint8 IsNewFlag;
} SRSMod_t; //SRS WOW ��Ч
typedef struct
{
    int16 TimeMs;
    int16 TimeSecond;
} Lyric_time_t;//add by Rcmai
//AP_MUSICʹ�õ�ϵͳ����
typedef struct
{
    uint16 magic; // +0
    uint8 acc_vol;//����OK ��������
    uint8 karaoke_flag; //����OK���ر�־
    Repeat_Mode_t repeat_mode; //repeat �˵������  //+1
    Open_mod_t MusicOpenMod; //�򿪷�ʽ +6
    breakPT_t BreakPTSave; //�ϵ���� +36
    time_t BreakPTDisTime; //�ϵ�ʱ�䱸�� + //
    uint8 fselmod; //�ļ�ѡ������ģʽ
    uint16 check_sum[2];//�ļ���У��ͣ�ֻ��exfat��Ч
    uint8 cur_disk;//��ǰ����ʹ�õ��̷�
    file_location_t location[2]; //+ 9
    breakPT_t BreakPoit[2];
    uint16 BP_checksum[2];//�ϵ���ϢУ���
} music_vars_t;

typedef struct
{
    //�������ڸ���¼���ͶԱȷ�������ʾԭ����
    uint8 Bitrate_sav[3]; //������
    uint8 TotalTime_sav[3]; //��ʱ��
} TimeRateData_sav_t;

//typedef struct
//{
//    uint16 HeadIcon;//ͷ��ͼ���IDֵ
//    uint16 HeadStr;//ͷ����Ϣ��IDֵ
//    uint8 *string;//��Ϣ��ʾ������ʾ���ִ���ַ
//} dialog_t; //�Ի���

//===========================================================
//���������ⲿ����
//===========================================================
extern music_vars_t g_music_vars;
extern Play_status_t PlayStatus;
extern Music_type_t g_currentMusicType;
extern uint16 g_musicamount;
extern uint16 g_musicsequence;
extern uint8 g_TagInfoBuf[92];
extern Open_param_t g_Openparam;
extern bool Read_VMflag;
extern uint8 disk_char[];
extern uint8 first_init;

extern char report_buf[60 + 4]; //��������ݵ�buffer, 60=TIT1(30)+TPE1(30)+��������֡(4)

extern uint8 g_eq_channal;

extern uint8 FF_flag;
extern uint8 get_fileinfo_flag;
extern uint8 auto_switch_device_flag;//�Զ��л����̱�־
extern uint8 no_device_flag;//�޴��̻�������ļ���־
extern file_location_t CurUselocation;//��ǰ����ʹ�õ�location��Ϣ
extern uint16 dir_no;//��ǰĿ¼���
extern Play_status_t prev_PlayStatus;
extern uint8 scan_flag;//�Ƿ���Ҫɨ��ȫ�̱�־
extern uint8 prev_key_direc;//��¼��һ�������ϻ��������л�����
extern uint8 load_str[5];
extern uint8 err_str[5];
extern uint8 micin_status;//��ǰmic�߲���״̬
extern uint16 err_music_count;
extern uint16 max_music_err;
extern uint8 g_power_flag;//�Ƿ�ͻȻ�ϵ��־
extern EQ_VM_t g_eq_para;

/**********************bank code functions**************************/
//music_main.c
void read_var(void);
void read_VM(void);
void OpenPA(void);
void openDAC(void);
void init_mic_in_gpio(void);

//ui_play.c
uint8 ui_play(void);
void save_dir_info(void);
void get_music_location(uint8 flag);
uint8 fast_switch_music(void);
uint8 fast_switch_dir(void);
uint8 deal_next_prev_msg(uint8 key);
uint8 deal_play_key(void);
void SelMusicByDigital(void);
uint8 deal_mode_msg(void);
uint8 deal_digital_key(uint8 key);
uint8 deal_folder_key(uint8 key);
uint8 deal_key_msg(void);
uint8 get_file_info(void);
uint8 switch_play_status(uint8 report_flag);

//sub_function.c
void volume_callback(uint16 Vol);
void SelectCurMusic(void);
void DisInFBHead(uint8 DisMod);
uint16 DealMusicFileEnd(uint8 DisMod);
void FadeOut(bool DisMod);
void deal_long_keyup(void);
void EQMode_callback(EQ_set_mod_t mode);
uint8 deal_eq_msg(uint8 key);
void deal_mute_msg(void);
uint8 deal_repeat_msg(uint8 key);
//uint8 deal_vol_msg(uint8 key);
void reinit_file_info(void);
uint8 deal_device_msg(uint8 key);
uint8 deal_folder_longmsg(uint8 key);
uint8 deal_next_prev_longmsg(uint8 key);

//file_selector.c
uint8 cmp_location(void);
void read_breakpoint(void);
uint16 InitFileSelector_sub(char disk);
uint16 InitFileSelector(void);
uint16 StartFileSelector(void);
uint16 UpdateFileSelector(void);
uint8 deal_playing_err(void);
void write_poweroff_flag(uint8 flag);

//get_musicinfo.c
uint8 deal_change_music_msg(uint8 key);
uint8 ChangeMusic(uint8 Direct, Music_sel_mod_t mod);
uint8 get_next_prev_music(uint8 direct, Music_sel_mod_t mod);
Music_type_t near CheckMusicType(uint8 *filename);
void GetInformation(void);
void DealTagDataBuffer(void);

//sub_display.c
void show_disk_icon(uint8 on_off);
void show_status_icon(uint8 on_off);
void show_col_icon(uint8 on_off);
void show_play_ui(void);
void show_fileno_ui(void);
void show_dirno_ui(void);
void show_volume_ui(void);
void show_eq_ui(void);
void show_repeat_ui(void);
void show_digital_ui(void);
void RefreshMainScreen(void);

//sub_caraoke.c
void OpenMicRoute(int16 gain);
void CloseMicRoute(void);
void acc_vol_callback(int16 vol);
void SpeechMute(uint16 flag);
void setDECGain(uint8 DecGain);
void SetCaraokeEqMode(uint16 eqmode);
void SetVol(int8 volmode);
void CaraokeEnable(int8 volmode);
void CaraokeDisable(void);
void deal_caraok_msg(void);
void deal_mic_msg(void);

//ui_clock.c
void ShowTimer(void);
void show_time_setting(void);
void show_clock_ui(void);
uint8 set_clock_loop(void);
uint8 set_clock_ui(uint8 auto_return);

/*********************rcode functions************************************/
//sub_init.c
void stop_music_play(uint8 bakup_flag);
void ShowNowTime(void);
void time_writetoreg(void);
void BackupCurrentTime(void);
uint8 RealDisplay(void);
uint8 deal_play_status(void);
uint8 deal_music_filehead(void);

//music_command.c
bool musicpSendCommand_bank(uint8 cmd, void *param);
bool musicpSendCommand(uint8 cmd, void *param);
void ClearTimePara(void);
void close_usb_phyctrol(void);

//comval function
extern void sCaraokeEnable(uint8 sample_rate, uint8 flag);
extern void sCaraokeDisable(void);

/*********************TTS functions************************************/

#if defined(MUSIC_AP)
//ap_report_sub.c
uint8 deal_report_result(uint8 retval);
uint8 report_disk_info(void);
uint8 deal_play_report(void);
uint8 deal_clock_report(void);
//deal_music_report.c
int report_music_tag(char *report_buf, char cur_disk);
BOOL check_MP3_id3(unsigned char *buf);
int convert_music_id3_string(char *report_buf, char type);
int convert_music_name_string(char *report_buf);
#endif

#endif     //_ap_music_H
