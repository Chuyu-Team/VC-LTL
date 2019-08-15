# VC-LTL - An elegant way to compile lighter binaries.

- [简体中文](ReadMe.osc.md)

![Logo](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/Logo.png)<br/>
I would like to turn into a stone bridge, go through 500 years of wind, 500 years of Sun, rain for 500 years, but she walked off a bridge!

## 1. About VC-LTL
VC-LTL is an open source CRT library based on the MS VCRT that reduce program binary size and say goodbye to Microsoft runtime DLLs, such as msvcr120.dll, api-ms-win-crt-time-l1-1-0.dll and other dependencies.

VC-LTL was originally a dedicated Dism++ runtime and was isolated and open sourced from Dism++ source code on Mar 6, 2017.

There are plenty of modules in large projects. If all binaries uses static 
compilation, it will use plenty of disk space and finally may crashed because 
of the fiber local storage (FLS) limitation.

But the VC-LTL can make your project using the built-in `msvcrt.dll` in the 
Windows. It solves the C runtime deployment problem and the fiber local storage
(FLS) limitation problem effectively, and greatly reduce the binaries size. 
What a handy library!

Everyone can use it for free, even for the commerical use. Of course, I hope 
that if you mentioned the VC-LTL in your project, because I want to help more
people.

* GitHub: [github.com/Chuyu-Team/VC-LTL (English)](https://github.com/Chuyu-Team/VC-LTL)    
* Gitee: [gitee.com/Chuyu-Team/VC-LTL (Chinese)](https://gitee.com/Chuyu-Team/VC-LTL)    
* QQ Group: [633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a)

### 1.1. The principle of VC-LTL
After using the VC-LTL, the binaries will be dynamically linked to the built-in
`msvcrt.dll` in the Windows to reduce the binaries size. It is generally 
available if you use the VC-LTL in the project using the C Runtime and the STL.
However you cannot use the VC-LTL in the MFC project with the VC-LTL because it
is too complex to support.

> After using the VC-LTL, the size of the binaries will reduce about 30%
if compiled from the C++ source code, and about 50% if compiled 
from the pure C source code.

### 1.2. Highlight
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 无缝使用最新C/C++库以及最新编译器，尽情的使用最新规范。神马异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。

> Let's say goodbye to the Visual Studio 2008.

## 2. VC-LTL Compatibility

|  Module  | XP Mode | Vista Mode | UCRT Mode | Files 
| :------: | :-----: | :--------: | :-------: | --------
|  CRT     | 94.716% | 96.347%    | 100%      | vcruntime.lib, libvcruntime.lib, msvcrt.lib, msvcmrt.lib, msvcrt_Platform.lib, libucrt.lib, ucrt.lib, libucrt_shared.lib, vc.lib
|  STL     | 100%    | 100%       | 100%      | libcpmt.lib, msvcprt.lib
|  ConcRT  | 100%    | 100%       | 100%      | libconcrt.lib, concrt.lib
|  WinRT   |  X      | 100%       |  X        | vccorlib.lib (Windows 8.1 or later)
|  ATL     | 100%    | 100%       | 100%      | -
|  MFC     |  X      |  X         | 100%      | -
|  AMP     |  -      |  -         |  -        | -
|  OpenMP  | 100%    | 100%       | 100%      | Requires `vcomp140.dll` from the Visual C++ Runtime

### 2.1. Supported Visual Studio Versions
* Visual Studio 2015 (Support Clang with Microsoft CodeGen, Clang 3.7 with Microsoft CodeGen, Clang-LLVM)
* Visual Studio 2017 (Support Clang with Microsoft CodeGen, Clang-LLVM)
* Visual Studio 2019

### 2.2. Supported Build Tools
|   Build Tool   | Helper      
| -------------- | --
| Visual Studio  | [VC-LTL helper for Visual Studio.props](#32-using-vc-ltl-in-visual-studio)
| CMake          | [VC-LTL helper for cmake.cmake](#33-using-vc-ltl-in-cmake)
| NMake, CL      | [VC-LTL helper for nmake.cmd](#34-using-vc-ltl-in-nmakecl)
| QMake          | VC-LTL helper for qmake.pri

### 2.3. Supported Windows Versions
|         OS                                          | x86 | x64 | arm | arm64 
|        ----                                         | --- | --- | --- | ----
| Windows XP, Windows Server 2003                     | √  | √  | -   | -
| Windows Vista, Windows Server 2008                  | √  | √  | -   | -
| Windows 7, Windows Server 2008 R2                   | √  | √  | -   | -
| Windows 8, Windows Server 2012, Windows RT          | √  | √  | √  | -
| Windows 8.1, Windows Server 2012 R2, Windows RT 8.1 | √  | √  | √  | -
| Windows 10, Windows Server 2016, Windows Server 2019| √  | √  | √  | √

> The binaries compiled with the VC-LTL is compatible with Windows XP and 
later, even in the environment which installed no hotfixes.

## 3. How to used?
We will enter the theme, we have prepared a rich [VC-LTL Samples](https://github.com/Chuyu-Team/vc-ltl-samples) for your reference, and welcome to join our QQ group (633710173).

### 3.1. Install VC-LTL
If you download and unzip [VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases/latest) to `D:\Src\VC-LTL`, please double-click `D:\Src\VC-LTL\Install.cmd`.

> The script will save the information in the registry `HKCU\Code\VC-LTL`.

### 3.2. Using VC-LTL in Visual Studio

#### 3.2.1. Add VC-LTL Property Sheet
Copy `VC-LTL helper for Visual Studio.props` to your project, then open the Property Manager ( View - Property Manager ) and right-click on the Release Configuration, click on `Add Existing Property Sheet...`, and finally select `VC-LTL helper for Visual Studio.props`.

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/AddShared.png)

#### 3.2.2. Configure Project Properties
* C/C++ - Code Generation -`Runtime Library` adjust to `Multi-threaded (/MT)`

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/ConfigurationProject.png)

> For XP support, please use the `Windows XP toolset` in the platform toolset, or modify `VC-LTL helper for Visual Studio.props` to enable `<SupportWinXP>true</SupportWinXP>`.

### 3.3. Using VC-LTL in CMake

#### 3.3.1. Add VC-LTL Module File

Copy `VC-LTL helper for cmake.cmake` to your project. Then add `include("VC-LTL helper for cmake.cmake")` to `CMakeLists.txt`.

**Example:**
```
cmake_minimum_required(VERSION 3.5.2)
project(ltltest)

include("VC-LTL helper for cmake.cmake")

add_subdirectory(src)
```

#### 3.3.2. Modify The Configuration

> Make sure to use `/MT` to compile project when using VC-LTL. For XP support, please modify `VC-LTL helper for cmake.cmake` to enable `set(SupportWinXP "true")`.

### 3.4. Using VC-LTL in NMake/CL

#### 3.4.1. Run VC-LTL Cmd Script

Copy `VC-LTL helper for nmake.cmd` to your project. Run `vcvars32.bat` or `vcvars64.bat` and execute this script. The script will automatically modify the `include` and `lib` environment variables.

**Example:**
```
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
call "D:\VC-LTL\VC-LTL helper for nmake.cmd"

nmake /f Test.mak
```
#### 3.4.2. Modify The Configuration

> Make sure to use `/MT` to compile project when using VC-LTL. For XP support, please modify `VC-LTL helper for nmake.cmd` to enable `set SupportWinXP=true`.

### 3.5. Rebuild (Release only)
Is the file size smaller? If you fail to compile, please refer to [4. FAQ](#4-faq). You can also feedback and work together to improve VC-LTL.

If VC-LTL is referenced correctly, it will be output at the time of generation: `note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`.

> Compile with VC-LTL must be compiled with `/MT`, and all dependent static libraries must also be recompiled with VC-LTL.

![AppBuildByVC-LTL](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/AppWithLTL.png)


## 4. FAQ
### 4.1. 未共享到msvcrt.dll
#### Cause
未正确引用VC-LTL。建议看看生成日志，是否包含：`note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`

#### Workaround
1：请务必确保 `VC-LTL helper for Visual Studio.props` 已经添加到工程。

2：确保以下设置正确：
* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
* VC++ 目录 - 库目录 - 【√ 从父项或项目默认设置继承(I)】

### 4.2. 无法解析外部符号 delete 等
#### Cause

没有正确引入vc.lib、msvcrt_Platform.lib。

#### Workaround
* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
* VC++ 目录 - 库目录 - 【√ 从父项或项目默认设置继承(I)】

### 4.3. 检测到RuntimeLibrary的不匹配项
#### Cause

引入了没有使用VC-LTL编译的静态lib文件。

#### Workaround

使用VC-LTL重新编译对应的静态lib（具体lib名称错误日志会给出）。


## 5. Known Issues
* 由于WinXP本身Bug，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 `_CRT_STDIO_SIZE_MAX` 宏。（4.0.2.5 Advanced模式已经修正此问题）
* 由于WinXP本身Bug，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。（4.0.2.5 Advanced模式已经修正此问题）
* 由于msvcrt本身限制，`setlocale/_create_locale`相关函数不支持UCRT的locale name，使用时必须按VC 2008规范使用，比如 `setlocale(0, "chs");` 这样调用，而不是传入 `setlocale(0, "zh-CN");`。
* 由于FH4异常（`/d2FH4` VS2019新增功能）实现过程中使用了TLS，因此在兼容“Windows XP(2003) x64”时请务必确保不要在DllMain中使用FH4 catch，否则将导致dll直接加载失败。


## Excursus - Known project using VC-LTL

|  Project                                                     | Note
|  ---                                                         | ----
| [NSudo](https://github.com/M2Team/NSudo)                     | 一个强大的系统管理工具。VC-LTL的帮助下减少30%的程序体积，包括NSudo for arm64。
| [Menu98](https://github.com/rikka0w0/Menu98)                 | 经典样式可自定义的开始按钮右键菜单。使用VC-LTL移除运行时依赖。
| [壁虎浏览器](http://bhbrowser.com/)                          | 一款专业解决DNS劫持的浏览器。使用VC-LTL有效减少程序体积，减少安装包大小。
| [librech551](https://github.com/rgwan/librech551)            | 开源跨平台的CH55x ISP软件。使用VC-LTL移除运行时依赖。
| [Dism++](https://www.chuyu.me/)                              | Dism GUI版。初雨团队自身项目，使用VC-LTL有效减少程序体积，减少安装包大小。
| [360安全卫士](https://www.360.cn/)                           | 奇虎360推出的上网安全软件。360EvtMgr.exe、360leakfixer.exe、360Util.dll、leakrepair.dll等文件使用VC-LTL编译，在VC-LTL的支持下：升级新编译器，减少文件尺寸，完美兼容WinXP，一箭三雕。


## Excursus - Third Party Licenses
* [Visual Studio](https://github.com/Chuyu-Team/VC-LTL/tree/master/src/Licenses)
* [Windows 10 SDK](https://github.com/Chuyu-Team/VC-LTL/tree/master/src/ucrt/Licenses)
* [boost](https://github.com/Chuyu-Team/VC-LTL/tree/master/src/boost_1_66_0/LICENSE_1_0.txt)


## Changes

### 1.0.0.1 May 23, 2016 13:42
* 解决C++异常无法使用问题。


### 1.0.0.2 June 14, 2016 12:52
* 解决C编译不通过问题。


### 1.0.0.3 Nov 28, 2016 12:54
* 改进对C工程的兼容性。


### 1.0.0.4 - Initial Release (Mar 6, 2017 16:15)
* Separated from Dism++.


### 1.0.0.5 - 新增Visual Studio 2017支持 (Mar 16, 2017 20:53)
* 新增VC 2017支持。


### 1.0.0.6 - 优化引用方式 (Mar 18, 2017 13:46)
* 优化文件引用。


### 1.0.0.7 - 初步添加Windows XP支持 (Apr 22, 2017 19:26)
* 初步添加Windows XP支持。
* 添加C++异常支持。
* 添加/GS特性支持。
* 添加/guard:cf支持。
* VC140库升级到Vistual Studio 2015 Update3。


### 1.0.0.8 - 简化VC-LTL使用 (Apr 25, 2017 20:37)
* 简化库引用方式。
* 更新14393 ucrt到最新版（2017-01-05版）。
* 添加15063 ucrt支持。


### 1.0.0.9 - 添加C++异常支持(May 26, 2017 14:46)
* 改进Windows XP支持。
* 优化库结构裁剪ltl库体积。
* 解决使用自定义异常导致程序编译不通过问题。
* 调整`*_p`系列函数定义，以免使用`*_p`系列函数时编译不通过。
* 解决使用浮点除法时导致编译不通过问题。


### 1.0.0.10 - 改进对Windows XP的支持 (July 28, 2017 20:28)
* 解决[Bug 9](https://github.com/Chuyu-Team/VC-LTL/issues/9)，某些时候编译器引用异常导致XP模式时意外引入`_except_handler4_common`（感谢 HwangBae）。
* 解决[Bug 8](https://github.com/Chuyu-Team/VC-LTL/issues/8)，修复typeid功能无法使用问题（感谢 HwangBae）。
* 调整异常实现代码，尽可能复用msvcrt.dll代码减少代码体积。
* 解决Bug，修复无法使用`__argc`、`__argv`、`__wargv`、`_environ`、`_wenviron`全局变量问题（感谢 亮叔叔）。
* 解决微软Bug，修复使用ATL库的程序不支持XP RTM问题。


### 1.0.0.11 - 解决lib依赖问题 (Aug 23, 2017 19:00)
* 解决Bug，使用`_difftime64`时Windows XP无法运行。
* 解决Bug，`_msvcrt.cpp`始终会引入urct_14393.lib问题（感谢 亮叔叔）。
* 更新VC141头文件以及实现，全部同步到最新14.11。


### 1.0.0.12 - 解决某些函数无法使用问题 (Sep 15, 2017 13:33)
* 解决Bug，使用strcat_s时在Windows XP中提示找不到指定符号（感谢 stsm85）。
* 解决Bug，解决SSE2除法导致编译不通过问题（感谢 stsm85）。
* 解决Bug，解决wcstoll、vsnprintf、rand_s、strtoll无法使用问题（感谢 stsm85）。
* 代码调整，消除所有VC-LTL编译警告，强迫症患者福音。

> 本次更新后，FastCopy、winpck相关程序直接可以使用VC-LTL编译。


### 1.0.0.13 - 新增 Windows 10 16299 UCRT支持 (Oct 11, 2017 14:00)
* 解决Bug，atanh、acosh、asinh无法使用问题（感谢 stsm85）。
* 新增Windows 10 16299 UCRT支持。
* 移除Windows 10 14393 UCRT支持。

> 16299已经发布，因此移除老版本14393支持。相关项目请迁徙到15063或者最新16299。


### 2.0.0.1 - 新增C++类库支持 (Oct 29, 2017 22:23)
* 新增iostream、stringstream支持
* 解决使用`_fstat32`、`_fstat32i64`、`_fstat64i32`、`_stat32`、`_stat32i64`、`_stat64i32`、`_wstat32`、`_wstat32i64`、`_wstat64i32`导致编译不通过问题。
* 修正`__acrt_iob_func`始终返回输入流问题。
* 解决 type_info operator != 功能无法使用问题（感谢 sonyps5201314）。
* 解决_daylight，_dstbias，_timezone，_tzname无法使用问题（感谢 sonyps5201314）。
* 解决32位 SSE高精度数据函数无法使用问题，比如`_libm_sse2_tan_precise`，`_libm_sse2_sqrt_precise`，`_libm_sse2_sin_precise`（感谢 stsm85）。


### 2.0.0.2 - 扩充对C++类库的支持 (Nov 5, 2017 14:14)
* 新增 `_configthreadlocale`、`_get_current_locale`、`_create_locale`、`_free_locale`接口支持。
* 为Windows XP添加 `_time32`、`_fseeki64`静态实现。
* 解决[Bug 14](https://github.com/Chuyu-Team/VC-LTL/issues/14)，新增 `_getpid`、`_sys_nerr`、`_sys_errlist`无法使用问题（感谢 HwangBae）。
* 新增C++类，mutex、thread、xtime、xonce支持。
* 优化编译方式，消除无意义符号，减少ltl库体积。


### 2.0.0.3 - 补充大量XP静态实现 (Nov 18, 2017 14:18)
* Vista编译模式，CRT函数覆盖率达到90%以上。
* XP编译模式，CRT函数覆盖率达到80%以上。
* 简化注册表引用方式。
* 添加std::thread支持。


### 2.0.0.4 - 全面覆盖STL、ConcRT (Dec 3, 2017 20:40)
* 全面覆盖STL、ConcRT库，让C++如鱼得水，尽情使用STL。
* 全新的weak别名转发技术，实现零jmp解决新老CRT命名冲突。


### 2.0.0.5 - 增强C++ 17支持 (Dec 10, 2017 20:56)
* 新增C++ 17 align new/delete支持。
* 解决兼容WinXP时可能出现的ntdll.lib依赖失败以及某些函数转发切换到weak别名技术。

> 本次更新主要添加对Vistual Studio 2017 15.5以及更高版本新增的C++ 17功能支持。


### 2.0.0.6 - 2018新春贺岁版 (Jan 17, 2018 17:15)
* 添加Vistual Studio 2017 15.5新增函数支持。
* 新增Shared.props改进属性表引用灵活性。
* 新增VC-LTL轻量模式以及高级模式支持（修改Shared.props可以调整模式）。
* 继续扩充CRT函数支持。


### 2.0.0.7 - 重新规范VC-LTL (Mar 6, 2018 17:17)
* 重新整理VC-LTL，尽可能减少对原版改动。
* 解决一些Bug（感谢 亮亮、layerfsd、waiting4love）。


### 2.0.0.8 - 累计BUG修复以及适配新版本 (Mar 23, 2018 14:58)
* 解决一些潜在找不到符号问题以及链接失败问题（感谢 Too Simple）。
* 修正`__crtLCMapString`相关字符串操作兼容性问题（感谢 Too Simple）。
* 解决`_getptd_noexit`在获取msvcrt.dl的DllMain中创建的 ptd结构时会返回失败问题（感谢 亮亮）。
* 添加最新Vistual Studio 2017 15.6支持。


### 3.0.0.1 - Improve Platform Support (Apr 26, 2018 18:48)
* Fix [Bug 27](https://github.com/Chuyu-Team/VC-LTL/issues/27), `_set_abort_behavior` maybe output a warning message (Thanks myfreeer).
* Fix [Bug 21](https://github.com/Chuyu-Team/VC-LTL/issues/21), `nothrow` symbol conflict problem.
* Fix Bug, solved the x64 system cann't use the `__p__*` functions (Thanks 昌平).
* Improve, solved an error when RC include _msvcrt.h (Thanks 风清凉).
* Improve, `Shared.props` rename to `VC-LTL helper for Visual Studio.props` and improve support.
* Add [Fea 25](https://github.com/Chuyu-Team/VC-LTL/issues/25), add `VC-LTL helper for nmake.cmd`, provide NMake support.
* Add Fea, add `VC-LTL helper for cmake.cmake`, provide CMake support (Thanks Sandro).
* Add Fea, add ARM and ARM64 support.


### 3.0.0.2 - Add Vistual Studio 2017 15.7 Support (May 18, 2018 21:07)
* Add Fea, add VC 14.0.24234&14.14.26428.
* Add Fea, add UCRT 10.0.17134.0.
* Add Fea, add Spectre Mitigtion libs.
* Fix Bug, add `gets` symbol ( Thanks npc ).

> VC-LTL removed all libs already, please download [VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases) then compile your project(s).


### 3.0.0.3 - Children's Day Version (May 31, 2018 17:07)
* Fix [Bug 32](https://github.com/Chuyu-Team/VC-LTL/issues/32), add `__p__fmode` symbol (Thanks augustheart).
* Update License.


### 3.1.0.1 - Add Clang support (June 18, 2018 13:13)
* Add [Fea 34](https://github.com/Chuyu-Team/VC-LTL/issues/34), add Clang with Microsoft CodeGen, Clang 3.7 with Microsoft CodeGen and Clang-LLVM Toolsets support (Thanks to hzqst).
* Fix Bug, in the non-Chinese code page `corecrt.h` will be reported C4828 warning (Thanks to 临渊羡鱼乎).
* Fix [Bug 35](https://github.com/Chuyu-Team/VC-LTL/issues/35), warning when calling vsnprintf C4389: "==": signed/unsigned does not match (Thanks to zhaooptimus).
* Fix Bug, reduce the binary size of the _hypotf program referenced by the amd64 program.
* Fix Bug, Solve VS15.7 special_math problem can not be used.


### 4.0.0.2 - Add VC-LTL dynamic library support (Aug 14, 2018 12:30)
> The new VC-LTL 4.0, code-named "Dawning" has been released. As a major version upgrade, you need to pay attention to the following items:
> 1. Beginning with VC-LTL 4.0, if you need static compilation (dependent msvcrt.dll only), please use `/MT`.
> 2. VC-LTL 4.0 is not compatible with VC-LTL 3.X compiled static libraries, you need to recompile with VC-LTL 4.0 (Dawning).

* Add Fea, add VC-LTL dynamic library support, use `/MD` link to DLL such as `ucrtbase_ltl.dll`.
* Add Fea, discard DDK, refactor CRT library, add support pointer confusion, `quick_exit`, `__stdio_common`, etc.
* Add Fea, support ARM, ARM64 link to msvcrt.dll.
* Add Fea, add WinRT (CX) support (Windows 8.1 and higher).
* Improve, there is no need to specify `Minimum required version` in XP mode.
* Improve, use msvcp60.dll to reduce the program size.


### 4.0.0.10 - Add Vistual Studio 2017 15.8 Support (Aug 26, 2018 17:20)
* Fix Bug, Solve "VC-LTL helper for cmake.cmake" syntax error (Thanks to MaMy丶).
* Fix Bug, Solve the wrong way to determine the path of the CMake script (Thanks to Simple, MaMy丶).
* Fix Bug, Solve the CMkae and NMake script lib path error problem.
* Fix Bug, Solve MD mode report not found symbol problem such as `_libm_sse2_tan_precise` (Thanks to 13732650).
* Fix Bug, Remove `/sdl` and avoid using STL crash (Thanks to 漆黑の牙).
* Add Fea, Add Vistual Studio 2017 15.8 Support.
* Add Fea, Add a lib file check, and report an error if it does not exist.


### 4.0.0.17 - Add C++/CLI Support (Sep 26, 2018 10:00)
* Fix [Bug 40](https://github.com/Chuyu-Team/VC-LTL/issues/40), solved link failed when using RTTI in XP x64 mode (Thanks to killvxk).
* Fix [Bug 41](https://github.com/Chuyu-Team/VC-LTL/issues/41), solved `_byteswap_ulong`, `_byteswap_uint64` and `_byteswap_ushort` were not found when the internal function was disabled (Thanks to wangwenx190).
* Fix Bug, solved Windows XP cannot find entry points for `_abs64`, `_rotl64` and `_rotr64` functions when internal functions was disabled.
* Add Fea, add `wcstold`、`_wcstof_l`、`_wcstod_l`、`_wcstold_l`、`_strtod_l`(xp) and `_strftime_l`(Vista).
* Add Fea, add C++/CLI Support (Thanks to naturalj0).


### 4.0.0.26 - Cumulative bug fixes (Dec 19, 2018 18:00)
* Fix Bug, solved Windows XP mode `_Atexit` function depends on `EncodePointer` problem (Thanks to 亮亮).
* Fix MSBug, solved the wrong use of `EncodePointer` in Microsoft's original details::Etw.
* Fix Bug, solved a memory leak in `_initialize_onexit_table_downlevel` (Thanks to g-qa-ck).
* Fix Bug, solved Windows XP can't find function `operator new(size_t,int,char const *,int) and operator new[](size_t,int,char const *,int)` entry point (Thanks to 希望).
* Fix Bug, solved missing exsup4 symbol problem under Spectre with WinXP 32 (Thanks to Too Simple).
* Add Fea, add `delete[](void *, std::nothrow_t const &)` (Thanks to Too Simple).
* Add Fea, add UCRT 10.17763 (Thanks to 毛利).
* Add Fea, add Vistual Studio 2017 15.9 Support.
* Add Fea, add `_IsExceptionObjectToBeDestroyed, _CreateFrameInfo, _FindAndUnlinkFrame` to Windows XP (Thanks to 夏思畅).


### 4.0.0.28 - Cumulative bug fixes (Feb 16, 2019 14:50)
* Fix [Bug 45](https://github.com/Chuyu-Team/VC-LTL/issues/45), the parameter is expanded by default (Thanks to xspeed1989).
* Fix Bug, VS 14.14.26428 or higher missed `pplerror.cpp` problem (Thanks to xspeed1989).
* Add Fea, improved support for Clang-LLVM in Visual Studio 2017 (Thanks to luciouskami).
* Add Fea, add `VC-LTL helper for qmake.pri` to provide support for QMake (Thanks to GPBeta).


### 4.0.1.10 - Improved Support (June 1, 2019 13:40)
* Fix Bug, Windows XP mode does not reference `advapi32.lib`, which may cause symbols such as `ImpersonateSelf` to be unresolvable (Thanks to 昌平517).
* Fix Bug, `std::thread` depends on `GetLogicalProcessorInformation` causing XP RTM report not to find entry point (Thanks to 小古).
* Fix Bug, Add `_mbsinc` in XP mode because the old version of msvcrt.dll has an out-of-bounds access the bug (Thanks to 亮亮).
* Fix Bug, `std::mutex` depends on `GetNumaHighestNodeNumber` causing XP RTM report not to find entry point.
* Fix Bug, the "ucrtbase_ltl.dll" module does not export the "llabs" function（Thanks to GPBeta）。
* Fix Bug, remove the accidentally omitted `_downlevel` suffix (Thanks to 过客).
* Fix Bug, this config.pri syntax error caused QT to fail to load (thanks to GPBeta).
* New Fea, `VC-LTL helper for qmake.pri` is compatible with the old version of QMake (Thanks to BigBrother).
* New Fea, the CMake script adds vcpkg support (Thanks to BigBrother).
* New Fea, add Vistual Studio 2019 Support.
* New Fea, add 10.0.18362.0 UCRT support (Thanks to 毛利).
* Improve, the `_ATL_XP_TARGETING` and `_USING_V110_SDK71_` macros in Vista mode are adjusted from error to warning.


### 4.0.2.14 - Improved Support (Aug 14, 2019 18:00)
* Fix Bug, Windows XP mode can't find `__wcsrtombs_utf8` symbol (Thanks To 水边).
* Fix [Bug 56](https://github.com/Chuyu-Team/VC-LTL/issues/56), solve (w)printf, std::(w)cout, etc. can not output text other than ASCII.
* Fix Bug, _tzname declaration is inconsistent (Thanks To BigBrother).
* Fix Bug, `_splitpath_s, _wsplitpath_s` bug in Windows XP mode (Thanks To 水边).
* New Fea, add `_CRT_STDIO_ISO_WIDE_SPECIFIERS` macro and `legacy_stdio_definitions.lib` support (Thanks to BigBrother).
* New Fea, add `_initialize_invalid_parameter_handler`, `_initialize_denormal_control` and `_get_startup_thread_locale_mode (Vista mode only)` support。
* New Fea, add 14.22.27905 toolset support.
* New Fea, add `_atoi_l, _strtol_l, _atoflt_l, _atoldbl_l`, etc. to Windows XP mode.
* New Fea, upgrade 14.16.27023 to the 14.16.27033 version.
* New [Fea 17](https://github.com/Chuyu-Team/VC-LTL/issues/17), add `_create_locale、_wcreate_locale、_get_current_locale` to Windows XP mode.
* Improve compatibility with Lenovo one-click audio and video (This Issue also exists in Microsoft's UCRT).
* Improve compatibility with Windows 7 RTM and older systems (This Issue also exists in Microsoft's UCRT).
* [Improve 53](https://github.com/Chuyu-Team/VC-LTL/issues/53), disable reference elimination for the STL library and avoid LLVM link failure (Thanks to hotxp, BigBrother).
