/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib file, udisk use this file too
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: deal_EQTool_cmd.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */

#include "eqtool.h"

EQ_VM_t far g_eq_vrampara;

//默认EQ音效表
const uint8 defaultEQ[SUPPORT_EQ_CNT][ONE_EQPARA_BYTE] =
{
    0, 0, 0, 0, 0, 0, 0, //none
    5, 3, 0, -1, 0, 4, 5, //rock
    3, 0, 0, 0, 0, -1, -2, //Pop
    0, 0, 0, -1, -1, -2, -4, //Classic
    0, 0, 1, 2, 0, 1, 1, //Soft
    3, 0, 0, -1, 0, 2, 4, //Jazz
    5, 2, 0, 0, 0, 0, 0, //Dbb
    0, 0, 0, 0, 0, 0, 0, //user
};

#pragma name(COM_EQTOOL_SAVE)

uint8 deal_eqsave_check(uint8 *eqbuf)
{
    uint8 eqmode;
    utest_eq_info_t eqinfo;
    BOOL result;
    uint8 ret;
    uint8 *ptr;

    ptr = (uint8 *) &eqinfo;
    eqmode = *eqbuf; // 1:normal 2:Rock 3:Pop 4:Classic 5:Soft 6:Jazz 7:Dbb 8:自定义
    eqbuf++;
    memcpy((void *) &eqinfo, eqbuf, sizeof(utest_eq_info_t)); //get 14 bytes eq para

    result = VMRead(&g_eq_vrampara, VM_PCSET_EQ, sizeof(g_eq_vrampara));
    if (g_eq_vrampara.magic != EQ_VRAM_MAGIC)
    {
        //给VRAM赋初始值
        g_eq_vrampara.magic = EQ_VRAM_MAGIC;
        memcpy(g_eq_vrampara.left_eq_para, defaultEQ[0], ONE_EQPARA_BYTE * SUPPORT_EQ_CNT);
        memcpy(g_eq_vrampara.right_eq_para, defaultEQ[0], ONE_EQPARA_BYTE * SUPPORT_EQ_CNT);
        VMWrite(&g_eq_vrampara, VM_PCSET_EQ, sizeof(g_eq_vrampara));
    }

    if (eqmode <= SUPPORT_EQ_CNT)
    {
        eqmode--; // 0---7
        //left
        memcpy(&g_eq_vrampara.left_eq_para[ONE_EQPARA_BYTE * eqmode], ptr, ONE_EQPARA_BYTE);
        //right
        ptr += ONE_EQPARA_BYTE;
        memcpy(&g_eq_vrampara.right_eq_para[ONE_EQPARA_BYTE * eqmode], ptr, ONE_EQPARA_BYTE);
        //将参数表写入VRAM
        VMWrite(&g_eq_vrampara, VM_PCSET_EQ, sizeof(g_eq_vrampara));
        ret = 0;
    }
    else
    {
        //传递下来的EQ mode不在范围内, 返回status: fail
        ret = 2;
    }

    if (ret != 0)
    {
        return ret;
    }

    //重新读出，比较
    result = VMRead(&g_eq_vrampara, VM_PCSET_EQ, sizeof(g_eq_vrampara));
    if (g_eq_vrampara.magic != EQ_VRAM_MAGIC)
    {
        ret = 2;
        return ret;
    }
    ptr = (uint8 *) &eqinfo;
    if (memcmp(&g_eq_vrampara.left_eq_para[ONE_EQPARA_BYTE * eqmode], ptr, ONE_EQPARA_BYTE) != 0)
    {
        //左声道参数不匹配
        ret = 2;
        return ret;
    }
    ptr += ONE_EQPARA_BYTE;
    if (memcmp(&g_eq_vrampara.right_eq_para[ONE_EQPARA_BYTE * eqmode], ptr, ONE_EQPARA_BYTE) != 0)
    {
        //右声道参数不匹配
        ret = 2;
        return ret;
    }
    return ret;
}
