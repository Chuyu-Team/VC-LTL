.model	flat


;定义此函数是因为新版本中_open有一份C++版，此代码用于解决_open命名冲突问题
extern __imp___open:dword

.code

?_open@@YAHQBDHH@Z PROC
jmp __imp___open
?_open@@YAHQBDHH@Z ENDP

end