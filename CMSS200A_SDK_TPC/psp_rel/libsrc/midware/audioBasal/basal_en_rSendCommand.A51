#include <actos.h> 

EXTRN ECODE(srSendCommand??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public rSendCommand??
rSendCommand??:
MOV	WR8,#WORD0(srSendCommand??)
MOV	R10, #BYTE2(srSendCommand??) 
LJMP    RSTMIDAPI 

END