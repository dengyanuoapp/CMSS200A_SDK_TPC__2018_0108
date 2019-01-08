#ifndef _MIDWARE_H
#define _MIDWARE_H
#include "enhanced.h"

#define API_Mp3rProceed 		0x00
#define API_Mp3SendCommand  	0x01
#define API_Mp3GetID3Info 		0x02
#define API_WmaGetTAGInfo 		0x03

#ifdef __C251__

int mp3rProceed(void);
BOOL pSendCommand(BYTE cmd, BYTE music_type, int param);
bool mp3GetID3Info(const char *filename, ID3Info_t *ID3Info, int string_id);
bool wmaGetTAGInfo(const char *filename, ID3Info_t *ID3Info);

#endif


#endif