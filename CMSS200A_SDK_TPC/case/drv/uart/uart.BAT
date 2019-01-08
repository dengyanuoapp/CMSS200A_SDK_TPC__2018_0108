SET C251INC=E:\Keil\C251\INC\Intel\;E:\Keil\C251\INC\
SET C251LIB=E:\Keil\C251\LIB
SET CPU_TYPE=Flip80251 Typhoon
SET CPU_VENDOR=Dolphin
SET UV2_TARGET=uart
SET CPU_XTAL=0x00F42400
E:\Keil\C251\BIN\A251.EXE @.\output\DRV_HE~1._IA
E:\Keil\C251\BIN\C251.EXE @.\output\uart_msg.__i
E:\Keil\C251\BIN\C251.EXE @.\output\uart.__i
E:\Keil\C251\BIN\C251.EXE @.\output\UART_R~1.__I
E:\Keil\C251\BIN\A251.EXE @.\output\UART_A~1._IA
E:\Keil\C251\BIN\L251.EXE @.\output\drv_uart.lnp
"E:\Keil\C251\BIN\OH251.EXE" ".\output\drv_uart" H386 RANGE (0 - 0xFFFFFF)
.\builddrv.bat
