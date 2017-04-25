
# VC LTL——编译精巧的程序原来如此简单

VC LTL 是一个开源的第三方修改VC库，大家都可以免费，无条件，甚至是用于商业环境。本库基于VC140/141修改，所以也同样适用Common Public License协议。

但我也希望如果你可以在程序的说明文件中声明下，使用了此库，来使更多人受益。

——mingkuang


## 原理：
使用 VC LTL后可以将程序动态链接到系统自带的msvcrt.dll中，来减少程序体积。一般来说一些C/C++的Win32应用程序都可以使用。但是MFC工程不能使用，因为MFC类库太复杂了。

## 支持平台：
* Vistual Studio 2015
* Vistual Studio 2017


## 使用方法：
1 将内容解压到工程目录VC-LTL

2 修改含目录，在最开始设置为以下内容
* $(SolutionDir)\VC-LTL\VC\$(PlatformToolsetVersion)\include
* $(SolutionDir)\VC-LTL\VC\$(PlatformToolsetVersion)\atlmfc\include
* $(SolutionDir)\VC-LTL\ucrt\$(TargetUniversalCRTVersion)

3 在库目录增加：
* $(SolutionDir)VC-LTL\$(PlatformShortName)


4 在工程属性 C++ - 所有选项：
* 【运行库】调整为 【多线程DLL/MD】
* 【目标平台】调整为【Windows 10 14393】或者【Windows 10 15063】

5 重新编译你的应用程序（Release），是不是体积就小了很多。如果你编译不通过，咋们可以一起研究研究，共同改进VC LTL。


## Changes：
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

1.0.0.6 2016-3-18 13:46
* 优化文件引用

1.0.0.5 2016-3-16 20:53
* 新增VC 2017支持

1.0.0.3 2016-11-28 12:54
* 改进对C工程的兼容性

1.0.0.2 2016-06-14 12:52
* 解决C编译不通过问题

1.0.0.1 13:42 2016/5/23
* 解决C++异常无法使用问题