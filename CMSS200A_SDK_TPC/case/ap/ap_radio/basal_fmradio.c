/*
 *******************************************************************************
 *                ACTOS AP
 *        FM tunner basal module implementation
 *
 *        (c) Copyright Actions Co,Ld.
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(MOD_BS_FMRADIO)

uint16 m_FMStationTab[MAX_STATION_COUNT]; //电台频率对照表(注:频率范围87M-108M 以K为精度).

static uint8 cmd_fm_mid_findstation(void *param1, void *param2);

/*********************************************************
 *input:  buf---ptr point to buffer which is wait to deal
 *          curfreq----current freq which will be inserted to buffer
 *          totalnum---current total freq num in freq list
 *output: no
 *Description:  insert current freq into freq list
 *note:  freq list from small to large
 **********************************************************
 */
bool SequenceFreqlist(uint16 *buf, uint16 curfreq, uint8 totalnum)
{
    uint8 i, cur_pos;
    uint16 *dealbuf;
    int8 j;

    dealbuf = buf;
    cur_pos = 0;

    for (i = 0; i < MAX_STATION_COUNT; i++)
    {
        //dealbuf 的数据是从小到大排序

        if (curfreq < (*dealbuf)) //小与当前指向的，则把当前以后的往后移，并插入到当前
        {
            cur_pos = i;
            if (cur_pos == (MAX_STATION_COUNT - 1)) //如果是最后一项，直接替换
            {
                *dealbuf = curfreq;
                g_FMStatus.station = i + 1;
                break;
            }

            for (j = (MAX_STATION_COUNT - 2); j >= (int8) cur_pos; j--) //从小到大排序，插入中间，需要把后面的数据往后移一位
            {
                *(buf + j + 1) = *(buf + j);
            }
            *dealbuf = curfreq; //插入在中间
            g_FMStatus.station = i + 1;

            break;
        }
        else if (curfreq == (*dealbuf))
        {
            return FALSE; //有重复，不更新表
        }
        else
        {
            if ((*dealbuf == 0) && (i < totalnum)) //检测到最后了，直接插入到结尾
            {
                *dealbuf = curfreq;
                g_FMStatus.station = i + 1;
                break;
            }
            dealbuf++;
        }
    }
    return TRUE;
}

/*
 ********************************************************************************
 *             void FM_VolumeSet(uint8 Vol)
 *
 * Description : 设置PA音量
 *
 * Arguments   :	  需设置的音量值
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void FM_VolumeSet(uint8 Vol)
{
    set_dac_volume(Vol);
}

//安装fm驱动, 进行模组初始化
uint8 cmd_fm_open(void *param1, void *param2)
{
    //形参确认OK
    //param1 常数，暂时没用到的，仅仅传递进去
    uint8 mode = (uint8) param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //将常驻代码切换进来
    return FMdrv_Init(mode);
}

//模组进入standby, 并卸载fm驱动
uint8 cmd_fm_close(void *param1, void *param2)
{
    //形参确认OK
    param1 = param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //将常驻代码切换进来
    return FMdrv_Standby();
}

//FM 硬件相关设置
uint8 cmd_fm_hardware(void *param1, void *param2)
{
    //形参确认OK
    uint8 onoff = (uint8) param1; //onoff:  1--打开FM时    0--关闭FM时
    uint8 PA_mode = (uint8) param2; //PA_mode:   1-- 要操作PA    0--不操作PA

    BankSwitch((uint32) FMdrv_mute); //将常驻代码切换进来
    return FMdrv_hardware(onoff, PA_mode);
}

//设置单个频点播放
uint8 cmd_fm_setfreq(void *param1, void *param2)
{
    //形参确认OK
    uint8 result;
    uint16 Freq = (uint16) param1;
    param2 = param2;

    result = FMdrv_setfreq((void *) Freq);

    return result;
}

//FM静音或解除静音
uint8 cmd_fm_mute(void *param1, void *param2)
{
    //形参确认OK
    FM_MUTE_e mode = (uint8) param1;
    param2 = param2;
    return FMdrv_mute(mode);
}

//设置波段
uint8 cmd_fm_setband(void *param1, void *param2)
{
    //形参确认OK
    radio_band_e band = (uint8) param1;
    param2 = param2;
    return FMdrv_setband(band);
}

//设置搜台门限
uint8 cmd_fm_setth(void *param1, void *param2)
{
    //形参确认OK
    uint8 level = (uint8) param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //将常驻代码切换进来
    return FMdrv_setthrod(level);
}

//设置软件搜台
uint8 cmd_fm_search(void *param1, void *param2)
{
    //形参确认OK
    fm_status_t *sptr = (fm_status_t *) param1;
    uint8 SeekDirect = (*(uint8 *) param2);

    return FMdrv_search(sptr->freq, SeekDirect);
}

//启动seek (硬件搜台)
uint8 cmd_fm_seek(void *param1, void *param2)
{
    //形参确认OK
    uint16 start = (uint16) param1;
    uint16 stop = (uint16) param2;
    uint8 step = 1;
    return FMdrv_seek(start, stop, step);
}

//获取seek状态
uint8 cmd_fm_getseekflag(void *param1, void *param2)
{
    //形参确认OK
    uint8 *flag = param1;
    param2 = param2;
    return FMdrv_getseekflag(flag);
}

//退出seek
uint8 cmd_fm_breakseek(void *param1, void *param2)
{
    //形参确认OK
    param1 = param1;
    param2 = param2;
    return FMdrv_breakseek(); //退出硬件搜台
}

//获取当前频率值
uint8 cmd_fm_getfreq(void *param1, void *param2)
{
    //形参确认OK
    uint8 result;
    uint16 *pfreg = param1;
    param2 = param2;
    result = FMdrv_getfreq(pfreg);
    //根据电台频率找到电台号
    cmd_fm_mid_findstation(NULL, NULL);
    return result;
}

//获取当前电台信号强度
uint8 cmd_fm_getintensity(void *param1, void *param2)
{
    //形参确认OK
    uint8 *pvalue = param1;
    param2 = param2;
    return FMdrv_getintensity(pvalue);
}

//获取当前电台状态
uint8 cmd_fm_getstatus(void *param1, void *param2)
{
    //形参确认OK
    uint8 result;
    fm_status_t *pBuf = param1; //FMStatus_Tab_t 结构体类型也可以
    param2 = param2;
    result = FMdrv_getstatus(pBuf);
    //根据电台频率找到电台号
    cmd_fm_mid_findstation(NULL, NULL);
    return result;
}

//播放电台
uint8 cmd_fm_mid_play(void *param1, void *param2)
{
    //形参确认OK
    uint16 Tempfreq;

    param1 = param1;
    param2 = param2;

    //FMcmd(FM_MUTE, SetMUTE,NULL);   //静音
    fmSendCommand(FM_MUTE, SetMUTE, NULL);

    {
        Tempfreq = g_FMStatus.freq; //(g_FMStatus.freq << 6) + 0x0010;
    }

    FMdrv_setfreq(Tempfreq);

    //应该延时一下
    ap_sleep(1);

    FMdrv_mute(releaseMUTE); //解除静音

    return 0;
}

//保存当前电台
uint8 cmd_fm_mid_save(void *param1, void *param2)
{
    //即旧代码里的:MC_BACKUP 命令

    param2 = param2;

#ifndef PC
    memcpy(((FMRADIO_param_t *) param1)->param1, m_FMStationTab, sizeof(m_FMStationTab));
    memcpy(((FMRADIO_param_t *) param1)->param2, &g_FMStatus, sizeof(fm_status_t));
#else
    memcpy((void *)param1, m_FMStationTab, sizeof(m_FMStationTab));
    memcpy((void *)(param1 + 2), &g_FMStatus, sizeof(fm_status_t));
#endif
    return 0;
}

//保存当前电台对应的电台表项号。
uint8 cmd_fm_mid_savestation(void *param1, void *param2)
{
    //uint8 i,k;
    uint8 result = FALSE;
    param1 = param1;
    param2 = param2;

    if (g_FMStatus.station_count >= MAX_STATION_COUNT)
    {
        return FALSE;
    }

    //把电台频率保存到电台表，如果已经存在，会返回失败
    g_FMStatus.station_count++;
    if (SequenceFreqlist(m_FMStationTab, g_FMStatus.freq, g_FMStatus.station_count) == FALSE)
    {
        g_FMStatus.station_count--; //添加失败
        return FALSE;
    }

    return TRUE;

}

//查找当前电台对应的电台表项号。
//保存在 g_FMStatus.station 里
uint8 cmd_fm_mid_findstation(void *param1, void *param2)
{
    uint8 i = g_FMStatus.station_count;
    param1 = param1;
    param2 = param2;
    while (i--)
    {
        if (g_FMStatus.freq == m_FMStationTab[i])
        {
            g_FMStatus.station = (i + 1);
            return 0;
        }
    }
    g_FMStatus.station = 0;
    return 0;
}

//根据电台表项号加载频率。
uint8 cmd_fm_mid_loadfreq(void *param1, void *param2)
{
    uint8 i = g_FMStatus.station_count;
    param1 = param1;
    param2 = param2;
    if (g_FMStatus.station_count != 0)
    {
        g_FMStatus.freq = m_FMStationTab[g_FMStatus.station - 1];
    }
    return 0;
}

//还原当前电台
uint8 cmd_fm_mid_restore(void *param1, void *param2)
{
    param2 = param2;

#ifndef PC
    memcpy(m_FMStationTab, ((FMRADIO_param_t *) param1)->param1, sizeof(m_FMStationTab));
    memcpy(&g_FMStatus, ((FMRADIO_param_t *) param1)->param2, sizeof(fm_status_t));
#else
    memcpy(m_FMStationTab, (void *)param1, sizeof(m_FMStationTab));
    memcpy(&g_FMStatus, (void *)(param1 + 2), sizeof(fm_status_t));
#endif
    return 0;
}

//切换下一个电台
uint8 cmd_fm_mid_turn(void *param1, void *param2)
{
    uint8 i;

    //param1 为 UP 或 DOWN
    param2 = param2;

    if (g_FMStatus.station_count >= 1) //有电台
    {
        if (g_FMStatus.station) //电台已经保存了
        {
            if (param1 == UP)
            {
                g_FMStatus.station++;
                if (g_FMStatus.station > g_FMStatus.station_count)
                {
                    g_FMStatus.station = 1;
                }
            }
            else if (param1 == DOWN)
            {
                g_FMStatus.station--;
                if (g_FMStatus.station == 0)
                {
                    g_FMStatus.station = g_FMStatus.station_count;
                }
            }
        }
        else
        {
            //电台还没保存，应该先求出电台在哪个台与台之间
            i = g_FMStatus.station_count;
            while (i)
            {
                //电台从小到大排序的，先找到小于此频道的电台
                if (g_FMStatus.freq > m_FMStationTab[--i])
                {
                    i++;
                    break;
                }
            }
            //i == 0 时，频率比任何一个电台还低
            //i == g_FMStatus.station_count ,则比任何一个电台还高
            //否则电台在 i 与 i+ 1 之间
            if (param1 == UP)
            {
                if (i == g_FMStatus.station_count)
                {
                    g_FMStatus.station = 1;
                }
                else
                {
                    g_FMStatus.station = i + 1;
                }
            }
            else if (param1 == DOWN)
            {
                if (i == 0)
                {
                    g_FMStatus.station = g_FMStatus.station_count;
                }
                else
                {
                    g_FMStatus.station = i;
                }
            }

        }
        g_FMStatus.freq = m_FMStationTab[g_FMStatus.station - 1];

        FMdrv_setfreq((void *) g_FMStatus.freq);

        return TRUE;
    }

    return FALSE;
}

//删除当前电台
//以g_FMStatus.station 为基准来删掉
//如果g_FMStatus.station 为没保存，则以 g_FMStatus.freq 为基准
uint8 cmd_fm_mid_delete(void *param1, void *param2)
{
    //即旧代码里的:MC_DELONE 命令

    uint8 k;

    param1 = param1;
    param2 = param2;

    if (g_FMStatus.station == 0)
    {
        cmd_fm_mid_findstation(NULL, NULL); //如果电台没保存，确认是否真的没保存
        if (g_FMStatus.station == 0)
        {
            return FALSE;
        }
    }
    //m_FMStationTab[g_FMStatus.station-1]=0;
    //for (k = g_FMStatus.station; k < (g_FMStatus.station_count + 1); k++)
    for (k = g_FMStatus.station; k < (g_FMStatus.station_count); k++)
    {
        m_FMStationTab[k - 1] = m_FMStationTab[k]; //中间需要删除，所以后面的需要前移一位
    }
    g_FMStatus.station_count--;
    m_FMStationTab[g_FMStatus.station_count] = 0;
    g_FMStatus.station = 0;

    return TRUE;
}

//删除全部电台
uint8 cmd_fm_mid_delall(void *param1, void *param2)
{
    //即旧代码里的:MC_DELALL 命令
    uint8 i;

    param1 = param1;
    param2 = param2;

    for (i = 0; i < MAX_STATION_COUNT; i++)
    {
        m_FMStationTab[i] = 0;
    }
    g_FMStatus.station = 0;
    g_FMStatus.station_count = 0;

    return 0;
}

//确定所选城市的第一个城市电台
uint8 cmd_fm_mid_selcityinit(void *param1, void *param2)
{
    param1 = param1;
    param2 = param2;

    return 0;
}

//在所选城市中上下切换城市电台
uint8 cmd_fm_mid_cityswitch(void *param1, void *param2)
{
    param1 = param1;
    param2 = param2;
    return 0;
}

//设置声音
uint8 cmd_fm_mid_setvolume(void *param1, void *param2)
{
    uint8 Vol = (uint8) param1;
    param2 = param2;

    FM_VolumeSet(Vol);
    return 0;
}

//电台频率微调，以100KHZ为步进
uint8 cmd_fm_mid_stepgrid(void *param1, void *param2)
{
    uint16 Tempfreq;

    uint8 dir = (uint8) param1;

    param2 = param2;

    switch (dir)
    {
    case UP:
        if (((g_FMStatus.freq + 1) > USFREQ_MAX) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MIN;
        }
        else if (((g_FMStatus.freq + 1) > JPFREQ_MAX) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MIN;
        }
        else
        {
            g_FMStatus.freq += 1;
        }
        break;
    case DOWN:
        if ((g_FMStatus.freq == USFREQ_MIN) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MAX;
        }
        else if ((g_FMStatus.freq == JPFREQ_MIN) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MAX;
        }
        else
        {
            g_FMStatus.freq -= 1;
        }
        break;
    default:
        break;
    }

    Tempfreq = g_FMStatus.freq; //(g_FMStatus.freq << 6) + 0x0010;

    FMdrv_setfreq(Tempfreq);
    return TRUE;
}

const pfunc_cmd far fm_cmd_pfunc[CMD_COUNT] =
{
    /* FM 中间层命令宏, 部分涉及到调用FM驱动接口 */
    cmd_fm_open, //安装fm驱动, 进行模组初始化
    cmd_fm_close, //模组进入standby, 并卸载fm驱动
    cmd_fm_hardware, //FM 硬件相关设置
    cmd_fm_setfreq, //设置单个频点播放
    cmd_fm_mute, //FM静音或解除静音
    cmd_fm_setband, //设置波段
    cmd_fm_setth, //设置搜台门限
    cmd_fm_search, //设置软件搜台
    cmd_fm_seek, //启动seek (硬件搜台)
    cmd_fm_getseekflag, //获取seek状态
    cmd_fm_breakseek, //退出seek

    cmd_fm_getfreq, //获取当前频率值
    cmd_fm_getintensity, //获取当前电台信号强度
    cmd_fm_getstatus, //获取当前电台状态

    //中间逻辑层:与FM驱动无关
    cmd_fm_mid_play,
    cmd_fm_mid_save, //保存当前电台
    cmd_fm_mid_savestation,
    cmd_fm_mid_findstation, //查找当前电台对应的电台表项号。
    cmd_fm_mid_loadfreq, //根据电台表项号加载频率。
    cmd_fm_mid_restore, //还原当前电台
    cmd_fm_mid_turn, //换台
    cmd_fm_mid_delete, //删除当前电台
    cmd_fm_mid_delall, //删除全部电台
    cmd_fm_mid_selcityinit, //确定所选城市的第一个城市电台
    cmd_fm_mid_cityswitch, //在所选城市中上下切换城市电台

    cmd_fm_mid_setvolume, //设置声音
    cmd_fm_mid_stepgrid
};

/*
 ********************************************************************************
 *             uint8 FMcmd(uint8 cmd, uint16 param1, void *param2)
 *
 * Description : FM命令处理
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 fmSendCommand(radio_cmd_e cmd, void *param1, void *param2)
{
    uint8 ret = 0;
    if (cmd < CMD_COUNT)
    {
        ret = (fm_cmd_pfunc[(uint16) cmd]) (param1, param2);
    }

    return ret;
}

