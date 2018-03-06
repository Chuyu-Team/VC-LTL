

"%~dp0YYLib.exe" /MACHINE:x64 /def:"%~dp0msvcrt_winxp.def"  /out:"%~dp0..\x64\msvcrt_win2003.obj"
"%~dp0YYLib.exe" /MACHINE:x86 /def:"%~dp0msvcrt_winxp.def"  /out:"%~dp0..\x86\msvcrt_winxp.obj"

"%~dp0YYLib.exe" /MACHINE:x64 /def:"%~dp0msvcrt_advanced.def"  /out:"%~dp0..\x64\msvcrt_advanced.obj"
"%~dp0YYLib.exe" /MACHINE:x86 /def:"%~dp0msvcrt_advanced.def"  /out:"%~dp0..\x86\msvcrt_advanced.obj"

"%~dp0YYLib.exe" /MACHINE:x64 /def:"%~dp0msvcrt_light.def"  /out:"%~dp0..\x64\msvcrt_light.obj"
"%~dp0YYLib.exe" /MACHINE:x86 /def:"%~dp0msvcrt_light.def"  /out:"%~dp0..\x86\msvcrt_light.obj"
pause