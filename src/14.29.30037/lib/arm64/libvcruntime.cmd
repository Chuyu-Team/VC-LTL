setlocal
pushd "%~dp0.."

rmdir /s /q arm64\libvcruntime
del arm64\libvcruntime-ltl.lib

md arm64\libvcruntime
md arm64\libvcruntime\arm64ec
md arm64\libvcruntime\arm64

lib arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm64ec\handlers_arm64ec.obj /out:arm64\libvcruntime\arm64ec\handlers_arm64ec.obj
lib arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm64ec\handlers_amd64.obj /out:arm64\libvcruntime\arm64ec\handlers_amd64.obj
lib arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm64ec\notify.obj /out:arm64\libvcruntime\arm64ec\notify.obj
lib arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm64\handlers.obj /out:arm64\libvcruntime\arm64\handlers.obj
lib arm64\libvcruntime.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm64\notify.obj /out:arm64\libvcruntime\arm64\notify.obj
lib arm64\libvcruntime\arm64ec\*.obj arm64\libvcruntime\arm64\*.obj /out:arm64\libvcruntime-ltl.lib /MACHINE:arm64x

popd