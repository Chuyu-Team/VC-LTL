.model	flat


;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern __imp_?terminate@@YAXXZ:dword

.code

_terminate PROC
jmp __imp_?terminate@@YAXXZ
_terminate ENDP

end
