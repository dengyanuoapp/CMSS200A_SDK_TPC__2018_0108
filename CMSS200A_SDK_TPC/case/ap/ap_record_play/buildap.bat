..\..\..\psp_rel\tools\apbuilder.exe -c ap_record_play.apc
copy .\Obj\ap_record_play.map ap_record_play.map
copy .\Obj\ap_record_play.hex ap_record_play.hex
copy .\Obj\ap_record_play ap_record_play

echo ..\..\..\psp_rel\tools\segcheck.exe ap_record_play.map ap_record_play.lin ap_record_play.apc
copy ap_rec_p.ap ..\..\fwpkg\ap_rec_p.ap
 		   