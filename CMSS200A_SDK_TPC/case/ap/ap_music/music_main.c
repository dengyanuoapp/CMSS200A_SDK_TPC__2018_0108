/*
 ******************************************************************************
 *                               AS211A
 *                            Module: music
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: main module of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-15 15:00     1.0             build this file
 ******************************************************************************
 */
#include "ap_music.h"

#pragma name(AP_MAIN)
/*常量段定义*/
uint8 g_TagInfoBuf[92];//用于放TAG信息的BUF
uint16 g_musicamount = 0;//当前目录歌曲总数
uint16 g_musicsequence = 0;//当前歌曲序号
/*全局数据段定义*/
bool g_PAOpenFlag = FALSE; //标志PA的开关状态
bool Read_VMflag = TRUE;//FALSE:不读VM ,TRUE:读VM
music_vars_t g_music_vars;//本AP的公共变量
file_location_t CurUselocation;//当前正在使用的location信息
Open_param_t g_Openparam;
Play_status_t PlayStatus = StopSta; //停止状态
Play_status_t prev_PlayStatus = StopSta; //上一次的播放状态

Music_type_t g_currentMusicType = Music_NON;//标志当前音乐的类型(mp3/wma/asf/wmv)

uint8 FF_flag;//当前处理快进状态标记
uint8 get_fileinfo_flag;//是否需要获取文件信息标志
uint8 auto_switch_device_flag;//自动切换磁盘标志
uint8 no_device_flag;//无磁盘或磁盘无文件标志
uint16 dir_no;//当前目录序号
uint8 scan_flag = TRUE;//是否需要扫描全盘标志
//记录上一次的按键，主要是区分上一次是next还是prev,以
//便在切换上一曲遇到非法文件时可以继续向上切换，而不是继续切换到下一首
uint8 prev_key_direc = Key_Next;
uint8 micin_status;//mic是否插入状态。0-不存在mic.1-mic已经插着
uint16 err_music_count;//出错歌曲计数
uint16 max_music_err = MAX_ERR_MUSIC_NUM; //最大歌曲出错数
uint8 g_power_flag = 0;//是否突然断电标志
uint8 first_init = TRUE;//卡和u盘切换情况，防止卡和u盘都没文件时进入不断切换状态

uint8 disk_char[MAX_DISK_NUM] = //存储盘符信息,分别为u盘，卡盘，卡base
{ 'U', 'H', 'C' };
uint8 load_str[5] =
{ "LOAD" };
uint8 err_str[5] =
{ "ERRZ" };
uint8  support_fav = 0;          //是否支持收藏夹,默认不支持
EQ_VM_t g_eq_para;

//for analog mic
int8 AuCodecRun = 0;
int16 SentenceDectTime;
uint8 SentenceDectEnergy;
int16 SentenceDectTime_bak;
int8 c_decEffectFlag;
int8 SetenceFlag = 0;
uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
uint32 m_decSpdifddrSav;

int16 main(int16 param)
{
    int16 main_result;
    bool card_exist = FALSE, uhost_exist = FALSE;
    auto_switch_device_flag = 1;
    /* 读入公共变量 */
    read_var();
    read_VM();
    //    SetPAVolume(g_comval.volume);
    if (param == RESULT_MUSIC)
    {
        if (g_music_vars.cur_disk == disk_char[1])
        {
            param = RESULT_MUSIC_CPLAY;
        }
        else
        {
            param = RESULT_MUSIC_UPLAY;
        }
    }
    if (param == RESULT_MUSIC_CPLAY)
    {
        /* 选择卡盘播放 */
        if (DRV_DetectUD(1) == 0x20)
        {
            CurUselocation.disk = disk_char[1];
        }
        else if (USBH_Check_Udisk())
        {
            CurUselocation.disk = disk_char[0];
        }
        else
        {
            return RESULT_MAIN;
        }
    }
    else
    {
        /* 选择U盘播放 */
        if (USBH_Check_Udisk())
        {
            CurUselocation.disk = disk_char[0];
        }
        else if (DRV_DetectUD(1) == 0x20)
        {
            CurUselocation.disk = disk_char[1];
        }
        else
        {
            return RESULT_MAIN;
        }
    }
    g_music_vars.cur_disk = CurUselocation.disk;

    CurUselocation.fselTotal = 0;
    CurUselocation.ID3buffer = g_TagInfoBuf;
    g_Openparam.filename = CurUselocation.filename;//文件名地址
    g_Openparam.BreakPTSave = &g_music_vars.BreakPTSave;//备份参数地址
    g_Openparam.typeerror = 0;//默认为正常格式文件
    g_Openparam.SoftVolumeMax = 0;
    g_Openparam.FadeInTime = 0x03;

    g_music_vars.fselmod = FSEL_TYPE_COMMONDIR;//每次进入都是设为目录选择器
    LEDClearScreen();
    LEDPuts(NUMBER1, load_str, 1);
    if (InitFileSelector() == 0)
    {
        no_device_flag = TRUE;
        LEDPuts(NUMBER1, err_str, 1);
        ap_sleep(4);
        return RESULT_MAIN;
    }
    else
    {
        PlayStatus = StopSta;
        need_draw = TRUE;/* 需要画screen */
        get_fileinfo_flag = TRUE;
    }
    // OpenPA();
    //  openDAC();
    //    SetPAVolume_LR(g_comval.volume, g_comval.volume);
    // init_mic_in_gpio();
    //先过滤所有的消息，以免检测到不必要的消息
    while (ap_get_message() != NULL)
    {
    };
    main_result = (uint16) ui_play();// =============> 进入界面
    stop_music_play(TRUE);
    write_poweroff_flag(0);
    if(mute_flag != 0)
    {
        mute_flag = 0;
        set_dac_volume(g_comval.volume);
    }
    // DisablePA();
    mic_on_flag = 0;
    //    deal_mic_msg();
    g_music_vars.cur_disk = CurUselocation.disk;
    if (CurUselocation.disk == disk_char[0])
    {
        memcpy(&g_music_vars.location[0], &CurUselocation, sizeof(file_location_t));
    }
    else
    {
        memcpy(&g_music_vars.location[1], &CurUselocation, sizeof(file_location_t));
    }
    if(main_result == RESULT_MUSIC_UPLAY)
    {
        main_result = RESULT_MAIN;
        g_music_vars.cur_disk = disk_char[0];
    }
    VMWrite(&g_music_vars, VM_AP_MUSIC_CARD, sizeof(music_vars_t));
    VMWrite(&g_comval, VM_SYSTEM, sizeof(g_comval));
    //    VMWrite(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));
    return main_result;
}

/*
 ********************************************************************************
 *             void read_var(void)
 *
 * Description : 读系统变量和ap局部变量
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */
void read_var(void)
{
    int16 result;
    uint8 sfr_bak;

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_CMU_RMU;
    HOSC_CTL |= 0x06;

    SFR_BANK = 0x0c;
    g_power_flag = RTCRDM22;
    SFR_BANK = sfr_bak;
    //    VMRead(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));
    result = read_ap_comval();

}
/*
 ********************************************************************************
 *             void read_var(void)
 *
 * Description : 读music应用系统变量
 *
 * Arguments   : void
 *
 * Returns     : void
 *
 * Notes       :
 *
 ********************************************************************************
 */

void read_VM(void)
{
    int16 result;
    result = VMRead(&g_music_vars, VM_AP_MUSIC_CARD, sizeof(g_music_vars));
    if (g_music_vars.magic != MAGIC_MUSIC)
    {
        memset(&g_music_vars, 0x0, sizeof(music_vars_t));
        g_music_vars.magic = MAGIC_MUSIC;
        g_music_vars.repeat_mode = FSEL_ALL_REPEATALL;
        g_music_vars.cur_disk = disk_char[0];
        g_music_vars.location[0].disk = disk_char[0];
        g_music_vars.location[1].disk = disk_char[1];
        strcpy(g_music_vars.location[0].path, FS_CD_ROOT);
        strcpy(g_music_vars.location[1].path, FS_CD_ROOT);
        VMWrite(&g_music_vars, VM_AP_MUSIC_CARD, sizeof(g_music_vars));
        ClearTimePara();
        write_poweroff_flag(0);
    }
    if (CurUselocation.disk == disk_char[0])
    {
        memcpy(&CurUselocation, &g_music_vars.location[0], sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPTSave, &g_music_vars.BreakPoit[0], sizeof(breakPT_t));
    }
    else
    {
        memcpy(&CurUselocation, &g_music_vars.location[1], sizeof(file_location_t));
        memcpy(&g_music_vars.BreakPTSave, &g_music_vars.BreakPoit[1], sizeof(breakPT_t));
    }
    g_music_vars.karaoke_flag = 0;

}

void OpenPA(void)
{
    pa_t pa =
    { 0 };

    if (!g_PAOpenFlag)
    {
        g_PAOpenFlag = TRUE;//标志PA已打开

        pa.pa_in.dacin = 1;//默认
        pa.pa_in.micin = 1;
        pa.volume = g_comval.volume;
        pa.reserve[0] = g_comval.volume;
        EnablePA(&pa);
    }
    //openDAC();
}
void openDAC(void)
{
    dac_t dacattr;
    dacattr.dacselect = 0;
    dacattr.rate = FS_44K1;//默认
    EnableDAC(&dacattr);
}
/*
 ********************************************************************************
 * Description : init GPIOC4(check mic in)as input
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */

void init_mic_in_gpio(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    MIC_IN_GPIO_OUTPUT &= ~ (1 << MIC_IN_GPIO_NUM);
    MIC_IN_GPIO_INPUT |=  (1 << MIC_IN_GPIO_NUM);
    SFR_BANK = sfr_bak;
    micin_status = 0;
    mic_on_flag = 0;
    //    check_plug_device_msg(&micin_status, MIC_IN);
}

