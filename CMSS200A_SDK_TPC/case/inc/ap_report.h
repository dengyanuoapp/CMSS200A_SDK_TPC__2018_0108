/*
 *******************************************************************************
 *                        SDK30F_D1
 *                     music player header  file
 *
 *                (c) Copyright 2002, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File: ap_music.h
 * By  : rcmai
 * Version: 1> v1.00     first version     2004-3-19 8:28
 *******************************************************************************
 *******************************************************************************
 */
#ifndef REPORT_H
#define REPORT_H
#include "actos.h"//ϵͳͷ�ļ�
#include "mmcmd.h"//ģ�������������
#include "basal.h"//basal module ͷ�ļ�
#include "enhanced.h"//enhanced module ͷ�ļ�
#include "ap_common.h"//�ؼ�ͷ�ļ�
#include "uhdrv.h"
#include "report_res.h"

#include "fm_drv.h"

#define TIME_REPORT_EN          //ʱ�䲥��ʹ�ܶ���
//#define DATE_REPORT_EN          //���ڲ���ʹ�ܶ���

#ifdef MAIN_AP
#define AP_MAIN_REPORT_EN       //�������ӦӦ�ò���ʹ�ܶ���
#endif
#ifdef MUSIC_AP
#define AP_MUSIC_PEPORT_EN      //���ֲ���ʹ�ܶ���
#define AP_MUSIC_NAME           //���ָ���������ʹ�ܶ���
#endif
#ifdef RADIO_AP
#define AP_RADIO_REPORT_EN      //��������ʹ�ܶ���
#endif
#ifdef LINEIN_AP
#define AP_LINEIN_REPORT_EN     //LINEIN�еĲ���ʹ�ܶ���
#endif
#ifdef USOUND_AP
#define AP_USOUND_REPORT_EN     //USB�����еĲ���ʹ�ܶ���
#endif

typedef void (*tts_func)(char *);

typedef struct
{
    char *report_buf;                       //��������ݵ�ָ�룬ָ�򲥱������ݡ�
    char tts_lib_flag;                      //��ǰTTS����,1Ϊ��׼�⣬0Ϊ����
    char open_pa_flag;                      //�Ƿ��PA�ı�־λ
} info_report_t;

//data
extern BOOL report_stats;                   //��ǰ����״̬  0:ֹͣ  1:����
extern BOOL pa_close_flag;                  //����ǰPA��״̬
extern unsigned char ext_report_msg;    //���ص�������Ϣֵ��λͼ��Ϣ

//ext_report_msg ��λͼ��Ϣ
#define REPORT_SD_OUT   0X1
#define REPORT_UD_OUT   0X2
#define REPORT_SD_IN    0X4
#define REPORT_UD_IN    0X8

//report����Ϣ
#define REPORT_ERROR    -1
#define REPORT_OK       0

//extern bool g_PAOpenFlag;

/*Function*/
void init_report_param(void);
unsigned char stop_report(void);
void start_report(info_report_t *info);
BOOL check_report_state(info_report_t *info);
uint8 wait_report_end(void *param);
void report_str_fix(char *str);
BOOL report_res(WORD id, char *str);
int report_res_tag(WORD id, void *param);
void conver_time_to_string(char *buffer);
void report_date(char *buffer);
int report_music_tag(char *report_buf, char cur_disk);
int convert_music_id3_string(char *g_TagInfoBuf, char type);
int convert_music_name_string(char *name);
int report_frequency(fm_status_t *fm_status, char *str);
int report_fix_info(char type, char pa_open_flag, void *param);
int report_tag(char *report_buf);
void conver_time_to_string(char *buffer);
BOOL check_report_state(info_report_t *info);
uint8 report_time_info(uint8 disk_char, uint8 pa_open_flag);
#endif
