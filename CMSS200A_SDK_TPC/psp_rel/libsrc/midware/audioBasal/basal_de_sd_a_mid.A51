#include "actos.h" 

EXTRN ECODE(sapSdSendCommand??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public apSdSendCommand??
apSdSendCommand??:
MOV	WR8,#WORD0(sapSdSendCommand??)
MOV	R10, #BYTE2(sapSdSendCommand??) 
LJMP    RSTMIDAPI 

END