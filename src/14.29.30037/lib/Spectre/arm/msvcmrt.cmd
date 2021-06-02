setlocal
pushd "%~dp0..\.."

del Spectre\arm\mtss.obj
del Spectre\arm\msvcmrt_add.lib

lib Spectre\arm\msvcmrt.lib /EXTRACT:D:\agent\_work\9\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr_spectre\arm\mtss.obj /out:Spectre\arm\mtss.obj
lib Spectre\arm\mtss.obj /out:Spectre\arm\msvcmrt_add.lib

popd