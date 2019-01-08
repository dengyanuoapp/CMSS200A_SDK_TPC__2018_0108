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

#pragma name(COM_EQTOOL_RCODE)

void deal_pc_eqsrs(uint8 aptype)
{
    uint8 *usbtestcmd_p;
    uint8 eqcmd;

    usbtestcmd_p = (uint8 *) USBTESTABUFFER;
    eqcmd = *usbtestcmd_p;

    if ((eqcmd == EQSET) || (eqcmd == SRSSET) || (eqcmd == VOLUMESET) || (eqcmd == EQSAVE) || (eqcmd == SRSSAVE))
    {
        deal_pc_eqsrs_sub(aptype);
    }
    return;
}
