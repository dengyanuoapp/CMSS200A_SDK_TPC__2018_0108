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

#pragma name(COM_EQTOOL)

extern void volume_callback(uint16 Vol);

void callback_pc_eq(EQ_Channel_e left_right, uint8 *buf, uint8 aptype)
{
    if (buf == NULL)
    {
        //关闭EQ
        g_decControlInfor.EQFlag = 0x00;
        decEffectDeal();
    }
    else
    {
        if (left_right == Left_EQ)
        {
            //left
            g_decControlInfor.EQFlag = 0x67;
            memcpy(g_decControlInfor.EQVal, (void *) buf, sizeof(g_decControlInfor.EQVal));

            if (aptype == 0) //music ap
            {
                decEffectDeal();
            }
            else
            {
                if (AuCodecRun)
                {
                    decEffectDeal();
                }
            }
        }
        else
        {
            //right
            g_decControlInfor.EQFlag = 0x87;
            memcpy(g_decControlInfor.EQVal, (void *) buf, sizeof(g_decControlInfor.EQVal));

            if (aptype == 0) //music ap
            {
                decEffectDeal();
            }
            else
            {
                if (AuCodecRun)
                {
                    decEffectDeal();
                }
            }
        }
    }
}
void deal_pc_eqsrs_sub(uint8 aptype)
{
    uint8 eqcmd;
    uint8 *usbtestcmd_p;
    uint8 status;

    status = 0;
    usbtestcmd_p = (uint8 *) USBTESTABUFFER;
    eqcmd = *usbtestcmd_p;

    usbtestcmd_p++;
    *usbtestcmd_p = 1; //Status: not ready

    usbtestcmd_p++; // point to infor
    if (eqcmd == EQSET)
    {
        uint8 tmp[7];
        memcpy(tmp, usbtestcmd_p, 7);
        Set_Onechannel_EQ(tmp, 7, Left_EQ, aptype);
        usbtestcmd_p += 7;
        memcpy(tmp, usbtestcmd_p, 7);
        Set_Onechannel_EQ(tmp, 7, Right_EQ, aptype);
    }
    else if (eqcmd == VOLUMESET)
    {

        //add: 外挂PA音量设置
        usbtestcmd_p++;
        volume_callback(*usbtestcmd_p * 32 / 100);
        //}
    }
    else if (eqcmd == EQSAVE)
    {
        status = deal_eqsave_check(usbtestcmd_p);
    }
    else
    {
    }

    //处理完命令
    usbtestcmd_p = (uint8 *) USBTESTABUFFER;
    *usbtestcmd_p = 0x00; //清除cmd
    *(usbtestcmd_p + 1) = status;

    return;
}

bool check_allbuf0(uint8 *buf, uint8 len)
{
    uint8 i;

    for (i = 0; i < len; i++)
    {
        if (buf[i] != 0)
        {
            break;
        }
    }
    if (i >= len)
    {
        return TRUE;
    }
    return FALSE;
}

void Set_Onechannel_EQ(uint8 *buf, uint8 len, EQ_Channel_e left_right, uint8 aptype)
{
    bool isall0;
    isall0 = check_allbuf0(buf, len);
    if (isall0 == TRUE)
    {
        callback_pc_eq(left_right, NULL, aptype);
    }
    else
    {
        callback_pc_eq(left_right, buf, aptype);
    }
}

