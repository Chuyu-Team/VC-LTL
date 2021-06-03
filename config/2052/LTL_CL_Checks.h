
/*

实现一些额外的安全检查，多一层安全保障。

*/

#ifdef _DEBUG
#error "调试版无法使用VC LTL，请切换到Release然后重新编译！"
#endif

#ifdef _DLL
_LTL_PushWarning(1004, "从VC-LTL 4.0 开始MD模式将链接到VC-LTL自身的DLL中，如需静态编译（仅依赖msvcrt.dll），请切换到MT。")
#endif


#ifndef _VC_LTL_Include
#error "VC头文件（VC\X.XX.XXXXX\include）没有被加载，请确保 VC-LTL helper for Visual Studio.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif

#ifndef _UCRT_LTL_Include
#error "ucrt头文件（ucrt\X.XX.XXXXX.X）没有被加载，请确保 VC-LTL helper for Visual Studio.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif // !_UCRT_LTL_Include

#if defined(_LTL_VistaMode)

	#if defined _ATL_XP_TARGETING || defined _USING_V110_SDK71_
	_LTL_PushWarning(1006, "在Vista模式中启用XP兼容将导致程序体积变大，推荐移除 _ATL_XP_TARGETING 以及 _USING_V110_SDK71_宏。")
	#endif

	_LTL_PushNote("进入ltl Vista模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")

#elif defined(_LTL_XPMode)

	#ifdef _DLL
	//VC-LTL默认没有编译XP的动态库，如果需要请自行编译。
	_LTL_PushWarning(1006, "Windows XP模式使用无法正常使用 /MD 编译，建议使用 /MT 编译。")
	#endif

	_LTL_PushNote("进入ltl WinXP兼容模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
#else
	#error 未知模式！
#endif

#ifdef __NO_LTL_LIB
_LTL_PushWarning(1003, "VC-LTL 2.0 开始ltl超越模式已经弃用，此选项将被忽略。")
#endif

#ifdef _NO__LTL_Initialization
_LTL_PushWarning(1003, "VC-LTL 4.0 开始不在允许禁用VC-LTL初始化，此选项将被忽略。")
#endif //!_NO__LTL_Initialization

#if defined __cplusplus_winrt && _CRT_NTDDI_MIN <= /*NTDDI_WIN8*/0x06020000
_LTL_PushWarning(1005, "VC-LTL为了简化支持，当使用 /ZW 参数时编译的程序将无法支持 Windows 8。")
#endif
