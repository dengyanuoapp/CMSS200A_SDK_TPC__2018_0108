..\..\..\psp_rel\tools\apbuilder.exe -c ap_utest.apc

copy .\obj\ap_utest.hex ap_utest.hex
copy .\obj\ap_utest.map ap_utest.map
copy .\obj\ap_utest ap_utest

del .\obj\ap_utest.hex 
del .\obj\ap_utest.map
del .\obj\ap_utest


copy utest.ap ..\..\fwpkg\usbtest.ap
 