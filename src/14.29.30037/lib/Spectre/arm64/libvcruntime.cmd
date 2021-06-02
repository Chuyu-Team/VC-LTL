setlocal
pushd "%~dp0..\.."

rmdir /s /q Spectre\arm64\libvcruntime
del Spectre\arm64\libvcruntime-ltl.lib

md Spectre\arm64\libvcruntime
md Spectre\arm64\libvcruntime\arm64ec
md Spectre\arm64\libvcruntime\arm64

lib Spectre\arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm64ec\handlers_arm64ec.obj /out:Spectre\arm64\libvcruntime\arm64ec\handlers_arm64ec.obj
lib Spectre\arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm64ec\handlers_amd64.obj /out:Spectre\arm64\libvcruntime\arm64ec\handlers_amd64.obj
lib Spectre\arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm64ec\notify.obj /out:Spectre\arm64\libvcruntime\arm64ec\notify.obj
lib Spectre\arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm64\handlers.obj /out:Spectre\arm64\libvcruntime\arm64\handlers.obj
lib Spectre\arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr_spectre\arm64\notify.obj /out:Spectre\arm64\libvcruntime\arm64\notify.obj
lib Spectre\arm64\libvcruntime\arm64ec\*.obj Spectre\arm64\libvcruntime\arm64\*.obj /out:Spectre\arm64\libvcruntime-ltl.lib /MACHINE:arm64x

popd