/*
 ********************************************************************************
 *                       ACTOS
 *                  ap_radio head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : ap_radio.h
 * By     : mike
 * Version: 1> v1.00     first version     2004-1-12 13:45
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _APRADIO_H
#define _APRADIO_H
#include "actos.h"
#include "ap_common.h"
#include "filesys.h"
#include "basal.h"
#include "stringH.h"
//#include "fmradio.h"
#include "fm_drv.h"
#include "ap_report.h"
#include "eqtool.h"

//Ӳ���������
#define  MAXVOLUME		    40
//Ĭ����̨���޶���
#define  SEARCHLEVEL        4
//��̨�б���֧�ֵ���󱣴���
#define  MAX_STATION_COUNT  40

/* ������Ƶ�ʷ�Χ����,��100khzΪ��λ */
#define  USFREQ_MAX         0x0438               //108000khz
#define  USFREQ_MIN         0x036B               //87500khz
#define  JPFREQ_MAX         0x0384               //90000khz
#define  JPFREQ_MIN         0x02F8               //76000khz
/* ���е�̨�б�Ƶ�ʷ�Χ( �й� ) */
#define  CITYFREQ_MAX       0x0438             //108000khz
#define  CITYFREQ_MIN       0x036B            //87500khz          0x036B = 875
typedef enum
{
    FM_TO_RADIO = 0, //  0, ����FM,���е�̨����
    ALARM_TO_FM, //  1, Alarm����FM
    FM_TO_STANDBY,
    //  2, ʹFM����Standby
} fm_param_e;

typedef enum
{
    SEARCH_NULL = 0, //������
    SEARCH_NEXT = 10, //next��������
    SEARCH_LAST = 20, //last��������
    SEARCH_STOP = 50,
    //������ֹ
} FM_search_direct_e; //�ֶ���̨(Դ�������ô�ֵ��Ŀǰ��֪�����ã��ȱ���)


/* RDS ���ܿ���ö�� */
typedef enum
{
    RDS_Close, //RDS ��
    RDS_Open
    //RDS ��
} radio_RDS_e;

typedef struct
{
    uint16 FM_CurrentFreq; //��ǰƵ��
    FM_Audio_e FM_Stereo_Status; //0������������0��������
} FMStatus_Tab_t;

/* radio ui���ò��� */
typedef struct
{
    /* ħ����*/
    uint16 magic;
    /* Ԥ���б�( ��ͨƵ��)*/
    uint16 fmstation_us[MAX_STATION_COUNT];
    /* Ԥ���б�( �ձ�Ƶ��)*/
    uint16 fmstation_jp[MAX_STATION_COUNT];
    /* ��ǰ���ŵĵ�̨��Ϣ*/
    fm_status_t FMStatus;
    /* ��ǰ���Ų���, Band_MAX ��ʾ��ǰ����Ϊ���е�̨*/
    radio_band_e band_mode;
    /* �Ƿ�ʹ��RDS���ܿ��� */
    radio_RDS_e RDS_fun;
} radio_config_t;

/* radio ���е�̨�ṹ */
typedef struct
{
    /* ��ǰ���е�̨Ƶ��*/
    uint16 fmstation_city;
    /* ��ǰ���ŵĳ��е�̨���� */
    char station_name[40];
} radio_citylist_t;

/* FM�˵���	 */
typedef enum
{
    MENU_SAVE, /* �����̨*/
    MENU_RECORD, /* ��̨¼��*/
    MENU_DELETE, /* ɾ����̨*/
    MENU_DELETE_ALL, /* ɾ��ȫ��*/
    MENU_AUTO_SEARCH, /* �Զ���̨*/
    MENU_NORMAL_BAND, /* ��ͨƵ��*/
    MENU_JAPAN_BAND, /* �ձ�Ƶ��*/
    MENU_RDS, /* RDS ����*/

    MENU_FM_RECORD, //for FM Record by ccm

    MENU_EXIT, /* �˳�*/

    MENU_COUNT,
    //FM�˵�����Ŀ
} radio_menu_e;

/* �˵�����ָ������ */
typedef uint16 (*pfunc_menu)(uint16 *);

/* �˵�����ָ����ɵĽṹ�� */
typedef struct
{
    pfunc_menu menu_save; /* �����̨*/
    pfunc_menu menu_record; /* ��̨¼��*/
    pfunc_menu menu_delete; /* ɾ����̨*/
    pfunc_menu menu_delete_all; /* ɾ��ȫ��*/
    pfunc_menu menu_auto_search; /* �Զ���̨*/
    pfunc_menu menu_normal_band; /* ��ͨƵ��*/
    pfunc_menu menu_japan_band; /* �ձ�Ƶ��*/
    pfunc_menu menu_rds; /* RDS ����*/
    pfunc_menu menu_fm_record; //for FM Record by ccm
    pfunc_menu menu_exit; /* �˳�*/
} radio_menufunc_t;

/* FM�����ָ������ */
typedef uint8 (*pfunc_cmd)(void *, void *);

/* FM����ѡ�� */
typedef enum
{
    /* FM �м�������, �����漰������FM�����ӿ� */
    FM_OPEN = 0, //��װfm����, ����ģ���ʼ��
    FM_CLOSE, //ģ�����standby, ��ж��fm����
    FM_HARDWARE, //FM Ӳ���������
    FM_SETFREQ, //���õ���Ƶ�㲥��
    FM_MUTE, //FM������������
    FM_SETBAND, //���ò���
    FM_SETTH, //������̨����
    FM_SEARCH, //���������̨
    FM_SEEK, //����seek (Ӳ����̨)
    FM_GETSEEKFLAG, //��ȡseek״̬
    FM_BREAKSEEK, //�˳�seek

    FM_GETFREQ, //��ȡ��ǰƵ��ֵ
    FM_GETINTENSITY, //��ȡ��ǰ��̨�ź�ǿ��
    FM_GETSTATUS, //��ȡ��ǰ��̨״̬

    //�м��߼���:��FM�����޹�
    FM_MID_PLAY, //���ŵ�̨
    FM_MID_SAVE, //���浱ǰ��̨
    FM_MID_SAVESTATION, //���浱ǰ��̨��Ӧ�ĵ�̨����š�
    FM_MID_FINDSTATION, //���ҵ�ǰ��̨��Ӧ�ĵ�̨����š�
    FM_MID_LOADFREQ, //���ݵ�̨����ż���Ƶ�ʡ�
    FM_MID_RESTORE, //��ԭ��ǰ��̨
    FM_MID_TURN, //��̨
    FM_MID_DELETE, //ɾ����ǰ��̨
    FM_MID_DELALL, //ɾ��ȫ����̨
    FM_MID_SELCITYINIT, //ȷ����ѡ���еĵ�һ�����е�̨
    FM_MID_CITYSWITCH, //����ѡ�����������л����е�̨

    FM_MID_SETVOLUME, //��������
    FM_MID_STEPGRID, //��̨Ƶ��΢������100KHZΪ����

    CMD_COUNT,
    //FM������Ŀ
} radio_cmd_e;

/* Fm �ĳ�������ָ����� �ṹ�� */
typedef struct
{
    uint16 *param1; //��̨���ַָ�룬20����USģʽ��/20����Japanģʽ��
    uint16 *param2; //��ǰ��̨��Ϣ�ĵ�ַָ��
} FMRADIO_param_t;

/* �������� */
//main.c
extern radio_config_t g_radio_config; //radio ui ����������Ϣ
extern uint8 EnterFlag; //Ӧ�ý���ģʽ. 0xff--ֻ��ΪʹFMģ�����standby
extern uint8 AutoTabNum; //�Զ���̨������,��ǰ���ѵ�̨�ĵ�̨��
extern fm_status_t g_FMStatus; //��̨�����л�ȡ��״̬
extern uint8 seekflag; //Ӳ����̨���
extern radio_citylist_t g_city_station; //radio ���е�̨�б�ṹ��Ϣ
extern uint16 *pfreq_play; //ָ��ǰ�����ŵĵ�̨�б����
extern radio_RDS_e use_RDS; //RDS�رմ�״̬
extern fm_status_t FMStatusUS; //US��̨��Ϣ�ݴ�,8 bytes
extern fm_status_t FMStatusJP; //Japan��̨��Ϣ�ݴ�,8bytes
extern FMRADIO_param_t radio_param; //��ǰ��̨��Ϣ
extern uint8 flag_r; //��̨�Ƿ��ѵ���һ��̨֮�󷵻ر�־λ

//��VRAM�ж�����EQ,SRS������ͨ��EQ��������
extern EQ_VM_t g_eq_para;

extern int8 AuCodecRun;
extern int8 AuCodecEffect;
extern int16 SentenceDectTime;
extern uint8 SentenceDectEnergy;
extern int8 SetenceFlag;
extern uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
extern uint32 m_decSpdifddrSav;

extern int8 last_SetenceFlag;

#ifdef AP_RADIO_REPORT_EN
extern char report_buf[16]; //��������ݵ�buffer
#endif

//basal_fmradio.c
extern uint16 m_FMStationTab[MAX_STATION_COUNT];
extern void charge_pump_onoff(uint8 flag);

/* main.c */
uint16 FMinit(uint16 param);
void read_var(void);
void FMexit(uint16 result);
uint16 drawBandSwitch(void);

/* sub function */
extern uint16 fm_menu_save(uint16 *arg);  /* �����̨*/
extern uint16 fm_menu_record(uint16 *arg);  /* ��̨¼��*/
extern uint16 fm_menu_delete(uint16 *arg);  /* ɾ����̨*/
extern uint16 fm_menu_delete_all(uint16 *arg);  /* ɾ��ȫ��*/
extern uint16 fm_menu_auto_search(uint16 *arg);  /* �Զ���̨*/
extern uint16 fm_menu_normal_band(uint16 *arg);  /* ��ͨƵ��*/
extern uint16 fm_menu_japan_band(uint16 *arg);  /* �ձ�Ƶ��*/
extern uint16 fm_menu_rds(uint16 *arg);  /* RDS ����*/
extern uint16 fm_menu_fm_record(uint16 *arg);  //for FM Record by ccm
extern uint16 fm_menu_exit(uint16 *arg);  /* �˳� */

/* display.c */
void RefreshMainScreen(void);

/* deal_key_message.c */
int16 deal_key_msg(void);
void volume_callback(uint16 Vol);
void DealFadeIn(uint8 Volume);
void DealFadeInOut(void);

/* fm_deal.c */
uint16 FMdeal(uint16 RunFMStatues);

//SearchHandle.c
extern void ShowFreqInFM(void);
extern uint16 SearchHandle(BYTE Direct);
extern void DealFadeIn(uint8 Volume);
extern uint16 ChangeVolume(void);
extern void DealFadeInOut(void);
extern uint16 AutoSearchHandle(uint8 Direct);

/* Basal_fmradio.c */
uint8 fmSendCommand(radio_cmd_e cmd, void *param1, void *param2);

uint8 deal_mode_key(void);
uint8 deal_long_mode_key(void);
uint8 deal_chan_msg(uint8 key);
uint8 deal_radio_vol_msg(uint8 key);
uint8 deal_with_input_freq(void);
uint8 check_freq_valid(uint16 freq);
uint8 deal_timeout_msg(void);
uint8 deal_play_key(void);
uint8 deal_next_prev_key(uint8 key);
uint8 deal_next_prev_longkey(uint8 key);
uint8 deal_digital_key(uint8 key);

//eq set
void volume_callback(uint16 Vol);
void init_fmin_channel(void);
void exit_fmin_channel(void);
uint8 deal_eq_msg(uint8 key);

/* Analog */
void decinitAnalog(adin_t ain);
void decstartAnalog(adin_t ain);
void decAnalogexit(void);

#endif/*_APRADIO_H*/
