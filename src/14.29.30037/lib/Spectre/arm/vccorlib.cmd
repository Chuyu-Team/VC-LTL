setlocal
set libfile=%~dp0..\..\..\..\..\VC\%CurrentBuildVersion%\lib\Spectre\%Platform%\vccorlib.lib

del "%libfile%" /q
lib /MACHINE:%Platform% /DEF:"%~dp0..\..\..\..\..\src\%Platform%\wincorlib.def" /out:"%libfile%"
lib /MACHINE:%Platform% "%libfile%" "%~dp0vccorlib.lib"

lib "%libfile%" /remove:vccorlib140.dll
lib /MACHINE:%Platform% "%libfile%" "%~dp0..\..\..\..\..\Redist\%Platform%\vccorlib140_ltl.lib"
