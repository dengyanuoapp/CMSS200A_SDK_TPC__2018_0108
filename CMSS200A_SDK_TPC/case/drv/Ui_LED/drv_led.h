#ifndef _SDRV_S04311_H
#define _SDRV_S04311_H

#include "sysdef.h"
#include "display.h"
#include "lang_id.h"



#ifdef __A251__
#endif /*__251_SYSTEMS_ASM*/

#ifdef __C251__

#define SEGNUM 7
#define ADDRNUM 5
#define NUMBERADDR 4
#define ICON_TOTAL  18
#define NUMBER       35
// º¯Êý±í
//void Drv_Entrance(void);

bool Sdrv_Init(uint8 api_no);
void Sdrv_Exit(uint8 api_no);
void Sdrv_LEDDisplay(uint8 api_no, uint8 addr, uint8 content, uint8 type);
void Sdrv_ClearScreen(uint8 api_no);
void Hdrv_HardInit(void);
void Hdrv_DataSend(uint8 addr, uint8 val, uint8 distype);
//extern uint8 fontlibtype;
#endif //__C251__
#endif /* _SDRV_S04311_H */

