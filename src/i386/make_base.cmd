@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
@call "%~dp0..\..\VC-LTL helper for nmake.cmd"

set libfileroot=%~dp0..\..\lib\x86

set libfile=%libfileroot%\msvcrt.lib

copy "%~dp0msvcrt.lib" "%libfile%" /y


lib /MACHINE:%Platform% /def:"%~dp0msvcrt.def" /out:"%~dp0msvcrtdll.lib"
lib /MACHINE:%Platform% /def:"%~dp0..\msvcp60.def" /out:"%~dp0msvcp60.lib"
lib /MACHINE:%Platform% /def:"%~dp0..\msvcrt_form_msvcp60.def" /out:"%~dp0msvcrt_form_msvcp60.lib"
lib /MACHINE:%Platform% /def:"%~dp0..\msvcrt_form_msvcp60_WinXP.def" /out:"%~dp0msvcrt_form_msvcp60_WinXP.lib"

lib "%libfile%" "%~dp0msvcrtdll.lib"
lib "%libfile%" "%~dp0msvcrt_form_msvcp60.lib"
lib "%libfile%" "%~dp0msvcp60.lib"

echo 删除msvcrt.dll导出符号

del "%libfileroot%\objs" /S /Q /F

::删除所有obj文件
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" RemoveAllObj "%libfile%"

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:x86 /def:"%~dp0..\msvcrt_forward.def"  /out:"%libfileroot%\objs\msvcrt_forward"
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:x86 /def:"%~dp0..\msvcrt_vista.def"    /out:"%libfileroot%\objs\msvcrt_forward"

::填充XP不支持的接口
::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" x86 "%~dp0..\msvcrt_winxp.def" msvcrt.dll msvcrt2.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:x86 /def:"%~dp0..\msvcrt_winxp.def"  /out:"%libfileroot%\objs\msvcrt_winxp"

::填充轻量模式
::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" x86 "%~dp0..\msvcrt_light.def" msvcrt.dll msvcrt2.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:x86 /def:"%~dp0..\msvcrt_light.def"  /out:"%libfileroot%\objs\msvcrt_light"

::填充增强模式
::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" x86 "%~dp0..\msvcrt_advanced.def" msvcrt.dll msvcrt2.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" CreateWeaks  /MACHINE:x86 /def:"%~dp0..\msvcrt_advanced.def"  /out:"%libfileroot%\objs\msvcrt_advanced"


set libfiletmp=%libfile%

::copy "%libfile%" "%libfiletmp%" /y


::删除被外部标记的导入符号
::lib "%libfile%" /remove:msvcrt2.dll
::删除所有msvcrt.dll导入
::lib "%libfiletmp%" /remove:msvcrt.dll



::"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfiletmp%" x86 all msvcrt2.dll msvcrt.dll



pushd "%libfileroot%"

md "%libfileroot%\Vista"
md "%libfileroot%\Vista\Advanced"
md "%libfileroot%\Vista\Light"

md "%libfileroot%\WinXP"
md "%libfileroot%\WinXP\Advanced"
md "%libfileroot%\WinXP\Light\"


"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" x86 "%~dp0..\msvcrt_forward.def" msvcrt.dll msvcrt3.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%libfile%" x86 "%~dp0..\msvcrt_vista.def"   msvcrt.dll msvcrt3.dll
lib "%libfile%" /remove:msvcrt3.dll

::添加通用转发器
lib "%libfile%" objs\msvcrt_forward\*



::生成msvcrt_light.lib for vista
set tagetlibfile=%libfileroot%\Vista\Light\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_light\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_light.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll

::生成msvcrt_light_xp.lib
set tagetlibfile=%libfileroot%\WinXP\Light\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_winxp\*
lib "%tagetlibfile%" objs\msvcrt_light\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_winxp.def" msvcrt.dll msvcrt2.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_light.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll
lib "%tagetlibfile%" "%~dp0ntdlllite.lib"
lib "%tagetlibfile%" "%~dp0msvcrt_form_msvcp60_WinXP.lib"

::生成msvcrt_advanced.lib for vista
set tagetlibfile=%libfileroot%\Vista\Advanced\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_advanced\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_advanced.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll

::生成msvcrt_advanced_xp.lib
set tagetlibfile=%libfileroot%\WinXP\Advanced\msvcrt_Platform.lib
copy "%libfiletmp%" "%tagetlibfile%" /y
lib "%tagetlibfile%" objs\msvcrt_winxp\*
lib "%tagetlibfile%" objs\msvcrt_advanced\*

"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_winxp.def" msvcrt.dll msvcrt2.dll
"D:\用户数据\Documents\Visual Studio 2017\Projects\ltlbuild\Debug\LibMaker.exe" renamelib "%tagetlibfile%" x86 "%~dp0..\msvcrt_advanced.def" msvcrt.dll msvcrt2.dll
lib "%tagetlibfile%" /remove:msvcrt2.dll
lib "%tagetlibfile%" "%~dp0ntdlllite.lib"
lib "%tagetlibfile%" "%~dp0msvcrt_form_msvcp60_WinXP.lib"


popd


::lib "%libfile%" "%~dp0ntdlllite.lib"

del "%~dp0msvcrtdll.*" /q

pause