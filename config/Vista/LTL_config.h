#pragma once

//Vista mode

#if defined _ATL_XP_TARGETING || defined _USING_V110_SDK71_
#error "不允许在XP兼容模式包含此头文件。"
#endif

//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#define __ltlversionxp
#define _ACRTXPIMP __declspec(dllimport)
#define _ACRTXPINLINE __declspec(dllimport)
//XP以上系统inline以减少导入数量
#define _ACRTXPIMPINLINE __inline

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "VistaMode")
#endif


#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#pragma message(_ltlfilelen "note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
#endif //_DISABLE_DEPRECATE_LTL_MESSAGE
