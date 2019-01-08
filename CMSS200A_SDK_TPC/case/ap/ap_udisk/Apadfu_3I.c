#include "udisk.h"

//#pragma memory=dataseg(UDISK_RD)
#pragma	userclass(near=UDISK_RD)
extern char *DispBuf;

//#pragma memory=default
#pragma	userclass(near=default)

//#pragma memory=dataseg(ADFUAP_H)
#pragma	userclass(near=ADFUAP_H)
ADFU_SysInfo_t sysinfo;
//#pragma memory=default
#pragma	userclass(near=default)

//#pragma memory=constseg(ADFUAP_S)
#pragma renamecode(ADFUAP_S)
//#pragma memory=default

//#pragma codeseg(ADFUAP_C)
#pragma renamecode(ADFUAP_C)

void ShowFwVersion(void) //zxs change
{

}

void ShowLogoUPG_DONE(void)
{

}

//#pragma memory=default
#pragma	userclass(near=default)
