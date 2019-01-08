#ifndef _ACTOS_H
#define _ACTOS_H

#include "config.h"
#include "as260x.h"
#include "gl5115_bit.h"
#include "bitmem.h"
#include "actapi.h"
#include "adfu.h"
#include "comval.h"
#include "sdfs.h"
#include "driver.h"
#include "device.h"
#include "filesys.h"
#include "irq.h"
#include "mem.h"
#include "message.h"
#include "port.h"
#include "ap_manager.h"
#include "driver_manager.h"
#include "sddrv.h"
#include "sysdef.h"
#include "syserr.h"
#include "time.h"
#include "typeext.h"
#include "uddrv.h"
#include "sdram_drv.h"

#include "simulatorpc.h"


#define     UserMsgPoolDepth            0x10
#define     RTCMsgPoolDepth             0x04
#define     KeyMsgPoolDepth             0x04

// Added by Weimh 2010.03.15
#ifdef PC
//DLL_IMP void *MemCpy( void *dest, const void *src, int count );
//DLL_IMP void *MemSet( void *dest, int c, int count );
#define memcpy MemCpy
#define memset MemSet
#endif


#endif
