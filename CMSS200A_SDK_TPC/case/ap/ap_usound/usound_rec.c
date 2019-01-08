#include "actos.h"
#include "basal.h"
#include "enhanced.h"
#include "ap_common.h"
#include "ap_record.h"

#define MURAM1H_OFFSET 0x12c0
#define MURAM1H_ADDR (0x8000 + MURAM1H_OFFSET)

extern DLL_IMP uint32 RecSpare;
const uint8 g_dir_ext[] = "*  ";

const uint8 g_dir_urec[12] =
{ "UREC       " };

/* Usound录音文件名 */
const uint8 FilenameHead[] =
{ "UREC" };

/*************全局变量定义*********************************/

/* 可录总秒数*/
//uint32 total_sec = 0;

uint32 near g_free_space; //剩余磁盘空间//remaining disk space

/* 当前录音文件的page数*/
//uint32 near g_file_size;

//comval_t g_comval;
//alarm_vars_t g_alarm;
record_vars_t near g_record_vars;
//uint8 near g_rec_from; //当前的录音源 by ccm

/*  可录音总时间*/
//time_t_ext near time;

/* 录音状态*/
wav_rec_status_t rec_stat;

resource_t inputSelect =
{ 0 };

/* 录音参数结构*/
audio_format_t Rec_format;

const int8 log_table[64] =
{
    96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40,
    38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
};

uint16 StartMP2Record(void);

#pragma name(AP_USOUND_REC)

int16 prepare_urec(void)
{
    BOOL retbool;
    RecSpare = 0xffffffff;
    retbool = SetCard();

    if (!retbool)
    {
        //未插卡或驱动装载失败
        return USOUND_CARD_ERROR;
    }

    //设置USOUND录音目录
    //retbool = reset_record_dir();
    reset_record_dir();
    //if(!retbool)
    //{
    //设置录音目录失败
    //return USOUND_SETDIR_ERROR;
    //}

    //分析录音可录时间
    parse_total_time();

    if (g_free_space == 0)
    {
        //卡空间已满, 无法录音
        return USOUND_CARD_FULL;
    }

    format_filename();
    if (!gen_filename()) //产生录音文件序号名
    {
        //已达到该目录录音文件数限制
        return USOUND_DIR_FULL;
    }

    if (!StartMP2Record())
    {
        //创建文件失败
        return USOUND_CREAT_FAIL;
    }
    return 0;
}

/*
 ********************************************************************************
 * Description : 打开USOUND录音
 *
 * Arguments   : NULL
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
int16 usound_rec(void)
{
    int16 retval=0;
//  uint8 key;
//  uint16 i;
//  status_t status;
    retWavrProceed_e mc_result = WRITE_NONE;
#if 0
//    LEDDisplay(LED_PAUSE, 0xff, 0);
//    LEDDisplay(LED_PLAY, 0xff, 1);
    mp3rSendCommand(MC_RECORD, 0);
    status = recording;

    while (1)
    {
        if (status != saving)
        {
            key = ap_get_message();

            switch (key)
            {

            case AP_KEY_PLAY | AP_KEY_UP:
                if (status == recording)
                {
                    mp3rSendCommand(MC_PAUSE, 0);

                    retval = do_pause(); //暂停界面

                    if (retval != 0)
                    {
                        status = wait_to_save;

                        if (retval == RESULT_CARD_OUT)
                        {
                            status = saving;
                            //显示卡拔出
                            mp3rSendCommand(MC_STOP, 0);
                            retval = RESULT_SD_OUT;
                        }
                        else
                        {

                        }
                    }
                }
                break;

            case AP_KEY_MODE | AP_KEY_UP:
                status = wait_to_save;
                retval = 0;
                break;

            case AP_KEY_MODE | AP_KEY_LONG:
                longkey_flag = 1;
                status = wait_to_save;
                retval = RESULT_MAIN;
                break;

            case AP_MSG_SD_OUT:
                if (g_record_vars.location.disk == 'H')
                {
                    status = saving;
                    //显示卡拔出
                    mp3rSendCommand(MC_STOP, 0);
                    retval = RESULT_SD_OUT;
                }
                break;

            case AP_MSG_RTC:
                if (status == recording)
                {
                    mp3rSendCommand(MC_GETSTATUS, (void *) &rec_stat); //add by lei
                    if (rec_stat.status > 0xf0)
                    {
                        mp3rSendCommand(MC_STOP, 0);
                        return 0;
                    }
                }
                break;

            default:
                break;
            }//switch
        }
        if (status == wait_to_save)
        {
            //show_error(RSAVING);
            mp3rSendCommand(MC_STOP, 0);
            status = saving;
        }
        if (status == saving)
        {
            mp3rSendCommand(MC_GETSTATUS, (void *) &rec_stat);
            if (rec_stat.status == RECORDING_STOP)
            {
                return retval;
            }
            ClearWatchDog();
#if 0		 // 暂不超时退出, 避免文件不保存
            i++;
            if (i >= 2000)
            {
                return retval;
            }
#endif
        }
        mc_result = wavrProceed();

        if (mc_result == WRITE_ONE) //已写一个page
        {
            g_free_space--;
            if (g_free_space == 0)
            {
                mp3rSendCommand(MC_STOP, 0);
                i = 0;
                while (1)
                {
                    wavrProceed();
                    mp3rSendCommand(MC_GETSTATUS, (void *) &rec_stat);
                    i++;
                    if ((rec_stat.status == RECORDING_STOP) || (i >= 2000))
                    {
                        return RESULT_MAIN;
                    }
                }
            }
        }
        else if (mc_result == WRITE_WRONG) //致命错误
        {
            while (ap_get_message() != NULL)
            {
                ;//qac
            }
            if ((g_record_vars.location.disk == 'H') && ((DRV_DetectUD(1)) != 0x20))
            {
                mp3rSendCommand(MC_STOP, 0);
                return RESULT_MAIN;
            }
            mp3rSendCommand(MC_STOP, 0);
            return RESULT_MAIN;
        }
        else if (WRITE_HEAD == mc_result)
        {
            break;
        }
        else
        {

        }
    }//while(1)

    //return 0;
#endif
    return retval;

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
uint16 StartMP2Record(void)
{
    uint16 result;
    char *mp2_buff_addr = (char *) MURAM1H_ADDR;
    char *mp2_array_addr = log_table;
    memcpy(mp2_buff_addr, mp2_array_addr, sizeof(log_table));

    result = mp3rSendCommand(MC_OPEN, 0);

    mp3rSendCommand(MC_SET_RECORD_DELAY, (void *) g_record_vars.rec_DelayTime);
    mp3rSendCommand(MC_SETGAIN, (void *) g_record_vars.rec_InputGain);
    mp3rSendCommand(MC_SETGAIN_ADC, (void *) g_record_vars.rec_ADCGain);
    mp3rSendCommand(MC_SETGAIN_DSP, (void *) g_record_vars.rec_DSPGain);

    //set record source
    inputSelect.input = 0;

    //fpga test
    g_record_vars.rec_source = AP_LINEIN;
    mp3rSendCommand(MC_SETSOURCE, (void *) &inputSelect);

    //Rec_format.bits = 18;

    //固定采样率
    Rec_format.sample_rate = 8;

    //固定单通道
    Rec_format.channel = SINGLE_CH;

    //固定比特率 128k
    Rec_format.bit_rate[0] = 0 + '0';
    Rec_format.bit_rate[1] = 6 + '0';
    Rec_format.bit_rate[2] = 4 + '0';
    mp3rSendCommand(MC_SETAUDIOFMT, (void *) &Rec_format);

    /* 创建录音文件*/
    result = mp3rSendCommand(MC_LOAD, (void *) g_record_vars.location.filename);

    return result;
}

BOOL SetCard(void)
{
    ClearWatchDog();
    if (DRV_DetectUD(1) == 0x20)
    {
        if (DRV_ActiveDrive('H', MODE_SLOW))
        {
            g_record_vars.location.disk = 'H';
            return TRUE;
        }
    }
    return FALSE;
}

/*
 ********************************************************************************
 * Description : 设置录音缺省目录
 *
 * Arguments   :
 *
 * Returns     : 成功or 失败
 *
 * Notes       :  根据录音格式, 设置缺省目录为RECORD/VOICE
 *                   或RECORD/MUSIC
 ********************************************************************************
 */
BOOL reset_record_dir(void)
{
    //uint8  i;
    BOOL status;
    //uint8 file[12];

    g_record_vars.location.filename[0] = 0;

    strcpy(g_record_vars.location.path, g_dir_urec);

    g_record_vars.location.path[11] = 0x00;
    g_record_vars.location.filename[0] = 0;

    FS_CD(FS_CD_ROOT);
    status = FS_CD(g_dir_urec);
    if (!status)
    {
        status = FS_MakeDir(g_dir_urec);
        if (!status)
        { return FALSE; } // 创建目录失败//create directory fail
        else
        {
            FS_CD(g_dir_urec);
        }
    }

#if 0
    else
    { FS_CD("\\"); }

    i = 1;
    status = FS_Dir(file, g_dir_ext, FS_DIR_DIR, 0);
    do
    {
        if(0 == memcmp(file, g_dir_urec, 8))
        {
            break;
        }
        i++;
    }
    while ((status = FS_Dir(file, g_dir_ext, FS_DIR_DIR_CONT, 0)));
    if (!status)
    { return FALSE; }

    g_record_vars.location.DirNo[0] = i;

    g_record_vars.location.dirLayer = 0;
    status = FS_CD(g_dir_urec);

    g_record_vars.location.ClusterNo = FS_GetCurDir();
    g_record_vars.location.DirEntry = FS_GetCurDirEntry();
#endif

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
 *                   时间
 ********************************************************************************
 */
void parse_total_time(void)
{
    //uint32 total_sec_temp;
    g_free_space = (ulong) FS_GetSpace(FS_GET_SPACE_FREE);

    if (g_free_space > SPACE_LOW)
    {
        g_free_space -= SPACE_LOW;
    }
    else
    {
        g_free_space = 0;
        //total_sec = 0;
        return;
    }

    //total_sec = g_free_space / 32;                      // 128kbps mp2 record
    //total_sec_temp = total_sec;

    //time.hour = total_sec_temp / 3600;
    //total_sec_temp %= 3600;
    //time.minute = (uint8) (total_sec_temp / 60);
    //time.second = (uint8) (total_sec_temp % 60);
}

void format_filename(void)
{
    memset(g_record_vars.location.filename, 0, 12);
    strcpy(g_record_vars.location.filename, FilenameHead); //UREC001.MP3
    itoa3(g_record_vars.wav_num, &g_record_vars.location.filename[4]);
    strcat(g_record_vars.location.filename, " ");
    strcat(g_record_vars.location.filename, "WAV");
}

/*
 ********************************************************************************
 * Description : 根据序号生成新的可用文件名
 *
 * Arguments   : location->filename, 原来的文件名
 *               location->filename, 输出新的文件名
 *
 * Returns     :  新文件的序号, 01~99
 *        如果01~99 文件存在, 返回0
 *
 * Notes       :文件名字格式 : xxx000 WAV, xxx000 ACT, ***000 JPG...
 *        如果输入的文件不存在,则直接返回
 检查目录满最长时间15s
 顺序产生下一个目录名最长时间1s.
 *
 * TODO:        不要使用 atoi(), 节省128byte  (ok!)
 ********************************************************************************
 */
int GenNewName_forUrec(file_location_urec_t *location)
{
    int num;
    int loops = 0;
    uint8 new_name[12];
    handle f;

    //复制到new_name
    memcpy(new_name, location->filename, 12);

    // 检查文件夹是否含有001这个文件,如没有RETURN 1
    new_name[4] = '0';
    new_name[5] = '0';
    new_name[6] = '1';
    // 如果文件不存在
    f = FS_FOpen(new_name, 0);
    if (f == NULL)
    {
        memcpy(location->filename, new_name, 11);
        location->filename[11] = 0;
        // 删除文件长度为0的文件.
        //文件名已经存在，但文件长度为0时open也失败。
        FS_FRemove(location->filename);
        return 1;
    }
    else
    {
        FS_FClose(f);
    }

    // 按指定的值,继续查找.
    memcpy(new_name, location->filename, 12);
    num = (new_name[4] - '0') * 100 + (new_name[5] - '0') * 10 + (new_name[6] - '0');

    if ((num <= 0) || (num > 200))
    {
        num = 1;
        itoa3(num, &new_name[4]);
    }

    //如果文件存在,数字加1
    do
    {
        // 如果文件不存在
        ClearWatchDog();
        f = FS_FOpen(new_name, 0);
        if (f == NULL)
        {
            memcpy(location->filename, new_name, 11);
            location->filename[11] = 0;
            FS_FRemove(location->filename);
            return num;
        }
        else
        {
            FS_FClose(f);
            num++;
            if (num > 200)
            {
                num = 1;
            }
            itoa3(num, &new_name[4]);
        }
        loops++;
    }
    while (loops < 200);

    return 0;
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
    uint16 num = (uint16) GenNewName_forUrec(&g_record_vars.location);

    if (num == 0)
    {
        return FALSE;
    }

    g_record_vars.wav_num = num;
    return TRUE;
}

