copy .\UI_Release\obj\drv_ui.map drv_ui.map
copy .\UI_Release\obj\drv_ui drv_ui

..\..\..\psp_rel\tools\builddriver51 .\UI_Release\obj\drv_ui.hex  .\drv_led.drvl .\UI_Release\obj
copy .\UI_Release\obj\drv_ui.drv  ..\..\fwpkg\
IF {%1}=={} goto nocopy
if exist "..\..\fwpkg\%1" (copy "..\..\fwpkg\drv_ui.drv" "..\..\fwpkg\%1\drv_ui.drv")
:nocopy
echo build end 
