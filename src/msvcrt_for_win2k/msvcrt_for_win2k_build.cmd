@echo off

::用于生成msvcp60_static.obj 以及 msvcrt_for_win2k.obj
pushd "%~dp0"

set AdditionalOptions=/O1 /Os /Oi /GS- /Z7 /MT /Zl /arch:IA32 /Zc:inline /c /D "NDEBUG" /D _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS /D _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH /D _CRTBLD


::x86
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"

setlocal
set SupportWinXP=true
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcrt_for_win2k.obj" msvcrt_for_win2k\msvcrt_for_win2k.cpp
endlocal

setlocal
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
endlocal

endlocal

::x64
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

setlocal
set SupportWinXP=true
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
endlocal

setlocal
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
endlocal

endlocal

::arm
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_arm.bat"
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
endlocal

::arm64
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_arm64.bat"
call "..\..\VC-LTL helper for nmake.cmd"
set include=%VC_LTL_Root%\src\ucrt\inc;%VC_LTL_Root%\src\%VCToolsVersion%\vcruntime;%VC_LTL_Root%\src;%include%

cl %AdditionalOptions% /Fo"%VC_LTL_Root%\lib\%Platform%\%VCLTLPlatformName%\msvcp60_static.obj" msvcp60\msvcp60_static.cpp
endlocal

popd
pause
goto:eof
