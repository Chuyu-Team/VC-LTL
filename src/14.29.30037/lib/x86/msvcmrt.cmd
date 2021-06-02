setlocal
pushd "%~dp0.."

del x86\mtss.obj
del x86\msvcmrt_add.lib

lib x86\msvcmrt.lib /EXTRACT:d:\agent\_work\4\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr\x86\mtss.obj /out:x86\mtss.obj
lib x86\mtss.obj /out:x86\msvcmrt_add.lib

popd