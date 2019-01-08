/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib header  file
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_common.h,v 1.1.1.1 2008/01/11 07:28:06 rowena Exp $
 *******************************************************************************
 */

#ifndef AP_COMMON_H
#define AP_COMMON_H

#include "actos.h"
#include "ui_res.h"
#include "enhanced.h"
#include "display.h"
#include "key.h"
#include "common.h"
#include "ap_cfg.h"
#include "AlarmDef.h"

#include "stringH.h"

#define     MAX_DISK_NUM    3
//event, 1~8 ϵͳ����, 9~16 �û�ʹ��
#define        AP_EVENT_MAIN        0x0001
#define        AP_EVENT_STANDBY    0x0002
#define        AP_EVENT_RECORD        0x0004
#define        AP_EVENT_UDISK        0x0008
#define        AP_EVENT_REDRAW        0x0010
#define        AP_EVENT_CAMERA     0x0020
// ~8 os reserved
#define        AP_EVENT_9        0x0100
#define        AP_EVENT_10        0x0200
#define        AP_EVENT_11        0x0400
#define        AP_EVENT_12        0x0800
#define        AP_EVENT_13        0x1000
#define        AP_EVENT_14        0x2000
#define        AP_EVENT_15        0x4000
#define        AP_EVENT_16        0x8000

#define        AP_EVENT_ALL        0xffff
#define        AP_EVENT_ANY        0x0000

//main ap startup ap id
#define        RESULT_NULL         0
#define        RESULT_MUSIC        1             //����
#define        RESULT_MOVIE        7            //��Ӱ
#define        RESULT_REC_NOSTART  2            //¼��
#define        RESULT_CLOCK        8            //ʱ����ʾ
#define        RESULT_RADIO        3            //FM����
#define        RESULT_USBTEST     6            //��PCͨ��
#define        RESULT_SYSTEM       4          //ϵͳ����
#define        RESULT_READER       12            //������
#define        RESULT_GAME         9           //��Ϸ
#define        RESULT_TELBOOK      10            //�绰��
#define        RESULT_BROWSER      11         //�ļ����
#define        RESULT_CALENDAR     5         //����
#define        RESULT_LINEIN       13         //LINEIN
#define        RESULT_DATAEXCH 14         //���ݽ���
//14 - 19Ԥ��Ϊ������AP�Ľ����Ϣ
#define        RESULT_RECORD        20      //¼���ȼ���Ϣ
#define        RESULT_TESTER        21
#define        RESULT_STANDBY       22
#define        RESULT_UPGRADE       23
#define        RESULT_UDISK         24
#define        RESULT_MAIN          25
#define        RESULT_FIRST_BOOT    26
#define        RESULT_PDDRM         27
#define        RESULT_XML2HDS       28
#define        RESULT_BUILDLIST      29
#define        RESULT_ONLINEDEVICE_SWITCH   30
#define        RESULT_UDISK1                31
#define        RESULT_MYTST            32

#define        RESULT_MUSICEDIT         33
//radio.ap return param.
#define        RESULT_FMREC_START       34
#define        RESULT_FMREC_NOSTART     35
#define        RESULT_FMREC_WAV         36
#define        RESULT_FM2STANDBY 37
#define        RESULT_FMREC2FM        38
#define        RESULT_FMREC2STANDBY      39
#define        RESULT_CAMERA            40
#define        RESULT_MUSIC_PLAY        41
#define        RESULT_VOICE_PLAY        42
#define        RESULT_MOVIE_PLAY        43
#define        RESULT_READER_PLAY       44
#define        RESULT_VIEW_PLAY         45

//return message id
#define        RESULT_REDRAW         46    //������Ҫ�ػ�
#define        RESULT_POWERON        47    //��standby����
#define        RESULT_IGNORE         48    //�ؼ�ȡ��ѡ��
#define        RESULT_SD_IN          49
#define        RESULT_SD_OUT         50
#define        RESULT_SYSTEM_ADFU       51
#define        RESULT_UHOST_IN     52
#define        RESULT_AUTORECORD        53
#define        RESULT_AUTORECORD_START  54
#define        RESULT_TIMER             55
#define        RESULT_FMRECORD_POWERON  56
#define        RESULT_ALARM_POWERON     57
#define        RESULT_ALARM             58   //����
#define        RESULT_RETURN_TO_MAIN    59
#define        RESULT_AUTO_BACK         60
#define        RESULT_UHOST_OUT         61
#define        RESULT_MUSIC_CPLAY       62
#define        RESULT_MUSIC_UPLAY       63
#define	    RESULT_ALARMFM		    64

//user define��Ԥ��15��
#define        RESULT_USER1             65

#define        USB_disk                 67
#define        USB_Audio                68
#define        USB_Charge               69
#define        RESULT_USBAUDIO_PLAY     70
#define        RESULT_USBUDISK_PLAY		71
#define        RESULT_PITCH     		72
//system  msg  define

#define        AP_MSG_SD_OUT                 MSG_SD_OUT
#define        AP_MSG_SD_IN                  MSG_SD_IN
#define        AP_MSG_ALARM                  0xb2
#define        AP_MSG_FMRECORD               0xb3
#define        AP_MSG_TIMER                  0xb4
#define        AP_MSG_LOCK                   0xb5
#define        AP_MSG_USB_PLUGIN             0xb6
#define        AP_MSG_CHARGING               0xb7
#define        AP_MSG_RTC                    0xb8
#define        AP_MSG_STANDBY                0xb9
#define        AP_MSG_WAIT_TIMEOUT           0xba
#define        AP_MSG_FORCE_STANDBY          0xbb    //ǿ�ƹػ�
#define 	   AP_MSG_UH_IN					 MSG_UH_IN
#define  	   AP_MSG_UH_OUT				 MSG_UH_OUT
#define		   AP_MSG_FM					 0xbc
#define		   AP_MSG_LOWBAT				 0xbd
#define		   AP_MSG_LINEIN				 0xbe
#define		   AP_MSG_LINEIN_IN				 0xbf
#define		   AP_MSG_LINEIN_OUT				 0xC8

#define		   AP_MSG_MIC_IN				 0xcc
#define		   AP_MSG_MIC_OUT				 0xCd

#define        USB_PLUGIN_FINISH             0xc9
#define        MSG_USB_STICK_FLAG            0xca
#define        AP_MSG_USB_PLUGIN_FLAG        0xcb

#define MAX_AP_NUM    4         //��ǰ����Ӧ�ø���

#define MIC_IN_GPIO_INPUT GPIOCINEN
#define MIC_IN_GPIO_OUTPUT GPIOCOUTEN
#define MIC_IN_GPIO_DAT  GPIOCDAT
#define MIC_IN_GPIO_NUM  4

//����pa��Դ����
#define PA_POWER_GPIO_OUTPUT GPIOBOUTEN
#define PA_POWER_GPIO_DAT GPIOBDAT
#define PA_POWER_GPIO_NUM    7

#define MAX_VOl_NUM   41
typedef struct
{
    uint8 magic[4]; //'R', 'U', '2', 0x19
    uint16 counts; //��Դ�ĸ���
} res_head_t; //6 uint8s


/*��Դ��������������ݽṹ*/
typedef struct
{
    uint32 dwOffset; //��Դ�����������ƫ��
    uint16 wCount; //��Դ��������
    uint8 bItemType; //'P'--PIC Table,'S'--String Table,'X' -- XML File
    uint8 type;
} res_entry_t;

/*��Դ������Ϣ���������ݽṹ*/
typedef struct
{
    uint32 dwOffset; //ͼƬ���������ļ���ƫ��,4 uint8s
    uint32 dwLength; //��Դ����, ��� 4G��4 uint8s
    uint8 bType; //��Դ����,0x01--language string ,0x02--PIC
    uint16 wWidth; //����ͼƬ�������ͼƬ�������ַ����������ID����
    uint16 wHeight; //����ͼƬ�������ͼƬ���������ַ��������������Ե�ID.
} res_infor_t; //13 uint8s


/*���������ԴID����������ݽṹ*/
typedef struct
{
    uint32 dwOffset; // �ַ�ID�Ŷ�Ӧ�ַ����������ļ��ڵ�ƫ��
    uint16 dwLength; //  �ַ�������.��unicode�����ַ������ֽ���
} res_langid_entry_t; // 6 uint8s

typedef struct
{
    uint8 filetype; //�ļ����� 0-- Ŀ¼  1  �ļ�
    char name[12];
    int32 DirEntry;
} file_record_m;

typedef struct
{
    uint16 total; //�ܹ��м���item
    uint16 active; //�״̬�ı��
    const uint16 *string; //�ַ���id ����
    uint8 check_disk; //�Ƿ���Ҫ������u����Ϣ���
} function_menu_t;

typedef struct
{
    uint16 top; //�˵�������
    uint16 bottom; //�˵���������
    uint16 active; //�˵���ļ�����
    uint16 old_active;//�˵���ɵļ�����
    uint16 total; //�ܵĲ˵���
    uint16 cur_group; //��ǰ�˵�����������
} menu_list_t;

typedef struct
{
    uint16 magic; // +0
    //uint8  autoback_flag;
    //char   File_Type;
    //char Cur_SelNUM;
    file_location_t location; //+ 9
} browser_vars_t;
typedef struct
{
    uint16 TotalNum; //�б�����
    uint32 ClusterNO; //�б����
    uint8 DirNO[8]; //�б�λ��
    uint16 DirLayer;
} music_playlist;
typedef enum
{
    Key_None, //�޷���
    Key_Next, //����ķ���
    Key_Prev
    //��ǰ�ķ���
} Sel_Directory_t;

typedef enum
{
    LINEIN, //linein
    MIC_IN, //mic
    USB_IN,
    MAX_TYPE
    //��ǰ�ķ���
} Plug_Device_t;

typedef enum
{
    RED_LIGHT, //linein
    BLUE_LIGHT, //mic
    MAX_LIGHT_TYPE
} light_type_t;

typedef enum
{
    Music_UI, //���ֲ��Ž���
    Volume_UI,//������������
    FileNo_UI,//�ļ���Ž���
    Digital_UI, //���ֵ�����
    EQ_UI, //EQ���ý��棨�ǵ������йأ�
    Repeat_UI, //ѭ��ģʽ���棨�ǵ������йأ�
    DIRNAME_UI, //Ŀ¼����ʾ����
    STANDBY_UI, //��ʱ�ػ����ý���
    NODEVICE_UI, //���豸����
    Clock_UI, //���ڽ���
    TimeSet_UI, //ʱ�����ý���
    AlarmSet_UI,//�������ý���
    Linein_UI,//LINEIN����
    RecStop_UI,//¼��ֹͣ����
    RecOn_UI,//����¼������
    RecPause_UI,//¼����ͣ����
    LowPower_UI,
    FMFreq_UI,//FM��ʾƵ�����
    FMChannel_UI,//FM��ʾ��ǰ��̨�Ž���
    FMInputFreq_UI,//����FMƵ�����
    FMAutoSearch_UI,//�Զ���̨����
    FMManualSearch_UI,//�ֶ���̨����
    MaxUI_type
} UI_Type_t;

typedef struct
{
    //magic
    uint16 magic;
    //sleep��ʱ
    uint8 SleepTime;
    uint8 FMBuildInFlag; //add battery select and fm select flag

    //¼������,0:����¼��(V-Record),1:����¼��(M-Record). add by ccm 2004-06-08
    uint8 RecordType;
    //bit0~bit1: Udisk���к�֧��:0:no sn; 1:unified sn; 2:random sn;
    //bit4: Udisk��֤ģʽ֧��: 1:����Udisk��֤ģʽ,����ʱ��ӿ�,֧��suspend��resume��USB��֤����
    int8 udisk_setting;

    uint8 SuppKeyTone; //֧�ְ�����
    uint8 KeyTone; //add by mzh 2007.3.14 0: no key tone 1:have key tone
    uint8 ap_result;//��ǰ����ʹ�õ�apֵ
    uint8 VolumeMax; //�������������
    uint8 VolumeDef; //Ĭ������ֵ
    uint8 Mute_flag; //������־
    eq_t eq_type; // EQģʽ
    uint8 RecordGain;
    uint8 RecordTime_max;//¼�Ƶ�����u���ϵ�¼���ʱ��(��λΪСʱh)
    uint8 Nor_RecordTime_max;//¼�Ƶ�nor�ϵ�¼���ʱ��(��λΪ��S)
    uint8 volume; //��ǰ����ʵ��ֵ
    uint8 vol_display; //��ǰ������ʾֵ
    uint8 SuppTTS; //�Ƿ�֧��TTS
    uint8 SuppNorRec;     // Nor¼������֧��   0�� ��֧��   1�� ֧��
    alarm_vars_t g_alarm;
    //        int8 reserve[32-30];
} ap_comval_t;

extern uint16 g_standby_time; //�Զ��ػ�ʱ��,0.5��Ϊ��λ
extern uint16 g_rtc_counter; //rtc ��Ϣ����
extern uint8 usbplugflag; //USB������
extern uint16 key_count; //��ǰ������Ϣ�����Ĵ���
extern uint8 key_value; //��ǰ������ֵ
extern uint16 lowPower_counter;//��ʾ�͵���Ϣ�Ĵ���

extern uint8 file_name[12]; //����ļ���������


extern int8 alarmtimerflag;
extern uint8 longkey_flag;
extern uint8 ap_result;//��ǰ�����ݵ�apֵ
extern uint8 power_on_flag;//����Ƿ�ΪӲ���ؽ���standby��־
extern uint8 mic_on_flag;//mic ���ش򿪱�־
extern uint8 usb_on_flag;//���usb���Ƿ���ű�ʶ��0:usb���Ѱε���1:usb�߲���

extern char temp_buffer[9];
extern time_t time;
extern date_t date;//���ڽṹ��
extern time_t tmp_time;
extern uint8 set_what;
extern uint8 show_what;
extern uint8 DigitalCount; //��ǰ���ֽ��ĸ���
extern uint8 DigitalBuf[4]; //�洢���ּ�����Ӧ������
extern uint8 show_ui_flag;//�����ʼ������־
extern uint8 show_count;//��ʼ�����������ļ���
extern uint8 CurUI_Type;
extern uint8 need_draw;
extern alarm_vars_t g_alarm;
extern ap_comval_t g_comval; //ϵͳ����,ap���ܸ���
extern uint8 mute_flag;

extern BOOL need_draw;
extern uint8 linein_status; //linein�ߴ���״̬��0-û�в���linein�ߣ�1-linein�߲���
extern uint8 holdkey_flag;//�����������
extern uint8 lowpower_count;//���ܵ��͵�ļ���
extern uint8 standby_time[6];
//extern uint8 insert_msg[MAX_TYPE];
//extern uint8 pull_out_msg[MAX_TYPE];

/***Message���******/
uint8 ap_get_message(void);
uint8 ap_handle_hotkey(uint8 key);
uint8 ap_handle_hotkey_core(uint8 key);
uint8 ap_get_message_core(uint8 key);
uint8 DealLongKey(uint8 key);
uint8 check_longkey_msg(uint8 key);
//uint8 check_plug_device_msg(uint8 * cur_status, Plug_Device_t plug_dev_type);
uint8 check_hard_switch(void);
uint8 deal_standby_msg(void);
void set_poweroff_HCL(void);
void close_usb_phyctrol(void);


extern uint8 ap_sleep(uint16 half_sec);
void itoa(uint16 src_int, uint8 *pbuf);
/*����ת��Ϊ10�����ַ�*/
extern uint8 *itoa1(uint8 i, uint8 *buf);
extern uint8 *itoa2(uint8 i, uint8 *buf);
extern uint8 *itoa3(uint16 i, uint8 *buf);
extern uint8 *itoa4(uint16 i, uint8 *buf);
/*
 #define itoa_1 itoa1
 #define itoa_2 itoa2
 #define itoa_3 itoa3
 #define itoa_4 itoa4*/
#define itoa_1(i,buf){itoa1(i,buf);*((buf)+1)=0;}
#define itoa_2(i,buf){itoa2(i,buf);*((buf)+2)=0;}
#define itoa_3(i,buf){itoa3(i,buf);*((buf)+3)=0;}
#define itoa_4(i,buf){itoa4(i,buf);*((buf)+4)=0;}
#define  abs(x)  (x>0 ? x: -x)

uint8 itoa_num(uint16 num, uint8 *str, uint8 counts);

void Show_FM_Frequency(uint16 frequency);
void Show_Music_Frequency(uint8 pingpu_id);
void FM_TX_init(void);
void FM_TX_Set_Freq(uint16 frq);

void SetCurUI_Type(uint8 UI_Type);
uint8 set_clock_ui(uint8 auto_return);
void set_autosleep_time(uint8 key);

void ShowTimer(void);
void KeyAdjustTime(uint8 key);
void DigitalAdjustTime(void);
void disable_flicker(void);
void enable_flicker(void);
void deal_clock_mode_msg(void);
uint8 deal_clock_digital_key(uint8 key);
void deal_clock_next_prev(uint8 key);
void init_alarm_setting(void);
void disable_alarm(void);
uint8 read_ap_comval(void);
uint16 DigitalSwitchToRealNum(void);
void switch_pa_power(uint8 flag);
uint8 deal_vol_msg(uint8 key, uint8 UI_Type);
void show_volume_ui(void);
uint8 get_onoff_key_status(void);
void set_dac_volume(uint8 cur_vol);
uint8 get_dac_real_vol(uint8 cur_vol);
uint8 switch_key_func(uint8 key);
#define APINO_KEY_INIT             0x0
#define APINO_KEY_EXIT             0x01
#define APINO_KY_GETCHARGESTATUS         0x02
#define APINO_KY_BEEP              0x03
#define APINO_KY_GETKEYTABADDRESS  0x04
#define APINO_CHECK_PLUG_DEVICE_MSG  0x05
#define APINO_SWITCH_PILOT_LIGHT  0x06

bool API_key_init(uint8 api_no);
#define key_init() API_key_init(APINO_KEY_INIT)

void API_key_exit(uint8 api_no);
#define key_exit() API_key_exit(APINO_KEY_EXIT)

uint8 API_KY_GetChargeStatus(uint8 api_no);

#define KY_GetChargeStatus() API_KY_GetChargeStatus(APINO_KY_GETCHARGESTATUS)

bool API_KY_Beep(uint8 api_no, uint8 value);
#define KY_Beep(value) API_KY_Beep(APINO_KY_BEEP,value)

uint8 *API_KY_GetKeyTabAddress(uint8 api_no);
#define sKY_GetKeyTabAddress() API_KY_GetKeyTabAddress(APINO_KY_GETKEYTABADDRESS)

uint8 API_check_plug_device_msg(uint8 api_no, uint8 *cur_status, Plug_Device_t plug_dev_type);
#define check_plug_device_msg(cur_status,plug_dev_type)  API_check_plug_device_msg(APINO_CHECK_PLUG_DEVICE_MSG,cur_status,plug_dev_type)

uint8 API_switch_pilot_light(uint8 api_no, uint8 light_mode, uint8 flag);
#define switch_pilot_light(light_mode,flag)  API_switch_pilot_light(APINO_SWITCH_PILOT_LIGHT,light_mode,flag)

#endif  /* AP_COMMON_H*/

