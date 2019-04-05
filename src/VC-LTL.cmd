@echo off

::自动生成VC-LTL库


::@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set Path=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE;%Path%

::先生成静态库
call:Build Static "x86 x64 ARM ARM64"

call:Build Static_Spectre "x86 x64 ARM ARM64"

call:Build Static_WinXP "x86 x64"

call:Build Static_WinXP_Spectre "x86 x64"

::生成动态UCRT库
call:Build Redist "x86 x64 ARM ARM64" "ucrt\ucrt.vcxproj"
call:Build Dynamic "x86 x64 ARM ARM64" "ucrt\ucrt.vcxproj"



::生成 CRT

call:BuildCRT 14.0.23918 "x86 x64 ARM"

call:BuildCRT 14.0.24210 "x86 x64 ARM"

call:BuildCRT 14.0.24225 "x86 x64 ARM"

call:BuildCRT 14.0.24231 "x86 x64 ARM"

call:BuildCRT 14.10.25017 "x86 x64 ARM ARM64"

call:BuildCRT 14.11.25503 "x86 x64 ARM ARM64"

call:BuildCRT 14.12.25827 "x86 x64 ARM ARM64"

call:BuildCRT 14.13.26128 "x86 x64 ARM ARM64"

call:BuildCRT 14.14.26428 "x86 x64 ARM ARM64"

call:BuildCRT 14.15.26726 "x86 x64 ARM ARM64"

call:BuildCRT 14.16.27023 "x86 x64 ARM ARM64"

call:BuildCRT 14.20.27508 "x86 x64 ARM ARM64"

::生成动态Spectre库
call:Build Dynamic_Spectre "x86 x64 ARM ARM64"

pause

goto:eof


::BuildCRT 14.0.24231 "x86 x64 ARM ARM64"
:BuildCRT

::生成msvcrt.lib
call:Build Dynamic %2 "%1\Build\ltlbuild\ltlbuild.vcxproj"

::生成msvcmrt.lib
call:Build Dynamic %2 "%1\Build\ltlbuild_CLR\ltlbuild_CLR.vcxproj"

::生成vcruntime.lib
call:Build Redist  %2 "%1\Build\vcruntime\vcruntime.vcxproj"

::生成msvcprt.lib
call:Build Redist  %2 "%1\Build\stl\stl.vcxproj"
call:Build Dynamic %2 "%1\Build\stl\stl.vcxproj"

::生成concrt.lib
call:Build Redist  %2 "%1\Build\ConcRT\ConcRT.vcxproj"

goto:eof


::Build Dynamic "x86 x64 ARM ARM64" ["ucrt\ucrt.vcxproj"]
:Build

for %%i in (%~2)do call:BuildWithErrorCheck "%1|%%i" %3

goto:eof

:BuildWithErrorCheck :: Dynamic|x86   ["ucrt\ucrt.vcxproj"]

if "%2"=="" ( devenv "%~dp0VC-LTL.sln" /Build %1 ) else ( devenv "%~dp0VC-LTL.sln" /Build %1 /project %2 )

if %ERRORLEVEL% NEQ 0 echo %1 %2 编译遇到问题，请查错后继续。&&pause

goto:eof
