..\..\..\psp_rel\tools\apbuilder.exe -c ap_main.apc	  

copy .\Obj\ap_main.map ap_main.map
copy .\Obj\ap_main.hex ap_main.hex
copy .\Obj\ap_main ap_main
..\..\..\psp_rel\tools\GenAPBank.exe ap_main.hex ap_main_bank.hex
..\..\..\psp_rel\tools\segcheck ap_main.map ap_main.lin ap_main.apc
copy main.ap ..\..\fwpkg\


 