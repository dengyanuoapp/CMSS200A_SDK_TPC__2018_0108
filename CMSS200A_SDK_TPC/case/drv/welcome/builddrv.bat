"..\..\..\psp_rel\tools\HexToBin251.EXE" ".\obj\welcome.hex" "..\..\fwpkg\welcome.bin" ff2000 500
copy .\Obj\welcome.map welcome.map
copy .\Obj\welcome.hex welcome.hex
copy .\Obj\welcome welcome
IF {%1}=={} goto nocopy
if exist "..\..\fwpkg\%1" (copy "..\..\fwpkg\welcome.bin" "..\..\fwpkg\%1\welcome.bin")
:nocopy
echo build end 
