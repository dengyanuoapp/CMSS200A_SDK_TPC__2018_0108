$include (actos.inc)



API30 SEGMENT 'ECODE_APISEG'

/**************************SetAsciiFont µÄAPIÈë¿Ú******************************/
public API_key_init{RPrefix}
public API_key_exit{RPrefix}  
public API_KY_GetChargeStatus{RPrefix}
public API_KY_Beep{RPrefix}
public API_KY_GetKeyTabAddress{RPrefix}
public API_check_plug_device_msg{RPrefix}
public API_switch_pilot_light{RPrefix}
RSEG   API30
API_key_init{RPrefix}:
API_key_exit{RPrefix}:	  
API_KY_GetChargeStatus{RPrefix}:
API_KY_Beep{RPrefix}:   
API_KY_GetKeyTabAddress{RPrefix}: 
API_check_plug_device_msg{RPrefix}:
API_switch_pilot_light{RPrefix}:
	EJMP	 RSTKEYAPI 
END

