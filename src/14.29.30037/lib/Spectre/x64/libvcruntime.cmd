setlocal
pushd "%~dp0..\.."

rmdir /s /q Spectre\x64\libvcruntime
del Spectre\x64\libvcruntime-ltl.lib

md Spectre\x64\libvcruntime

lib Spectre\x64\libvcruntime.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\amd64\handlers.obj /out:Spectre\x64\libvcruntime\handlers.obj
lib Spectre\x64\libvcruntime.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\amd64\notify.obj /out:Spectre\x64\libvcruntime\notify.obj
lib Spectre\x64\libvcruntime\*.obj /out:Spectre\x64\libvcruntime-ltl.lib

popd