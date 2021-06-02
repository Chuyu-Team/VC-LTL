setlocal
pushd "%~dp0.."

rmdir /s /q x64\libvcruntime
del x64\libvcruntime-ltl.lib

md x64\libvcruntime

lib x64\libvcruntime.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\amd64\handlers.obj /out:x64\libvcruntime\handlers.obj
lib x64\libvcruntime.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\libvcruntime.nativeproj_520857879\objr\amd64\notify.obj /out:x64\libvcruntime\notify.obj
lib x64\libvcruntime\*.obj /out:x64\libvcruntime-ltl.lib

popd