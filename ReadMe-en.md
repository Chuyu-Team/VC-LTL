# VC-LTL - An elegant way to compile lighter binaries.

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

## The principle of VC-LTL
After using the VC-LTL, the binaries will be dynamically linked to the built-in
`msvcrt.dll` in the Windows to reduce the binaries size. It is generally 
available if you use the VC-LTL in the project using the C Runtime and the STL.
However you cannot use the VC-LTL in the MFC project with the VC-LTL because it
is too complex to support.

Notice: After using the VC-LTL, the size of the binaries will reduce about 30 
percent if compiled from the C++ source code, and about 50 percent if compiled 
from the pure C source code.

## 亮点
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 无缝使用最新C/C++库以及最新编译器，尽情的使用最新规范。神马异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧！！
* 拥有比微软原版更好的兼容性，即使想兼容Windows XP RTM也可以安心的对新编译器说“Yes”。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为VC-LTL添砖加瓦。

Let's say goodbye to the Visual Studio 2008.

## Supported Platforms
### Supported Visual Studio Versions
* Visual Studio 2015
* Visual Studio 2017

### Supported Windows SDK Versions
* 7.x (Windows XP Platform Toolset)
* 8.1
* 10.0.10240.0
* 10.0.15063.0 (Will be removed when the next SDK version come out.)
* 10.0.16299.0 (Recommend to use)

### Supported Windows Versions
Notice: All versions we mentioned is started with RTM.

* x86 and x64 of Windows XP / Windows Server 2003 (Need to enable the XP Support.)
* x86 and x64 of Windows Vista / Windows Server 2008
* x86 and x64 of Windows 7 / Windows Server 2008 R2
* x86 and x64 of Windows 8 / Windows Server 2012
* x86 and x64 of Windows 8.1 / Windows Server 2012 R2
* x86, x64 and arm64 of Windows 10 / Windows Server 2016

> The binaries compiled with the VC-LTL is compatible with Windows XP and 
later, even in the environment which installed no hotfixes.

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

![AddShared](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/AddShared.png)

> 如果你不希望使用 `Shared.props` 属性表，那么请手工将属性表的设置转移到你的工程配置中。


#### 2.2. 配置工程属性
* C/C++ - 代码生成 -【运行库】调整为【多线程 DLL (/MD)】

![ConfigurationProject](https://raw.githubusercontent.com/wiki/Chuyu-Team/VC-LTL/en/image/ConfigurationProject.png)

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

#### 4.4. 支持XP时从msvcrt.dll导入大量XP不支持的函数
问题原因：可能没有开启引用消除

解决方案：
* C/C++ - 所有选项 - 移除未引用的代码和数据 - 【是(/Zc:inline)】
* 连接器 - 所有选项 - 引用 - 【是(/OPT:REF)】

## VC-LTL Compatibility
此表展示了VC-LTL，C/C++库函数覆盖率，通过覆盖情况，可以大致了解VC-LTL的完善程度。

|  Module  | Normal Mode |  XP Support  | Files 
|  ------  | ----------- |  ----------  | --------
|  CRT     | 91.726%     | 88.389%      | ltl.lib，ltlxp.lib，msvcrt.lib，msvcrt_advanced.obj，msvcrt_light.obj，msvcrt_win2003.obj，msvcrt_winxp.obj，ucrt.lib，vc.lib
|  STL     | 100%        | 100.1% ([1]) | ltlcprt.lib，ltlcprtxp.lib
|  ConcRT  | 100%        | 100%         | libconcrt.lib，libconcrtxp.lib
|  ATL     | 100%        | 100%         | -
|  AMP     | -           | -            | -
|  MFC     | No Support  | No Support   | -

PS:
- [1] Extended support.

### 已知问题规避
* 由于WinXP本身BUG，printf相关函数输入缓冲区最大字符数为0x3FFFFFFF（包含）。当你需要兼容XP时，请务必确认缓冲区输入长度小于0x3FFFFFFF，或者直接使用 _CRT_STDIO_SIZE_MAX 宏。_s 版本不存在此问题。
* 由于WinXP本身BUG，printf相关函数无法正常支持`%ll`。当你需要兼容XP时，请优先考虑使用`%I64`代替。_s 版本也存在此问题。
* 由于msvcrt本身限制，setlocale/_create_locale相关函数不支持UCRT的locale name，使用时必须按VC 2008规范使用，比如 setlocale(0, ".936") 这样调用，而不是传入setlocale(0, "zh-CN")。

### The known VC-LTL compatible projects 
This list simply indicates that some developers have already compiled these 
projects successfully with using the VC-LTL. Do not mean the VC-LTL is only 
compatible with the following projects.

| Project                                                      | Normal Mode | XP Support
| ---                                                          | ----------- | ---------- 
| [duilib](https://github.com/duilib/duilib)                   |      √     |     √
| [FastCopy](https://ipmsg.org/tools/fastcopy.html.en)         |      √     |     √
| [WinPCK](http://www.winpak.com/en/home/)                     |      √     |     √
| [RapidXml](http://rapidxml.sourceforge.net/)                 |      √     |     √
| [JsonCPP](https://github.com/open-source-parsers/jsoncpp)    |      √     |     √
| [icu](http://source.icu-project.org/repos/icu/trunk)         |      √     |   √([1])
| [SQLite](http://www.sqlite.org/download.html)                |      √     |     √
| [LuaJIT](http://luajit.org/)                                 |      √     |     √
| [Qt](https://www.qt.io/)                                     |      √     |     X
| [llvm](http://llvm.org/)                                     |      √     |     ?
| [openssl](https://www.openssl.org/)                          |      √     |     √
| [libcurl](https://curl.haxx.se/libcurl/)                     |      √     |     √
| [ninja](https://ninja-build.org/)                            |      √     |     ?
| [NSudo](https://github.com/M2Team/NSudo)                     |      √     |   X([2])
| [GacUI](https://github.com/vczh-libraries/Release)           |      √     |   X([2])

PS:
- [1] Need to Disable _create_locale.
- [2] The project don't support Windows XP.
