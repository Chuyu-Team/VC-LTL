setlocal
pushd "%~dp0.."

rmdir /s /q arm\libvcruntime
del arm\libvcruntime-ltl.lib

md arm\libvcruntime

lib arm\libvcruntime.lib /EXTRACT:D:\agent\_work\14\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm\handlers.obj /out:arm\libvcruntime\handlers.obj
lib arm\libvcruntime.lib /EXTRACT:D:\agent\_work\14\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\arm\notify.obj /out:arm\libvcruntime\notify.obj
lib arm\libvcruntime\*.obj /out:arm\libvcruntime-ltl.lib

popd