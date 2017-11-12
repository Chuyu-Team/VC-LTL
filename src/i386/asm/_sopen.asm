.model	flat


;定义此函数是因为新版本中_sopen有一份C++版，此代码用于解决_sopen命名冲突问题
extern __imp___sopen:dword

.code

?_sopen@@YAHQBDHHH@Z PROC
jmp __imp___sopen
?_sopen@@YAHQBDHHH@Z ENDP

end