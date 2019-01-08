..\..\..\psp_rel\tools\apbuilder.exe -c ap_music.apc
copy .\Obj\ap_music.map ap_music.map
copy .\Obj\ap_music.hex ap_music.hex
copy .\Obj\ap_music ap_music

echo ..\..\..\psp_rel\tools\segcheck.exe ap_music.map ap_music.lin ap_music.apc
copy music.ap ..\..\fwpkg\music.ap

 