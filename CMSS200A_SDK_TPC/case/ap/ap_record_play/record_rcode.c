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

#pragma name(AP_RCODE)

bool recSendCommand(BYTE cmd, void *param)
{
    bool ret;

    if (g_record_vars.rec_type == RECORD_FILE_TYPE_MP2)
    {
        //mp2 record
        ret = mp3rSendCommand(cmd, param);
    }
    else
    {
        //wav record
        ret = recordSendCommand(cmd, param);
    }
    return ret;
}
