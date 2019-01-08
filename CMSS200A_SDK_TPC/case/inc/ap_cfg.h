/*
*******************************************************************************
*                ACTOS AP
*        ap config header  file
*
*        (c) Copyright, Actions Co,Ld.
*             All Right Reserved
*
*  $Id: ap_cfg.h,v 1.7 2009/07/30 14:54:29 mikeyang Exp $
*******************************************************************************
*/
#ifndef AP_CFG_H
#define AP_CFG_H

#ifdef MAIN_AP
#include "cfg_MainAP.h"
#endif

#ifdef UDISK_AP
#include "CFGUDISK_AP.h"
#endif

#ifdef MUSIC_AP
#include "cfgmusic_ap.h"
#endif

#ifdef RADIO_AP
#include "cfgRADIO_AP.h"
#endif
//define this flag when release
#define SupportCard              // ֧�ֲ忨 
#define SupportWMAREC          //֧��WMA��ʽ¼�� 
#define Msg_CTC2HZ             //ʹ��CTC��ʱ��2HZ��Ϣ
#define UNICODE_SUPPORT       //֧��unicode�����������
#define NEWFORMATRES          //֧��Excel����Unicode�����¸�ʽRes�ļ�
//#define ALARM_CLOCK             //֧�����ӹ���
//#define AUTO_FMREC              //֧��FM��ʱ�Զ�¼������
#define  FUN_SWITCH_KEY            /* ֧��һ�������л� */


#define SOFTVOL_MAX    40

//card base or nor base define
//#define USE_CARDBASE

//�̼�������غ�
#define SIZE_CONFIGFILE_HEAD     16                 //fwcfg.bin ͷ��Ϣ�ṹ�ֽ���
#define SIZE_ONE_ITEM      32              // cfg_item_t �ṹ�ֽ���

#define SIZE_PARA_HEAD    16          //����������ͷ��Ϣ�ṹ����
#define SIZE_PARA_ONEITEM    48    //���������൥���������
#define SIZE_STRINGSET_ITEM    64     //�ַ���������ṹ�����

#define SIZE_APCFG_HEAD      16       // ap config ������ͷ�ṹ����
#define SIZE_APINFOR_ITEM    32       // ap ��������

#define SIZE_FUNC_HEAD     16       //����������ͷ�ṹ����

#define  SLEEPTIME_VALUE       0               //�ػ�ʱ��
#define  SLEEPTIME_MAX          120
#define  SLEEPTIME_MIN           0
#define  SLEEPTIME_STEP           10

#define  VOLUME_VALUE       40               //�����ȼ�
#define  VOLUME_MAX          40
#define  VOLUME_MIN           0
#define  VOLUME_STEP           1

#define  DEFVOLUME_VALUE       40               //Ĭ������32
#define  DEFVOLUME_MAX          40
#define  DEFVOLUME_MIN           0
#define  DEFVOLUME_STEP           1

#define  RECGAIN_VALUE       15               //¼������
#define  RECGAIN_MAX          15
#define  RECGAIN_MIN           0
#define  RECGAIN_STEP           1

#define  RECTIME_VALUE       3               //���¼��ʱ��
#define  RECTIME_MAX          20
#define  RECTIME_MIN           0
#define  RECTIME_STEP           1

#define  NOR_RECTIME_VALUE       15               //Ĭ�� nor¼����������λΪS��
#define  NOR_RECTIME_MAX          15			  //���nor¼��ʱ��	 (��)
#define  NOR_RECTIME_MIN           1
#define  NOR_RECTIME_STEP           1

#ifdef __C251__
/*�̼�����������ݽṹ����*/
typedef struct
{
    uint8  Total; //֧�ֵ���������,���25������
    uint8  Active;//��ǰĬ�����Ե�ID��
    uint8  Table[62]; //����ID�����飬ÿ�������Ա��ʾ��ͬ���Ե�ID��ȡֵ��ΧΪΪ0-29��0xff��ʾ����β
} lan_set_t;    // 64 bytes

typedef struct
{
    uint8  Total;      //֧�ֵİ����������������֧��16������
    uint8  Table[16];  //������Ϣֵ�������飬ÿ�������Ա��ʾ��ͬ�İ�����Ϣ
} key_set_t;   // 17 bytes

typedef struct
{
    uint8  Total;     //֧�ֵ�AP������
    uint8  Active;    //�������˵�Ĭ��ap�ı��
    uint8 Table[16]; //AP��Ӧ�����飬ÿ�������Ա�����ֽڣ���һ����ʾAP���,�ڶ�����ʾAP״̬
} ap_set_t;  // 18 bytes

typedef struct
{
    uint8  DefValue;  //Ĭ��ֵ
    uint8  Max;      //���ֵ
    uint8  Min;      //��Сֵ
    uint8  Step;     //����
} value_set_t;  // 4 bytes

typedef struct
{
    uint16        Magic;
    lan_set_t     LanInfor;
    key_set_t     KeyInfor;
    ap_set_t      ApInfor;
    value_set_t   SleepTime;
    value_set_t   Volume;
    value_set_t   VolDefault;
    value_set_t   RecordGain;
    value_set_t   RecordTimeMax; //֧�����¼��ʱ��
    value_set_t   Nor_RecordTimeMax; //֧��nor ���¼��ʱ��,��λΪ��
    uint8  DevVesion[32];
    uint8  FirmwareDate[32];
    uint8  FMBuildInFlag;	  //FM֧�֣�0����֧�֣�1��֧��
    uint8  ClassDFlag;	  //D�๦�ţ��������ʱʹ�ã�0����֧�֣�1��֧��
    uint8  KeyToneFlag;//����������֧��   0�� ��֧�ְ������� 1��֧��
    uint8  Support_WAV;     // ֧��WAV¼����0��ֻ֧��mp3¼����1-ֻ֧��WAV¼��
    uint8  TTS_Report;     // TTS ��������֧��   0�� ��֧��TTS   1�� ֧��
    uint8  Support_NorRec; // Nor ¼������֧��   0�� ��֧��Nor¼����ʹ�ÿ���u�̽���¼��    1�� ֧��Nor¼��
} vm_cfg_t;


/*���������ݽṹ����*/
typedef struct
{
    char   CfgMagic[4];
    uint8  TotalItem;
    char   Reserv[11];
} file_cfg_head_t; // 16 bytes

typedef struct
{
    uint32 Offset;
    char   EName[14];
    char   CName[14];
} cfg_item_t; // 32 bytes


/*������������ͷ��Ϣ���ݽṹ����*/
typedef struct
{
    uint32 ValueOffset;
    uint8  ValueTotal;
    uint32 StringOffset;
    uint8  StringTotal;
    char   Reserv[6];
} paraset_head_t; //16 bytes

/*��ֵ�����������ݽṹ����*/
typedef struct
{
    uint8  DefValue;
    uint8  Max;
    uint8  Min;
    uint8  Step;
    uint8  Reserv[12];
    char   EName[16];
    char   CName[16];
} value_t; //48 bytes

/*�ַ��������������ݽṹ����*/
typedef struct
{
    char   StringCfg[32];
    char   EName[16];
    char   CName[16];
} string_t; //64 bytes

/*���������������ݽṹ����*/
typedef struct
{
    uint8  TotalKey;
    uint8  TotalItem;
    char   Reserv[14];
    char   KeyValue[16];
} key_cfg_t; //32 bytes

typedef struct
{
    uint8  Value;
    uint8  Status; //0 ��ʾ�����õİ�����1��ʾ��������
    char   Description[14];
} key_infor_t; //16 bytes

/*AP������ͷ��Ϣ���ݽṹ����*/
typedef struct
{
    uint8  Total;
    uint8  Active;
    char   Reserv[14];
} ap_cfg_head_t; //16 bytes

/*ap ������Ϣ���ݽṹ*/
typedef struct
{
    uint8  Index;
    uint8  Status;
    char   EName[15];
    char   CName[15];
} ap_infor_t; //32 bytes

typedef struct
{
    uint8  Total; //֧�ֵ���������,���25������
    uint8  Active;//��ǰĬ�����Ե�ID��
    uint8  Table[62]; //����ID�����飬ÿ�������Ա��ʾ��ͬ���Ե�ID��ȡֵ��ΧΪΪ0-29��0xff��ʾ����β
} lang_infor_t; //64 bytes

typedef struct
{
    uint8  Total; //��ǰ���滻�Ĺ̼��ļ�
    uint8  Reserv[15]; //����
} file_ex_t; //16 bytes

typedef struct
{
    uint8  Total; //��ǰ�������������
    uint8  Reserv[15]; //����
} func_cfg_t; //16 bytes

typedef struct
{
    uint8  State; //����֧�����״̬��0����֧�֣�1��֧�֡�
    uint8  Reserv[15]; //����
    uint8  EName[16];  //Ӣ����
    uint8  CName[16];  //������
} func_item_t; //48 bytes

#endif      /*end of #ifdef __C251__*/

#endif

