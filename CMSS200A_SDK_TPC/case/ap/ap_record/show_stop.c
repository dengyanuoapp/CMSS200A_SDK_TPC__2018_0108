/*
 ********************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: show_stop.c,v 1.13 2009/10/09 09:53:51 dengyulin Exp $
 *  History Record��
 *  2008-03-22  15:47
 *
 *********************************************************
 */
#include "actos.h"
#include "basal.h"
#include "enhanced.h"
#include "ap_common.h"
#include "ap_record.h"

#define MURAM1H_OFFSET 0x12c0
#define MURAM1H_ADDR (0x8000 + MURAM1H_OFFSET)

#pragma name(AP_STOP)

/*******************�ⲿ��������*********************/
/* �������ʶ�Ӧ��ÿ����¼��������32kbps, 64kbps, 128kbps, 192kbps, 256kbps, 384kbps */
//const uint8 rec_bitrate_sector_ratio[] =
//{ 8, 16, 32, 48, 64, 96 };
const uint8 voice_ext[][4] =
{ "WAV", "MP3" };
const uint8 FilenameHead[] =
{ "REC" };
const uint8 g_dir_ext[] = "*  ";
const uint8 g_dir_record[] = "RECORD     ";
/* �������ʶ�Ӧ�Ĳ���������, ���codec ���
 ** Ŀǰ��������Ӧ�Ĳ�����Ϊ
 ** 8000,11025,12000,16000,22050,24000,32000,44100,48000
 ** �ݶ�32kbps,  64kbps, 128kbps, 192kbps, 256kbps ��Ӧ������
 ** �ֱ�8kHz,  8kHz,  32kHz, 32kHz, 32kHz
 */
/*

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

*/

//Ŀǰ¼���Ĳ����ʶ���ͳһΪ48k,

//�����Ҫ�����Ĳ����ʣ�ֻ��Ҫ�޸�Fsrate_ap_codec�е�ֵ��OK,

//Ŀǰ���ֻ֧�ֵ�48K.

const uint8 Fsrate_ap_codec[] =

{ 8, 8, 8, 8, 8, 8 }; //���е�8��Ӧ��samplerate_t�е�_FS_48K

const int8 log_table[64] =
{
    96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40,
    38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
};
const uint8 err_str[4] =
{ "ERR" };
/* ADPCM ����ģʽ�±���������*/
const ap_bitrate_e bitrate_ap_adpcm[3] =
    //{ AP_BT_64K, AP_BT_128K, AP_BT_256K};
{ AP_BT_384K, AP_BT_192K, AP_BT_384K }; /* FPGA */

/* PCM ����ģʽ�±���������*/
const ap_bitrate_e bitrate_ap_pcm[3] =
    //{ AP_BT_64K, AP_BT_128K, AP_BT_256K};
{ AP_BT_256K, AP_BT_128K, AP_BT_256K }; /* FPGA */

/* MP2 ����ģʽ�±���������*/
const ap_bitrate_e bitrate_ap_mp2[3] =
{ AP_BT_64K, AP_BT_128K, AP_BT_256K };

//const uint8 root_dirname[] = "ROOT";


const uint8 rec_bitrate_sector_ratio[] =
{ 8, 16, 32, 48, 64, 96 };

/*
 ********************************************************************************
 * Description : ȷ��¼���ı����ʼ�ͨ����
 *
 * Arguments   :  NULL
 *
 * Returns     :  NULL
 *
 * Notes       :  ���ݲ˵�ѡ���¼��ģʽ( ��ʱ, ��ͨ, ����)  �Լ�
 *                   �����¼����ʽ( ADPCM,  PCM,  MP2) ,  ����¼������
 *
 * ��������IC  ֧�ּ���������������е���
 ********************************************************************************
 */
void confirm_rec_param(void)
{
    if (g_record_vars.rec_type == RECORD_FILE_TYPE_WAV)
    {
        if (g_record_vars.rec_encode == ENCODE_WAV_ADPCM)
        {
            //wav, ADPCM
            rec_bitrate = bitrate_ap_adpcm[g_record_vars.rec_mode];
        }
        else
        {
            //wav, PCM
            rec_bitrate = bitrate_ap_pcm[g_record_vars.rec_mode];
        }
    }
    else
    {
        //mp2
        rec_bitrate = bitrate_ap_mp2[g_record_vars.rec_mode];
    }
    if (rec_bitrate == AP_BT_128K)
    {
        rec_channel = DUAL_CH;
    }
    else
    {
        rec_channel = SINGLE_CH;
    }
#if 0
    //channel number
    if ((rec_bitrate == AP_BT_32K) || (rec_bitrate == AP_BT_128K) || (rec_bitrate == AP_BT_192K))
    {
        rec_channel = SINGLE_CH;
    }
    else
    {
        rec_channel = DUAL_CH;
        if ((rec_bitrate == AP_BT_64K) && (g_record_vars.rec_type == RECORD_FILE_TYPE_MP2))
        {
            //MP2 64Kbps ¼�� ������
            rec_channel = SINGLE_CH;
        }
    }
#endif
}

/***************************************************/
//#pragma renamecode(AP_STOP)

/*
 ********************************************************************************
 * Description : ��ʽ���ļ���
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       : RECxxx.WAV or RECxxx.MP3
 *
 ********************************************************************************
 */
void near format_filename(void)
{
    memset(g_record_vars.location.filename, 0, 12);
    strcpy(g_record_vars.location.filename, FilenameHead); //REC000.WAV

    if(g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        itoa3(g_record_vars.mp2_num, &g_record_vars.location.filename[3]);
    }
    else
    {
        itoa3(g_record_vars.wav_num, &g_record_vars.location.filename[3]);
    }
    strcat(g_record_vars.location.filename, "  ");
    strcat(g_record_vars.location.filename, voice_ext[(uint8)g_record_vars.rec_type]);
}
#if 0
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
void near openDAC(void)
{
    dac_t dacattr;
    dacattr.dacselect = 0;
    dacattr.rate = FS_44K1;//Ĭ��
    EnableDAC(&dacattr);
}
#endif
/*
 ********************************************************************************
 * Description : �����µ�¼���ļ���
 *
 * Arguments   : NULL
 *
 * Returns     : TRUE, �ɹ�
 FALSE, ���01~99 �ļ�������
 ȫ�ֱ���
 g_record_vars.location
 g_record_vars.wav_num
 g_file_name_time[], �ļ���,�ļ�ʱ���ַ���
 *
 * Notes       :
 *
 ********************************************************************************
 */
BOOL gen_filename(void)
{
    date_t date;
    uint16 num = (uint16) GenNewName(&g_record_vars.location);

    if (num == 0)
    {
        return FALSE;
    }

    memset(g_file_name_time, 0, sizeof(g_file_name_time));

    if (g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        g_record_vars.mp2_num = num;
    }
    else
    {
        g_record_vars.wav_num = num;
    }
    TM_GetDate(&date);
    itoa4(date.year, &g_file_name_time[5]); // "----2004"
    memcpy(g_file_name_time, g_record_vars.location.filename, 6);

    g_file_name_time[6] = ' '; //"WAVxxx 04"
    g_file_name_time[9] = '/'; //"WAVxxx 04/"
    itoa2(date.month, &g_file_name_time[10]);
    g_file_name_time[12] = '/'; //"WAVxxx 04/01/"
    itoa2(date.day, &g_file_name_time[13]); //"WAVxxx 04/01/01"

    return TRUE;
}

/*
 ********************************************************************************
 * Description : ����ʣ��ռ�,�����¼��ʱ��
 *
 * Arguments   :  NULL
 *
 * Returns     :  NULL
 *
 * Notes       :  ����ʣ��ռ�g_free_space  ( ������Ϊ��λ) , �����¼
 *                ʱ��
 ********************************************************************************
 */
void parse_total_time(void)
{
    uint32 total_sec_temp;

    if (need_fetch_space)
    {
        g_free_space = (ulong) FS_GetSpace(FS_GET_SPACE_FREE);
    }

    if (g_free_space > SPACE_LOW)
    {
        g_free_space -= SPACE_LOW;
    }
    else
    {
        g_free_space = 0;
        total_sec = 0;
        return;
    }

    if (RecSpare != 0xffffffff)
    {
        /* �����õ����� */
        g_free_space = total_space_setting - total_space_used;
        if (g_free_space <= SPACE_LOW)
        {
            g_free_space = 0;
            total_sec = 0;
            return;
        }
    }
    total_sec = g_free_space / (rec_bitrate_sector_ratio[(uint8) rec_bitrate]);
    total_sec_temp = total_sec;

    g_total_time.hour = total_sec_temp / 3600;
    total_sec_temp %= 3600;
    g_total_time.minute = (uint8) (total_sec_temp / 60);
    g_total_time.second = (uint8) (total_sec_temp % 60);
}

/*
 ********************************************************************************
 * Description : ���㵱ǰĿ¼�µ���¼���ļ�����
 *
 * Arguments   :  NULL
 *
 * Returns     :    g_file_num, ��¼���ļ�����
 *
 * Notes       :
 *
 ********************************************************************************
 */
void parse_total_num(void)
{
    uint8 name[12];
    g_file_num = 0;

    if (0 != FS_Dir(name, voice_ext[(uint8) g_record_vars.rec_type], FS_DIR_FILE, 1))
    {
        g_file_num++;
        while (0 != FS_Dir(name, voice_ext[(uint8) g_record_vars.rec_type], FS_DIR_FILE_CONT, 1))
        {
            g_file_num++;
            if(g_file_num >= 200)
            {
                break;
            }
        }
    }
}
uint8 change_device(uint8 first_init)
{
    if (first_init == FALSE)
    {
        return 0;
    }
    if (g_record_vars.location.disk == g_disk_char[0])
    {
        if (DRV_DetectUD(1) == 0x20)
        {
            g_record_vars.location.disk = g_disk_char[1];
            return NULL;
        }
    }
    else
    {
        if (USBH_Check_Udisk())
        {
            g_record_vars.location.disk = g_disk_char[0];
            return NULL;
        }
    }
    return RESULT_MAIN;
}
uint8 init_rec_param(void)
{
    /* ¼��׼������*/
    uint8 result;
    confirm_rec_param();
    if (g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        result = StartMP2Record();
    }
    else
    {
        result = StartWAVRecord();
    }

    if (!result)
    {
        g_rec_event = AP_EVENT_CODEC_ERR;
        return show_err_msg();
    }
    recSendCommand(MC_RECORD, 0);
    return 0;
}
uint8 show_err_msg(void)
{
    LEDClearScreen();
    LEDPuts(NUMBER2, err_str, 1);
    ap_sleep(4);
    return RESULT_MAIN;
}
uint8 rec_init(void)
{
    uint8 first_init = TRUE;
    uint8 retval;
continue_init:
    if (review_disk != 0)
    {
        retval = CheckStoreOK();
        if (retval == FALSE)
        {
            retval = change_device(first_init);
            if (retval == 0)
            {
                return show_err_msg();
            }
            else
            {
                first_init = 0;
                goto continue_init;
            }
        }
    }
    parse_total_time();
    if (g_free_space == 0)
    {
        retval = change_device(first_init);
        if (retval == 0)
        {
            return show_err_msg();
        }
        else
        {
            first_init = 0;
            review_disk = 1;
            goto continue_init;
        }
    }
    parse_total_num();
    if (g_file_num >= 200) //����200��¼���ļ�
    {
        retval = change_device(first_init);
        if (retval == 0)
        {
            return show_err_msg();
        }
        else
        {
            first_init = 0;
            review_disk = 1;
            goto continue_init;
        }
    }
    format_filename(); //��ʽ��¼���ļ���
    if (!gen_filename()) //����¼���ļ������
    {
        //�Ѵﵽ��Ŀ¼¼���ļ�������
        return show_err_msg();
    }
    return init_rec_param();
}
/*
 ********************************************************************************
 * Description : ��ȡ��ǰ��¼��Դ,����¼��g_resource.input��.
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :�ڵ���MC_SETRESOURCE֮ǰ,���ô˺���!
 *
 ********************************************************************************
 */
void get_rec_source(void)
{
#if 0
    //    inputSelect.input = 0;

    //fpga test
    //    g_record_vars.rec_source = AP_MICIN;
    inputSelSave = g_record_vars.rec_source;
    switch (g_record_vars.rec_source)
    {
    case AP_LINEIN://AP_FMIN
        inputSelect.input = ENC_LINEIN;
        break;

    case AP_FMIN:
        inputSelect.input = ENC_FM;
        break;

    case AP_MICIN:
        inputSelect.input = ENC_MIC;
        break;

    default:
        inputSelect.input = ENC_MIC;
        break;
    }
#endif
}

/*
 ********************************************************************************
 * Description :    ׼����ʼWAV��ʽ¼��
 *
 * Arguments   :  NULL
 *
 * Returns     :   success/fail
 *
 * Notes       :   ��Ҫ�ǵ���basal�ӿں���������
 *
 ********************************************************************************
 */
uint8 StartWAVRecord(void)
{
    uint8 result;

    //��ʼ��codec ����,  ����2   ��ʶ¼����ʽ, ���codec  ����ƥ��
    result = recSendCommand(MC_OPEN, 2);
    recSendCommand(MC_RECODEMODE, (void *) g_record_vars.rec_encode);
    recSendCommand(MC_SET_RECORD_DELAY, (void *) g_record_vars.rec_DelayTime);
    recSendCommand(MC_SETGAIN, (void *) g_record_vars.rec_InputGain);
    recSendCommand(MC_SETGAIN_ADC, (void *) g_record_vars.rec_ADCGain);
    recSendCommand(MC_SETGAIN_DSP, (void *) g_record_vars.rec_DSPGain);

    //MUST TO BE SET
    // get_rec_source();
    //    recSendCommand(MC_SETSOURCE, (void*) &inputSelect);
    Rec_format.sample_rate = (uint8) (Fsrate_ap_codec[(uint8) rec_bitrate]);

    Rec_format.channel = (uint8) rec_channel;

    Rec_format.bit_rate[0] = (currentBitrate[0]);
    Rec_format.bit_rate[1] = (currentBitrate[1]);
    Rec_format.bit_rate[2] = (currentBitrate[2]);
    recSendCommand(MC_SETAUDIOFMT, (void *) &Rec_format);

    /* ����¼���ļ�*/
    result = recSendCommand(MC_LOAD, (void *) g_record_vars.location.filename);
    return result;
}

/*
 ********************************************************************************
 * Description :    ׼����ʼMP2   ��ʽ¼��
 *
 * Arguments   :  NULL
 *
 * Returns     :   success/fail
 *
 * Notes       :   ��Ҫ�ǵ���basal�ӿں���������
 *
 ********************************************************************************
 */
uint8 StartMP2Record(void)
{
    uint8 result;
    char *mp2_buff_addr = (char *) MURAM1H_ADDR;
    char *mp2_array_addr = log_table;
    memcpy(mp2_buff_addr, mp2_array_addr, sizeof(log_table));
    result = recSendCommand(MC_OPEN, 0);

    recSendCommand(MC_SET_RECORD_DELAY, (void *) g_record_vars.rec_DelayTime);
    //    recSendCommand(MC_SETGAIN, (void*) g_record_vars.rec_InputGain);
    //    recSendCommand(MC_SETGAIN_ADC, (void*) g_record_vars.rec_ADCGain);
    recSendCommand(MC_SETGAIN_DSP, (void *) g_record_vars.rec_DSPGain);

    //set record source
    get_rec_source();
    //    recSendCommand(MC_SETSOURCE, (void*) &inputSelect);

    //Rec_format.bits = 18;

    Rec_format.sample_rate = (uint8) (Fsrate_ap_codec[(uint8) rec_bitrate]);

    Rec_format.channel = (uint8) rec_channel;
#if 0
    Rec_format.bit_rate[0] = '0';
    Rec_format.bit_rate[1] = (currentBitrate[0]);
    Rec_format.bit_rate[2] = (currentBitrate[1]);
    Rec_format.bit_rate[3] = (currentBitrate[2]);
#endif
    Rec_format.bit_rate[0] = (currentBitrate[0]);
    Rec_format.bit_rate[1] = (currentBitrate[1]);
    Rec_format.bit_rate[2] = (currentBitrate[2]);
    recSendCommand(MC_SETAUDIOFMT, (void *) &Rec_format);

    /* ����¼���ļ�*/
    result = recSendCommand(MC_LOAD, (void *) g_record_vars.location.filename);

    return result;
}
/*
 ********************************************************************************
 * Description : ����¼��ȱʡĿ¼
 *
 * Arguments   :
 *
 * Returns     : �ɹ�or ʧ��
 *
 * Notes       :  ����¼����ʽ, ����ȱʡĿ¼ΪRECORD/VOICE
 *                   ��RECORD/MUSIC
 ********************************************************************************
 */
BOOL reset_record_dir(bool need_init)
{
    uint8 i;
    BOOL status;
    uint8 file[12];
    uint8 short_name[12];

    if (need_init)
    {
        fselInit(DIRECTORY_ALL, FSEL_DIR_END, FSEL_TYPE_COMMONDIR, &g_record_vars.location, 0);
    }
    g_record_vars.location.filename[0] = 0;

    //    type = (uint8) g_record_vars.rec_type;
    //    g_dir_voice = g_dir_rec_voice[type];
    g_dir_voice = g_dir_record;
    strcpy(g_record_vars.location.path, g_dir_voice);

    g_record_vars.location.path[11] = 0x00;
    g_record_vars.location.filename[0] = 0;

    FS_CD(FS_CD_ROOT);
#if 0
    status = FS_CD(g_dir_record);
    if (!status)
    {
        status = FS_MakeDir(g_dir_record);
        if (!status)
            return FALSE; // ����Ŀ¼ʧ��//create directory fail
    }
    else
        FS_CD("\\");

    /* ΪexFAT���, ��ȡ���� */
    memset(short_name, 0x0, 12);
    status = FS_JustBeing(g_dir_record, short_name, 0x11);
    if (!status)
    {
        return FALSE;
    }
    if (short_name[0] == 0x0)
    {
        memcpy(short_name, g_dir_record, 12);
    }
    i = 1;
    status = FS_Dir(file, g_dir_ext, FS_DIR_DIR, 0);
    do
    {
        //if(0 == memcmp(file, g_dir_record, 8))
        if (0 == memcmp(file, short_name, 8))
        {
            break;
        }
        i++;
    }
    while ((status = FS_Dir(file, g_dir_ext, FS_DIR_DIR_CONT, 0)));
    if (!status)
        return FALSE;

    g_record_vars.location.DirNo[0] = i;

    FS_CD(g_dir_record);
#endif
    status = FS_CD(g_dir_voice);
    if (!status)
    {
        status = FS_MakeDir(g_dir_voice);
        if (!status)
            return FALSE; // ����Ŀ¼ʧ��
    }
    else
        FS_CD("\\");

    /* ΪexFAT���, ��ȡ���� */
    memset(short_name, 0x0, 12);
    status = FS_JustBeing(g_dir_voice, short_name, 0x11);
    if (!status)
    {
        return FALSE;
    }
    if (short_name[0] == 0x0)
    {
        memcpy(short_name, g_dir_voice, 12);
    }
    i = 1;
    status = FS_Dir(file, g_dir_ext, FS_DIR_DIR, 0);
    do
    {
        //if(0 == memcmp(file, g_dir_voice, 8))
        if (0 == memcmp(file, short_name, 8))
        {
            break;
        }
        i++;
    }
    while ((status = FS_Dir(file, g_dir_ext, FS_DIR_DIR_CONT, 0)));
    if (!status)
        return FALSE;

    g_record_vars.location.DirNo[1] = i;
    g_record_vars.location.dirLayer = 1;
    g_record_vars.location.ID3buffer = NULL;
    status = FS_CD(g_dir_voice);

    g_record_vars.location.dirLayer++;
    g_record_vars.location.ClusterNo = FS_GetCurDir();
    g_record_vars.location.DirEntry = FS_GetCurDirEntry();

    return TRUE;
}

/*
 ********************************************************************************
 * Description : ���������õ�¼����ʽ��ΪGet_Space��׼��
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       : �������¼��ʱ��Ϊ0�� ���ʾʹ��ʵ�ʴ�������¼��
 *               ���򣬱�ʾ�¼��ʱ��Ϊ�����õ�Сʱ��
 *               ���ú�ϵͳ������ �ļ�ϵͳFS_GetSpace�ӿڷ�������
 * �������ʵ�����������������������򷵻����õ�����
 * �������ʵ������С���������������򷵻ش���ʵ������
 ********************************************************************************
 */
void near prepare_GetSpace(void)
{
    uint32 total_sec_setting = 0;

    if(g_comval.RecordTime_max == 0)
    {
        //��ʵ������
        RecSpare = 0xffffffff;
    }
    else
    {
        //����������
        total_sec_setting = (uint32)g_comval.RecordTime_max * 3600;
        RecSpare = (rec_bitrate_sector_ratio[(uint8)rec_bitrate]) * total_sec_setting;
        total_space_setting = RecSpare;
    }
    total_space_used = 0;
}
/*
 ********************************************************************************
 * Description : ����¼��ȱʡĿ¼�����ʣ������
 *
 * Arguments   : NULL
 *
 * Returns     : true -- ¼��ȱʡĿ¼���óɹ�, ʣ�������㹻
 *                   false -- ¼��ȱʡĿ¼����ʧ�ܻ���������
 * Notes       :
 *
 ********************************************************************************
 */
BOOL CheckStoreOK(void)
{
    BOOL status;
    prepare_GetSpace();
    if (0 == fselSetLocation(&g_record_vars.location))
    {
        status = reset_record_dir(TRUE);
    }

    g_free_space = (uint32) FS_GetSpace(FS_GET_SPACE_FREE);

    need_fetch_space = FALSE;
    if (g_free_space <= SPACE_LOW)
    {
        overflag = TRUE;
        status = FALSE;
    }
    else
    {
        status = TRUE;
    }
    if ((RecSpare != 0xffffffff) && (g_free_space < total_space_setting))
    {
        /* �����õ�����,������ʵ������С���������� */
        total_space_setting = g_free_space;
    }
    return status;
}

