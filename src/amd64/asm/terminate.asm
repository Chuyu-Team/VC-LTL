

;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern __imp_?terminate@@YAXXZ:qword


.code

terminate PROC
jmp __imp_?terminate@@YAXXZ
terminate ENDP

end