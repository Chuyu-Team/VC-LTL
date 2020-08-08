@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_arm.bat"

set libfile=%~dp0..\..\..\..\VC\14.27.29110\lib\%Platform%\vccorlib.lib

del "%libfile%" /q
lib /MACHINE:%Platform% /DEF:"%~dp0..\..\..\..\src\%Platform%\wincorlib.def" /out:"%libfile%"
lib /MACHINE:%Platform% "%libfile%" "%~dp0vccorlib.lib"

lib "%libfile%" /remove:vccorlib140.dll
lib /MACHINE:%Platform% "%libfile%" "%~dp0..\..\..\..\Redist\%Platform%\vccorlib140_ltl.lib"

pause