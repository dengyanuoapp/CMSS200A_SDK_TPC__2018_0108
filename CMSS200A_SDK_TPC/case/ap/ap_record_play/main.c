/*
 *******************************************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: main.c,v 1.10 2009/10/09 09:53:51 dengyulin Exp $
 *******************************************************************************
 */
#include "ap_record_play.h"
#include "uhdrv.h"

#pragma name(AP_MAIN)
#define DISK_C 0
#define DISK_H 1

/********************��������***********************/

/******************ȫ�ֱ�������*********************/

//������
//uint8 DiskFlag = 0;
//uint16 near g_rec_event = 0;

bool show_time = TRUE;

/*  ��ǰĿ¼��¼���ļ��ܸ���*/
uint8 near g_file_num;

/* ��¼������*/
uint32 total_sec = 0;

/* ���õĿ�¼������ */
uint32 total_space_setting = 0;

/* ���ν���¼�����ۼ���¼������ */
uint32 total_space_used = 0;

/* �����ļ��������� */
uint32 onefile_space = 0;
uint16 near g_rec_event = 0;

uint32 near g_free_space; //ʣ����̿ռ�//remaining disk space

/* ��ǰ¼���ļ���page��*/
//uint8 near g_total_time[11]; //000:00:00
uint8 near g_now_time[10]; //000:00:00
uint8 near g_file_name_time[16]; //"WAV001 04/01/01"

/* ��ǰ¼��������ASCILL , ������ʾ*/
uint8 near currentBitrate[4];

record_vars_t near g_record_vars;
//uint8 near g_rec_from; //��ǰ��¼��Դ by ccm

/*  ��¼����ʱ��*/
time_t_ext near g_total_time;
/*  ʵʱ��ʾ¼����ʱ��*/
time_t near g_rec_time;
/* ¼��״̬*/
wav_rec_status_t rec_stat;

//resource_t inputSelect =
//{ 0 };

/* ¼�������ṹ*/
audio_format_t Rec_format;
//bool unicodeFlagofFolder;

/* �Ƿ�FM  ¼��*/
bool isFmRecord;
/* �ռ��Ƿ�����*/
bool overflag;
uint8 avrEnableSave;
//ap_inputSelect_e inputSelSave;

//uint8 dir_name[30];

/* ¼��������*/
ap_bitrate_e rec_bitrate;
/* ¼��������*/
ap_samplerate_e Rec_Fsrate;
/* ¼��ͨ����*/
channelnum_t rec_channel;
/* ָ��ǰĬ��¼��Ŀ¼*/
const uint8 *g_dir_voice;

/* ����״̬bitmap */
uint8 g_DiskStateFlag = 0;

/* �Ƿ���Ҫȡʣ������ */
bool need_fetch_space;

bool card_exist = FALSE;
bool UDisk_exist = FALSE;
uint8 rec_status;//��ǰ¼��״̬��0-ֹͣ״̬;1-¼��״̬;2-��ͣ״̬
uint8 need_change_rec_status;//�Ƿ���Ҫת��¼��״̬��־

//�����������ֵ
uint32 Mval = 200000;//��ֵ������ֵ1024
uint32 Mblock = 38;//����ֵ60���ӽ�1��  Mblock=(time*256000/8)/512

uint8 g_disk_char[MAX_DISK_NUM] =
{ 'U', 'H', 'M'};

const uint8 rec_str[4] =
{ "REC" };
const uint8 nodevice_str[5] =
{ "NDEV" };
const uint8 err_str[4] =
{ "ERR" };

//#pragma renamecode(AP_MAIN)


/*
 ********************************************************************************
 * Description : ʹ��DAC
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near openDAC_main(void)
{
    dac_t dacattr;
    dacattr.dacselect = 0;
    dacattr.rate = FS_44K1;//Ĭ��
    EnableDAC(&dacattr);

}

/*
 ********************************************************************************
 * Description : �ر�¼��ͨ��
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near Close_RecRoute(void)
{
    if (!g_comval.KeyTone)
    {

    }
}

/*
 ********************************************************************************
 * Description : Record Ӧ�ò�����ʼ��
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       :
 *
 ********************************************************************************
 */
void near read_var(void)
{
    BOOL result;
    result = read_ap_comval();

    {
        memset(&g_record_vars, 0, sizeof(g_record_vars));
        g_record_vars.maigc = MAGIC_RECORD;
        g_record_vars.location.disk = 0;
        g_record_vars.rec_source = AP_MICIN;        //Ĭ��ѡ��MIC¼��

        g_record_vars.rec_type = RECORD_FILE_TYPE_WAV;
        g_record_vars.rec_encode = ENCODE_WAV_PCM;
        g_record_vars.rec_mode = 2;

        g_record_vars.wav_num = 1;
        g_record_vars.mp2_num = 1;
        g_record_vars.rec_InputGain = g_comval.RecordGain;
        g_record_vars.rec_ADCGain = 3;
        g_record_vars.rec_DSPGain = 0;
        g_record_vars.rec_DelayTime = 15;
    }
}


/*
 ********************************************************************************
 * Description : Record Ӧ�����������
 *
 * Arguments   :
 param:
 0, ���Զ���ʼ¼��
 1, �Զ���ʼ¼��
 RESULT_FMREC_NOSTART,  FM¼��,   ���Զ���ʼ
 RESULT_FMREC_START,  FM¼��, �Զ���ʼ
 *
 *
 * Returns     : �����Ϣ��0
 *
 * Notes       :
 *
 ********************************************************************************
 */
int main(int param)
{
    BOOL mc_result;
    BOOL ReturnToFm = 0;
    uint8 result = 0;
    uint8 sfr_bank;
    overflag = FALSE;

    SetPLL(PLL_48MHZ);

    need_fetch_space = TRUE;
    read_var();

    LEDClearScreen();
    LEDPuts(NUMBER2, rec_str, 1);

    /* ����ֻ֧��NOR FLASH */
    if(g_comval.SuppNorRec == 1)
    {
        g_record_vars.location.disk = 'M';
    }
    else
    {
        if(USBH_Check_Udisk())
        {
            g_record_vars.location.disk = 'U';
        }
        else if (DRV_DetectUD(1) == 0x20)
        {
            g_record_vars.location.disk = 'H';
        }
    }

    /* û���豸 */
    if(g_record_vars.location.disk == 0)
    {
        LEDPuts(NUMBER1, nodevice_str, 1);
        return RESULT_NULL;
    }

    set_dac_volume(g_comval.volume);

    while(1)
    {
        enable_adda();
        result = show_rec();
        if((result != 0) && (result != RESULT_CURRENT_AP))
        {
            break;
        }
        disable_adda();

        result = ui_play();
        if((result != 0) && (result != RESULT_CURRENT_AP))
        {
            break;
        }
    }

    if((g_record_vars.location.disk == 'H') || (g_record_vars.location.disk == 'U'))
    {
        DeleteRecFile();
    }

    SetPLL(PLL_48MHZ);

    mc_result = VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));

    if (result == RESULT_MAIN)
    {
        if (param != RESULT_FMREC_START)
        {
            result = RESULT_NULL;
        }
        else
        {
            result = RESULT_FMREC2FM;
        }
    }
    sfr_bank = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    MRCR2 &= 0xdf;
    SFR_BANK = sfr_bank;

    return result;
}
void enable_adda(void)
{
    if(g_record_vars.rec_source != AP_FMIN)
    {
        return;
    }
    decAddainit(AD_FM);
    set_dac_volume(g_comval.volume);
}
void disable_adda(void)
{
    if(g_record_vars.rec_source != AP_FMIN)
    {
        return;
    }
    decAddaexit();
}

uint8 show_err_msg(void)
{
    LEDClearScreen();
    LEDPuts(NUMBER2, err_str, 1);
    ap_sleep(4);
    return RESULT_MAIN;
}

