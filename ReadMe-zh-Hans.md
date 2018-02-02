# VC-LTL——编译精巧的程序原来如此简单

VC-LTL 是一个基于微软VC修改的开源VC库，与微软原版库源码级高度兼容。创建初衷是为了解决`运行库部署问题`以及 `Fls128上限问题`。

在大型项目中往往有众多模块，如果都采用静态编译那么造成的空间浪费先不说，最后也会因为`Fls上限`导致程序无法正常运行。

而VC-LTL能让你的项目如同系统文件一样共享系统内置`msvcrt.dll`，有效的解决`Fls上限`以及`运行时部署问题`，同时`大大缩减程序体积`，可以说一箭三雕！

大家都可以免费，无条件，甚至是用于商业环境。当然我也希望如果大家可以在程序的说明文件中声明下使用了VC-LTL，来使更多人受益。

——mingkuang

[ [VC-LTL交流群 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]

## 原理：
使用 VC-LTL后可以将程序动态链接到系统自带的msvcrt.dll中，来减少程序体积。目前使用CRT以及STL的工程一般都可以使用。但是MFC工程不能使用，因为MFC类库太复杂了，尚未适配。

温馨提示：使用VC-LTL，C++程序体积大约缩减30%，而纯C程序则大约缩减50%。

## 亮点
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 无缝使用最新C/C++库以及最新编译器，尽情的使用最新规范。神马异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。


让我们一起跟VS 2008说拜拜！


## 支持平台
### 支持的IDE
* Visual Studio 2015
* Visual Studio 2017

### 支持的Windows SDK版本
* 7.X（WinXP平台工具集）
* 8.1
* 10.0.10240.0
* 10.0.15063.0（强烈建议使用16299，下个Windows SDK发布时将删除对15063的支持！）
* 10.0.16299.0（推荐使用）

### 支持的操作系统
* 基于x86以及x64的 Windows XP、Windows Server 2003（需要启用WinXP Support）
* 基于x86以及x64的 Windows Vista、Windows Server 2008
* 基于x86以及x64的 Windows 7、Windows Server 2008 R2
* 基于x86以及x64的 Windows 8、Windows Server 2012
* 基于x86以及x64的 Windows 8.1、Windows Server 2012 R2
* 基于x86以及x64的 Windows 10、Windows Server 2016

> 采用VC-LTL编译后的程序能兼容Windows XP RTM以上所有操作系统，无需安装任何SP补丁包。

## 使用方法：
### 1. 配置VC-LTL加载路径
你可以在 1.1 或者 1.2 中任意选择一种VC-LTL加载方式

#### 1.1. 通过配置共享VC-LTL
> 如果你有多个不同位置的工程需要使用VC-LTL，那么优先推荐使用此方式。

假如，你将VC-LTL下载至 `D:\Src\VC-LTL`（具体位置无任何要求），双击 `D:\Src\VC-LTL\Install.cmd` 即可。

然后呢？没有然后了，脚本自动会在 `HKCU\Code\VC-LTL` 创建注册表。


#### 1.2. 通过目录独享VC-LTL
> 此方案不利于源代码共享，我们优先推荐你使用 1.1 中描述的方式。

假如，你的Sln文件在 `D:\MySln\MySln.sln`。你必须把VC-LTL放在 `D:\VC-LTL` 或者 `D:\MySln\VC-LTL`。

简单的说，你需要把 VC-LTL 放在 `Solution根目录` 或者 `Solution父目录`。


### 2. 加载VC-LTL（仅Release）
#### 2.1. 添加Shared属性表
将属性表 `Shared.props` 复制到你的工程目录，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键 `添加现有属性表` ，然后选择 `Shared.props` 即可。

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/zh-Hans/image/AddShared.png)

> 如果你不希望使用 `Shared.props` 属性表，那么请手工将属性表的设置转移到你的工程配置中。


#### 2.2. 配置工程属性
* 常规 - 【Windows SDK版本】调整为【7.X/8.1/10.0.10240.0/10.0.15063.0/10.0.16299.0（推荐）】（从中选择任意SDK版本，但是尽量不要选择15063，因为在不久会删除15063 SDK 支持）
* C/C++ - 代码生成 -【运行库】调整为【多线程 DLL (/MD)】

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/zh-Hans/image/ConfigurationProject.png)

> 如果需要支持XP，请在平台工具集中，切换到Windows XP，或者修改 `Shared.props` 启用 `<SupportWinXP>true</SupportWinXP>` 即可。


### 3. 重新编译（仅Release）
现在是不是体积就小了很多。如果你编译不通过，可以先参考 第 4 节。如果还是不通过可以反馈，共同改进VC-LTL。

> 如果正确引用VC-LTL，那么 会在生成时输出 `note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`

![AppBuildByVC-LTL](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/image/AppWithLTL.png)


### 4. 常见问题
#### 4.1. 未共享到msvcrt.dll
问题原因：未正确引用VC-LTL。建议看看生成日志，是否包含 `note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。`

解决方案：

1：请务必确保Shared.props已经添加到工程。

2：确保以下设置正确：
	* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
	* 连接器 - 输入 - 附加依赖项 - 【√ 从父项或项目默认设置继承(I)】

#### 4.2. 无法解析外部符号 delete 等
问题原因：没有正确引入vc.lib、msvcrt_advanced.obj。

解决方案：
* VC++ 目录 - 包含目录 - 【√ 从父项或项目默认设置继承(I)】
* 连接器 - 输入 - 附加依赖项 - 【√ 从父项或项目默认设置继承(I)】

#### 4.3. 检测到RuntimeLibrary的不匹配项
问题原因：引入了没有使用VC-LTL编译的静态lib文件。

解决方案：
使用VC-LTL重新编译对应的静态lib（具体lib名称错误日志会给出）。


## VC-LTL兼容性
此表展示了VC-LTL，C/C++库函数覆盖率，通过覆盖情况，可以大致了解VC-LTL的完善程度。

|  模块  | 一般模式 | XP工具集            | 相关文件 
|  ----  | -------- | --------            | --------
|   CRT  | 91.726%  | 88.389%             | ltl.lib，ltlxp.lib，msvcrt.lib，msvcrt_advanced.obj，msvcrt_light.obj，msvcrt_win2003.obj，msvcrt_winxp.obj，ucrt.lib，vc.lib
|   STL  | 100%     | 100.1%（超标准支持）| ltlcprt.lib，ltlcprtxp.lib
| ConcRT | 100%     | 100%                | libconcrt.lib，libconcrtxp.lib
|   ATL  | 100%     | 100%                | -
|   AMP  |   -      |   -                 | -
|   MFC  |  不支持  |   不支持            | -

### 已知问题规避
* 由于WinXP本身BUG，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 _CRT_STDIO_SIZE_MAX 宏。_s 以及 _l 相关版本不存在此问题。
* 由于WinXP本身BUG，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。_s 以及 _l 相关版本不存在此问题。

### 已知与VC-LTL兼容的项目
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
