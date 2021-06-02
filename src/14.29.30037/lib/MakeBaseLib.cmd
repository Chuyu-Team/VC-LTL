set CurrentBuildVersion=14.29.30037



set BuiltInVsWhereExe="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not defined ProgramFiles(x86) ( set "BuiltInVsWhereExe="%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"" )

if not exist %BuiltInVsWhereExe% (echo 请安装VS2017或者更高版本然后继续！& goto:eof )

for /f "tokens=*" %%i in ('%BuiltInVsWhereExe% -latest -prerelease -property installationPath') do ( set LatestVisualStudioRoot=%%i)


pushd "%~dp0"

::arm Base构建
setlocal
call "%LatestVisualStudioRoot%\VC\Auxiliary\Build\vcvarsamd64_arm.bat"

call "arm\libvcruntime.cmd"
call "arm\msvcmrt.cmd"
call "arm\msvcrt.cmd"
call "arm\vccorlib.cmd"

call "Spectre\arm\libvcruntime.cmd"
call "Spectre\arm\msvcmrt.cmd"
call "Spectre\arm\msvcrt.cmd"
call "Spectre\arm\vccorlib.cmd"
endlocal

::arm64 Base构建
setlocal
call "%LatestVisualStudioRoot%\VC\Auxiliary\Build\vcvarsamd64_arm64.bat"

call "arm64\libvcruntime.cmd"
call "arm64\msvcmrt.cmd"
call "arm64\msvcrt.cmd"
call "arm64\vccorlib.cmd"

call "Spectre\arm64\libvcruntime.cmd"
call "Spectre\arm64\msvcmrt.cmd"
call "Spectre\arm64\msvcrt.cmd"
call "Spectre\arm64\vccorlib.cmd"
endlocal

::x64 Base构建
setlocal
call "%LatestVisualStudioRoot%\VC\Auxiliary\Build\vcvars64.bat"

call "x64\libvcruntime.cmd"
call "x64\msvcmrt.cmd"
call "x64\msvcrt.cmd"
call "x64\vccorlib.cmd"

call "Spectre\x64\libvcruntime.cmd"
call "Spectre\x64\msvcmrt.cmd"
call "Spectre\x64\msvcrt.cmd"
call "Spectre\x64\vccorlib.cmd"
endlocal

::x86 Base构建
setlocal
call "%LatestVisualStudioRoot%\VC\Auxiliary\Build\vcvarsamd64_x86.bat"

call "x86\msvcmrt.cmd"
call "x86\msvcrt.cmd"
call "x86\vccorlib.cmd"

call "Spectre\x86\msvcmrt.cmd"
call "Spectre\x86\msvcrt.cmd"
call "Spectre\x86\vccorlib.cmd"
endlocal

popd