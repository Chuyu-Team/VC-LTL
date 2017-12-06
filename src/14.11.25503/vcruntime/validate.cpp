/***
*validate.cpp - Routines to validate the data structures.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Routines to validate the Exception Handling data structures.
*
*       Entry points:
*
*       Error reporting:
*       * EHRuntimeError - reports the error with
*         a popup or print to stderr, then quits.
*
*       Pointer validation:
*       * _ValidateRead   - Confirms that a pointer is valid for reading
*       * _ValidateWrite   - Confirms that a pointer is valid for writing
*       * _ValidateExecute - Confirms that a pointer is valid to jump to
*
*       Data structure dumpers:
*       * DumpTypeDescriptor
*       * DumpFuncInfo
*       * DumpThrowInfo
*
*       Execution tracing (only in /DENABLE_EHTRACE builds):
*       * EHTraceOutput
****/

#include <eh.h>
#include <ehassert.h>

#include <Windows.h>

#if defined(DEBUG)

int __cdecl
dprintf( char *format, ... )
{
        static char buffer[512];

        int size = vsprintf_s( buffer, sizeof(buffer), format, (char*)(&format+1) );
        OutputDebugString( buffer );

		return size;
}
#endif

#if defined(DEBUG) && defined(_M_IX86)
//
// dbRNListHead - returns current value of FS:0.
//
// For debugger use only, since debugger doesn't seem to be able to view the
// teb.
//
EHRegistrationNode *dbRNListHead(void)
{
        EHRegistrationNode *pRN;

        __asm {
            mov     eax, dword ptr FS:[0]
            mov     pRN, eax
            }

        return pRN;
}
#endif

#if defined(ENABLE_EHTRACE) && (!defined(_CRT_APP) || defined(_DEBUG))

#include <stdio.h>
#include <stdarg.h>

//
// Current EH tracing depth, stack for saving levels during __finally block
// or __except filter.
//
int __ehtrace_level;
int __ehtrace_level_stack_depth;
int __ehtrace_level_stack[128];

//
// EHTraceOutput - Dump formatted string to OutputDebugString
//
_VCRTIMP void __cdecl EHTraceOutput(const char *format, ...)
{
    va_list arglist;
    char buf[1024];

    sprintf_s(buf, sizeof(buf), "%p ", &format);
    OutputDebugString(buf);

    va_start(arglist, format);
    vsprintf_s(buf, sizeof(buf), format, arglist);

    OutputDebugString(buf);
}

//
// EHTraceIndent - Return string for current EH tracing depth
//
_VCRTIMP const char* __cdecl EHTraceIndent(int level)
{
    static char indentbuf[128 + 1];

    // Reset global level to recover from stack unwinds
    __ehtrace_level = level;

    int depth = max(0, level - 1);
    if (depth > (sizeof(indentbuf) - 1) / 2) {
        depth = (sizeof(indentbuf) - 1) / 2;
    }

    for (int i = 0; i < depth; ++i) {
        indentbuf[2 * i] = '|';
        indentbuf[2 * i + 1] = ' ';
    }
    indentbuf[2 * depth] = '\0';

    return indentbuf;
}

//
// EHTraceFunc - Chop down __FUNCTION__ to simple name
//
_VCRTIMP const char * __cdecl EHTraceFunc(const char *func)
{
    static char namebuf[128];

    const char *p = func + strlen(func) - 1;

    if (*p != ')') {
        // Name already simple (no arg list found)
        return func;
    }

    // Skip backwards past the argument list
    int parendepth = 1;
    while (p > func && parendepth > 0) {
        switch (*--p) {
        case '(':
            --parendepth;
            break;
        case ')':
            ++parendepth;
            break;
        }
    }

    // Find beginning of name
    // NOTE: Won't work for funcs which return func-ptrs
    const char *pEnd = p;
    while (p > func && p[-1] != ' ') {
        --p;
    }

    size_t len = min(pEnd - p, sizeof(namebuf) - 1);
    memcpy(namebuf, p, len);
    namebuf[len] = '\0';

    return namebuf;
}

//
// EHTracePushLevel - Push current trace depth on stack to allow temporary
// resetting of level with __finally block or __except filter.
//
_VCRTIMP void __cdecl EHTracePushLevel(int new_level)
{
    if (__ehtrace_level_stack_depth < sizeof(__ehtrace_level_stack) / sizeof(__ehtrace_level_stack[0])) {
        __ehtrace_level_stack[__ehtrace_level_stack_depth] = __ehtrace_level;
    }
    ++__ehtrace_level_stack_depth;
    __ehtrace_level = new_level;
}

//
// EHTracePopLevel - Pop saved trace depth from stack on completion of
// __finally block or __except filter, and optionally restore global depth.
//

_VCRTIMP void __cdecl EHTracePopLevel(bool restore)
{
    --__ehtrace_level_stack_depth;
    if (restore &&
        __ehtrace_level_stack_depth < sizeof(__ehtrace_level_stack) / sizeof(__ehtrace_level_stack[0]))
    {
        __ehtrace_level = __ehtrace_level_stack[__ehtrace_level_stack_depth];
    }
}

//
// EHTraceExceptFilter - Dump trace info for __except filter.  Trace level must
// have been pushed before entry with EHTracePushLevel, so any functions called
// for the 'expr' argument are dumped at the right level.
//
_VCRTIMP int __cdecl EHTraceExceptFilter(const char *func, int expr)
{
    EHTraceOutput("In   : %s%s: __except filter returns %d (%s)\n",
                  EHTraceIndent(__ehtrace_level), EHTraceFunc(func), expr,
                  expr < 0 ? "EXCEPTION_CONTINUE_EXECUTION" :
                  expr > 0 ? "EXCEPTION_EXECUTE_HANDLER" :
                  "EXCEPTION_CONTINUE_SEARCH");

    EHTracePopLevel(expr <= 0);
    return expr;
}

//
// EHTraceHandlerReturn - Dump trace info for exception handler return
//
_VCRTIMP void __cdecl EHTraceHandlerReturn(const char *func, int level, EXCEPTION_DISPOSITION result)
{
    EHTraceOutput( "Exit : %s%s: Handler returning %d (%s)\n", \
                   EHTraceIndent(level), EHTraceFunc(func), result,
                   result == ExceptionContinueExecution ? "ExceptionContinueExecution" :
                   result == ExceptionContinueSearch ? "ExceptionContinueSearch" :
                   result == ExceptionNestedException ? "ExceptionNestedException" :
                   result == ExceptionCollidedUnwind ? "ExceptionCollidedUnwind" :
                   "unknown" );
}

#endif  /* ENABLE_EHTRACE */
