..\..\..\psp_rel\tools\apbuilder.exe -c ap_record.apc
copy .\Obj\ap_record.map ap_record.map
copy .\Obj\ap_record.hex ap_record.hex
copy .\Obj\ap_record ap_record

echo ..\..\..\psp_rel\tools\segcheck.exe ap_record.map ap_record.lin ap_record.apc
copy record.ap ..\..\fwpkg\record.ap
 		   