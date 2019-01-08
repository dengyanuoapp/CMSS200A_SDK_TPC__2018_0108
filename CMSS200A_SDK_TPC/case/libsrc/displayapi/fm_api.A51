$include (actos.inc)



API30 SEGMENT 'ECODE_APISEG'

/**************************FM驱动的API入口******************************/
//rcode api
PUBLIC API_SetFreq{RPrefix}  
PUBLIC API_GetSeekFlag{RPrefix}
PUBLIC API_GetFreq{RPrefix}
PUBLIC API_GetStatus{RPrefix}  
PUBLIC API_Search{RPrefix}

  
//bank api
PUBLIC API_SetMute{RPrefix}  
PUBLIC API_SetBand{RPrefix}  
PUBLIC API_SetThrod{RPrefix} 
PUBLIC API_StartSeek{RPrefix} 
PUBLIC API_BreakSeek{RPrefix} 
PUBLIC API_GetIntensity{RPrefix} 
PUBLIC API_SetHard{RPrefix} 
PUBLIC API_FMStandby{RPrefix} 

PUBLIC API_FM_init{RPrefix}
PUBLIC API_FM_exit{RPrefix}

RSEG   API30

API_SetFreq{RPrefix}:
API_GetSeekFlag{RPrefix}:
API_GetFreq{RPrefix}:
API_GetStatus{RPrefix}:
API_Search{RPrefix}:
API_SetMute{RPrefix}:
API_SetBand{RPrefix}:
API_SetThrod{RPrefix}:
API_StartSeek{RPrefix}:
API_BreakSeek{RPrefix}:
API_GetIntensity{RPrefix}: 
API_SetHard{RPrefix}:
API_FMStandby{RPrefix}:
API_FM_init{RPrefix}:
API_FM_exit{RPrefix}:
	EJMP	 RSTFMAPI 
END

