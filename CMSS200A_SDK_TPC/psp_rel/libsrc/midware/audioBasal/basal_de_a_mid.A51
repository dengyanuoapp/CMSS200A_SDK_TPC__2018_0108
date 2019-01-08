#include "actos.h" 

EXTRN ECODE(sapSendCommand??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public apSendCommand??
apSendCommand??:
MOV	WR8,#WORD0(sapSendCommand??)
MOV	R10, #BYTE2(sapSendCommand??) 
LJMP    RSTMIDAPI 

END