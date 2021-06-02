setlocal
pushd "%~dp0..\.."

del Spectre\x64\mtss.obj
del Spectre\x64\msvcmrt_add.lib

lib Spectre\x64\msvcmrt.lib /EXTRACT:D:\agent\_work\13\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr_spectre\amd64\mtss.obj /out:Spectre\x64\mtss.obj
lib Spectre\x64\mtss.obj /out:Spectre\x64\msvcmrt_add.lib

popd