@echo off
:::::::::::::::::::::::::::::::::::::::::::::::::::::
:: ����1��ʾ�ýű��ĵ��÷��� ��%1=DBT��ʾ��DBT���øýű�
:: ����2��ʾ���ɵĹ̼� Ŀǰֻ�е�%1ΪDBTʱ%2��������
:: ����3��ʾMake���ɵı�����Ϣ
:: ����4��ʾ�ýű��ķ���ֵ ���гɹ�����1 ����ʧ�ܷ���0
:::::::::::::::::::::::::::::::::::::::::::::::::::::

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


::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: make the image
::::::::::::::::::::::::::::::::::::::::::::::::::::::::
if "%callfrom%"=="" (
    rem ˫������
    %toolDir%\Maker.exe -db %afiDir%\%binAfi% -c %cfgFwimage% -o %fwdb%

    set retval=1

    goto make_finish
)

if not %callfrom%==DBT (
    set retval=0
    goto make_finish
)

rem now callfrom must be DBT
if "%firmware%"=="" (
    rem firmware path must exist
    set retval=0
    goto make_finish
)
if "%msgfile%"=="" (
    %toolDir%\Maker.exe -db %afiDir%\%binAfi% -c %cfgFwimage% -o %fwdb%
    set retval=1
    goto make_finish
)
%toolDir%\Maker.exe -c fwimage_snor.cfg -o %fwdb% >%msgfile% 2>&1




set retval=1

:make_finish
 ..\..\psp_rel\tools\Maker.exe -db CMSS200A.fw -k LFI -o fwImageFile
if "%callfrom%"=="" (
    pause
)

:make_finish
if "%callfrom%"=="" (
    pause
)

if not "%4"=="" (
    set %4=%retval%
)
goto :eof

