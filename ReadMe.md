
# VC-LTL——编译精巧的程序原来如此简单


## ReadMe

- [简体中文](ReadMe-zh-Hans.md)


## Changes

### 2.0.0.5 2017-12-10 20:56
* 新增C++ 17 align new/delete支持
* 解决兼容WinXP时可能出现的ntdll.lib依赖失败以及某些函数转发切换到weak别名技术

本次更新主要添加对Vistual Studio 2017 15.5以及更高版本新增的C++ 17功能支持


### 2.0.0.4 2017-12-03 20:40
* 全面覆盖STL、ConcRT库，让C++如鱼得水，尽情使用STL。
* 全新的weak别名转发技术，实现零jmp解决新老CRT命名冲突。


### 2.0.0.3 2017-11-18 14:18
* Vista编译模式，CRT函数覆盖率达到90%以上
* XP编译模式，CRT函数覆盖率达到80%以上
* 简化注册表引用方式。
* 添加std::thread支持


### 2.0.0.2 2017-11-05 14:14
* 新增 _configthreadlocale、_get_current_locale、_create_locale、_free_locale接口支持
* 为Windows XP添加 _time32、_fseeki64静态实现
* 解决BUG 14，新增 _getpid、_sys_nerr、_sys_errlist无法使用问题（感谢 HwangBae）
* 新增C++类，mutex、thread、xtime、xonce支持
* 优化编译方式，消除无意义符号，减少ltl库体积


### 2.0.0.1 2017-10-29 22:23
* 新增iostream、stringstream支持
* 解决使用_fstat32、_fstat32i64、_fstat64i32、_stat32、_stat32i64、_stat64i32、_wstat32、_wstat32i64、_wstat64i32导致编译不通过问题
* 修正 __acrt_iob_func 始终返回输入流问题
* 解决 type_info operator != 功能无法使用问题（感谢  sonyps5201314）
* 解决_daylight，_dstbias，_timezone，_tzname无法使用问题（感谢  sonyps5201314）
* 解决32位 SSE高精度数据函数无法使用问题，比如_libm_sse2_tan_precise，_libm_sse2_sqrt_precise，_libm_sse2_sin_precise（感谢 stsm85）


### 1.0.0.13 2017-10-11 14:00
* 解决BUG，atanh、acosh、asinh无法使用问题（感谢 stsm85）
* 新增Windows 10 16299 UCRT支持
* 移除Windows 10 14393 UCRT支持

PS：16299已经发布，因此移除老版本14393支持。相关项目请迁徙到15063或者最新16299。

### 1.0.0.12 2017-09-15 13:33
* 解决BUG，使用strcat_s时在Windows XP中提示找不到指定符号。（感谢 stsm85）
* 解决BUG，解决SSE2除法导致编译不通过问题（感谢 stsm85）
* 解决BUG，解决wcstoll、vsnprintf、rand_s、strtoll无法使用问题（感谢 stsm85）
* 代码调整，消除所有VC-LTL编译警告，强迫症患者福音。

PS:本次更新后，FastCopy、winpck相关程序直接可以使用VC-LTL编译。


### 1.0.0.11 2017-08-23 19:00
* 解决BUG，使用_difftime64时Windows XP无法运行。
* 解决BUG，_msvcrt.cpp始终会引入urct_14393.lib问题（感谢 亮叔叔）
* 更新VC141头文件以及实现，全部同步到最新14.11

### 1.0.0.10 2017-07-28 20:28
* 解决BUG 9，某些时候编译器引用异常导致XP模式时意外引入_except_handler4_common（感谢 HwangBae）
* 解决BUG 8，修复typeid功能无法使用问题（感谢 HwangBae）
* 调整异常实现代码，尽可能复用msvcrt.dll代码减少代码体积
* 解决BUG，修复无法使用__argc、__argv、__wargv、_environ、_wenviron全局变量问题（感谢 亮叔叔）
* 解决微软BUG，修复使用ATL库的程序不支持XP RTM问题

### 1.0.0.9 2017-05-26 14:46
* 改进Windows XP支持
* 优化库结构裁剪ltl库体积
* 解决使用自定义异常导致程序编译不通过问题
* 调整*_p系列函数定义，以免使用*_p系列函数时编译不通过
* 解决使用浮点除法时导致编译不通过问题

### 1.0.0.8 2017-04-25 20:37
* 简化库引用方式
* 更新14393 ucrt到最新版（2017-01-05版）
* 添加15063 ucrt支持

### 1.0.0.7 2017-04-22 19:26
* 初步添加Windows XP支持
* 添加C++异常支持
* 添加/GS特性支持
* 添加/guard:cf支持
* VC140库升级到Vistual Studio 2015 Update3

### 1.0.0.6 2017-3-18 13:46
* 优化文件引用

### 1.0.0.5 2017-3-16 20:53
* 新增VC 2017支持

### 1.0.0.3 2016-11-28 12:54
* 改进对C工程的兼容性

### 1.0.0.2 2016-06-14 12:52
* 解决C编译不通过问题

### 1.0.0.1 13:42 2016/5/23
* 解决C++异常无法使用问题
