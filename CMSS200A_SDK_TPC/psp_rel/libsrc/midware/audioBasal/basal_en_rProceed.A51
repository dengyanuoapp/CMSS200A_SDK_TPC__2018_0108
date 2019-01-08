#include <actos.h> 

EXTRN ECODE(srProceed??)

MIDAPI SEGMENT ECODE

rseg MIDAPI

///////////////////////////////////////////////
public rProceed??
rProceed??:
MOV	WR8,#WORD0(srProceed??)
MOV	R10, #BYTE2(srProceed??) 
LJMP    RSTMIDAPI 

END