/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main header file.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_record.h,v 1.5 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */

#ifndef AP_RECORD_H
#define AP_RECORD_H

#include "actos.h"
#include "ap_common.h"
#include "enhanced.h"
#include "basal.h"
#include "mmcmd.h"//ģ�������������
//#define	 TEST_HOST	1

//message
#define  RESULT_USER_STOP        (RESULT_USER1+1)    //�û���������
#define  RESULT_LOW_POWER_STOP   (RESULT_USER1+2)    //�͵�ѹ����
#define  RESULT_DIR_CHANGED      (RESULT_USER1+3)
#define  RESULT_TRACKED          (RESULT_USER1+5)    //�����˳����
//event
#define  AP_EVENT_CODEC_ERR    AP_EVENT_9    //codec ����
//main(int param) ������ڲ�����ֵ
#define  NON_FMREC      0                     //��FM¼��,��ʱ¼��Դ��g_mrecord_vars.rec_source����
#define  MICREC_START   1                   //RESULT_RECORD         //��FM������,��REC��ֱ�ӽ���MIC¼��.
#define  FMREC_START    RESULT_FMREC_START    //FM¼��.FM����������̨ʱ���ȼ�REC���е�FM¼��
#define  FMREC_NOSTART  RESULT_FMREC_NOSTART  //FM¼��.FM����������̨ʱѡ��"��̨¼��"�˵����е�FM¼��
#define  FMREC_AUSTART  RESULT_AUTORECORD_START

//¼��Դ��������(ȫ�ֱ���:g_rec_from���õ�!)
#define  R_SPDIF          0x80
#define  R_FM             0x40
#define  R_LINEIN         0x20
#define  R_MIC            0x10

//defines
#define SPACE_LOW           150L          // exFAT Support //60L    //ʣ��������
#define LOW_POWER_COUNT     4      //ֹͣ¼��ʱ�͵�ѹ�����Ĵ���
//#define FPGA_DEBUG_ADC


/* ¼����ʽö��*/
typedef enum
{
    RECORD_FILE_TYPE_WAV = 0, RECORD_FILE_TYPE_MP2
} ap_rectype_e;

/* ¼��������ö��*/
typedef enum
{
    AP_FS_8K = 0, AP_FS_12K, AP_FS_32K, AP_FS_48K
} ap_samplerate_e;

/* ¼��������ö��*/
typedef enum
{
    AP_BT_32K = 0, AP_BT_64K, AP_BT_128K, AP_BT_192K, AP_BT_256K, AP_BT_384K
    /* FPGA �׶�, ADPCM ˫����Ϊ384KBPS */
} ap_bitrate_e;

/* ¼��Դö��*/
typedef enum
{
    AP_MICIN = 0, AP_LINEIN, AP_FMIN
} ap_inputSelect_e;

/* ¼��VRAM ���������ݽṹ*/
typedef struct
{
    uint16 maigc; //�����ж�vm��������Ƿ���Ч
    file_location_t location;
    uint16 wav_num; //��ǰ¼���ļ��ı��
    uint16 mp2_num; //��ǰ¼��mp2�ļ����

    /* ����¼��ģʽ,  ¼����ʽ�Լ�wav ���뷽ʽȷ��FS,  Bitrate ��channel num */
    uint8 rec_mode; //0:  ��ʱ¼��  1:  ��ͨ¼��  2:  ����¼��
    wav_encmode_t rec_encode; //wav ��ʽ���뷽ʽ,  ADPCM  or PCM
    ap_rectype_e rec_type; //¼����ʽ
    ap_inputSelect_e rec_source; //��ǰ���õ�¼��Դ
    //uint8 channelnum;
    //uint8 Rec_Fsrate; //������
    //uint8 rec_bitrate; //0/1/2/3/4/5:32/48/64/96/128/192bps.(ͨ��BitRate_Tab[]ת����module��Ľӿڲ���)

    uint8 rec_InputGain;//5
    uint8 rec_ADCGain;//3
    uint8 rec_DSPGain;//0

    uint8 rec_DelayTime;//15 * 20ms per
    //uint8 rec_AVREnable;//1 voice control ,0 turn off
    //uint8 rec_AVRDelay;//3
    //uint16 rec_AVRLevel;//700
} record_vars_t;

typedef struct
{
    uint16 hour;
    uint8 minute;
    uint8 second;
} time_t_ext;

typedef enum
{
    recording, wait_to_save, saving
} status_t;
typedef enum
{
    rec_stop, rec_start, rec_pause
} rec_status_t;

typedef enum
{
    RecStop_UI, RecOn_UI, RecPause_UI
} UI_Type_rec_t;

typedef uint16 (*MenuCallBack)(void);

//global
extern record_vars_t g_record_vars;
//extern fmrecord_vars_t fm_record_vars;
extern audio_format_t Rec_format;
extern wav_rec_status_t rec_stat; //¼��״̬
extern uint32 total_sec;
extern bool isFmRecord;
extern uint8 avrEnableSave;
extern ap_inputSelect_e inputSelSave;

extern uint8 g_file_num; //¼���ļ��ܸ���
//extern uint8 g_total_time[];          //00:00:00
extern uint8 g_now_time[]; //00:00:00
extern time_t_ext g_total_time; //��ʱ����ʾ
extern time_t g_rec_time; //ʵʱ��ʾ¼����ʱ��
extern uint8 g_file_name_time[16]; //"WAV01 2004/01/01"

extern uint32 g_free_space; //ʣ����̿ռ�
extern uint32 g_file_size; //��ǰ¼���ļ���page��
extern uint16 g_rec_event;

extern resource_t inputSelect;

//extern const char BitRate_Tab[];
extern uint8 currentBitrate[4];

//extern char g_noise_level;      //for vox recording
//extern char FMAutoRecord_flag;
extern BOOL show_time;
//extern BOOL	unicodeFlagofFolder;

//extern const char rec_Fsrate[4]; //= {8,8,32,32};
//extern const char rec_FsrateCodec_WAV[4]; //= {_FS_8K,_FS_8K/*,_FS_12K,_FS_16K,_FS_24K,_FS_32K*/,_FS_32K,_FS_32K};
//extern const uint8 voice_ext[];
extern uint8 g_disk_char[];

extern const uint8 rec_source_tab[];//{ R_LINEIN, R_SPDIF, R_MIC };

extern const uint16 stop_string[];// = {RSTOP1, RECORDFILETYPE,RECORDSETTING, CHANGEVO};
extern const uint16 stop_string_card[];// = {RSTOP1, MSTOP2, RECORDFILETYPE, RECORDSETTING,CHANGEVO};

extern const uint8 rec_bitrate_sector_ratio[];
/* ¼��������*/
extern ap_bitrate_e rec_bitrate;
/* ¼��������*/
extern ap_samplerate_e Rec_Fsrate;
/* ¼��ͨ����*/
extern channelnum_t rec_channel;

//extern uint8 dir_name[30];

extern bool need_fetch_space;

extern bool card_exist;
extern bool UDisk_exist;

/* �����õ�¼������: ����Ϊ��λ */
extern DLL_IMP uint32 RecSpare;
/* ���õĿ�¼������ */
extern uint32 total_space_setting;

/* ���ν���¼�����ۼ���¼������ */
extern uint32 total_space_used;

/* �����ļ��������� */
extern uint32 onefile_space;
extern uint8 rec_status;
extern uint8 need_change_rec_status;
extern uint8 review_disk;
/* ADPCM ����ģʽ�±���������*/
extern const ap_bitrate_e bitrate_ap_adpcm[3];

/* PCM ����ģʽ�±���������*/
extern const ap_bitrate_e bitrate_ap_pcm[3];

/* MP2 ����ģʽ�±���������*/
extern const ap_bitrate_e bitrate_ap_mp2[3];
extern const uint8 *g_dir_voice;
extern bool overflag;
uint8 rec_init(void);
BOOL CheckStoreOK(void);
uint8 deal_mode_msg(void);
uint8 StartMP2Record(void);
uint8 StartWAVRecord(void);

uint16 show_stop(BOOL auto_start);
uint16 show_pause(void);
uint16 do_rec(void);
void stop_rec(uint8 get_status);

//extern void make_dirname(void);
extern void get_rec_source(void);
extern void parse_total_time(void);
extern void parse_total_num(void);
//extern void Open_RecRoute(void);

extern BOOL reset_record_dir(bool need_init);

//extern uint16 Show_AVRLevel_Menu(void);
//extern uint16 Show_AVRDelay_Menu(void);
extern void confirm_rec_param(void);

extern bool recSendCommand(BYTE cmd, void *param);

/* main.c */
uint16 show_error(uint16 msg_id);
uint16 show_msg(uint16 msg_id);
BOOL SetCard();
BOOL SetUDisk();

/* show_rec.c */
void ui_draw(void);
uint8 time_draw(uint8 mode);
BOOL is_lowbatt(uint8 batt);
uint8 show_rec(void);
uint8 show_err_msg(void);
uint8 deal_clock_report(void);
void enable_adda(void);
void disable_adda(void);

extern void decAddainit(adin_t ain);
extern void decAddaexit(void);
#endif  /* AP_RECORD_H*/
