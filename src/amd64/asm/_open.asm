

;定义此函数是因为新版本中_open有一份C++版，此代码用于解决_open命名冲突问题
extern __imp__open:qword

.code

?_open@@YAHQEBDHH@Z PROC
jmp __imp__open
?_open@@YAHQEBDHH@Z ENDP

end