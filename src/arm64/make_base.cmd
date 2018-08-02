@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsamd64_arm64.bat"
::@call "%~dp0..\..\VC-LTL helper for nmake.cmd"

set libfileroot=%~dp0..\..\lib\arm64

set libfile=%libfileroot%\msvcrt.lib


lib /MACHINE:%Platform% /def:"%~dp0msvcrt.def" /out:"%libfile%"
lib /MACHINE:%Platform% /def:"%~dp0..\msvcp60.def" /out:"%~dp0msvcp60.lib"
lib /MACHINE:%Platform% /def:"%~dp0..\msvcrt_form_msvcp60.def" /out:"%~dp0msvcrt_form_msvcp60.lib"

lib "%libfile%" "%~dp0msvcrt_form_msvcp60.lib"
lib "%libfile%" "%~dp0msvcp60.lib"

del "%libfileroot%\objs" /S /Q /F

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:arm64 /def:"%~dp0..\msvcrt_forward.def"  /out:"%libfileroot%\objs\msvcrt_forward"

::填充轻量模式
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:arm64 /def:"%~dp0..\msvcrt_light.def"  /out:"%libfileroot%\objs\msvcrt_light"

::填充增强模式
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:arm64 /def:"%~dp0..\msvcrt_advanced.def"  /out:"%libfileroot%\objs\msvcrt_advanced"


set libfiletmp=%libfile%

::copy "%libfile%" "%libfiletmp%" /y

::删除被外部标记的导入符号
::lib "%libfile%" /remove:msvcrt2.dll
::删除所有msvcrt.dll导入
::lib "%libfiletmp%" /remove:msvcrt.dll

::删除所有obj文件
::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" RemoveAllObj "%libfiletmp%"

::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfiletmp%" x64 all msvcrt2.dll msvcrt.dll



pushd "%libfileroot%"

md "%libfileroot%\Vista"
md "%libfileroot%\Vista\Advanced"
md "%libfileroot%\Vista\Light"


"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" arm64 "%~dp0..\msvcrt_forward.def" msvcrt.dll msvcrt3.dll
lib "%libfile%" /remove:msvcrt3.dll

::添加通用转发器
lib "%libfile%" objs\msvcrt_forward\*



::生成msvcrt_light.lib for vista
set tagetlibfile=%libfileroot%\Vista\Light\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_light\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" arm64 "%~dp0..\msvcrt_light.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll

::生成msvcrt_advanced.lib for vista
set tagetlibfile=%libfileroot%\Vista\Advanced\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_advanced\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" arm64 "%~dp0..\msvcrt_advanced.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll


::lib "%libfile%" "%~dp0ntdlllite.lib"

popd

pause