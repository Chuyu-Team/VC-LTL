setlocal
pushd "%~dp0..\.."

rmdir /s /q Spectre\arm\libvcruntime
del Spectre\arm\libvcruntime-ltl.lib

md Spectre\arm\libvcruntime

lib Spectre\arm\libvcruntime.lib /EXTRACT:D:\agent\_work\14\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm\handlers.obj /out:Spectre\arm\libvcruntime\handlers.obj
lib Spectre\arm\libvcruntime.lib /EXTRACT:D:\agent\_work\14\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm\notify.obj /out:Spectre\arm\libvcruntime\notify.obj
lib Spectre\arm\libvcruntime\*.obj /out:Spectre\arm\libvcruntime-ltl.lib

popd