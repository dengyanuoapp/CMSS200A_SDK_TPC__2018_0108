 
$include (actos.inc)
 
#include "uddrv.h"
#include "actapi.h"
#include "driver.h"

$IF ROMHUGE == 1
 		API30	SEGMENT	 'ECODE_APISEG'
$ELSE
 		API30	SEGMENT	 'CODE_APISEG'
$ENDIF

//******************************************************************************
//*     driver management api
//******************************************************************************
#include "driver.h"
//******************************************************************************
 
        public  DRV_CheckCard{RPrefix}
        rseg    API30
DRV_CheckCard{RPrefix}:
//        MOV  WR8,   #API_DRV_CheckCard
$IF ROMHUGE == 1
//          PUSH      #WORD0(RSTBankAPI)
//		  EJMP		RSTFARAPI
$ELSE
          JMP      RSTBankAPI
$ENDIF
         

 
end