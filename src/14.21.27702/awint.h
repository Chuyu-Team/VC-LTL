#include "..\awint.h"

#undef DYNAMICGETCACHEDFUNCTION

#define DYNAMICGETCACHEDFUNCTION(FunType, FunName, FunPtr)                \
    FunType FunPtr = (FunType)GetProcAddress(GetModuleHandleA("KERNEL32"),#FunName)