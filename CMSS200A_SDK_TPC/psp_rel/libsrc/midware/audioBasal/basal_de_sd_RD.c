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

#pragma name(MW_BS_AD_CMDR_SD)


SD_FILE *m_deSd_Fp;
Open_param_t *m_deSdOpenparam;
//uint32 deSdApointerSave;
//uint32 deSdBpointerSave;
uint32 deSdBreakPointSave; //当前文件断点的地址保存，一个扇区为单位，m_read_date更新
//decBreakPointInfor_t m_SdbreakPointInfo;//只保存一次断点信息，例如A点
uint32 deSdFilepages;//当前播放文件总页数
//BYTE m_Sdbackupmode;
play_status_t Sdplay_status;
BYTE *m_deSdDataBuffer; //[512];//解码时用的BUFFER
uint32 m_deSdOldDspIntAddrSav;//保存旧的INT地址
uint32 m_deSdOldDmaIntAddrSav;
uint8 m_deSdcTimer_Sav;
time_t m_SdTotaltimelength;
//bool m_deSdABok_sav;

#define READ_SECTOR_BYTE_NUMBER 512
#define MOVE_BACKWARD_ONE_PAGE_FLAG  0x22
#define MOVE_FORWARD_ONE_PAGE_FLAG 0x11
#define RANDOM_CHOOSE_ONE_PAGE_FLAG 0x33

#define ONE_PAGE_BITNUM 9

#ifndef PC
#pragma ROM(large)
//void GetSDErrorTYPE(void);
#pragma ROM(huge)
#else
//void GetSDErrorTYPE(void);
//decBreakPointInfor_t g_decBreakPointInfor;
#endif

#pragma renamecode(MW_BS_AD_CMDR_SD)

#pragma ROM(huge)
uint8 m_deSdRead_Data(uchar readFileDirection, uint32 position)
{
    bool result = TRUE;
    int16 readLen = 0;
    uint32 posPage = 0;
    uint32 posDistance = 0;
    uint32 decurpos;
    ClearWatchDog();
    position = position;

    //断点记录的是解码过的信息
    if ((Sdplay_status.status == PLAYING_REACH_END) || (Sdplay_status.status == PLAYING_ERROR) ||
            (Sdplay_status.status == PLAYING_REACH_HEAD))
    {
        return 0xff;
    }

    decurpos = (uint32)(SD_FTell(m_deSd_Fp) - m_deSd_Fp->startaddr); //- 1;
    deSdBreakPointSave = (decurpos >> 9);

    if (deSdBreakPointSave >= deSdFilepages)
    {
        Sdplay_status.status = PLAYING_REACH_END;
        return 0xff;
    }

    if (MOVE_BACKWARD_ONE_PAGE_FLAG == readFileDirection)
    {
        result = SD_FSeek(m_deSd_Fp, SEEK_CUR, -512);
    }
    else if (RANDOM_CHOOSE_ONE_PAGE_FLAG == readFileDirection)
    {
        //result = SD_FSeek(m_deSd_Fp, SEEK_SET, (int32)position);
    }
    else
    {
    }
    if (FALSE == result)
    {
        return FALSE;
    }

    readLen = SD_FRead(m_deSd_Fp, m_deSdDataBuffer, 512);

    deSdBreakPointSave = (uint32)(SD_FTell(m_deSd_Fp));
    g_decReadFilePara.ReadFileCurAddr = deSdBreakPointSave - m_deSd_Fp->startaddr - 0x200;
    if (readLen == 0)
    {
        return FALSE;
    }
    return TRUE;
}

#ifndef PC
#pragma ROM(huge)
#endif
