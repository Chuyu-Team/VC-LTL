

;定义此函数是因为新版本中_set_se_translator规范变成了C，此代码用于解决_set_se_translator命名冲突问题
extern __imp_?_set_se_translator@@YAP6AXIPEAU_EXCEPTION_POINTERS@@@ZP6AXI0@Z@Z:qword

.code

_set_se_translator PROC
jmp __imp_?_set_se_translator@@YAP6AXIPEAU_EXCEPTION_POINTERS@@@ZP6AXI0@Z@Z
_set_se_translator ENDP

end
