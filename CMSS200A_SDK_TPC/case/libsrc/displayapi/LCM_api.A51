$include (actos.inc)



API30 SEGMENT 'ECODE_APISEG'

/**************************SetAsciiFont µÄAPIÈë¿Ú******************************/
PUBLIC API_LEDInit{RPrefix}
PUBLIC API_LEDExit{RPrefix}
PUBLIC API_LEDClearScreen{RPrefix}
PUBLIC API_LEDDisplay{RPrefix}
PUBLIC API_LEDPuts{RPrefix}
RSEG   API30
API_LEDInit{RPrefix}:
API_LEDExit{RPrefix}:
API_LEDDisplay{RPrefix}:
API_LEDClearScreen{RPrefix}:
API_LEDPuts{RPrefix}:
	EJMP	 RSTUIAPI 
END

