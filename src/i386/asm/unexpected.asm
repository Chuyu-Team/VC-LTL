.model	flat


;定义此函数是因为新版本中unexpected规范变成了C，此代码用于解决unexpected命名冲突问题
extern __imp_?unexpected@@YAXXZ:dword


.code

_unexpected PROC
jmp __imp_?unexpected@@YAXXZ
_unexpected ENDP

end