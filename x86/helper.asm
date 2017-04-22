;VC-LTL汇编部分

.model	flat

_TEXT	SEGMENT
extern ?terminate@@YAXXZ:near
;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题

_terminate PROC
call ?terminate@@YAXXZ
ret
_terminate ENDP

_TEXT	ENDS

END