# VC-LTL - An elegant way to compile lighter binaries.
![license](https://img.shields.io/github/license/Chuyu-Team/VC-LTL)
![downloads](https://img.shields.io/github/downloads/Chuyu-Team/VC-LTL/total)
![contributors](https://img.shields.io/github/contributors-anon/Chuyu-Team/VC-LTL)
![release](https://img.shields.io/github/v/release/Chuyu-Team/VC-LTL?include_prereleases)
![nuget](https://img.shields.io/nuget/vpre/VC-LTL)

- [简体中文](ReadMe.osc.md)

> [VC-LTL 5.0](https://github.com/Chuyu-Team/VC-LTL5) available, 4.0 version no longer provides support!!!

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
* Visual Studio 2019 (Support Clang-LLVM)

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

### 3.1. Using VC-LTL in Visual Studio

#### 3.1.1. Choose reference mode

##### 3.1.1.1. Reference via NuGet (recommend)

Right-click on the project and select "Manage NuGet Packages", then search for `VC-LTL` and choose the version that suits you, and finally click Install.

![InstallByNuGet](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/InstallByNuGet.png)

##### 3.1.1.2. Reference via Registry
If you download and unzip [VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases/latest) to `D:\Src\VC-LTL`, please double-click `D:\Src\VC-LTL\Install.cmd`.

> The script will save the information in the registry `HKCU\Code\VC-LTL`.

Copy `VC-LTL helper for Visual Studio.props` to your project, then open the Property Manager ( View - Property Manager ) and right-click on the Release Configuration, click on `Add Existing Property Sheet...`, and finally select `VC-LTL helper for Visual Studio.props`.

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/AddShared.png)

#### 3.1.2. Configure Project Properties
* C/C++ - Code Generation -`Runtime Library` adjust to `Multi-threaded (/MT)`

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/ConfigurationProject.png)

> For XP support, please Right click on the project, Properties - 初雨团队 VC-LTL - 启用 Windows XP兼容 - "Yes".

### 3.2. Using VC-LTL in CMake
If you download and unzip [VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases/latest) to `D:\Src\VC-LTL`, please double-click `D:\Src\VC-LTL\Install.cmd`.

> The script will save the information in the registry `HKCU\Code\VC-LTL`.

#### 3.2.1. Add VC-LTL Module File

Copy `VC-LTL helper for cmake.cmake` to your project. Then add `include("VC-LTL helper for cmake.cmake")` to `CMakeLists.txt`.

**Example:**
```
cmake_minimum_required(VERSION 3.5.2)
project(ltltest)

include("VC-LTL helper for cmake.cmake")

add_subdirectory(src)
```

#### 3.2.2. Modify The Configuration

> Make sure to use `/MT` to compile project when using VC-LTL. For XP support, please modify `VC-LTL helper for cmake.cmake` to enable `set(SupportWinXP "true")`.

### 3.3. Using VC-LTL in NMake/CL

#### 3.3.1. Run VC-LTL Cmd Script
If you download and unzip [VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases/latest) to `D:\Src\VC-LTL`, please double-click `D:\Src\VC-LTL\Install.cmd`.

> The script will save the information in the registry `HKCU\Code\VC-LTL`.

Copy `VC-LTL helper for nmake.cmd` to your project. Run `vcvars32.bat` or `vcvars64.bat` and execute this script. The script will automatically modify the `include` and `lib` environment variables.

**Example:**
```
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
call "D:\VC-LTL\VC-LTL helper for nmake.cmd"

nmake /f Test.mak
```
#### 3.3.2. Modify The Configuration

> Make sure to use `/MT` to compile project when using VC-LTL. For XP support, please modify `VC-LTL helper for nmake.cmd` to enable `set SupportWinXP=true`.

### 3.4. Rebuild (Release only)
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
[Changes](https://github.com/Chuyu-Team/VC-LTL/wiki#changes)
