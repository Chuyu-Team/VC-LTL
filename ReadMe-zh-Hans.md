# VC-LTL——编译精巧的程序原来如此简单

VC-LTL 是一个开源的第三方修改VC库，大家都可以免费，无条件，甚至是用于商业环境。本库基于VC140/141修改，所以也同样适用Common Public License协议。

但我也希望如果你可以在程序的说明文件中声明下，使用了此库，来使更多人受益。

——mingkuang

[ [VC-LTL交流群 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]

## 原理：
使用 VC-LTL后可以将程序动态链接到系统自带的msvcrt.dll中，来减少程序体积。目前使用CRT以及STL的工程一般都可以使用。但是MFC工程不能使用，因为MFC类库太复杂了，尚未适配。

## 亮点
* 无缝使用最新C/C++库以及最新编译器，无需使用陈旧的类库或者编译器，尽情的使用最新规范！
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 支持编译器最新特性，异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。


让我们一起跟VS 2008说拜拜！


## 支持平台
### 支持的IDE
* Vistual Studio 2015
* Vistual Studio 2017

### 支持的目标平台（UCRT版本）
* Windows XP平台工具集（Windows 7.0 SDK & Windows 10 10240 UCRT）
* Windows 8.1 目标平台
* Windows 10 10240目标平台
* Windows 10 15063目标平台（强烈建议使用16299，下个Windows SDK发布时将删除对15063的支持！）
* Windows 10 16299目标平台（推荐使用）

### 支持的操作系统
* Windows XP RTM, Windows XP 64 RTM, Windows 2003 RTM（平台工具集需要调整为 Windows XP平台工具集）
* Windows Vista RTM, Windows 2008 RTM
* Windows 7 RTM, Windows 2008 R2 RTM
* Windows 8 RTM, Windows 2012 RTM
* Windows 8.1 RTM, Windows 2012 R2 RTM
* Windows 10, Windows 2016

> 采用VC-LTL编译后的程序能兼容Windows XP RTM以上所有操作系统，无需安装任何SP补丁包。

## 使用方法：
### 1. 配置VC-LTL加载路径
你可以在 1.1 或者 1.2 中任意选择一种VC-LTL加载方式

#### 1.1 通过配置共享VC-LTL
VC-LTL属性表优先从注册表加载路径。如果你希望在多个工程中共享VC-LTL那么此方法会比较友好。

假如，你将VC-LTL下载至`D:\Src\VC-LTL`，双击 `D:\Src\VC-LT\Install.cmd` 即可。

双击后，cmd脚本会在 HKCU\Code\VC-LTL 创建注册表。


随后将属性表 `Shared.props` 复制到你的工程目录，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键 `添加现有属性表` ，然后选择 `Shared.props` 即可。

PS：如果需要支持XP，请在平台工具集中，切换到Windows XP，或者修改 `Shared.props` 启用 `<SupportWinXP>true</SupportWinXP>` 即可。


#### 1.2 通过目录独享VC-LTL
> 此方案不利于源代码共享，我们优先推荐你使用 1.1 中描述的方式。

假如，你的Sln文件在	`D:\MySln\MySln.sln`，这时你把VC-LTL放在 `D:\VC-LTL` 或者 `D:\MySln\VC-LTL`。

随后将属性表 `Shared.props` 复制到你的工程目录，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键 `添加现有属性表` ，然后选择 `Shared.props` 即可。

PS：如果需要支持XP，请在平台工具集中，切换到Windows XP，或者修改 `Shared.props` 启用 `<SupportWinXP>true</SupportWinXP>` 即可。

> 如果你不希望使用 `Shared.props` 属性表，那么请手工将属性表的设置转移到你的工程配置中。


### 2. 在工程属性（Release配置） C++ - 所有选项：
* 【运行库】调整为 【多线程DLL/MD】
* 【目标平台】调整为【8.1/10.0.10240.0/10.0.15063.0/10.0.16299.0（推荐）】（从中选择任意SDK版本，但是尽量不要选择15063，因为在不久会删除15063 SDK 支持）

### 3. 重新编译你的应用程序（Release）
现在是不是体积就小了很多。如果你编译不通过，咋们可以一起研究研究，共同改进VC-LTL。

### 4. VC-LTL兼容性

此表展示了VC-LTL，C/C++库函数覆盖率，通过覆盖情况，可以大致了解VC-LTL的完善程度。

|  模块  | 一般模式 | XP工具集            | 相关文件 
|  ----  | -------- | --------            | --------
|   CRT  | 91.259%  | 87.084%             | ltl.lib，ltlxp.lib，msvcrt.lib，msvcrt_advanced.obj，msvcrt_light.obj，msvcrt_win2003.obj，msvcrt_winxp.obj，ucrt.lib，vc.lib
|   STL  | 100%     | 100.1%（超标准支持）| ltlcprt.lib，ltlcprtxp.lib
| ConcRT | 100%     | 100%                | libconcrt.lib，libconcrtxp.lib
|   ATL  | 100%     | 100%                | -
|   AMP  |   -      |   -                 | -
|   MFC  |  不支持  |   不支持            | -

#### 4.1 已知问题规避
* 由于WinXP本身BUG，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 _CRT_STDIO_SIZE_MAX 宏。_s 以及 _l 相关版本不存在此问题。
* 由于WinXP本身BUG，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。_s 以及 _l 相关版本不存在此问题。

#### 4.2 已知与VC-LTL兼容的项目
此列表只是表示已经有开发者使用VC-LTL编译并使用，并不代表VC-LTL仅能兼容以下项目。

|  项目                                                        | 一般模式 | XP工具集 
|  ---                                                         | -------- | --------
| [duilib](https://github.com/duilib/duilib)                   |    √    | √
| [FastCopy](https://ipmsg.org/tools/fastcopy.html.en)         |    √    | √
| [WinPCK](http://www.winpak.com/en/home/)                     |    √    | √
| [RapidXml](http://rapidxml.sourceforge.net/)                 |    √    | √
| [JsonCPP](https://github.com/open-source-parsers/jsoncpp)    |    √    | √
| [icu](http://source.icu-project.org/repos/icu/trunk)         |    √    | √（但是需要禁用_create_locale）
| [SQLite](http://www.sqlite.org/download.html)                |    √    | √
| [LuaJIT](http://luajit.org/)                                 |    √    | √
| [Qt](https://www.qt.io/)                                     |    √    | X
| [llvm](http://llvm.org/)                                     |    √    | ?
| [openssl](https://www.openssl.org/)                          |    √    | √
| [libcurl](https://curl.haxx.se/libcurl/)                     |    √    | √
| [ninja](https://ninja-build.org/)                            |    √    | ?
| [NSudo](https://github.com/M2Team/NSudo)                     |    √    | X（NSudo本身不支持XP）
| [GacUI](https://github.com/vczh-libraries/Release)           |    √    | X（GacUI本身不支持XP）
