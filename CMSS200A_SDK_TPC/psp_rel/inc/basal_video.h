#ifndef _BASAL_VIDEO_H
#define _BASAL_VIDEO_H
#include "typeext.h"
#include "codec_video.h"
#include "basal.h"

#define   AMVSTOP          1           // amv stop command
#define   AMVFORWARD       2           // amv forward command
#define   AMVBACK          3           // amv back  command
#define   AMVNORMALPLAY    4           // clear the forward and back command

#define   RGB565  0
#define   RGB888  1

#define   FADEOUT_NON    0
#define   FADEOUT_SEC1   1
#define   FADEOUT_SEC2   2


typedef enum
{
    AMV_AMV,
    AMV_AVI,
    MUSIC_NON
} Music_type_t;

uint8 pSendCommand(uint8 cmd, uint16 video_type, int16 param);


#endif
