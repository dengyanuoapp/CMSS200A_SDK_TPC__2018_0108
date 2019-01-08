..\..\..\psp_rel\tools\apbuilder.exe -c ap_usound.apc
copy .\obj\ap_usound.hex ap_usound.hex
copy .\obj\ap_usound.map ap_usound.map
copy .\obj\ap_usound ap_usound

echo ..\..\..\psp_rel\tools\segcheck.exe ap_usound.map ap_usound.lin ap_usound.apc
copy usbsound.ap ..\..\fwpkg\usbsound.ap

 