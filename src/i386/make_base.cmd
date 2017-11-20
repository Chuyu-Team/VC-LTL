pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\bin\HostX86\x86"

set libfile=%~dp0..\..\x86\msvcrt.lib


copy "%~dp0msvcrt.lib" "%libfile%" /y

echo É¾³ýmsvcrt.dllµ¼³ö·ûºÅ

lib "%libfile%" /remove:e:\5359.obj.x86fre\minkernel\crts\crtw32\misc\dlllib\objfre\i386\gs_report.obj
lib "%libfile%" /remove:e:\5359.obj.x86fre\minkernel\crts\crtw32\eh\dlllib\objfre\i386\ehvecctr.obj
lib "%libfile%" /remove:e:\5359.obj.x86fre\minkernel\crts\crtw32\eh\dlllib\objfre\i386\ehvecdtr.obj


lib /def:"%~dp0msvcrt.def" /out:"%~dp0msvcrtdll.lib"

lib "%libfile%" "%~dp0msvcrtdll.lib"

del "%~dp0msvcrtdll.*" /q

popd

pause