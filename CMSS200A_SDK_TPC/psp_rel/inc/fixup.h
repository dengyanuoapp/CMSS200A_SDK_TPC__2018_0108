#ifndef _FIX_UP_H_
#define _FIX_UP_H_

#include "as260x.h"
#include "comval.h"


#ifdef __C251__

typedef struct
{
    uint8 error_addr0[3];
    uint8 error_addr1[3];
    uint8 error_addr2[3];
    uint8 error_addr3[3];
    uint8 fixmanger_entry[3];
} fix_sfr_struct;
typedef struct
{
    uint8 fixaddr0[3];
    uint8 fixaddr1[3];
    uint8 fixaddr2[3];
    uint8 fixaddr3[3];
} fix_new_addr_struct;
typedef struct
{
    fix_sfr_struct  fix_sfr_str;
    fix_new_addr_struct rfix_new_addr;
} fix_struct;


//};

void read_fix_sfr(fix_sfr_struct *pfixsfr);
void write_fix_sfr(fix_sfr_struct *pfixsfr);
#endif
#endif