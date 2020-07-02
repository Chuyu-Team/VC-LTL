# VC-LTL——编译精巧的程序原来如此简单

- [English](ReadMe.md)

![Logo](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/Logo.png)<br/>
我愿化成一座做石桥，经受五百年的风吹，五百年的日晒，五百年的雨打，只求她从桥上走过！

## 1. 关于VC-LTL

VC-LTL是一个基于微软VC修改的开源运行时，有效减少应用程序体积并摆脱微软运行时DLL，比如msvcr120.dll、api-ms-win-crt-time-l1-1-0.dll等依赖。

VC-LTL最初是Dism++专用运行时。2017年3月6号从Dism++源代码中分离，并正式对外开源，为社区贡献自己的微薄之力。

在大型项目中往往有众多模块，如果都采用静态编译那么造成的空间浪费先不说，最后也会因为`Fls上限`导致程序无法正常运行。

而VC-LTL能让你的项目如同系统文件一样共享系统内置`msvcrt.dll`，有效的解决`Fls上限`以及`运行时部署问题`，同时`大大缩减程序体积`，可以说一箭三雕！

所有人都可以无条件、免费使用，包括用于商业环境。当然如果大家在自己的程序说明文件中声明使用了VC-LTL那就更好了。

* GitHub：[github.com/Chuyu-Team/VC-LTL（英文）](https://github.com/Chuyu-Team/VC-LTL)    
* 码云：[gitee.com/Chuyu-Team/VC-LTL（中文）](https://.com/Chuyu-Team/VC-LTL)    
* QQ群：[633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a)

### 1.1. 原理
使用VC-LTL后可以将程序动态链接到系统自带的msvcrt.dll中，来减少程序体积。目前使用CRT以及STL的工程一般都可以使用。但是MFC工程不能使用，因为MFC类库太复杂了，尚未适配。

> 使用VC-LTL，C++程序体积大约缩减30%，而纯C程序则大约缩减50%。

### 1.2. 亮点
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 无缝使用最新C/C++库以及最新编译器，尽情的使用最新规范。神马异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。

> 让我们一起跟VS 2008说拜拜！

## 2. VC-LTL兼容性
此表展示了VC-LTL，C/C++库函数覆盖率，通过覆盖情况，可以大致了解VC-LTL的完善程度。

|  模块  | XP模式  | Vista模式 | UCRT模式 | 相关文件 
| :----: | :-----: | :-------: | :------: | --------
|   CRT  | 94.716% | 96.347%   |   100%   | vcruntime.lib、libvcruntime.lib、msvcrt.lib、msvcmrt.lib、msvcrt_Platform.lib、libucrt.lib、ucrt.lib、libucrt_shared.lib、vc.lib
|   STL  | 100%    | 100%      |   100%   | libcpmt.lib、msvcprt.lib
| ConcRT | 100%    | 100%      |   100%   | libconcrt.lib、concrt.lib
| WinRT  |   X     | 100%      |    X     | vccorlib.lib（仅支持Windows 8.1以及更高版本）
|   ATL  | 100%    | 100%      |   100%   | -
|   MFC  |   X     |   X       |   100%   | -
|   AMP  |   X     |   X       |    X     | -
| OpenMP | 100%    | 100%      |   100%   | Visual Studio自身提供，需要带上`vcomp140.dll`

### 2.1. 支持的IDE
* Visual Studio 2015（包含Clang with Microsoft CodeGen、Clang 3.7 with Microsoft CodeGen、Clang-LLVM）
* Visual Studio 2017（包含Clang with Microsoft CodeGen、Clang-LLVM）
* Visual Studio 2019（包含Clang-LLVM）

### 2.2. 支持的编译工具
|    编译工具    | 支持文件
| -------------- | --
| Visual Studio  | [VC-LTL helper for Visual Studio.props](#32-在visual-studio中使用vc-ltl)
| CMake          | [VC-LTL helper for cmake.cmake](#33-在cmake中使用vc-ltl)
| NMake、CL      | [VC-LTL helper for nmake.cmd](#34-在nmake纯cl中使用vc-ltl)
| QMake          | VC-LTL helper for qmake.pri

### 2.3. 支持的操作系统
|   操作系统                                          | x86 | x64 | arm | arm64 
|    ----                                             | --- | --- | --- | ----
| Windows XP、Windows Server 2003                     | √  | √  | -   | -
| Windows Vista、Windows Server 2008                  | √  | √  | -   | -
| Windows 7、Windows Server 2008 R2                   | √  | √  | -   | -
| Windows 8、Windows Server 2012、Windows RT          | √  | √  | √  | -
| Windows 8.1、Windows Server 2012 R2、Windows RT 8.1 | √  | √  | √  | -
| Windows 10、Windows Server 2016、Windows Server 2019| √  | √  | √  | √

> 采用VC-LTL编译后的程序能兼容Windows XP RTM以上所有操作系统，无需安装任何SP补丁包。

## 3. 使用方法
下面我们将进入主题，我们给大家准备了丰富的[VC-LTL示例](https://github.com/Chuyu-Team/vc-ltl-samples)供大家参考，也欢迎加入我们的QQ群（633710173）。


### 3.1. 在Visual Studio中使用VC-LTL

#### 3.1.1. 引用VC-LTL

##### 3.1.1.1. 通过NuGet引用（推荐）

在 项目右键，选择“管理 NuGet 程序包”，然后搜索 `VC-LTL` 并选择适合您的版本，最后点击安装即可。

![InstallByNuGet](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/zh-Hans/image/InstallByNuGet.png)

##### 3.1.1.2. 通过注册表引用
假如，你将[VC-LTL Binary](https://gitee.com/Chuyu-Team/VC-LTL/releases)下载并解压至`D:\Src\VC-LTL`（具体位置无任何要求），双击`D:\Src\VC-LTL\Install.cmd`即可。

> 脚本会在`HKCU\Code\VC-LTL`创建注册表。

将属性表`VC-LTL helper for Visual Studio.props`复制到你的工程目录，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键`添加现有属性表`，然后选择`VC-LTL helper for Visual Studio.props`即可。

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/zh-Hans/image/AddShared.png)

#### 3.1.2. 配置工程属性
* C/C++ - 代码生成 -【运行库】调整为【多线程 (/MT)】

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/zh-Hans/image/ConfigurationProject.png)

> 如需支持XP，请右键项目 - 属性 - 初雨团队 VC-LTL - 启用 Windows XP 兼容 －『是』 即可。

### 3.2. 在CMake中使用VC-LTL

假如，你将[VC-LTL Binary](https://gitee.com/Chuyu-Team/VC-LTL/releases)下载并解压至`D:\Src\VC-LTL`（具体位置无任何要求），双击`D:\Src\VC-LTL\Install.cmd`即可。

> 脚本会在`HKCU\Code\VC-LTL`创建注册表。

#### 3.2.1. 添加VC-LTL配置文件

将模块文件`VC-LTL helper for cmake.cmake`复制到你的工程目录（顶层CMakeLists.txt同级目录）。然后在`CMakeLists.txt`中添加一行 `include("VC-LTL helper for cmake.cmake")` 即可。

**示例：**
```
cmake_minimum_required(VERSION 3.5.2)
project(ltltest)

include("VC-LTL helper for cmake.cmake")

add_subdirectory(src)
```

#### 3.2.2. 调整配置工程

> 务必确保使用`/MT`编译代码。如需支持XP，请修改`VC-LTL helper for cmake.cmake`启用 `set(SupportWinXP "true")` 即可。

### 3.3. 在NMake/纯CL中使用VC-LTL

假如，你将[VC-LTL Binary](https://gitee.com/Chuyu-Team/VC-LTL/releases)下载并解压至`D:\Src\VC-LTL`（具体位置无任何要求），双击`D:\Src\VC-LTL\Install.cmd`即可。

> 脚本会在`HKCU\Code\VC-LTL`创建注册表。

#### 3.3.1. 运行VC-LTL辅助脚本

将辅助脚本`VC-LTL helper for nmake.cmd`复制到你的工程目录。启动`vcvars32.bat/vcvars64.bat`执行此脚本即可，脚本将自动修改`include`以及`lib`环境变量。

**示例：**
```
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
call "D:\VC-LTL\VC-LTL helper for nmake.cmd"

nmake /f Test.mak
```
#### 3.3.2. 配置工程属性

> 务必确保使用`/MT`编译代码。如需支持XP，请修改`VC-LTL helper for nmake.cmd`启用 `set SupportWinXP=true`。


### 3.4. 重新编译（仅Release）
现在是不是体积就小了很多。如果你编译不通过，可以先参考 [4. 常见问题](#4-常见问题)。如果还是不通过可以反馈，共同改进VC-LTL。

如果正确引用VC-LTL，那么 会在生成时输出：`note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`。

> 使用VC-LTL编译时必须采用`/MT`编译，并且所有依赖的静态库也必须使用VC-LTL重新编译。

![AppBuildByVC-LTL](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/AppWithLTL.png)


## 4. 常见问题
### 4.1. 未共享到msvcrt.dll
#### 问题原因
未正确引用VC-LTL。建议看看生成日志，是否包含：`note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`。

#### 解决方案
1：请务必确保 `VC-LTL helper for Visual Studio.props` 已经添加到工程。

2：确保以下设置正确：
* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
* VC++ 目录 - 库目录 - 【√ 从父项或项目默认设置继承(I)】

### 4.2. 无法解析外部符号 delete 等
#### 问题原因

没有正确引入vc.lib、msvcrt_Platform.lib。

#### 解决方案
* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
* VC++ 目录 - 库目录 - 【√ 从父项或项目默认设置继承(I)】

### 4.3. 检测到RuntimeLibrary的不匹配项
#### 问题原因

引入了没有使用VC-LTL编译的静态lib文件。

#### 解决方案

使用VC-LTL重新编译对应的静态lib（具体lib名称错误日志会给出）。


## 5. 已知问题
* 由于WinXP本身Bug，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 `_CRT_STDIO_SIZE_MAX` 宏。（4.0.2.5 Advanced模式已经修正此问题）。
* 由于WinXP本身Bug，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。（4.0.2.5 Advanced模式已经修正此问题）。
* 由于msvcrt本身限制，`setlocale/_create_locale`相关函数不支持UCRT的locale name，使用时必须按VC 2008规范使用，比如 `setlocale(0, "chs");` 这样调用，而不是传入 `setlocale(0, "zh-CN");`。

## 附：已知使用VC-LTL的官方项目

|  项目                                                        | 备注
|  ---                                                         | ----
| [NSudo](https://github.com/M2Team/NSudo)                     | 一个强大的系统管理工具。VC-LTL的帮助下减少30%的程序体积，包括NSudo for arm64。
| [Menu98](https://github.com/rikka0w0/Menu98)                 | 经典样式可自定义的开始按钮右键菜单。使用VC-LTL移除运行时依赖。
| [壁虎浏览器](http://bhbrowser.com/)                          | 一款专业解决DNS劫持的浏览器。使用VC-LTL有效减少程序体积，减少安装包大小。
| [librech551](https://github.com/rgwan/librech551)            | 开源跨平台的CH55x ISP软件。使用VC-LTL移除运行时依赖。
| [Dism++](https://www.chuyu.me/)                              | Dism GUI版。初雨团队自身项目，使用VC-LTL有效减少程序体积，减少安装包大小。
| [360安全卫士](https://www.360.cn/)                           | 奇虎360推出的上网安全软件。360EvtMgr.exe、360leakfixer.exe、360Util.dll、leakrepair.dll等文件使用VC-LTL编译，在VC-LTL的支持下：升级新编译器，减少文件尺寸，完美兼容WinXP，一箭三雕。


## 附：第三方依赖项许可
* [Visual Studio](https://gitee.com/Chuyu-Team/VC-LTL/tree/master/src/Licenses)
* [Windows 10 SDK](https://gitee.com/Chuyu-Team/VC-LTL/tree/master/src/ucrt/Licenses)
* [boost](https://gitee.com/Chuyu-Team/VC-LTL/tree/master/src/boost_1_66_0/LICENSE_1_0.txt)


## 更新日志

### 1.0.0.1 2016-05-23 13:42
* 解决C++异常无法使用问题。


### 1.0.0.2 2016-06-14 12:52
* 解决C编译不通过问题。


### 1.0.0.3 2016-11-28 12:54
* 改进对C工程的兼容性。


### 1.0.0.4 - 公众第一版（2017-03-06 16:15）
* 与Dism++彻底分离，方便以后与Dism++共享代码。


### 1.0.0.5 - 新增Visual Studio 2017支持（2017-03-16 20:53）
* 新增VC 2017支持。


### 1.0.0.6 - 优化引用方式（2017-03-18 13:46）
* 优化文件引用。


### 1.0.0.7 - 初步添加Windows XP支持（2017-04-22 19:26）
* 初步添加Windows XP支持。
* 添加C++异常支持。
* 添加/GS特性支持。
* 添加/guard:cf支持。
* VC140库升级到Vistual Studio 2015 Update3。


### 1.0.0.8 - 简化VC-LTL使用（2017-04-25 20:37）
* 简化库引用方式。
* 更新14393 ucrt到最新版（2017-01-05版）。
* 添加15063 ucrt支持。


### 1.0.0.9 - 添加C++异常支持（2017-05-26 14:46）
* 改进Windows XP支持。
* 优化库结构裁剪ltl库体积。
* 解决使用自定义异常导致程序编译不通过问题。
* 调整`*_p`系列函数定义，以免使用`*_p`系列函数时编译不通过。
* 解决使用浮点除法时导致编译不通过问题。


### 1.0.0.10 - 改进对Windows XP的支持（2017-07-28 20:28）
* 解决[Bug 9](https://github.com/Chuyu-Team/VC-LTL/issues/9)，某些时候编译器引用异常导致XP模式时意外引入`_except_handler4_common`（感谢 HwangBae）。
* 解决[Bug 8](https://github.com/Chuyu-Team/VC-LTL/issues/8)，修复typeid功能无法使用问题（感谢 HwangBae）。
* 调整异常实现代码，尽可能复用msvcrt.dll代码减少代码体积。
* 解决Bug，修复无法使用`__argc`、`__argv`、`__wargv`、`_environ`、`_wenviron`全局变量问题（感谢 亮叔叔）。
* 解决微软Bug，修复使用ATL库的程序不支持XP RTM问题。


### 1.0.0.11 - 解决lib依赖问题（2017-08-23 19:00）
* 解决Bug，使用`_difftime64`时Windows XP无法运行。
* 解决Bug，`_msvcrt.cpp`始终会引入urct_14393.lib问题（感谢 亮叔叔）。
* 更新VC141头文件以及实现，全部同步到最新14.11。


### 1.0.0.12 - 解决某些函数无法使用问题（2017-09-15 13:33）
* 解决Bug，使用strcat_s时在Windows XP中提示找不到指定符号（感谢 stsm85）。
* 解决Bug，解决SSE2除法导致编译不通过问题（感谢 stsm85）。
* 解决Bug，解决wcstoll、vsnprintf、rand_s、strtoll无法使用问题（感谢 stsm85）。
* 代码调整，消除所有VC-LTL编译警告，强迫症患者福音。

> 本次更新后，FastCopy、winpck相关程序直接可以使用VC-LTL编译。


### 1.0.0.13 - 新增 Windows 10 16299 UCRT支持（2017-10-11 14:00）
* 解决Bug，atanh、acosh、asinh无法使用问题（感谢 stsm85）。
* 新增Windows 10 16299 UCRT支持。
* 移除Windows 10 14393 UCRT支持。

> 16299已经发布，因此移除老版本14393支持。相关项目请迁徙到15063或者最新16299。


### 2.0.0.1 - 新增C++类库支持（2017-10-29 22:23）
* 新增iostream、stringstream支持。
* 解决使用`_fstat32`、`_fstat32i64`、`_fstat64i32`、`_stat32`、`_stat32i64`、`_stat64i32`、`_wstat32`、`_wstat32i64`、`_wstat64i32`导致编译不通过问题。
* 修正`__acrt_iob_func`始终返回输入流问题。
* 解决 type_info operator != 功能无法使用问题（感谢 sonyps5201314）。
* 解决`_daylight`，`_dstbias`，`_timezone`，`_tzname`无法使用问题（感谢 sonyps5201314）。
* 解决32位 SSE高精度数据函数无法使用问题，比如`_libm_sse2_tan_precise`，`_libm_sse2_sqrt_precise`，`_libm_sse2_sin_precise`（感谢 stsm85）。


### 2.0.0.2 - 扩充对C++类库的支持（2017-11-05 14:14）
* 新增 `_configthreadlocale`、`_get_current_locale`、`_create_locale`、`_free_locale`接口支持。
* 为Windows XP添加 `_time32`、`_fseeki64`静态实现。
* 解决[Bug 14](https://github.com/Chuyu-Team/VC-LTL/issues/14)，新增 `_getpid`、`_sys_nerr`、`_sys_errlist`无法使用问题（感谢 HwangBae）。
* 新增C++类，mutex、thread、xtime、xonce支持。
* 优化编译方式，消除无意义符号，减少ltl库体积。


### 2.0.0.3 - 补充大量XP静态实现（2017-11-18 14:18）
* Vista编译模式，CRT函数覆盖率达到90%以上。
* XP编译模式，CRT函数覆盖率达到80%以上。
* 简化注册表引用方式。
* 添加std::thread支持。


### 2.0.0.4 - 全面覆盖STL、ConcRT（2017-12-03 20:40）
* 全面覆盖STL、ConcRT库，让C++如鱼得水，尽情使用STL。
* 全新的weak别名转发技术，实现零jmp解决新老CRT命名冲突。


### 2.0.0.5 - 增强C++ 17支持（2017-12-10 20:56）
* 新增C++ 17 align new/delete支持。
* 解决兼容WinXP时可能出现的ntdll.lib依赖失败以及某些函数转发切换到weak别名技术。

> 本次更新主要添加对Vistual Studio 2017 15.5以及更高版本新增的C++ 17功能支持。


### 2.0.0.6 - 2018新春贺岁版（2018-01-17 17:15）
* 添加Vistual Studio 2017 15.5新增函数支持。
* 新增Shared.props改进属性表引用灵活性。
* 新增VC-LTL轻量模式以及高级模式支持（修改Shared.props可以调整模式）。
* 继续扩充CRT函数支持。


### 2.0.0.7 - 重新规范VC-LTL（2018-03-06 17:17）
* 重新整理VC-LTL，尽可能减少对原版改动。
* 解决一些Bug（感谢 亮亮、layerfsd、waiting4love）。


### 2.0.0.8 - 累计BUG修复以及适配新版本（2018-03-23 14:58）
* 解决一些潜在找不到符号问题以及链接失败问题（感谢 Too Simple）。
* 修正`__crtLCMapString`相关字符串操作兼容性问题（感谢 Too Simple）。
* 解决`_getptd_noexit`在获取msvcrt.dl的DllMain中创建的 ptd结构时会返回失败问题（感谢 亮亮）。
* 添加最新Vistual Studio 2017 15.6支持。


### 3.0.0.1 - 改进平台支持（2018-04-26 18:48）
* 解决[Bug 27](https://github.com/Chuyu-Team/VC-LTL/issues/27)，非中文环境调用`_set_abort_behavior`输出警告问题（感谢 myfreeer）。
* 解决[Bug 21](https://github.com/Chuyu-Team/VC-LTL/issues/21)，特定情况nothrow符号冲突问题（感谢 waiting4love）。
* 解决Bug，解决x64系统无法使用`__p__*`系列函数问题（感谢 昌平）。
* 改进体验，当RC中意外引入_msvcrt.h时 自动跳过处理，避免RC报错（感谢 风清凉）。
* 改进体验，“Shared.props”改名为“VC-LTL helper for Visual Studio.props”并大幅度增强对某些极端工程的支持。
* 新增[Fea 25](https://github.com/Chuyu-Team/VC-LTL/issues/25)，添加“VC-LTL helper for nmake.cmd”，方便nmake引入VC-LTL。
* 新增Fea，添加“VC-LTL helper for cmake.cmake”，方便cmake引入VC-LTL（感谢 Sandro）。
* 新增Fea，全面支持ARM、ARM64支持。


### 3.0.0.2 - 添加Vistual Studio 2017 15.7支持（2018-05-18 21:07）
* 新增Fea，添加VC 14.0.24234以及14.14.26428支持。
* 新增Fea，添加UCRT 10.0.17134.0支持。
* 新增Fea，添加Spectre缓解库支持。
* 解决Bug，找不到gets符号问题（感谢 npc）。

> VC-LTL本次从仓库中剔除了lib文件，VC-LTL用户请下载[VC-LTL Binary](https://gitee.com/Chuyu-Team/VC-LTL/releases)，然后再使用VC-LTL编译代码。


### 3.0.0.3 - 儿童节专版（2018-05-31 17:07）
* 解决[Bug 32](https://github.com/Chuyu-Team/VC-LTL/issues/32)，链接时找不到符号`__p__fmode`（感谢 augustheart）。
* 更新许可。


### 3.1.0.1 - 添加Clang支持（2018-06-18 13:13）
* 新增[Fea 34](https://github.com/Chuyu-Team/VC-LTL/issues/34)，添加Clang with Microsoft CodeGen、Clang 3.7 with Microsoft CodeGen、Clang-LLVM工具集支持（感谢 hzqst）。
* 解决Bug，非中文代码页corecrt.h报告 C4828警告（感谢 临渊羡鱼乎）。
* 解决[Bug 35](https://github.com/Chuyu-Team/VC-LTL/issues/35)，调用vsnprintf时触发警告C4389: “==”: 有符号/无符号不匹配（感谢 zhaooptimus）
* 解决Bug，减少amd64程序调用_hypotf函数时的二进制体积。
* 解决Bug，解决VS15.7 special_math无法使用问题


### 4.0.0.2 - 添加VC-LTL动态库支持（2018-08-14 12:30）
> 全新的VC-LTL 4.0，代号——“破晓”，作为大版本升级某些行为已经更改：
> 1. VC-LTL 4.0 开始，如需静态编译（仅依赖msvcrt.dll）请使用`/MT`编译。
> 2. VC-LTL 4.0 不兼容VC-LTL 3.X编译的静态库，你需要使用VC-LTL 4.0（破晓）重新编译。

* 新增Fea，添加VC-LTL动态库支持，使用`/MD`编译即可链接到 ucrtbase_ltl.dll 等DLL。
* 新增Fea，彻底抛弃DDK，重构CRT库，全面支持指针混淆、`quick_exit`、`__stdio_common`等功能。
* 新增Fea，添加ARM、ARM64链接到msvcrt.dll支持。
* 新增Fea，添加WinRT（CX）支持（仅限于Windows 8.1以及更高版本支持）。
* 改进体验，XP模式时无需手动指定`所需的最低版本`。
* 改进体验，充分利用msvcp60.dll缩小程序体积。


### 4.0.0.10 - 添加Vistual Studio 2017 15.8支持（2018-08-26 17:20）
* 解决Bug，解决CMake脚本语法错误（感谢 MaMy丶）。
* 解决Bug，解决CMake脚本文件路径判断方式不正确问题。
* 解决Bug，解决CMkae、NMake脚本lib路径错误问题（感谢 Too Simple、MaMy丶）。
* 解决Bug，修正MD模式报告找不到`_libm_sse2_tan_precise`相关符号问题（感谢 13732650）。
* 解决Bug，由于微软类库自身设计缺陷，必须关闭sdl，阻止类成员0初始化，避免特定场合STL崩溃问题（感谢 漆黑の牙）。
* 新增Fea，添加Vistual Studio 2017 15.8支持。
* 新增Fea，添加lib文件检测，如果不存在则报错。


### 4.0.0.17 - 添加C++/CLI支持（2018-09-26 10:00）
* 解决[Bug 40](https://github.com/Chuyu-Team/VC-LTL/issues/40)，XP x64模式中使用RTTI时链接失败问题（感谢 killvxk）。
* 解决[Bug 41](https://github.com/Chuyu-Team/VC-LTL/issues/41)，当禁用内部函数时无法使用`_byteswap_ulong`、`_byteswap_uint64`、`_byteswap_ushort`（感谢 wangwenx190）。
* 解决Bug，当禁用内部函数时，Windows XP提示找不到`_abs64`、`_rotl64`以及`_rotr64`函数的入口点。
* 新增Fea，添加`wcstold`、`_wcstof_l`、`_wcstod_l`、`_wcstold_l`、`_strtod_l`(xp)以及`_strftime_l`(Vista)。
* 新增Fea，添加C++/CLI支持（感谢 naturalj0）。


### 4.0.0.26 - 累计问题修复（2018-12-19 18:00）
* 解决Bug，修复Windows XP模式`_Atexit`函数依赖`EncodePointer`问题（感谢 亮亮）。
* 解决Bug，修复微软原版details::Etw中错误的使用`EncodePointer`问题。
* 解决Bug，修复`_initialize_onexit_table_downlevel`一处内存泄漏问题（感谢 g-qa-ck组）。
* 解决Bug，修复Windows XP找不到函数`operator new(size_t,int,char const *,int)`、`operator new[](size_t,int,char const *,int)`入口点（感谢 希望）。
* 解决Bug，修复Spectre + WinXP 32下缺失exsup4符号问题（感谢 Too Simple）。
* 新增Fea，添加`delete[](void *, std::nothrow_t const &)`（感谢 Too Simple）。
* 新增Fea，添加Windows 10 17763 UCRT支持（感谢 毛利）。
* 新增Fea，添加VS 2017 14.16.27023工具集支持。
* 新增Fea，为Windows XP添加`_IsExceptionObjectToBeDestroyed、_CreateFrameInfo、_FindAndUnlinkFrame`（感谢 夏思畅）。


### 4.0.0.28 - 累计问题修复（2019-02-16 14:50）
* 解决[Bug 45](https://github.com/Chuyu-Team/VC-LTL/issues/45)，main函数argv参数默认被展开问题（感谢 xspeed1989 ）。
* 解决Bug，VS 14.14.26428以及更高平台缺少`pplerror.cpp`问题（感谢 xspeed1989 ）。
* 新增Fea，改进Visual Studio 2017中对Clang-LLVM的支持（感谢 神话）。
* 新增Fea，新增`VC-LTL helper for qmake.pri`脚本，改进对QMake的支持（感谢 漆黑の牙）。


### 4.0.1.10 - 改进支持（2019-06-01 13:40）
* 解决Bug，Windows XP模式没有引用`advapi32.lib`，可能导致`ImpersonateSelf`等符号无法解析（感谢 昌平517）。
* 解决Bug，微软原版std::thread依赖GetLogicalProcessorInformation导致XP RTM无法运行（感谢 小古）。
* 解决Bug，为XP模式添加_mbsinc，解决此函数越界问题（感谢 亮亮）。
* 解决Bug，微软原版std::mutex依赖GetNumaHighestNodeNumber导致XP RTM无法运行。
* 解决Bug，ucrtbase_ltl.dll没有导出llabs（感谢 GPBeta）。
* 解决Bug，将意外遗漏的`_downlevel`后缀删除（感谢 过客）。
* 解决Bug，config.pri错误导致QT无法加载问题（感谢 GPBeta）。
* 新增Fea，`VC-LTL helper for qmake.pri`脚本添加老版本QMake兼容（感谢 大胸滴）。
* 新增Fea，CMake脚本添加vcpkg兼容（感谢 大胸滴）。
* 新增Fea，添加Visual Studio 2019支持。
* 新增Fea，新增10.0.18362.0 UCRT支持（感谢 毛利）
* 改进体验，Vista模式将`_ATL_XP_TARGETING`以及`_USING_V110_SDK71_`宏从错误降级到警告。


### 4.0.2.14 - 改进支持（2019-08-14 18:00）
* 解决Bug，Windows XP模式找不到 `__wcsrtombs_utf8` 符号问题（感谢 水边）。
* 解决[Bug 56](https://github.com/Chuyu-Team/VC-LTL/issues/56)，解决(w)printf、std::(w)cout等无法输出中文问题。
* 解决Bug，_tzname声明错误（感谢 大胸）。
* 解决Bug，Windows XP模式中`_splitpath_s、_wsplitpath_s`文件名为空问题（感谢 水边）。
* 新增Fea，添加 `_CRT_STDIO_ISO_WIDE_SPECIFIERS` 宏以及 `legacy_stdio_definitions.lib` 支持（感谢 大胸）。
* 新增Fea，添加`_initialize_invalid_parameter_handler`、`_initialize_denormal_control`、`_get_startup_thread_locale_mode（仅Vista模式）`支持。
* 新增Fea，添加 14.22.27905 工具集支持。
* 新增Fea，为Windows XP添加`_atoi_l、_strtol_l、_atoflt_l、_atoldbl_l`等支持。
* 新增[Fea 17](https://github.com/Chuyu-Team/VC-LTL/issues/17)，为Windows XP添加`_create_locale、_wcreate_locale、_get_current_locale`支持。
* 新增Fea，14.16.27023同步到14.16.27033版本。
* 改进体验，改进与联想一键影音的兼容性，由于联想一键影音错会乱Hook，导致LoadLibraryExW行为异常。这样将导致VC-LTL等在没有安装KB2533623的系统上无法正常使用问题（微软原版也同样存在此问题）。
* 改进体验，改进Windows 7 RTM以及以下系统的兼容性，由于这些老版本系统由于在LoadLibraryExW期间不会恢复重定向，因此当目标线程关闭重定向时可能导致VC-LTL无法正常工作（微软原版也同样存在此问题）。
* [改进体验 53](https://github.com/Chuyu-Team/VC-LTL/issues/53)，关闭对STL库的引用消除，规避LLVM链接失败问题（感谢 hotxp、BigBrother）。


### 4.0.3.8 - 改进支持（2019-12-21 14:00）
* 改进体验，改进VS用户使用VC-LTL的体验（感谢 毛利）。
* 改进体验，改进QT支持（感谢 昌平517）。
* 新增Fea，添加14.23.28105支持。
* 新增Fea，添加14.24.28314支持。
* 新增Fea，添加LLVM for Visual Studio 2019支持。
* 行为调整，消除FH4对TLS的依赖。
* 解决Bug，消除 corecrt_wstdio.h 以及 mbstring.h中的 C4201警告（感谢 大胸）。
* 解决Bug，解决XP `legacy_stdio_definitions.lib` 缺少 vsnprintf_s 问题（感谢 昌平517）。
* 解决Bug，轻量模式中，`legacy_stdio_definitions.lib`可能出现符号冲突问题（感谢 昌平517）。


### 4.1.0.4 - 改进支持（2020-07-01 16:00）
* 改进体验，增强_Getcvt对C++17支持。
* 新增Fea，添加msvcp60静态库模式，增强某些精简系统的兼容性。
* 新增Fea，添加14.25.28610、14.26.28801支持。
* 新增Fea，添加UCRT 10.0.19041.0支持。
* 解决Bug，轻量模式中，`legacy_stdio_definitions.lib`可能出现符号冲突问题（感谢 Big吊）。
* 解决Bug，消除CString::Format警告（感谢 大胸滴）。
* 解决Bug，解决 14.26.28801 syserror_import_lib 缺失问题（感谢 噬元兽）。

> 温馨提示：VC-LTL 4.1 与 4.0版本在STL stream支持中存在ABI不兼容问题，对应静态库请重新编译！
