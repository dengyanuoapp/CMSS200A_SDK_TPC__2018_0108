#ifndef  Alarm_SET_H
#define  Alarm_SET_H

#include "basal.h"
#include "AlarmDef.h"

#define USE_FADEIN_TIME      FADE_100MS

extern Music_type_a g_currentMusicType;

extern Music_type_a ALARMCheckMusicType(uint8 *filename);

extern Open_param_t g_Openparam;
extern uint16 playing_count;
extern uint8 play_times;//²¥·Å´ÎÊý
extern bool AlarmMusicpSendCommand(uint8 cmd, void *param);
extern void AlarmGetInfo(void);

#endif
