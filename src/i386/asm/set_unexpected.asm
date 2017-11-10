.model	flat


;定义此函数是因为新版本中set_unexpected规范变成了C，此代码用于解决set_unexpected命名冲突问题
extern __imp_?set_unexpected@@YAP6AXXZP6AXXZ@Z:dword

.code

_set_unexpected PROC
jmp __imp_?set_unexpected@@YAP6AXXZP6AXXZ@Z
_set_unexpected ENDP

end
