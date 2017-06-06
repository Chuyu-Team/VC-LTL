;VC-LTL x86汇编部分

.model	flat

_TEXT	SEGMENT

;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern ?terminate@@YAXXZ:dword
_terminate PROC
jmp ?terminate@@YAXXZ
_terminate ENDP

_TEXT	ENDS

END