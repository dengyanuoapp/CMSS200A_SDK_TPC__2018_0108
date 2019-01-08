#include <actos.h> 

EXTRN ECODE(smp3GetID3Info??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public mp3GetID3Info??
mp3GetID3Info??:
MOV	WR8,#WORD0(smp3GetID3Info??)
MOV	R10, #BYTE2(smp3GetID3Info??) 
LJMP    RSTMIDAPI 

END