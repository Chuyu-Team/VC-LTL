set BuiltInVsWhereExe="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not defined ProgramFiles(x86) ( set "BuiltInVsWhereExe="%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"" )

if not exist %BuiltInVsWhereExe% (echo 请安装VS2017或者更高版本然后继续！& goto:eof )

for /f "tokens=*" %%i in ('%BuiltInVsWhereExe% -latest -prerelease -property installationPath') do ( set LatestVisualStudioRoot=%%i)

@call "%LatestVisualStudioRoot%\VC\Auxiliary\Build\vcvarsamd64_arm64.bat"

set libfile=%~dp0..\..\..\..\VC\14.11.25503\lib\%Platform%\vccorlib.lib

del "%libfile%" /q
lib /MACHINE:%Platform% /DEF:"%~dp0..\..\..\..\src\%Platform%\wincorlib.def" /out:"%libfile%"
lib /MACHINE:%Platform% "%libfile%" "%~dp0vccorlib.lib"

lib "%libfile%" /remove:vccorlib140.dll
::lib /MACHINE:%Platform% "%libfile%" "%~dp0..\..\..\..\Redist\%Platform%\vccorlib140_ltl.lib"

pause