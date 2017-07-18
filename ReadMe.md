
# VC LTL——编译精巧的程序原来如此简单

VC LTL 是一个开源的第三方修改VC库，大家都可以免费，无条件，甚至是用于商业环境。本库基于VC140/141修改，所以也同样适用Common Public License协议。

但我也希望如果你可以在程序的说明文件中声明下，使用了此库，来使更多人受益。

——mingkuang


## 原理：
使用 VC LTL后可以将程序动态链接到系统自带的msvcrt.dll中，来减少程序体积。一般来说一些C/C++的Win32应用程序都可以使用。但是MFC工程不能使用，因为MFC类库太复杂了。

## 亮点
* 无缝使用最新C/C++库以及最新编译器，无需使用陈旧的类库或者编译器，尽情的使用最新规范！
* 晚起的鸟儿也有虫虫吃，优雅的引用方式，仅添加一个属性表就能享受极致的体积体验。
* 支持编译器最新特性，异常流防护（guard:cf）、静态对象线程安全初始化（threadSafeInit）……统统放马过来吧~

## 支持平台
### 支持的IDE
* Vistual Studio 2015
* Vistual Studio 2017

### 支持的目标平台（UCRT版本）
* Windows XP平台工具集（Windows 7.0 SDK & Windows 10 10240 UCRT）
* Windows 10 10240目标平台
* Windows 10 14393目标平台（强烈建议使用15063，下个Windows SDK发布时将删除对14393的支持！）
* Windows 10 15063目标平台（推荐使用）

> 目标平台并不影响你的程序兼容老版本Windows（比如Windows XP），一般我们都推荐大家使用高版本目标平台。

### 支持的操作系统
* Windows XP, Windows XP 64, Windows 2003（平台工具集需要调整为 Windows XP平台工具集）
* Windows Vista, Windows 2008
* Windows 7, Windows 2008 R2
* Windows 8, Windows 2012
* Windows 8.1, Windows 2012 R2
* Windows 10, Windows 2016

> 由此所见，采用VC-LTL编译后的程序能兼容Windows XP以上所有操作系统。

## 使用方法：
### 1. 将内容解压到工程目录VC-LTL
比如你的Sln文件在 D:\MySln\MySln.sln，这时你把VC-LTL整个目录复制到D:\MySln\VC-LTL即可。


### 2. 重定向C/C++库到VC LTL
我将相关库引用设置封装在了`VC-LTL\ltlvcrt.props`，你可以打开属性管理器（视图 - 属性管理器），然后Release配置上右键`添加现有属性表`，然后选择`VC-LTL\ltlvcrt.props`即可。
PS：如果需要支持XP，那么请选择`VC-LTL\ltlvcrtWinXp.props`。

如果你不希望使用ltlvcrt.props/ltlvcrtWinXp.props属性表，那么可以手动在Release配置中添加以下设置（2.1 ~ 2.4）

#### 2.1 在包含目录（include）的最上方添加以下内容：
* $(SolutionDir)VC-LTL\VC\\$(PlatformToolsetVersion)\include
* $(SolutionDir)VC-LTL\VC\\$(PlatformToolsetVersion)\atlmfc\include
* $(SolutionDir)VC-LTL\ucrt\\$(TargetUniversalCRTVersion)

#### 2.2 在库目录（library）增加：
* $(SolutionDir)VC-LTL\\$(PlatformShortName)

#### 2.3 在 C/C++ - 命令行 增加：
（仅在需要支持XP时才需要做，不需要支持XP直接无视此步骤！）
`/Zc:threadSafeInit-` 禁用线程安全静态初始化，这是编译器本身的BUG，否则Windows XP在全局变量中使用静态变量会导致程序崩溃。

#### 2.4 在链接器 - 输入 - 附加依赖项 增加：
（仅在需要支持XP时才需要做，不需要支持XP直接无视此步骤！）
msvcrt_winxp.obj（32位环境）/msvcrt_win2003.obj（64位环境）

### 3. 在工程属性（Release配置） C++ - 所有选项：
* 【运行库】调整为 【多线程DLL/MD】
* 【目标平台】调整为【Windows 10 10240/14393/15063（推荐）】（从中选择任意SDK版本，但是尽量不要选择14393，因为在不久会删除14393 SDK 支持）

### 4. 重新编译你的应用程序（Release）
现在是不是体积就小了很多。如果你编译不通过，咋们可以一起研究研究，共同改进VC LTL。


### 5. 文件说明
* ltl140.lib/ltl141.lib VC2015/VC2017 编译平台使用到的扩展函数，其内容通过编译_msvcrt.cpp得到
* ltl140xp.lib/ltl141xp.lib VC2015/VC2017 XP模式 编译平台使用到的扩展函数，其内容通过编译_msvcrt.cpp得到
* msvcrt_base.lib Vista msvcrt.dll删除过的导出符号表（从原版中删除了一些无用符号，并且加入了helper.asm转发函数）
* msvcrt_vista.lib Vista msvcrt.dll 原版符号表（从DDK提取）
* ucrt_14393.lib/ucrt_15063.lib 从libucrt.lib提取的一些原有lib不支持的函数集合。
* vc140.lib/vc141.lib 从msvcrt.lib提取的一些原有lib不支持的函数集合。
* vc140xp.lib/vc141xp.lib 从msvcrt.lib提取的一些原有lib不支持的函数集合（支持 XP）。

## Changes：
1.0.0.9 2017-05-26 14:46
* 改进Windows XP支持
* 优化库结构裁剪ltl库体积
* 解决使用自定义异常导致程序编译不通过问题
* 调整*_p系列函数定义，以免使用*_p系列函数时编译不通过
* 解决使用浮点除法时导致编译不通过问题

1.0.0.8 2017-04-25 20:37
* 简化库引用方式
* 更新14393 ucrt到最新版（2017-01-05版）
* 添加15063 ucrt支持

1.0.0.7 2017-04-22 19:26
* 初步添加Windows XP支持
* 添加C++异常支持
* 添加/GS特性支持
* 添加/guard:cf支持
* VC140库升级到Vistual Studio 2015 Update3

1.0.0.6 2017-3-18 13:46
* 优化文件引用

1.0.0.5 2017-3-16 20:53
* 新增VC 2017支持

1.0.0.3 2016-11-28 12:54
* 改进对C工程的兼容性

1.0.0.2 2016-06-14 12:52
* 解决C编译不通过问题

1.0.0.1 13:42 2016/5/23
* 解决C++异常无法使用问题
