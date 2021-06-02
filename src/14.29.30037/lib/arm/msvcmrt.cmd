setlocal
pushd "%~dp0.."

del arm\mtss.obj
del arm\msvcmrt_add.lib

lib arm\msvcmrt.lib /EXTRACT:D:\agent\_work\14\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr\arm\mtss.obj /out:arm\mtss.obj
lib arm\mtss.obj /out:arm\msvcmrt_add.lib

popd