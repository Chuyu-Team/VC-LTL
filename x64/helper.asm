;VC-LTL汇编部分



_TEXT	SEGMENT
extern ?terminate@@YAXXZ:near
;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题

terminate PROC
call ?terminate@@YAXXZ
ret
terminate ENDP

_TEXT	ENDS

END