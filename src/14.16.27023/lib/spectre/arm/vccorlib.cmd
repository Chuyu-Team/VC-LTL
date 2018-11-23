@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsamd64_arm.bat"

set libfile=%~dp0..\..\..\..\..\VC\14.16.27023\lib\Spectre\%Platform%\vccorlib.lib

del "%libfile%" /q
lib /MACHINE:%Platform% /DEF:"D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\src\%Platform%\wincorlib.def" /out:"%libfile%"
lib /MACHINE:%Platform% "%libfile%" "%~dp0vccorlib.lib"

lib "%libfile%" /remove:vccorlib140.dll
lib /MACHINE:%Platform% "%libfile%" "D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\Redist\%Platform%\vccorlib140_ltl.lib"

pause