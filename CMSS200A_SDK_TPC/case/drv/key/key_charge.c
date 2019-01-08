#include "drv_key.h"


#pragma	name(BAC_CHARGE)
uint8 *KY_GetKeyTabAddress(uint8 api_no)
{
    api_no = api_no;
    return key_map;
}
/*
 **************************************************************
 *  char    sKY_ChargeSet(char type)          *
 *  input:   type,  0:get the charge status
 *                       1: start charge battery
 *                       2:stop charge battery
 *   output:
 *              char charge status:  0:no charge
 *                                   1:charge state
 2:batteryfull
 **************************************************************
 */
uint8 sKY_GetChargeStatus(uint8 api_no)
{
    api_no = api_no;
    return TheCharge;
}

