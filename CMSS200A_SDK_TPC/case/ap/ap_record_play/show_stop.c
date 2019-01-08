/*
 ********************************************************
 *                ACTOS AP
 *        voice recorder main program.
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: show_stop.c,v 1.13 2009/10/09 09:53:51 dengyulin Exp $
 *  History Record：
 *  2008-03-22  15:47
 *
 *********************************************************
 */
#include "actos.h"
#include "basal.h"
#include "enhanced.h"
#include "ap_common.h"
#include "ap_record_play.h"

#define MURAM1H_OFFSET 0x12c0
#define MURAM1H_ADDR (0x8000 + MURAM1H_OFFSET)

#pragma name(AP_STOP)

/*******************外部函数声明*********************/
const uint8 voice_ext[][4] =
{ "WAV", "MP3" };
const uint8 FilenameHead[] =
{ "REC" };
const uint8 FileNameRecPlay[12] = "RECPLAY WAV";
const uint8 FileNameRecPlayLong[] = {'R', 0x0, 'E', 0x0, 'C', 0x0, 'P', 0x0, 'L', 0x0, 'A', 0x0, \
                                     'Y', 0x0, 0x2e, 0x0, 'W', 0x0, 'A', 0x0, 'V', 0x0, 0x0, 0x0
                                    };
const uint8 g_dir_ext[] = "*  ";
const uint8 g_dir_record[] = "RECORD     ";
const uint8 load_str[] = "LOAD";
/* 各比特率对应的采样率配置, 需和codec 配合
 ** 目前各索引对应的采样率为
 ** 8000,11025,12000,16000,22050,24000,32000,44100,48000
 ** 暂定32kbps,  64kbps, 128kbps, 192kbps, 256kbps 对应采样率
 ** 分别8kHz,  8kHz,  32kHz, 32kHz, 32kHz
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
//目前录音的采样率都是统一为48k,
//如果需要其他的采样率，只需要修改Fsrate_ap_codec中的值就OK,
//目前最高只支持到48K.
const uint8 Fsrate_ap_codec[] =
{ 0, 0, 0, 2, 3, 5 }; //其中的8对应到samplerate_t中的_FS_48K

const int8 log_table[64] =
{
    96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40,
    38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
};
/* ADPCM 三种模式下比特率配置*/
const ap_bitrate_e bitrate_ap_adpcm[3] =
{ AP_BT_64K, AP_BT_128K, AP_BT_256K};

/* PCM 三种模式下比特率配置*/
const ap_bitrate_e bitrate_ap_pcm[3] =
{ AP_BT_64K, AP_BT_128K, AP_BT_256K};

/* MP2 三种模式下比特率配置*/
const ap_bitrate_e bitrate_ap_mp2[3] =
{ AP_BT_64K, AP_BT_128K, AP_BT_256K };

/* 各比特率对应的每秒所录的扇区数32kbps, 64kbps, 128kbps, 192kbps, 256kbps, 384kbps */
const uint8 rec_bitrate_sector_ratio[] =
{ 8, 16, 32, 48, 64, 96 };

/*
 ********************************************************************************
 * Description : 确定录音的比特率及通道数
 *
 * Arguments   :  NULL
 *
 * Returns     :  NULL
 *
 * Notes       :  根据菜单选择的录音模式( 长时, 普通, 优质)  以及
 *                   具体的录音格式( ADPCM,  PCM,  MP2) ,  决定录音参数
 *
 * 后续根据IC  支持及方案具体需求进行调整
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
    //channel number
    if (g_record_vars.rec_encode == ENCODE_WAV_PCM)
    {
        rec_channel = SINGLE_CH;
    }
    else
    {
        //mp2 64k比特率只有单声道
        if ((g_record_vars.rec_type == RECORD_FILE_TYPE_MP2) && (rec_bitrate == AP_BT_64K))
        {
            rec_channel = SINGLE_CH;
        }
        else
        {
            rec_channel = DUAL_CH;
        }
    }
}

/*
 ********************************************************************************
 * Description : 生成新的录音文件名
 *
 * Arguments   : NULL
 *
 * Returns     : TRUE, 成功
 FALSE, 编号01~99 文件都存在
 全局变量
 g_record_vars.location
 g_record_vars.wav_num
 g_file_name_time[], 文件名,文件时间字符串
 *
 * Notes       :
 *
 ********************************************************************************
 */
BOOL gen_filename(void)
{
    date_t date;
    uint16 num = 0x01;

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
 * Description : 分析剩余空间,计算可录音时间
 *
 * Arguments   :  NULL
 *
 * Returns     :  NULL
 *
 * Notes       :  根据剩余空间g_free_space  ( 以扇区为单位) , 计算可录
 *                时间
 ********************************************************************************
 */
void parse_total_time(void)
{
    uint32 total_sec_temp;

    if (need_fetch_space)
    {
        g_free_space = (ulong) FS_GetSpace(FS_GET_SPACE_FREE);
    }
    //NOR FLASH只录一个文件，不需要保留空间。
    if(g_record_vars.location.disk != 'M')
    {
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
            /* 用配置的容量 */
            g_free_space = total_space_setting - total_space_used;
            if (g_free_space <= SPACE_LOW)
            {
                g_free_space = 0;
                total_sec = 0;
                return;
            }
        }
    }
    else if(g_free_space == 0)
    {
        total_sec = 0;
        return;
    }
    total_sec = g_free_space / (rec_bitrate_sector_ratio[(uint8) rec_bitrate]);
    total_sec_temp = total_sec;

    g_total_time.hour = total_sec_temp / 3600;
    total_sec_temp %= 3600;
    g_total_time.minute = (uint8) (total_sec_temp / 60);
    g_total_time.second = (uint8) (total_sec_temp % 60);
}

uint8 change_device(void)
{
    /* NOR FLASH不切换到其它盘符 */
    if(g_record_vars.location.disk == 'M')
    {
        g_record_vars.location.disk = 0;
    }
    else if(g_record_vars.location.disk == 'U')
    {
        g_record_vars.location.disk = 'H';
    }
    else
    {
        g_record_vars.location.disk = 'U';
    }

    return 0;
}

void DeleteRecFile(void)
{
    char strFile[12];
    char nameBuf[sizeof(FileNameRecPlayLong)];
    char extBuf[4] = "WAV";
    uint8 err = 1;
    uint8 tmp;

    /* removed old record file */
    if(!FS_CD(FS_CD_ROOT))
        return;

    err = FS_Dir(strFile, extBuf, FS_DIR_FILE, 1);
    if(err)
    {
        while(err)
        {
            nameBuf[0] = 0x20;
            tmp = FS_GetName(nameBuf, sizeof(FileNameRecPlayLong) / 2 - 1);
            /* 获取长名成功，对比文件长名 */
            if((tmp) && \
                    (memcmp(FileNameRecPlayLong, nameBuf, sizeof(FileNameRecPlayLong) - 2) == 0))
                break;
            /* 获取长名失败，对比文件短名 */
            if((tmp == 0) && \
                    (memcmp(FileNameRecPlay, strFile, sizeof(FileNameRecPlay) - 1) == 0))
                break;
            err = FS_Dir(strFile, extBuf, FS_DIR_FILE_CONT, 1);
            ClearWatchDog();
        }
    }
    if(err)
    {
        FS_FRemove(0);
    }
}

uint8 init_rec_param(void)
{
    /* 录音准备过程*/
    uint8 result;

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

uint8 rec_init(void)
{
    uint8 first_init = TRUE;
    uint8 retval;
    uint8 error_count = 0;

    confirm_rec_param();

continue_init:
    retval = CheckStoreOK();
    if(retval == FALSE)
    {
        error_count++;
        if(error_count > 1)
        {
            return show_err_msg();
        }
        change_device();
        if(!g_record_vars.location.disk)
        {
            return show_err_msg();
        }
        goto continue_init;
    }
    parse_total_time();
    if (g_free_space == 0)
    {
        error_count++;
        if(error_count > 1)
        {
            return show_err_msg();
        }
        change_device();
        if(!g_record_vars.location.disk)
        {
            return show_err_msg();
        }
        goto continue_init;
    }

    show_load();

    strncpy(g_record_vars.location.filename, FileNameRecPlay, sizeof(FileNameRecPlay));
    if(g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        memcpy(&g_record_vars.location.filename[8], voice_ext[1], sizeof(voice_ext[1]));
    }
    g_record_vars.location.filename[11] = 0;
    if((g_record_vars.location.disk == 'H') || (g_record_vars.location.disk == 'U'))
    {
        DeleteRecFile();
    }
    else
    {
        /* format norflash */
        FS_NorErase((pitch_write_page == 0) ? g_free_space : pitch_write_page);
    }

    gen_filename();
    return init_rec_param();
}
/*
 ********************************************************************************
 * Description : 获取当前的录音源,并记录在g_resource.input中.
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :在调用MC_SETRESOURCE之前,调用此函数!
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
 * Description :    准备开始WAV格式录音
 *
 * Arguments   :  NULL
 *
 * Returns     :   success/fail
 *
 * Notes       :   主要是调用basal接口函数发命令
 *
 ********************************************************************************
 */
uint8 StartWAVRecord(void)
{
    uint8 result;

    //初始化codec 变量,  传参2   标识录音格式, 需和codec  定义匹配
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

    /* 创建录音文件*/
    result = recSendCommand(MC_LOAD, (void *) g_record_vars.location.filename);
    return result;
}

/*
 ********************************************************************************
 * Description :    准备开始MP2   格式录音
 *
 * Arguments   :  NULL
 *
 * Returns     :   success/fail
 *
 * Notes       :   主要是调用basal接口函数发命令
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

    /* 创建录音文件*/
    result = recSendCommand(MC_LOAD, (void *) g_record_vars.location.filename);

    return result;
}

/*
 ********************************************************************************
 * Description : 根据所配置的录音方式，为Get_Space做准备
 *
 * Arguments   : NULL
 *
 * Returns     : NULL
 *
 * Notes       : 如配置最长录音时间为0， 则表示使用实际磁盘容量录音
 *               否则，表示最长录音时间为所设置的小时数
 *               设置好系统变量， 文件系统FS_GetSpace接口返回如下
 * 如果磁盘实际容量大于所设置容量，则返回设置的容量
 * 如果磁盘实际容量小于所设置容量，则返回磁盘实际容量
 ********************************************************************************
 */
void near prepare_GetSpace(void)
{
    uint32 total_sec_setting = 0;
    //这里强制将汤姆猫录音录制到卡或u盘的时间设置为1小时
    uint8 card_rec_time = 1;
    //    if(g_comval.RecordTime_max == 0)
    if(card_rec_time == 0)
    {
        //用实际容量
        RecSpare = 0xffffffff;
    }
    else
    {
        //用设置容量
        //        total_sec_setting = (uint32)g_comval.RecordTime_max * 3600;
        total_sec_setting = (uint32)card_rec_time * 3600;
        RecSpare = (rec_bitrate_sector_ratio[(uint8)rec_bitrate]) * total_sec_setting;
        total_space_setting = RecSpare;
    }
    total_space_used = 0;
}
/*
 ********************************************************************************
 * Description : 设置录音缺省目录并检查剩余容量
 *
 * Arguments   : NULL
 *
 * Returns     : true -- 录音缺省目录设置成功, 剩余容量足够
 *                   false -- 录音缺省目录设置失败或容量已满
 * Notes       :
 *
 ********************************************************************************
 */
BOOL CheckStoreOK(void)
{
    BOOL status = FALSE;

    if((g_record_vars.location.disk == 'H') && \
            (DRV_DetectUD(1) != 0x20))
    {
        return FALSE;
    }
    if((g_record_vars.location.disk == 'U') && \
            !USBH_Check_Udisk())
    {
        return FALSE;
    }

    if(!DRV_ActiveDrive(g_record_vars.location.disk, 1))
    {
        return FALSE;
    }

    prepare_GetSpace();
    g_free_space = (uint32) FS_GetSpace(FS_GET_SPACE_FREE);

    if(g_record_vars.location.disk == 'M')
    {
        uint32 nor_need_space;

        nor_need_space = g_comval.Nor_RecordTime_max * rec_bitrate_sector_ratio[(uint8)rec_bitrate];
        g_free_space = (nor_need_space > g_free_space) ? g_free_space : nor_need_space;
        if(g_free_space != 0)
        {
            status = TRUE;
        }
    }
    else
    {
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
            /* 用配置的容量,但磁盘实际容量小于配置容量 */
            total_space_setting = g_free_space;
        }
    }
    need_fetch_space = FALSE;
    return status;
}

void show_load(void)
{
    LEDClearScreen();
    LEDPuts(NUMBER1, load_str, 1);
}

