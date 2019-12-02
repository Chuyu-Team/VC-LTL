@echo off

::用于生成legacy_stdio_definitions
pushd "%~dp0"

set AdditionalOptions=/O1 /Os /Oi /GS- /Z7 /MT /Zl /arch:IA32 /Zc:inline /c /D "NDEBUG"


::x86
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"

setlocal
set SupportWinXP=true
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

setlocal
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

endlocal

::x64
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

setlocal
set SupportWinXP=true
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

setlocal
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

endlocal

::arm
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_arm.bat"
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

::arm64
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_arm64.bat"
call "..\VC-LTL helper for nmake.cmd"
call:Build
endlocal

popd
pause
goto:eof

:Build
setlocal
if "%Platform%"=="" set Platform=x86
set include=%~dp0%VCToolsVersion%\vcruntime;%include%

del "..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp" /S /Q /F >nul 2>nul

mkdir "..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp\objs" >nul 2>nul

cl %AdditionalOptions% /Fo"..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp\objs\legacy_conio_definitions.obj" legacy_conio_definitions.cpp
cl %AdditionalOptions% /Fo"..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp\objs\legacy_stdio_definitions.obj" legacy_stdio_definitions.cpp
lib /MACHINE:%Platform% /def:"legacy_stdio_definitions_%VCLTLPlatformName%.def" /out:"..\lib\%Platform%\%VCLTLPlatformName%\Light\legacy_stdio_definitions.lib"
LibMaker.exe CreateWeaks  /MACHINE:%Platform% /def:"%~dp0legacy_stdio_definitions_alias_%VCLTLPlatformName%.def"  /out:"%~dp0..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp\alias"

pushd "%~dp0..\lib\%Platform%\%VCLTLPlatformName%\Light\tmp"

lib "%~dp0..\lib\%Platform%\%VCLTLPlatformName%\Light\legacy_stdio_definitions.lib" "objs\*"
lib "%~dp0..\lib\%Platform%\%VCLTLPlatformName%\Light\legacy_stdio_definitions.lib" "alias\*"

popd
endlocal
goto:eof