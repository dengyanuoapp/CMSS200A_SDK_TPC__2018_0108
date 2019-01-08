copy .\output\drv_uart.map drv_uart.map
copy .\output\drv_uart drv_uart

..\..\..\psp_rel\tools\builddriver51 .\output\drv_uart.hex  .\drv_uart.drvl .\output
copy .\output\drv_uart.drv  ..\..\fwpkg\
IF {%1}=={} goto nocopy
if exist "..\..\fwpkg\%1" (copy "..\..\fwpkg\drv_uart.drv" "..\..\fwpkg\%1\drv_uart.drv")
:nocopy
echo build end 
