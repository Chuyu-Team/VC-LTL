
/*

实现一些额外的安全检查，多一层安全保障。

*/

#ifdef _DEBUG
#error Debug cannot use VC-LTL, please switch to Release and recompile!
#endif

#ifdef _DLL
_LTL_PushWarning(1004, "Starting from VC-LTL 4.0, MD mode will use the DLL provided by VC-LTL. If you need to compile statically (only dependent on msvcrt.dll), please switch to MT.")
#endif


#ifndef _VC_LTL_Include
#error Cannot find the VC header file (VC\X.XX.XXXXX\include), please make sure that the VC-LTL helper for Visual Studio.props property sheet is added to the project, and check the dependency order!
#endif

#ifndef _UCRT_LTL_Include
#error Cannot find the ucrt header file (ucrt\X.XX.XXXXX.X), please make sure that the VC-LTL helper for Visual Studio.props property sheet is added to the project, and check the dependency order!
#endif // !_UCRT_LTL_Include

#if defined(_LTL_VistaMode)

	#if defined _ATL_XP_TARGETING || defined _USING_V110_SDK71_
	_LTL_PushWarning(1006, "Enabling XP compatibility in Vista mode will cause the program to become larger. It's recommended to remove the _ATL_XP_TARGETING and _USING_V110_SDK71_ macros.")
	#endif

	_LTL_PushNote("It's now the Windows Vista mode of VC-LTL. Define _DISABLE_DEPRECATE_LTL_MESSAGE to turn off the information prompt.")

#elif defined(_LTL_XPMode)

	#ifdef _DLL
	//VC-LTL默认没有编译XP的动态库，如果需要请自行编译。
	_LTL_PushWarning(1006, "The /MD option cannot be used normally in Windows XP mode. It's recommended to use the /MT option.")
	#endif

	_LTL_PushNote("It's now the Windows XP compatibility mode of VC-LTL. Define _DISABLE_DEPRECATE_LTL_MESSAGE to turn off the information prompt.")
#else
	#error 未知模式！
#endif

#ifdef __NO_LTL_LIB
_LTL_PushWarning(1003, "Starting from VC-LTL 2.0, the __NO_LTL_LIB macro will be ignored.")
#endif

#ifdef _NO__LTL_Initialization
_LTL_PushWarning(1003, "Starting from VC-LTL 4.0, the _NO__LTL_Initialization macro will be ignored.")
#endif //!_NO__LTL_Initialization

#if defined __cplusplus_winrt && _CRT_NTDDI_MIN <= /*NTDDI_WIN8*/0x06020000
_LTL_PushWarning(1005, "When using the /ZW option, the program will not support Windows 8.")
#endif
