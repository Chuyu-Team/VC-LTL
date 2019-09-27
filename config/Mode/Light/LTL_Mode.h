
//VC-LTL 轻量化 模式，此模式中将尽可能的使用msvcrt.dll中的导出函数

#if !defined(_NO_CRT_STDIO_INLINE)
//关闭常用的pintf系列函数的内联操作，充分利用msvcrt现有函数进一步减少体积。
#define _NO_CRT_STDIO_INLINE 1
#endif

#pragma comment(lib, "legacy_stdio_definitions.lib")
