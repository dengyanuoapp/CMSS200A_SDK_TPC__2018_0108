/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *       History Record:
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(AP_MAIN)

/* 以下定义FM应用中常驻数据 */

//radio ui 的配置项信息
radio_config_t g_radio_config;

//应用进入模式. 0xff--只是为使FM模组进入standby
uint8 EnterFlag = 0xff;

//自动搜台过程中,当前所搜电台的电台号
uint8 AutoTabNum;

//搜台过程中获取的状态
fm_status_t g_FMStatus;

//硬件搜台标记, bit0  此轮硬件seek 是否结束    1:  已结束   0:  未结束
//bit1  此轮硬件seek 找到的是否有效台   1:  有效台
uint8 seekflag;

//radio 城市电台列表结构信息
radio_citylist_t g_city_station;

/* 指向当前所播放的电台列表表首*/
uint16 *pfreq_play;

//RDS关闭打开状态
radio_RDS_e use_RDS;

//fm_play_status_t FMStatusUS; 	//US电台信息暂存,8 bytes
//fm_play_status_t FMStatusJP; 	//Japan电台信息暂存,8bytes
fm_status_t FMStatusUS =
{ USFREQ_MIN, 0, 0, Mono };
fm_status_t FMStatusJP =
{ JPFREQ_MIN, 0, 0, Mono };

//当前电台信息
FMRADIO_param_t radio_param;

uint8 flag_r; //搜台是否搜到第一个台之后返回标志位

//从VRAM中读出的EQ,SRS参数表，通过EQ工具设置
EQ_VM_t g_eq_para;

int8 AuCodecRun = 0;
int16 SentenceDectTime;
uint8 SentenceDectEnergy;
int16 SentenceDectTime_bak;
int8 c_decEffectFlag;
int8 SetenceFlag = 0;
uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
uint32 m_decSpdifddrSav;

int8 last_SetenceFlag;

#ifdef AP_RADIO_REPORT_EN
char report_buf[16]; //存诸播报数据的buffer
info_report_t info =
{   report_buf, 0, TRUE};
#endif

uint8 report_enter_ap(uint8 report_msg);

/*
 ********************************************************************************
 *			  uint16 main(uint16 param)
 *
 * Description : Radio UI 应用主函数
 *
 * Arguments   :
 param:
 0, 进入FM,进行电台收听
 1, Alarm进入FM
 2, 使FM进入Standby
 *
 * Returns     : 结果消息或0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint16 main(uint16 param)
{
    uint16 result = 0;
    uint8 KeyTone_bak = 0;
    read_var();
#ifdef AP_RADIO_REPORT_EN
    result = report_enter_ap(EXTEND_FM);
    if(result != 0)
    {
        if(param != RESULT_RADIO)
        {
            fmSendCommand(FM_CLOSE, 0, NULL);
        }
        return result;
    }
#endif

    result = FMinit(param); //FM初始化
    if (result == RESULT_MAIN)
    {
        return result;
    }

    if (g_comval.KeyTone != 0)
    {
        KeyTone_bak = g_comval.KeyTone; //备份按键音
        g_comval.KeyTone = 0; //关闭按键音
    }

    result = FMdeal(param); //FM播放
    g_comval.KeyTone = KeyTone_bak; //恢复按键音

    fmSendCommand(FM_MID_SAVE, &radio_param, NULL); //保存变量
    exit_fmin_channel();
    if (result == RESULT_RECORD) //截获FM中所有Rec热键录音
    {
        result = RESULT_FMREC_START;
#ifdef AP_RADIO_REPORT_EN
        fmSendCommand(FM_MUTE, SetMUTE, NULL);
        result = report_enter_ap(EXTEND_RECORD);
        //init_fmin_channel();
        if(result == 0)
        {
            //return result;
            result = RESULT_FMREC_START;
        }
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //解除静音
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
#endif
    }
    FMexit(result); //FM退出处理

    return result;
}
/*
 ********************************************************************************
 *             void charge_pump_onoff(uint8 flag)
 *
 * Description : 打开和关闭charge pump
 *
 *
 * Arguments   : flag:0-关闭；1-打开
 *
 * Returns     : void
 *
 * Notes       :由于打开charge pump时，影响搜台效果，所以在自动和关自动搜索电台时
 *                 建议关闭charge pump,搜完台之后再打开,由于uhost依靠charge pump供电，关闭以后
 *                 如果插着usb，则会误发u盘插入消息，所以在搜台过程中需要过滤u盘插入消息
 *		         同时，由于重新打开charge pump后，系统检测到uhost消息需要一定的时间，所以得
 *                 延时至少500个ms才能正常检测，然后再过滤u盘消息
 *
 ********************************************************************************
 */

void charge_pump_onoff(uint8 flag)
{
    uint8 sfr_bak = SFR_BANK;
    //enable charge pump
    SFR_BANK = BANK_PMU;
    if(flag == 1)
    {
        CP_CTL0 |= 0x01; //电荷泵使能
        CP_CTL1 |= 0x08;	//charge pump直通功能使能
    }
    else
    {
        CP_CTL0 &= 0xFE; //电荷泵关闭
        CP_CTL1 &= 0xF7;
    }
    SFR_BANK = sfr_bak;
    //这里的延时是为了正确检测到u盘消息，以便过滤
    if(flag == 1)
    {
        TM_DelayMS(200);
        TM_DelayMS(200);
        TM_DelayMS(200);
    }

}
uint16 FMinit(uint16 param)
{
    uint16 result = RESULT_NULL;

    SetPLL(PLL_24MHZ);

    if ((param == FM_TO_STANDBY) || (param == RESULT_FMREC2STANDBY)) //让FM模组进standby
    {
        fmSendCommand(FM_OPEN, SEARCHLEVEL, NULL); //fmSendCommand(MC_OPEN, SEARCHLEVEL);
        fmSendCommand(FM_MUTE, SetMUTE, NULL); //fmSendCommand(MC_MUTE, SetMUTE);
        fmSendCommand(FM_CLOSE, 0, NULL); //fmSendCommand(MC_CLOSE, 0);
        SetPLL(PLL_48MHZ);
        return RESULT_MAIN;
    }

    //-------安装ud驱动------------
    DRV_ActiveUD(0x00, MODE_SLOW);
    //OpenSPK(sfr_bak);
    EnterFlag = 0; //when fm  open,  open PA
    mute_flag = FALSE;

    SetCurUI_Type(FMFreq_UI);

    if (param == ALARM_TO_FM) //设置闹铃频率
    {
        alarmtimerflag = 0;
    }

    return result;
}

void read_var(void)
{

    read_ap_comval();
    //   VMRead(&g_alarm, VM_ALARM, sizeof(g_alarm)); //读取闹钟全局变量

    VMRead(&g_radio_config, VM_AP_RADIO, sizeof(g_radio_config)); //读取配置信息
    if (g_radio_config.magic != MAGIC_FMRADIO) //判断是否需要初始化
    {
        memset(&g_radio_config, 0x0, sizeof(g_radio_config));
        g_radio_config.magic = MAGIC_FMRADIO;
        g_radio_config.band_mode = Bank_US_Europe; //默认 普通频道
        memcpy(&g_radio_config.FMStatus, &FMStatusUS, sizeof(fm_status_t)); //加载 普通频道 的数据

        memcpy(&g_FMStatus, &FMStatusUS, sizeof(fm_status_t)); //加载 普通频道 的数据

        //默认为使用RDS  电台功能
        use_RDS = RDS_Open;
    }
}

void FMexit(uint16 result)
{
    if ((result != RESULT_FMREC_NOSTART) && (result != RESULT_FMREC_START)) //非电台录音退出！for FM record by ccm
    {
        //DealFadeInOut();
        fmSendCommand(FM_CLOSE, 0, NULL);
    }
    //TM_KillClock((int8) Timer_Sav); //关闭定时器
    //若是电台录音退出，此时FM还在工作，则不能对HOSC进行操作
    VMWrite(&g_radio_config, VM_AP_RADIO, sizeof(radio_config_t));
    VMWrite(&g_comval, VM_SYSTEM, sizeof(ap_comval_t));
    //    VMWrite(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));

    SetPLL(PLL_48MHZ);
}

/* 保存电台*/
uint16 fm_menu_save(uint16 *arg)
{
    arg = arg;
    if (fmSendCommand(FM_MID_SAVESTATION, NULL, NULL) != FALSE) //保存当前电台对应的电台表项号。
    {
        fmSendCommand(FM_MID_SAVE, &radio_param, NULL); //保存当前电台
    }
    return RESULT_NULL;
}

/* 电台录音*/
uint16 fm_menu_record(uint16 *arg)
{
    arg = arg;
    return RESULT_FMREC_NOSTART; //RESULT_FMREC_START;
}

/* 删除电台*/
uint16 fm_menu_delete(uint16 *arg)
{
    arg = arg;
    if (fmSendCommand(FM_MID_DELETE, NULL, NULL) != FALSE)
    {
        fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    }
    fmSendCommand(FM_SEARCH, &g_FMStatus, UP);
    fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
    return RESULT_NULL;
}

/* 删除全部*/
uint16 fm_menu_delete_all(uint16 *arg)
{
    arg = arg;
    fmSendCommand(FM_MID_DELALL, NULL, NULL);
    fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    return RESULT_NULL;
}

/* 普通频段*/
uint16 fm_menu_normal_band(uint16 *arg)
{
    arg = arg;
    if (g_radio_config.band_mode == Bank_Japan)
    {
        g_radio_config.band_mode = Bank_US_Europe;
        radio_param.param1 = g_radio_config.fmstation_us; //US Tab  by ljs

        memcpy(&FMStatusJP, &g_FMStatus, sizeof(g_FMStatus)); //备份jp频道数据
        memcpy(&g_FMStatus, &FMStatusUS, sizeof(g_FMStatus)); //加载us频道数据
    }
    return RESULT_NULL;
}

/* 日本频段*/
uint16 fm_menu_japan_band(uint16 *arg)
{
    arg = arg;
    //如果原来是普通模式，就需要到日本模式
    if (g_radio_config.band_mode == Bank_US_Europe)
    {
        g_radio_config.band_mode = Bank_Japan;
        radio_param.param1 = g_radio_config.fmstation_jp; //JP Tab   by ljs

        memcpy(&FMStatusUS, &g_FMStatus, sizeof(fm_status_t)); //备份us频道数据
        memcpy(&g_FMStatus, &FMStatusJP, sizeof(fm_status_t)); //加载js频道数据
    }
    return RESULT_NULL;
}

/* RDS 功能*/
uint16 fm_menu_rds(uint16 *arg)
{
    arg = arg;
    return RESULT_NULL;
}

/* FM 录音*/
uint16 fm_menu_fm_record(uint16 *arg)
{
    arg = arg;

    return 0;
}

/* 退出*/
uint16 fm_menu_exit(uint16 *arg)
{
    arg = arg;
    //不需要处理直接退出
    return RESULT_NULL;
}

#ifdef AP_RADIO_REPORT_EN
uint8 report_enter_ap(uint8 report_msg)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS != 0)
    {
        report_buf[0] = ' ';
        report_buf[1] = report_msg;
        report_buf[2] = ' ';
        report_buf[3] = 0;
        check_report_state(&info);
        retval = wait_report_end(NULL);
    }
    return retval;
}
#endif
