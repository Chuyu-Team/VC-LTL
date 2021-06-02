setlocal
pushd "%~dp0.."

del x64\mtss.obj
del x64\msvcmrt_add.lib

lib x64\msvcmrt.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr\amd64\mtss.obj /out:x64\mtss.obj
lib x64\mtss.obj /out:x64\msvcmrt_add.lib

popd