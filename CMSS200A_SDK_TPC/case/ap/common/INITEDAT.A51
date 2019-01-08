;----------------------------------------------------------------------------
;
; Init records for EDATA (near) memory.  The record structure is as follows:
;
;    Byte 0  Byte 1  Byte 2  Byte 3  Byte 4
;   +-------+-------+-------+-------+-------+
;   |Len MSB|Len LSB|Adr MSB|Adr LSB|Content|
;   +-------+-------+-------+-------+-------+
;   |                               +Rep Len+
;   +--- Repeated for each Object ----------+
;
; The complete EDATA initilization is placed into the segment ?C_INITEDATA 
; (segment class is HCONST).  
;
;----------------------------------------------------------------------------

$include (reg251s.inc)

NAME		 ?C?INITEDATA

/************************* Segment Define ********************/
		?C_C51STARTUP	SEGMENT     CODE
 
/************************* public info ***********************/
			PUBLIC		 ?C?INITEDATA
            PUBLIC  	 ?C_START
/************************* EXTRN symbol **********************/
	 
/************************* code begin ***********************/
			RSEG	?C_C51STARTUP

?C_START:  
?C?INITEDATA:
eret   
END

