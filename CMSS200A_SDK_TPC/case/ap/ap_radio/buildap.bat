..\..\..\psp_rel\tools\apbuilder.exe -c ap_radio.apc
copy .\Obj\ap_radio.map ap_radio.map
copy .\Obj\ap_radio.hex ap_radio.hex
copy .\Obj\ap_radio ap_radio

..\..\..\psp_rel\tools\segcheck ap_radio.map ap_radio.lin ap_radio.apc 

copy radio.ap ..\..\fwpkg\radio.ap