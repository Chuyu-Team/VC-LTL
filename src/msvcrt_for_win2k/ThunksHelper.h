#pragma once

#define _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY

#if defined(_M_IX86)
	#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION) _CRT_CONCATENATE(_imp__, _FUNCTION)
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
	#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION) _CRT_CONCATENATE(__imp_, _FUNCTION)
#else
	 #error "不支持此体系"
#endif

#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION)                                                          \
    extern "C" __declspec(selectany) void const* const _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION) \
        = reinterpret_cast<void const*>(_FUNCTION)


#define _ASSERT_EXPR(expr, msg)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Precondition Validation Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef _DEBUG
    #define _INVALID_PARAMETER(expr) _invalid_parameter(expr, __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else
    #define _INVALID_PARAMETER(expr) _invalid_parameter_noinfo()
#endif

#define _VALIDATE_CLEAR_OSSERR_RETURN(expr, errorcode, retexpr)                \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _doserrno = 0L;                                                    \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(expr, errorcode)                 \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _doserrno = 0L;                                                    \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN(expr, errorcode, retexpr)                             \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_ERRCODE(expr, errorcode)                              \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN_ERRCODE_NOEXC(expr, errorcode)                        \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            errno = errorcode;                                                 \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN_NOERRNO(expr, retexpr)                                \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_NOEXC(expr, errorcode, retexpr)                       \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            errno = errorcode;                                                 \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_VOID(expr, errorcode)                                 \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return;                                                            \
        }                                                                      \
    }
