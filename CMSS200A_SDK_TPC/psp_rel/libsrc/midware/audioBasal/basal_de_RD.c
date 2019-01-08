/********************************************************************************
 *                              USDK 5102
 *                            Module: POWER MANAGER
 *                 Copyright(c) 2001-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *    wuyueqian     2009-9-07 9:42     1.0             build this file
 ********************************************************************************/
/*!
 * file
 * brief
 * Author   wuyueqian
 * par
 *
 *      Copyright(c) 2001-2007 Actions Semiconductor, All Rights Reserved.
 *
 * version 1.0
 * date  2008/6/12
 *******************************************************************************/

#include <basal.h>
#include <decC.h>

#pragma name(MW_BS_AD_CMDR)

HANDLE m_deFileHandle;
Open_param_t *m_deOpenparam;
uint32 deApointerSave;
uint32 deBpointerSave;
uint32 deBreakPointSave; //当前文件断点的地址保存，一个扇区为单位，m_read_date更新
decBreakPointInfor_t m_breakPointInfo;//只保存一次断点信息，例如A点
uint32 deFilepages;//当前播放文件总页数
BYTE m_backupmode;
play_status_t play_status;
BYTE *m_deDataBuffer; //[512];//解码时用的BUFFER
uint32 m_deOldDspIntAddrSav;//保存旧的INT地址
uint32 m_deOldDmaIntAddrSav;
uint8 m_decTimer_Sav;
time_t m_Totaltimelength;
bool m_deABok_sav;

#define READ_SECTOR_BYTE_NUMBER 512
#define MOVE_FORWARD_ONE_PAGE_FLAG 0x11
#define MOVE_BACKWARD_ONE_PAGE_FLAG  0x22
#define RANDOM_CHOOSE_ONE_PAGE_FLAG 0x33
#define ONE_PAGE_BITNUM 9

#ifdef FASTFORWARDBACKWARD
#define MOVE_FORWARD_MORE_PAGE_FLAG 0x91  //  用于加速快进
#define MOVE_BACKWARD_MORE_PAGE_FLAG  0xa2 // 用于加速快进
#endif


#ifndef PC
//#pragma ROM(large)
void GetFSErrorTYPE(void);
void save_bpt_to_rdm(void);
#pragma ROM(huge)
#else
void GetFSErrorTYPE(void);
decBreakPointInfor_t g_decBreakPointInfor;
#endif

const uint8 RTCRDM_table[] = {SFR_RTCRDM0, SFR_RTCRDM1, SFR_RTCRDM2, SFR_RTCRDM3, SFR_RTCRDM4, SFR_RTCRDM5, SFR_RTCRDM6, SFR_RTCRDM7, SFR_RTCRDM8,
                              SFR_RTCRDM9, SFR_RTCRDM10, SFR_RTCRDM11, SFR_RTCRDM12, SFR_RTCRDM13, SFR_RTCRDM14, SFR_RTCRDM15, SFR_RTCRDM16,
                              SFR_RTCRDM17, SFR_RTCRDM18
                             };

#pragma renamecode(MW_BS_AD_CMDR)

//#pragma ROM(large)
#pragma ROM(huge)
uint8 m_deRead_Data(uchar readFileDirection, uint32 position)
{
    bool result = TRUE;
    uint32 posPage = 0;
    uint32 posDistance = 0;
    //CLEAR_WATCHDOG_MIDWARE();
    ClearWatchDog();

    //断点记录的是解码过的信息
    if ((play_status.status == PLAYING_REACH_END) || (play_status.status == PLAYING_ERROR) ||
            (play_status.status == PLAYING_REACH_HEAD))
    {
        return 0xff;
    }

    deBreakPointSave = (uint32)(FS_FTell(m_deFileHandle));
    save_bpt_to_rdm();
    if (deBreakPointSave == deFilepages)
    {
        play_status.status = PLAYING_REACH_END;
        return 0xff;
    }

    if (MOVE_BACKWARD_ONE_PAGE_FLAG == readFileDirection)
    {
        result = FS_FSeek(2, FS_SEEK_BFROMCUR, m_deFileHandle);
    }
#ifdef FASTFORWARDBACKWARD
    else if(readFileDirection == MOVE_BACKWARD_MORE_PAGE_FLAG)
    {
        result = FS_FSeek(16, FS_SEEK_BFROMCUR, m_deFileHandle);	//如果有需要，可以修改扇区数目，便于加速快进退快退。往前seek 2n个扇区，准备读n个扇区
    }
#endif
    else if (RANDOM_CHOOSE_ONE_PAGE_FLAG == readFileDirection)
    {
        posPage = position >> ONE_PAGE_BITNUM;
        if (posPage > deBreakPointSave)
        {
            result = FS_FSeek((int32)(posPage - deBreakPointSave), FS_SEEK_FFROMCUR, m_deFileHandle);
        }
        else if (posPage < deBreakPointSave)
        {
            posDistance = (deBreakPointSave) - posPage;
            if (posDistance < (deBreakPointSave << 1))
            {
                result = FS_FSeek((int32)(posDistance), FS_SEEK_BFROMCUR, m_deFileHandle);
            }
            else
            {
                result = FS_FSeek((int32)(posPage), FS_SEEK_FFROMSTART, m_deFileHandle);
            }

        }
        else
        {
            //do nothing
        }
    }
    else
    {
        //do nothing
    }

    if (FALSE == result)
    {
        GetFSErrorTYPE();
        return FALSE;
    }

    switch (g_decControlInfor.ABSetFlag)
    {
    case AB_FLAG_DEAL_AB_PROCESS:
    {
        if (play_status.status == PLAYING_WAIT_B) //已经到达B点或者超过，等待stop命令
        {
            break;
        }

        if ((play_status.status == PLAYING_AB) && (deBreakPointSave >= deBpointerSave))//放到B点
        {
            if (FALSE != m_deABok_sav) //不用再播放了
            {
                //次数到,清B点
                m_deABok_sav = FALSE;
                play_status.status = PLAYING_PLAYING;
                g_decControlInfor.ABSetFlag = AB_CLEAR;
                g_decControlInfor.PlayMode = PLAY_MODE_NORMAL; //not need
                //play_status.signal = SIGNAL_CLEAR_B;//通知AB要清B点
                play_status.signal = 0;//modify by wuyueqian 2010-4-3
            }
            else
            {
                //次数未到,通知AP回到A点
                play_status.status = PLAYING_WAIT_B;
                play_status.signal = SIGNAL_REACH_B;
            }
        }
        break;
    }

    case AB_FLAG_SET_A: //设置完A点后ab flag为0，不进行AB处理，需等B点也设好后才开始AB播放处理
    {
        deApointerSave = deBreakPointSave;
        g_decControlInfor.ABSetFlag = AB_CLEAR;//其实非AB_FLAG_SET_A和非0XFF就可以了
        memcpy(&m_breakPointInfo, &g_decBreakPointInfor, sizeof(decBreakPointInfor_t));
        break;
    }

    case AB_FLAG_SET_B:
    {
        deBpointerSave = deBreakPointSave;
        play_status.signal = SIGNAL_REACH_B;
        play_status.status = PLAYING_WAIT_B; //只在这个m_deRead_Data函数中有效，StopBPlay后出变成PLAYING_WAIT_A
        g_decControlInfor.ABSetFlag = AB_FLAG_DEAL_AB_PROCESS; //以后检测是否到B点
        break;
    }

    default:
    {
        break;
    }

    }
#ifdef FASTFORWARDBACKWARD
    if((readFileDirection == MOVE_FORWARD_MORE_PAGE_FLAG) || (readFileDirection == MOVE_BACKWARD_MORE_PAGE_FLAG))
    {
        result = FS_FRead(m_deDataBuffer, 8, m_deFileHandle);	//如果有需要，可以修改扇区数目，便于加速快进退快退，每次读n个扇区
    }
    else
    {
        result = FS_FRead(m_deDataBuffer, 1, m_deFileHandle);
    }
#else
    result = FS_FRead(m_deDataBuffer, 1, m_deFileHandle);
#endif

    deBreakPointSave = (uint32)(FS_FTell(m_deFileHandle));
    g_decReadFilePara.ReadFileCurAddr = (deBreakPointSave - 1) * READ_SECTOR_BYTE_NUMBER;

    if (result == 0)
    {
        GetFSErrorTYPE();
        return FALSE;
    }
    return TRUE;
}

/*  FS_GetInfo()
 *描述
 获取文件系统出错信息
 *参数
 *返回
 char status：出错信息
 1   磁盘读写错误
 2   磁盘写保护
 3   磁盘未格式化
 4   文件操作超出文件边界,目录操作超出目录边界
 5   文件操作的目标文件,目录操作的目录项不存在
 6   表示文件操作时没有磁盘空间,不能写数据或者扩展目录
 表示目录操作时没有磁盘空间,不能扩展目录,新建子目录
 7   文件操作时根目录目录项满
 8   删除目录时返回,表示删除的目录非空
 */

void GetFSErrorTYPE(void)
{
    char result;

    result = FS_GetInfo();
    switch (result)
    {
    case FS_INFO_OVEFLOW: //FIX ME
    {
        if (FS_FTell(m_deFileHandle) == 0)
        {
            play_status.status = PLAYING_REACH_HEAD;
        }
        else
        {
            play_status.status = PLAYING_REACH_END;
        }
        break;
    }

    case FS_INFO_RW_ERR:
    {
        play_status.status = PLAYING_ERROR;
        break;
    }

    default:
    {
        break;
    }
    }
}
void save_bpt_to_rdm(void)
{
    uint8 sfr_bak = SFR_BANK, *pbpt;
    int i;
    WriteSFR write_sfr = (WriteSFR)WRITE_SFR_CODE;

    SFR_BANK = 0x0c;
    pbpt = (uint8 *)&g_decBreakPointInfor;
    for(i = 0; i < sizeof(g_decBreakPointInfor); i++)
    {
        write_sfr(0x0c, RTCRDM_table[i], pbpt[i]);
    }
    SFR_BANK = sfr_bak;
}
#ifndef PC
#pragma ROM(huge)
#endif
