@echo off
setlocal
set IsTextMode=No

echo VC-LTL Installer [Version 0.1.0]
echo (c) Chuyu Team. All rights reserved.
echo.

if /i "%1"=="" goto TextMode
if /i "%1"=="/?" goto Help
if /i "%1"=="/I" goto Install
if /i "%1"=="/U" goto UnInstall

echo Invaild Command Parameter.
echo.
goto Help

:Install
echo.
echo.Start writing to the registry
echo.[HKCU\Code\VC-LTL]
echo.Root="%~dp0"
echo.
reg ADD HKCU\Code\VC-LTL /v Root /t REG_SZ /d "%~dp0\" /f
echo.
goto ExitScript

:UnInstall
echo.
reg delete HKCU\Code\VC-LTL /f
echo.
goto ExitScript

:Help
echo.Manages VC-LTL information in the current user registry.
echo.
echo.Install [Options]
echo.
echo.  /I    Add VC-LTL information to the current user registry.
echo.  /U    Remove VC-LTL information to the current user registry.
echo.  /?    Show this content.
goto ExitScript

:TextMode
set IsTextMode=Yes
echo.Welcome to use VC-LTL Installer! If you want to invoke this script 
echo.silently, please use /? parameter to read help.
reg query HKCU\Code\VC-LTL /v Root >nul 2>nul && goto TextUnInstall || goto Install

:TextUnInstall
echo. 
set /p Answer=VC-LTL appears to be installed, do you want to uninstall it (Y/N)?
if /i "%Answer%"=="Y" goto UnInstall
echo.
echo.Operation canceled.
echo.
goto ExitScript

:ExitScript
if /i "%IsTextMode%"=="Yes" pause
@echo on