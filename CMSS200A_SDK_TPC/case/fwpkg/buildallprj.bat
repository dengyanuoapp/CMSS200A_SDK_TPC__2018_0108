@echo off 
setlocal EnableDelayedExpansion
set workDir=%cd%
set prjdir1=%cd%\..\libsrc
set prjdir2=%cd%\..\drv
set prjdir3=%cd%\..\ap
set pspdir=%cd%\..\..\psp_rel\uv2\prj


:::::::::::::::::::::::::::::::::::::::
:: 获取UV3目录
:::::::::::::::::::::::::::::::::::::::
@echo checking UV3 dir ......
@echo off
if exist C:\Keil\UV3\Uv3.EXE set UV3DIR="C:\Keil\UV3\Uv3.EXE"
if exist D:\Keil\UV3\Uv3.EXE set UV3DIR="D:\Keil\UV3\Uv3.EXE"
if exist E:\Keil\UV3\Uv3.EXE set UV3DIR="E:\Keil\UV3\Uv3.EXE"
if exist F:\Keil\UV3\Uv3.EXE set UV3DIR="F:\Keil\UV3\Uv3.EXE"
echo UV3DIR=%UV3DIR%
@echo cleaning OPT ......

:::::::::::::::::::::::::::::::::::::::
:: 清理OPT文件
:::::::::::::::::::::::::::::::::::::::
 
cd %prjdir1%
for /f "delims=" %%i in ('dir *.OPT /b /s 2^>nul') do if exist %%i del %%i
cd %prjdir2%
for /f "delims=" %%i in ('dir *.OPT /b /s 2^>nul') do if exist %%i del %%i
cd %prjdir3%
for /f "delims=" %%i in ('dir *.OPT /b /s 2^>nul') do if exist %%i del %%i
if exist "%workDir%\buildlist.log" del %workDir%\buildlist.log
@echo Gen buildlist ......

:genlist_end


:::::::::::::::::::::::::::::::::::::::
:: 依次编译
:::::::::::::::::::::::::::::::::::::::
@echo building psp......
for /f "tokens=*" %%a in (%workDir%\buildpsplist.txt) do (
echo ****************build %%a******************** >>%workDir%\buildlist.log
%UV3DIR% %%a -o %workDir%\.tmpout
findstr "error" %workDir%\.tmpout >>%workDir%\buildlist.log
findstr "ERROR" %workDir%\.tmpout >>%workDir%\buildlist.log
del %workDir%\.tmpout 
)


@echo building ......
for /f "tokens=*" %%a in (%workDir%\buildcaselist.txt) do (
echo ****************build %%a******************** >>%workDir%\buildlist.log
%UV3DIR% %%a -o %workDir%\.tmpout
findstr "error" %workDir%\.tmpout >>%workDir%\buildlist.log
findstr "ERROR" %workDir%\.tmpout >>%workDir%\buildlist.log
del %workDir%\.tmpout 
)
cd %workDir% 
:::::::::::::::::::::::::::::::::::::::
:: 检查结果
:::::::::::::::::::::::::::::::::::::::
@echo check result ......
@echo off
findstr "error" %workDir%\buildlist.log >nul
@echo off
if "%errorlevel%"=="0" goto builderr
goto finish
:builderr
msg %username% "build err"
:finish
 
:nobuild
pause