@echo off
::
::  请不要直接使用此脚本，应该使用VC-LTL helper for nmake.cmd
::

call:InitMuiStrings


if /i "%VC_LTL_Helper_Load%" == "true" goto:eof


if "%VC_LTL_Root%" == "" echo %ERROR_VC_LTL_DONOT_USE_THIS_FILE%&&goto:eof

if "%INCLUDE%" == "" echo %ERROR_VC_LTL_CANNOT_FOUND_INCLUDE_ENV%&&goto:eof

if "%LIB%" == "" echo %ERROR_VC_LTL_CANNOT_FOUND_LIB_ENV%&&goto:eof

if "%VisualStudioVersion%" == "14.0" set DefaultVCLTLToolsVersion=14.0.24231
if "%VisualStudioVersion%" == "15.0" set DefaultVCLTLToolsVersion=14.16.27023
if "%VisualStudioVersion%" == "16.0" set DefaultVCLTLToolsVersion=14.29.30037

if "%DefaultVCLTLToolsVersion%" == "" echo %ERROR_VC_LTL_NOT_SUPPORT_PLATFORM_TOOLSET%&&goto:eof

if /i "%Platform%" == "" goto Start_VC_LTL

if /i "%Platform%" == "x86" goto Start_VC_LTL

if /i "%Platform%" == "x64" goto Start_VC_LTL

if /i "%Platform%" == "arm" goto Start_VC_LTL

if /i "%Platform%" == "arm64" goto Start_VC_LTL

echo %ERROR_VC_LTL_NOT_SUPPORT_PLATFORM%

goto:eof


:Start_VC_LTL

::VC-LTL核心版本号，由于4.X并不兼容3.X。此值可以用于兼容性判断。
set LTL_CoreVersion=4

set VC_LTL_Helper_Load=true

set PlatformShortName=%Platform%

::用于解决一些新老版本兼容性问题，当适配新平台时可以无视里面的逻辑。
if "%VC-LTLUsedToolsVersion%" NEQ "" set VCLTLToolsVersion=%VC-LTLUsedToolsVersion%
if "%VC-LTLTargetUniversalCRTVersion%" NEQ "" set VCLTLTargetUniversalCRTVersion=%VC-LTLTargetUniversalCRTVersion%


if "%PlatformShortName%" == "" set PlatformShortName=x86

if "%VC_LTL_Root%" == "" call:FoundVC_LTL_Root

if "%VCLTLToolsVersion%" == "" call:FoundVCToolsVersion

if "%VCLTLTargetUniversalCRTVersion%" == "" call:FoundUCRTVersion

if not exist "%VC_LTL_Root%\lib\%Platform%" echo %ERROR_VC_LTL_FILE_MISSING%&&goto:eof
if not exist "%VC_LTL_Root%\VC\%VCLTLToolsVersion%\lib" echo %ERROR_VC_LTL_FILE_MISSING%&&goto:eof
if not exist "%VC_LTL_Root%\ucrt\%VCLTLTargetUniversalCRTVersion%\lib\%Platform%" echo %ERROR_VC_LTL_FILE_MISSING%&&goto:eof


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
echo VC-LTL Tools Version : %VCLTLToolsVersion%
echo VC-LTL UCRT Version : %VCLTLTargetUniversalCRTVersion%
echo Platform : %PlatformShortName%



if /i "%SupportWinXP%" == "true" (set VCLTLPlatformName=WinXP) else (set VCLTLPlatformName=Vista)

if /i "%Platform%" == "arm" set VCLTLPlatformName=Vista
if /i "%Platform%" == "arm64" set VCLTLPlatformName=Vista

if /i "%DisableAdvancedSupport%" == "true" (set LTL_Mode=Light) else (set LTL_Mode=Advanced)

echo Using VC-LTL %VCLTLPlatformName% %LTL_Mode% Mode


::修改Include
set INCLUDE=%VC_LTL_Root%\config\Mode\%LTL_Mode%;%VC_LTL_Root%\config\%VCLTLPlatformName%;%VC_LTL_Root%\VC\%VCLTLToolsVersion%\include;%VC_LTL_Root%\VC\%VCLTLToolsVersion%\atlmfc\include;%VC_LTL_Root%\ucrt\%VCLTLTargetUniversalCRTVersion%;%INCLUDE%

set LIB=%VC_LTL_Root%\lib\%PlatformShortName%\%VCLTLPlatformName%;%VC_LTL_Root%\lib\%PlatformShortName%\%VCLTLPlatformName%\%LTL_Mode%;%VC_LTL_Root%\VC\%VCLTLToolsVersion%\lib\%PlatformShortName%;%VC_LTL_Root%\VC\%VCLTLToolsVersion%\lib\%PlatformShortName%\%VCLTLPlatformName%;%VC_LTL_Root%\ucrt\%VCLTLTargetUniversalCRTVersion%\lib\%PlatformShortName%;%LIB%


::用于解决一些新老版本兼容性问题，当适配新平台时可以无视里面的逻辑。
set OsPlatformName=%VCLTLPlatformName%
set VC-LTLUsedToolsVersion=%VCLTLToolsVersion%
set VC-LTLTargetUniversalCRTVersion=%VCLTLTargetUniversalCRTVersion%

goto:eof



::搜索 VC工具集版本
:FoundVCToolsVersion

set VCLTLToolsVersion=%VCToolsVersion%
if "%VCLTLToolsVersion%" NEQ "" goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24245 Visual C++ CRT Headers Package（Visual Studio 2019 中的2015平台工具集），与14.0.24231差别几乎为0，就不更新了
set VCLTLToolsVersion=14.0.24231
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{F485D86A-9520-3127-B879-861A9315C734} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{F485D86A-9520-3127-B879-861A9315C734} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24234（Visual Studio 2017 15.7中的2015平台工具集），与14.0.24231完全一致
set VCLTLToolsVersion=14.0.24231
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{BDE574B5-6CFE-32B2-9854-C827567E9D6F} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{BDE574B5-6CFE-32B2-9854-C827567E9D6F} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24231（Visual Studio 2017 15.6中的2015平台工具集）
set VCLTLToolsVersion=14.0.24231
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{B0791F3A-6A88-3650-AECF-8AFBE227EC53} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{B0791F3A-6A88-3650-AECF-8AFBE227EC53} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24225（Visual Studio 2017 15.5中的2015平台工具集）
set VCLTLToolsVersion=14.0.24225
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::计算机已经安装Visual Studio 2015 Update3 v14.0.24210（正统Visual Studio 2015）
set VCLTLToolsVersion=14.0.24210
reg query HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A} /v DisplayVersion > nul
if %ERRORLEVEL% == 0 goto ReadVC2015VersionEnd

::如果找不到，那么指定为当前默认版本
set VCLTLToolsVersion=%DefaultVCLTLToolsVersion%
goto:eof

:ReadVC2015VersionEnd

if not exist "%VC_LTL_Root%\VC\%VCLTLToolsVersion%" set VCLTLToolsVersion=%DefaultVCLTLToolsVersion%


goto:eof


::搜索UCRT版本
:FoundUCRTVersion

set VCLTLTargetUniversalCRTVersion=%UCRTVersion%

if "%VCLTLTargetUniversalCRTVersion%" == "" goto FoundUCRTVersionEnd

if exist "%VC_LTL_Root%\ucrt\%VCLTLTargetUniversalCRTVersion%" goto:eof


:FoundUCRTVersionEnd

::找不到指定UCRT版本则默认为10240
set VCLTLTargetUniversalCRTVersion=10.0.10240.0


goto:eof


::获取当前活跃的代码页，用于识别语言环境
:GetCodePage
for /f "tokens=*" %%s in ('chcp') do set __codepage__=%%s


:TryFindCodePage
::为空，那么跳出循环
if "%__codepage__%" == "" goto:eof

echo %__codepage__% | findstr "^[0-9]" > nul

::匹配到只包含数字的，说识别成功，退出循环
if %ERRORLEVEL% == 0 goto:eof

::删除第一个字符串
set "__codepage__=%__codepage__:~1%"

goto:TryFindCodePage

goto:eof


::初始化多国语言资源
:InitMuiStrings

call:GetCodePage

set __LangID__=1033
if "%__codepage__%" == "936" (set __LangID__=2052)


call "%~dp0%__LangID__%\config.strings.cmd"


goto:eof