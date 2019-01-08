..\..\..\psp_rel\tools\apbuilder.exe -c ap_linein.apc	  

copy .\Obj\ap_linein.map ap_linein.map
copy .\Obj\ap_linein.hex ap_linein.hex
copy .\Obj\ap_linein ap_linein
..\..\..\psp_rel\tools\GenAPBank.exe ap_linein.hex ap_linein_bank.hex
..\..\..\psp_rel\tools\segcheck ap_linein.map ap_linein.lin ap_linein.apc
copy linein.ap ..\..\fwpkg\


 