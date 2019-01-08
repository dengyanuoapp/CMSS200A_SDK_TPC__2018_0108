..\..\..\psp_rel\tools\apbuilder.exe -c ap_alarm.apc
copy .\Obj\ap_alarm.map ap_alarm.map
copy .\Obj\ap_alarm.hex ap_alarm.hex
copy .\Obj\ap_alarm ap_alarm

echo ..\..\..\psp_rel\tools\segcheck.exe ap_alarm.map ap_alarm.lin ap_alarm.apc
copy alarm.ap ..\..\fwpkg\alarm.ap

 