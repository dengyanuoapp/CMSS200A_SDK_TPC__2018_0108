copy .\Obj\ky_hard.map ky_hard.map
copy .\Obj\ky_hard.hex ky_hard.hex
copy .\Obj\ky_hard ky_hard

..\..\..\psp_rel\tools\builddriver51 ky_hard.hex drv_key.drvl ky_hard.map
copy ky_hard.drv ..\..\fwpkg\
IF {%1}=={} goto nocopy
if exist "..\..\fwpkg\%1" (copy "..\..\fwpkg\ky_hard.drv" "..\..\fwpkg\%1\ky_hard.drv")
:nocopy
echo build end 