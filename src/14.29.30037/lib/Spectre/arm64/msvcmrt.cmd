setlocal
pushd "%~dp0..\.."

del Spectre\arm64\mtss.obj
del Spectre\arm64\msvcmrt_add.lib

lib Spectre\arm64\msvcmrt.lib /EXTRACT:D:\agent\_work\10\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr_spectre\arm64\mtss.obj /out:Spectre\arm64\mtss.obj
lib Spectre\arm64\mtss.obj /out:Spectre\arm64\msvcmrt_add.lib

popd