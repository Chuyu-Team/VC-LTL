@echo off
::
::  启动 vcvars32.bat/vcvars64.bat 可以执行此脚本，运行完成后将自动修改include以及lib环境变量。方便nmake以及纯cl用户引用VC-LTL。
::
::  你可以将此文件复制到你的项目中，其中VC-LTL设置你可以根据你的要求调整。
::
::



:::::::::::::::::::::::::::::::::::::::::::::::::VC-LTL设置:::::::::::::::::::::::::::::::::::::::::::::::::


::取消下方注释可以开启XP兼容模式，默认才用Vista兼容模式。
::set SupportWinXP=true

::取消下方注释可以开启VC-LTL轻量模式，轻量模式更加注重体积控制，但是CRT规范将会维持在VS2008。如果你需要高度兼容微软UCRT，那么请不要打开此选项！！！
::set DisableAdvancedSupport=true


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::





if "%VC_LTL_Root%" NEQ "" goto LoadConfig

::搜索VC-LTL目录

::脚本文件根目录存在VC-LTL？
if exist "%~dp0_msvcrt.h" set VC_LTL_Root=%~dp0&& goto LoadConfig

::脚本文件父目录存在VC-LTL？
if exist "%~dp0..\_msvcrt.h" set VC_LTL_Root=%~dp0..&& goto LoadConfig

::读取注册表 HKCU\Code\VC-LTL@Root
for /f "tokens=1,2*" %%i in ('reg query "HKCU\Code\VC-LTL" /v Root ') do set VC_LTL_Root=%%k

if "%VC_LTL_Root%" == "" goto:eof


:LoadConfig

call "%VC_LTL_Root%\config\config.cmd"
