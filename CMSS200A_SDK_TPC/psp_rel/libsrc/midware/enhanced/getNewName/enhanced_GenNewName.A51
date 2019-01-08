#include <actos.h> 

EXTRN ECODE(sGenNewName??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public GenNewName??
GenNewName??:
MOV	WR8,#WORD0(sGenNewName??)
MOV	R10, #BYTE2(sGenNewName??) 
LJMP    RSTMIDAPI 

END