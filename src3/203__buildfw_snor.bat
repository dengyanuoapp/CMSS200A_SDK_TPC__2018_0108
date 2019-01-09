@echo off
echo ==== debugXXX ==== 1001
rem ":::::::::::::::::::::::::::::::::::::::::::::::::::::"
rem ":: 参数1表示该脚本的调用方法 如%1=DBT表示是DBT调用该脚本"
rem ":: 参数2表示生成的固件 目前只有当%1为DBT时%2才有意义"
rem ":: 参数3表示Make生成的报告信息"
rem ":: 参数4表示该脚本的返回值 运行成功返回1 运行失败返回0"
rem ":::::::::::::::::::::::::::::::::::::::::::::::::::::"

echo ==== debugXXX ==== 1002

rem ----------------------------------------
set callfrom=%1
set firmware=%2
set msgfile=%3
set retval=0

set cur_disk=%~d0
set current_dir=%cur_disk%\%~p0

cd %current_dir%

set workDir=%cd%
set uv2Dir=..\..\..\fwsp\uv2
set toolDir=..\..\psp_rel\tools
set afiDir=..\..\psp_rel\bin
set binOriginal=..\..\psp_rel\bin_original
set binAfi=afi_snor.bin
set batBulidAfi=buildAFI_snor.bat
set cfgFwimage=fwimage.cfg

set fwdb=CMSS200A.fw
set prdtImg=snorbasefw

if exist %binOriginal%\%batBulidAfi% (
    cd %binOriginal%
    call %binOriginal%\%batBulidAfi%
    cd %workDir%
)

echo ==== debugXXX ==== 1009

rem "::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
rem ":: make the image"
rem "::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
if "%callfrom%"=="" (
    echo ==== debugXXX ==== 1011
    rem 双击运行
    echo "%toolDir%\Maker.exe -db %afiDir%\%binAfi% -c %cfgFwimage% -o %fwdb%"
          %toolDir%\Maker.exe -db %afiDir%\%binAfi% -c %cfgFwimage% -o %fwdb%

    set retval=1

    goto make_finish
)

echo ==== debugXXX ==== 1013
if not %callfrom%==DBT (
    set retval=0
    goto make_finish
)

echo ==== debugXXX ==== 1014
rem now callfrom must be DBT
if "%firmware%"=="" (
    rem firmware path must exist
    set retval=0
    goto make_finish
)

echo ==== debugXXX ==== 1015
if "%msgfile%"=="" (
    %toolDir%\Maker.exe -db %afiDir%\%binAfi% -c %cfgFwimage% -o %fwdb%
    set retval=1
    goto make_finish
)

echo ==== debugXXX ==== 1017
%toolDir%\Maker.exe -c fwimage_snor.cfg -o %fwdb% >%msgfile% 2>&1




set retval=1

:make_finish

echo ==== debugXXX ==== 1021
echo "..\..\psp_rel\tools\Maker.exe -db CMSS200A.fw -k LFI -o fwImageFile"
      ..\..\psp_rel\tools\Maker.exe -db CMSS200A.fw -k LFI -o fwImageFile

if "%callfrom%"=="" (
    echo ==== debugXXX ==== 1023
    rem pause
)

:make_finish
echo ==== debugXXX ==== 1031
if "%callfrom%"=="" (
    echo ==== debugXXX ==== 1033
    rem pause
)

echo ==== debugXXX ==== 1041
if not "%4"=="" (
echo ==== debugXXX ==== 1043
    set %4=%retval%
)
goto :eof

echo ==== debugXXX ==== 1099
