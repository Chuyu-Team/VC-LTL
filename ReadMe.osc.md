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
[更新日志](https://gitee.com/Chuyu-Team/VC-LTL/wikis/Home#%E6%9B%B4%E6%96%B0%E6%97%A5%E5%BF%97)
