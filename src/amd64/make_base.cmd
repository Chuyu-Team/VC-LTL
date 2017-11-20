pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\bin\HostX64\x64"

set libfile=%~dp0..\..\x64\msvcrt.lib


copy "%~dp0msvcrt.lib" "%libfile%" /y

echo É¾³ýmsvcrt.dllµ¼³ö·ûºÅ

lib "%libfile%" /remove:d:\5359.obj.amd64fre\minkernel\crts\crtw32\misc\dlllib\objfre\amd64\gs_report.obj
lib "%libfile%" /remove:d:\5359.obj.amd64fre\minkernel\crts\crtw32\eh\dlllib\objfre\amd64\ehvecctr.obj
lib "%libfile%" /remove:d:\5359.obj.amd64fre\minkernel\crts\crtw32\eh\dlllib\objfre\amd64\ehvecdtr.obj


lib /def:"%~dp0msvcrt.def" /out:"%~dp0msvcrtdll.lib"

lib "%libfile%" "%~dp0msvcrtdll.lib"

del "%~dp0msvcrtdll.*" /q

popd

pause