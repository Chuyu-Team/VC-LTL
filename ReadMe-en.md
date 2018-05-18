# VC-LTL - An elegant way to compile lighter binaries.

## 1. About VC-LTL
VC-LTL is an open source CRT library based on the MS VCRT, which is highly
compatible with the MS VCRT in the source code level. I created the VC-LTL
because I desire to solve the C runtime deployment problem and the fiber local
storage (FLS) 128 limitation problem.

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

By mingkuang, the creator of VC-LTL.

[ [VC-LTL QQ Group: 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]

### 1.1. The principle of VC-LTL
After using the VC-LTL, the binaries will be dynamically linked to the built-in
`msvcrt.dll` in the Windows to reduce the binaries size. It is generally 
available if you use the VC-LTL in the project using the C Runtime and the STL.
However you cannot use the VC-LTL in the MFC project with the VC-LTL because it
is too complex to support.

Notice: After using the VC-LTL, the size of the binaries will reduce about 30 
percent if compiled from the C++ source code, and about 50 percent if compiled 
from the pure C source code.

### 1.2. 亮点
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 无缝使用最新C/C++库以及最新编译器，尽情的使用最新规范。神马异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。

Let's say goodbye to the Visual Studio 2008.

## 2. VC-LTL Compatibility
此表展示了VC-LTL，C/C++库函数覆盖率，通过覆盖情况，可以大致了解VC-LTL的完善程度。

|  Module  | Normal Mode |  XP Support  | UCRT Mode | Files 
|  ------  | ----------- |  ----------  | --------- | --------
|  CRT     | 91.911%     | 88.845%      |    100%   | ltl.lib，msvcrt.lib，msvcrt_Platform.lib，ucrt.lib，vc.lib
|  STL     | 100%        | 100.1% (Extended support) |    100%   | ltlcprt.lib，ltlcprtxp.lib
|  ConcRT  | 100%        | 100%         |    100%   | libconcrt.lib，libconcrtxp.lib
|  ATL     | 100%        | 100%         |    100%   | -
|  AMP     | -           | -            |     -     | -
|  MFC     | No Support  | No Support   |     ?     | -


### 2.1. Supported Visual Studio Versions
* Visual Studio 2015
* Visual Studio 2017

### 2.2. Supported Build Tools
|   Build Tool   | Helper      
| -------------- | --
| Visual Studio  | [VC-LTL helper for Visual Studio.props](ReadMe-zh-Hans.md#32-在visual-studio中使用vc-ltl)
| CMake          | [VC-LTL helper for cmake.cmake](ReadMe-zh-Hans.md#33-在cmake中使用vc-ltl)
| NMake          | [VC-LTL helper for nmake.cmd](ReadMe-zh-Hans.md#34-在nmake纯cl中使用vc-ltl)
| CL             | [VC-LTL helper for nmake.cmd](ReadMe-zh-Hans.md#34-在nmake纯cl中使用vc-ltl)

### 2.3. Supported Windows Versions
|         OS                                          | x86 | x64 | arm | arm64 
|        ----                                         | --- | --- | --- | ----
| Windows XP, Windows Server 2003                     | √  | √  | -   | -
| Windows Vista, Windows Server 2008                  | √  | √  | -   | -
| Windows 7, Windows Server 2008 R2                   | √  | √  | -   | -
| Windows 8, Windows Server 2012, Windows RT          | √  | √  | √ (Requires [KB2999226](http://support.microsoft.com/kb/2999226) to be installed)  | -
| Windows 8.1, Windows Server 2012 R2, Windows RT 8.1 | √  | √  | √ (Requires [KB2999226](http://support.microsoft.com/kb/2999226) to be installed)  | -
| Windows 10, Windows Server 2016                     | √  | √  | √ | √

> The binaries compiled with the VC-LTL is compatible with Windows XP and 
later, even in the environment which installed no hotfixes.

## 3. 使用方法

### 3.1. 安装VC-LTL
假如，你将[VC-LTL Binary](https://github.com/Chuyu-Team/VC-LTL/releases)下载并解压至`D:\Src\VC-LTL`（具体位置无任何要求），双击`D:\Src\VC-LTL\Install.cmd`即可。

> 脚本会在`HKCU\Code\VC-LTL`创建注册表。

### 3.2. 在Visual Studio中使用VC-LTL

#### 3.2.1. 添加VC-LTL属性表
将属性表`VC-LTL helper for Visual Studio.props`复制到你的工程目录，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键`添加现有属性表`，然后选择`VC-LTL helper for Visual Studio.props`即可。

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/AddShared.png)

#### 3.2.2. 配置工程属性
* C/C++ - 代码生成 -【运行库】调整为【多线程 DLL (/MD)】

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/ConfigurationProject.png)

> 如果需要支持XP，请在平台工具集中，切换到`Windows XP`，或者修改`VC-LTL helper for Visual Studio.props`启用 `<SupportWinXP>true</SupportWinXP>` 即可。

### 3.3. 在CMake中使用VC-LTL

#### 3.3.1. 添加VC-LTL配置文件

将模块文件`VC-LTL helper for cmake.cmake`复制到你的工程目录（顶层CMakeLists.txt同级目录）。然后在`CMakeLists.txt`中添加配置上添加一行 `include("VC-LTL helper for cmake.cmake")` 即可。

示例：

```
cmake_minimum_required(VERSION 3.5.2)
project(ltltest)

include("VC-LTL helper for cmake.cmake")

add_subdirectory(src)
```

#### 3.3.2. 调整配置工程

> 确保使用VC-LTL时使用`/MD`编译代码，如需XP支持，修改`VC-LTL helper for cmake.cmake`启用 `set(SupportWinXP "true")` 即可。

### 3.4. 在NMake/纯CL中使用VC-LTL

#### 3.4.1. 运行VC-LTL辅助脚本

将辅助脚本`VC-LTL helper for nmake.cmd`复制到你的工程目录。启动`vcvars32.bat/vcvars64.bat`执行此脚本即可，脚本将自动修改`include`以及`lib`环境变量。

#### 3.4.2. 配置工程属性

> 确保使用VC-LTL时使用`/MD`编译代码，如需XP支持，修改`VC-LTL helper for nmake.cmd`启用 `set SupportWinXP=true`，并且需要自行调整连接器最低系统支持为`5.01`（WinXP x86）或者`5.02`（WinXP x64）。


### 3.5. 重新编译（仅Release）
现在是不是体积就小了很多。如果你编译不通过，可以先参考 [4. 常见问题](ReadMe-zh-Hans.md#4-常见问题)。如果还是不通过可以反馈，共同改进VC-LTL。

温馨提示：使用VC-LTL编译时必须采用`/MD`编译，并且所有依赖的静态库也必须使用VC-LTL重新编译。

> 如果正确引用VC-LTL，那么 会在生成时输出：`note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`

![AppBuildByVC-LTL](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/AppWithLTL.png)


## 4. 常见问题
### 4.1. 未共享到msvcrt.dll
#### 问题原因
未正确引用VC-LTL。建议看看生成日志，是否包含：`note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`

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

### 4.4. 支持XP时从msvcrt.dll导入大量XP不支持的函数
#### 问题原因

可能没有开启引用消除

#### 解决方案
* C/C++ - 所有选项 - 移除未引用的代码和数据 - 【是(/Zc:inline)】
* 连接器 - 所有选项 - 引用 - 【是(/OPT:REF)】


## 5. 已知问题
* 由于WinXP本身BUG，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 _CRT_STDIO_SIZE_MAX 宏。_s 版本不存在此问题。
* 由于WinXP本身BUG，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。_s 版本也存在此问题。
* 由于msvcrt本身限制，setlocale/_create_locale相关函数不支持UCRT的locale name，使用时必须按VC 2008规范使用，比如 `setlocale(0, ".936");` 这样调用，而不是传入 `setlocale(0, "zh-CN");`。

### The known VC-LTL compatible projects 
This list simply indicates that some developers have already compiled these 
projects successfully with using the VC-LTL. Do not mean the VC-LTL is only 
compatible with the following projects.

## Excursus - Known project using VC-LTL

|  Project                                                     | Note
|  ---                                                         | ----
| [NSudo](https://github.com/M2Team/NSudo)                     | 一个强大的系统管理工具。VC-LTL的帮助下减少30%的程序体积，包括NSudo for arm64。
| [Menu98](https://github.com/rikka0w0/Menu98)                 | 经典样式可自定义的开始按钮右键菜单。使用VC-LTL移除运行时依赖。
| [壁虎浏览器](http://bhbrowser.com/)                          | 一款专业解决DNS劫持的浏览器。使用VC-LTL有效减少程序体积，减少安装包大小。
| [librech551](https://github.com/rgwan/librech551)            | 开源跨平台的CH55x ISP软件。使用VC-LTL移除运行时依赖。
| [Dism++](https://www.chuyu.me/)                              | Dism GUI版。初雨团队自身项目，使用VC-LTL有效减少程序体积，减少安装包大小。
| [360安全卫士](https://www.360.cn/)                           | 奇虎360推出的上网安全软件。360EvtMgr.exe、360leakfixer.exe、360Util.dll、leakrepair.dll等文件使用VC-LTL编译，在VC-LTL的支持下：升级新编译器，减少文件尺寸，完美兼容WinXP，一箭三雕。
