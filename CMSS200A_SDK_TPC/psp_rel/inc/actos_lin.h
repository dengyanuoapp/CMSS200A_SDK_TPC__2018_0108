#ifndef _ACTOS_LIN_H
#define _ACTOS_LIN_H
#include "config.h"

#ifdef __A251__
$IF ROMHUGE == 1
Prefix	LIT '?'
RPrefix LIT	'??'
DPrefix LIT 'HDATA'
Model   LIT 'FAR'
PRSeg	LIT 'ECODE'
ASMret  LIT 'ERET'
$ELSE
Prefix  LIT ''
RPrefix LIT	'?_'
DPrefix LIT 'EDATA'
Model   LIT 'NEAR'
PRSeg	LIT 'CODE'
ASMret  LIT 'RET'
$ENDIF
#endif




#endif