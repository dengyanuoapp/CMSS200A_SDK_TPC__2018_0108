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

#pragma name(MW_BS_AD_CMD2_SD)

extern HANDLE m_deSd_Fp;
extern Open_param_t *m_deSdOpenparam; //参数表地址

//extern uint32 deSdApointerSave;
//extern uint32 deSdBpointerSave;
extern uint32 deSdBreakPointSave;
//extern decBreakPointInfor_t m_SdbreakPointInfo;
//extern BYTE m_Sdbackupmode;

#pragma renamecode(MW_BS_AD_CMD2_SD)
/*
 void reserve_function(void)
 {

 }
 */
bool SdBackupCommand(void)
{
    //    m_deSdOpenparam->BreakPTSave->ApointSave = deSdApointerSave;
    //    m_deSdOpenparam->BreakPTSave->BpointSave = deSdBpointerSave;
    m_deSdOpenparam->BreakPTSave->breakPT = deSdBreakPointSave;
    //    if (m_Sdbackupmode == 0)//断点续播备份
    //    {
    //        memcpy(&(m_deSdOpenparam->BreakPTSave->breakPointInfor), &g_decBreakPointInfor, sizeof(decBreakPointInfor_t));
    //    }
    //    else//AB 复读过程中的断点不保存，只是保存A点，以后断点续播从A点开始
    //    {
    //        memcpy(&(m_deSdOpenparam->BreakPTSave->breakPointInfor), &m_SdbreakPointInfo, sizeof(decBreakPointInfor_t));
    //    }
    return TRUE;
}

bool SdRestoreCommand(void)
{
    //    deSdApointerSave = m_deSdOpenparam->BreakPTSave->ApointSave;
    //    deSdBpointerSave = m_deSdOpenparam->BreakPTSave->BpointSave;
    deSdBreakPointSave = m_deSdOpenparam->BreakPTSave->breakPT;
    //    memcpy(&m_SdbreakPointInfo, &(m_deSdOpenparam->BreakPTSave->breakPointInfor), sizeof(decBreakPointInfor_t));
    return TRUE;
}

//bool SdAPointPlayCommand(void)
//{
//    g_decControlInfor.PlayMode = PLAY_MODE_BREAK;
//    memcpy(&g_decBreakPointInfor, &m_SdbreakPointInfo, sizeof(decBreakPointInfor_t));
//    SD_FSeek(0, FS_SEEK_FFROMSTART, m_deSd_Fp);
//    decInit();//计算时间不淡入
//    decstart();
//    return TRUE;
//}
