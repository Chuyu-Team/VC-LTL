

;定义此函数是因为新版本中set_terminate 规范变成了C，此代码用于解决set_terminate 命名冲突问题
extern __imp_?set_terminate@@YAP6AXXZP6AXXZ@Z:qword


.code

set_terminate PROC
jmp __imp_?set_terminate@@YAP6AXXZP6AXXZ@Z
set_terminate ENDP

end