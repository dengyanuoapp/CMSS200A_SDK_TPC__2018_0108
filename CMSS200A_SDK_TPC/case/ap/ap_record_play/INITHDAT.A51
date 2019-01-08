 

$include (reg251s.inc)

NAME	?C?INITHDATA

/************************* Segment Define ********************/ 
 
				?C_C51STARTUP	SEGMENT    CODE
/************************* public info ***********************/
				PUBLIC		?C?INITHDATA 
                PUBLIC  	?C_START1
/************************* EXTRN symbol **********************/
 
/************************* code begin ***********************/
		RSEG	?C_C51STARTUP

?C_START1:   
?C?INITHDATA:	
 		eret  
 		END