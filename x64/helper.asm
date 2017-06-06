;VC-LTL amd64汇编部分



_TEXT	SEGMENT

;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern ?terminate@@YAXXZ:qword
terminate PROC
jmp ?terminate@@YAXXZ
terminate ENDP

_TEXT	ENDS

END