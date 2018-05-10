@echo off
::
::  请不要直接使用此脚本，应该使用VC-LTL helper for nmake.cmd
::





if /i "%VC_LTL_Helper_Load%" == "true" goto:eof


if "%VC_LTL_Root%" == "" echo 请不要直接使用此脚本，应该使用VC-LTL helper for nmake&&goto:eof

if "%INCLUDE%" == "" echo 找不到环境变量INCLUDE，请在vcvars32.bat/vcvars64.bat执行后调用此脚本&&goto:eof

if "%LIB%" == "" echo 找不到环境变量LIB，请在vcvars32.bat/vcvars64.bat执行后调用此脚本&&goto:eof

if "%VisualStudioVersion%" == "14.0" set __DefaultVCToolsVersion=14.0.23918
if "%VisualStudioVersion%" == "15.0" set __DefaultVCToolsVersion=14.10.25017

if "%__DefaultVCToolsVersion%" == "" echo VC-LTL仅支持VS 2015以及2017&&goto:eof

if /i "%Platform%" == "" goto Start_VC_LTL

if /i "%Platform%" == "x86" goto Start_VC_LTL

if /i "%Platform%" == "x64" ( goto Start_VC_LTL ) else ( echo VC-LTL CMD脚本不支持体系 : %Platform% )

goto:eof


:Start_VC_LTL

set VC_LTL_Helper_Load=true

set PlatformShortName=%Platform%

if "%PlatformShortName%" == "" set PlatformShortName=x86

if "%VC_LTL_Root%" == "" call:FoundVC_LTL_Root

if "%VC-LTLUsedToolsVersion%" == "" call:FoundVCToolsVersion

if "%VC-LTLTargetUniversalCRTVersion%" == "" call:FoundUCRTVersion

echo #######################################################################
echo #                                                                     #
echo #     *         *      * *             *        * * * * *  *          #
echo #      *       *     *                 *            *      *          #
echo #       *     *     *       * * * * *  *            *      *          #
echo #        *   *       *                 *            *      *          #
echo #          *           * *             * * * *      *      * * * *    #
echo #                                                                     #
echo #######################################################################

echo VC-LTL Path : %VC_LTL_Root%
echo VC-LTL Tools Version : %VC-LTLUsedToolsVersion%
echo VC-LTL UCRT Version : %VC-LTLTargetUniversalCRTVersion%
echo Platform : %PlatformShortName%



if /i "%SupportWinXP%" == "true" (set OsPlatformName=WinXP) else (set OsPlatformName=Vista)

if /i "%DisableAdvancedSupport%" == "true" (set LTL_Mode=Light) else (set LTL_Mode=Advanced)

echo Using VC-LTL %OsPlatformName% %LTL_Mode% Mode


::修改Include
set INCLUDE=%VC_LTL_Root%\config\%OsPlatformName%;%VC_LTL_Root%\VC\%VC-LTLUsedToolsVersion%\include;%VC_LTL_Root%\VC\%VC-LTLUsedToolsVersion%\atlmfc\include;%VC_LTL_Root%\ucrt\%VC-LTLTargetUniversalCRTVersion%;%INCLUDE%

set LIB=%VC_LTL_Root%\%PlatformShortName%;%VC_LTL_Root%\%PlatformShortName%\%OsPlatformName%\%LTL_Mode%;%VC_LTL_Root%\VC\%VC-LTLUsedToolsVersion%\lib\%PlatformShortName%;%VC_LTL_Root%\VC\%VC-LTLUsedToolsVersion%\lib\%PlatformShortName%\%OsPlatformName%;%VC_LTL_Root%\ucrt\%VC-LTLTargetUniversalCRTVersion%\lib\%PlatformShortName%;%LIB%

goto:eof



::搜索 VC工具集版本
:FoundVCToolsVersion

set VC-LTLUsedToolsVersion=%VCToolsVersion%
if "%VC-LTLUsedToolsVersion%" NEQ "" goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24234（Visual Studio 2017 15.7中的2015平台工具集），与14.0.24231完全一致
set VC-LTLUsedToolsVersion=14.0.24231
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{BDE574B5-6CFE-32B2-9854-C827567E9D6F} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{BDE574B5-6CFE-32B2-9854-C827567E9D6F} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24231（Visual Studio 2017 15.6中的2015平台工具集）
set VC-LTLUsedToolsVersion=14.0.24231
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{B0791F3A-6A88-3650-AECF-8AFBE227EC53} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{B0791F3A-6A88-3650-AECF-8AFBE227EC53} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24225（Visual Studio 2017 15.5中的2015平台工具集）
set VC-LTLUsedToolsVersion=14.0.24225
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24210（正统Visual Studio 2015）
set VC-LTLUsedToolsVersion=14.0.24210
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::如果找不到，那么指定为当前支持的最低版本 2015 Update2
set VC-LTLUsedToolsVersion=%__DefaultVCToolsVersion%
goto:eof

:ReadVC2015VersionEnd

if not exist "%VC_LTL_Root%\VC\%VC-LTLUsedToolsVersion%" set VC-LTLUsedToolsVersion=%__DefaultVCToolsVersion%


goto:eof


::搜索UCRT版本
:FoundUCRTVersion

set VC-LTLTargetUniversalCRTVersion=%UCRTVersion%

if "%VC-LTLTargetUniversalCRTVersion%" == "" goto FoundUCRTVersionEnd

if exist "%VC_LTL_Root%\ucrt\%VC-LTLTargetUniversalCRTVersion%" goto:eof


:FoundUCRTVersionEnd

::找不到指定UCRT版本则默认为10240
set VC-LTLTargetUniversalCRTVersion=10.0.10240.0


goto:eof
