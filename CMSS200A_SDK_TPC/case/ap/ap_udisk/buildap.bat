..\..\..\psp_rel\tools\apbuilder.exe -c ap_udisk.apc

copy .\obj\ap_udisk.hex ap_udisk.hex
copy .\obj\ap_udisk.map ap_udisk.map
copy .\obj\ap_udisk ap_udisk

del .\obj\ap_udisk.hex 
del .\obj\ap_udisk.map
del .\obj\ap_udisk


copy udisk.ap ..\..\fwpkg\usbdisk.ap
 