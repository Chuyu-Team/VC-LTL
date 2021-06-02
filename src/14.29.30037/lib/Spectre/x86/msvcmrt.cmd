setlocal
pushd "%~dp0..\.."

del Spectre\x86\mtss.obj
del Spectre\x86\msvcmrt_add.lib

lib Spectre\x86\msvcmrt.lib /EXTRACT:d:\agent\_work\4\s\Intermediate\vctools\msvcmrt.nativeproj_1130085516\objr_spectre\x86\mtss.obj /out:Spectre\x86\mtss.obj
lib Spectre\x86\mtss.obj /out:Spectre\x86\msvcmrt_add.lib

popd