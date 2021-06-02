setlocal
pushd "%~dp0.."

del arm64\mtss.obj
del arm64\msvcmrt_add.lib

lib arm64\msvcmrt.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr\arm64\mtss.obj /out:arm64\mtss.obj
lib arm64\mtss.obj /out:arm64\msvcmrt_add.lib

popd