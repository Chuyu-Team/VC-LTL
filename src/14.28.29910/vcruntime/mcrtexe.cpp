/***
*crtexe.c - Initialization for console EXE using CRT DLL
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is the actual startup routine for apps linking to the CRT DLL.
*       It calls the user's main routine [w]main() or [w]WinMain after
*       performing C Run-Time Library initialization.
*
*       With ifdefs, this source file also provides the source code for:
*       wcrtexe.c   the startup routine for console apps with wide chars
*       crtexew.c   the startup routine for Windows apps
*       wcrtexew.c  the startup routine for Windows apps with wide chars
*
*******************************************************************************/

#ifdef MRTDLL

/*
 * SPECIAL BUILD MACROS! Note that crtexe.c (and crtexew.c) is linked in with
 * the client's code. It does not go into crtdll.dll! Therefore, it must be
 * built under the _DLL switch (like user code) and MRTDLL must be undefined.
 * The symbol SPECIAL_CRTEXE is turned on to suppress the normal CRT DLL
 * definition of _fmode and _commode using __declspec(dllexport).  Otherwise
 * this module would not be able to refer to both the local and DLL versions
 * of these two variables.
 */

#undef  MRTDLL
#ifndef _DLL
#define _DLL
#endif  /* _DLL */

#define SPECIAL_CRTEXE
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <locale.h>
#include <math.h>
#include <new.h>
#include <process.h>
#include <rtcapi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#pragma warning(disable:4439)   // C4439: function with a managed parameter must have a __clrcall calling convention

#if defined (_M_CEE_MIXED)

#pragma managed(push, off)

static int __cdecl _mixed_pre_c_init(void);

#pragma section(".CRT$XIAA",  long, read)
_CRTALLOC(".CRT$XIAA") static _PIFV mixed_pcinit = _mixed_pre_c_init;

#pragma managed(pop)

#endif  /* defined (_M_CEE_MIXED) */

#define SPACECHAR   _T(' ')
#define DQUOTECHAR  _T('\"')

#if !defined (_MANAGED_MAIN)
    #if defined (UNICODE)
        extern "C" int __CxxPureMSILEntry(int, wchar_t **, wchar_t **);
        #define __tinit_cmdline __winit_cmdline
    #else  /* defined (UNICODE) */
        extern "C" int __CxxPureMSILEntry(int, char **, char **);
        #define __tinit_cmdline __init_cmdline
    #endif  /* defined (UNICODE) */
    using System::String;
    using System::Int32;
    static int __clrcall __tinit_cmdline(cli::array<String^> ^arguments);
#else  /* !defined (_MANAGED_MAIN) */
    using System::String;
    using System::Int32;
    int __clrcall main(cli::array<String^>^);
    static void __clrcall __set_managed_app_type(void);
#endif  /* !defined (_MANAGED_MAIN) */
static void __CLRCALL_OR_CDECL _common_init();

#if defined (WPRFLAG)
#define _IMP___WCMDLN   (__p__wcmdln())
#else  /* defined (WPRFLAG) */
#define _IMP___ACMDLN   (__p__acmdln())
#endif  /* defined (WPRFLAG) */

/*
 * Declare/define communal that serves as indicator the default matherr
 * routine is being used.
 */
extern "C"
{
    extern int __scrt_default_matherr;
}

#if defined (_M_CEE_MIXED)

#pragma managed(push, off)

/***
*_mixed_pre_c_init(void)
*
*Purpose:
*       The code in mainCRTStartup that was executed before executing C
*       initializers was shifted in this function. Also this function is the
*       first thing that is executed in c init section.
*
*Entry:
*
*Exit:
*
*******************************************************************************/

#pragma warning(disable : 4792)
static int __cdecl _mixed_pre_c_init(void)
{
    /*
     * Mark this module as an EXE file so that atexit/_onexit
     * will do the right thing when called, including for C++
     * d-tors.
     */
    if (!__scrt_initialize_onexit_tables(__scrt_module_type::exe))
    {
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    if ( !__scrt_default_matherr )
            __setusermatherr(_matherr);

    /*
     * Run the RTC initialization code
     */
#ifdef _RTC
    _RTC_Initialize();
#endif  /* _RTC */

    _common_init();

    return 0;
}

#pragma managed(pop)

#endif  /* defined (_M_CEE_MIXED) */

/***
*mainCRTStartup(void)
*wmainCRTStartup(void)
*WinMainCRTStartup(void)
*wWinMainCRTStartup(void)
*
*Purpose:
*       These routines do the C runtime initialization, call the appropriate
*       user entry function, and handle termination cleanup.  For a managed
*       app, they then return the exit code back to the calling routine, which
*       is the managed startup code.  For an unmanaged app, they call exit and
*       never return.
*
*       Function:               User entry called:
*       mainCRTStartup          main
*       wmainCRTStartup         wmain
*       WinMainCRTStartup       WinMain
*       wWinMainCRTStartup      wWinMain
*
*Entry:
*
*Exit:
*       Managed app: return value from main() et al, or the exception code if
*                 execution was terminated by the __except guarding the call
*                 to main().
*       Unmanaged app: never return.
*
*******************************************************************************/

#if !defined (_MANAGED_MAIN)
#ifdef _WINMAIN_
#ifdef WPRFLAG
  #define _mainCRTStartup wWinMainCRTStartup
#else  /* WPRFLAG */
  #define _mainCRTStartup WinMainCRTStartup
#endif  /* WPRFLAG */
#else  /* _WINMAIN_ */
#ifdef WPRFLAG
  #define _mainCRTStartup wmainCRTStartup
#else  /* WPRFLAG */
  #define _mainCRTStartup mainCRTStartup
#endif  /* WPRFLAG */
#endif  /* _WINMAIN_ */
#else  /* !defined (_MANAGED_MAIN) */
    #define _mainCRTStartup __clrcall mainCRTStartupStrArray
#endif  /* !defined (_MANAGED_MAIN) */

#if !defined (_MANAGED_MAIN) && defined (_WINMAIN_)
int _mainCRTStartup(void)
#else  /* !defined (_MANAGED_MAIN) && defined (_WINMAIN_) */
int _mainCRTStartup(array<String^>^ arguments)
#endif  /* !defined (_MANAGED_MAIN) && defined (_WINMAIN_) */
{
        _TCHAR **envp;

        int mainret;

#ifdef _WINMAIN_
        _TCHAR *lpszCommandLine;
#endif  /* _WINMAIN_ */

        /*
         * Guard the initialization code and the call to user's main, or
         * WinMain, function in a __try/__except statement.
         */

        __try {

#if defined (_M_CEE_PURE)
            _common_init();
#endif  /* defined (_M_CEE_PURE) */

#ifdef WPRFLAG
            _configure_wide_argv(_get_startup_argv_mode());
#else  /* WPRFLAG */
            _configure_narrow_argv(_get_startup_argv_mode());
#endif  /* WPRFLAG */

            _set_new_mode(_get_startup_new_mode());

#if !defined (_MANAGED_MAIN) && !defined (_WINMAIN_)
            /* init the cmdline (_acmdln or _wcmdln) with the array<String^> */
            /* no need to do this if we use managed main or WinMain */
            int const argret = __tinit_cmdline(arguments);
            if (argret < 0)
            {
                abort();
            }
#endif  /* !defined (_MANAGED_MAIN) && !defined (_WINMAIN_) */

#ifdef WPRFLAG
            envp = _get_initial_wide_environment();
#else  /* WPRFLAG */
            envp = _get_initial_narrow_environment();
#endif  /* WPRFLAG */

#if defined (_MANAGED_MAIN)
            mainret = main(arguments);
#else  /* defined (_MANAGED_MAIN) */

#ifdef _WINMAIN_
            /*
             * Skip past program name (first token in command line).
             * Check for and handle quoted program name.
             */
#ifdef WPRFLAG
            lpszCommandLine = _get_wide_winmain_command_line();
#else  /* WPRFLAG */
            lpszCommandLine = _get_narrow_winmain_command_line();
#endif  /* WPRFLAG */

            mainret = _tWinMain(
                       NULL,
                       NULL,
                       lpszCommandLine,
                       __scrt_get_show_window_mode()
                      );
#else  /* _WINMAIN_ */
#ifdef WPRFLAG
            mainret = __CxxPureMSILEntry(__argc, __wargv, envp);
#else
            mainret = __CxxPureMSILEntry(__argc, __argv, envp);
#endif

#endif  /* _WINMAIN_ */

#endif  /* defined (_MANAGED_MAIN) */

        }
        __except ( _seh_filter_exe(GetExceptionCode(), GetExceptionInformation()) )
        {
            /*
             * Should never reach here
             */
            mainret = GetExceptionCode();
        } /* end of try - except */

        return mainret;
}

#if !defined (_MANAGED_MAIN) && !defined (_WINMAIN_)
_CRT_SECURITYCRITICAL_ATTRIBUTE
static int __clrcall __tinit_cmdline(array<String^> ^arguments)
{
    ::System::Text::StringBuilder sb;
    const ::System::Char SPACE_SCHAR = ' ';
    const ::System::Char DQUOTE_SCHAR = '\"';
    const ::System::Char SLASH_SCHAR = '\\';
    const _TCHAR SPACE_TCHAR = _T(' ');
    const _TCHAR DQUOTE_TCHAR = _T('\"');
    const _TCHAR QUESTIONMARK_TCHAR = _T('?');

    /* To add the arguments to the cmdline, we use the same rules used to parse the cmdline,
       from parse_cmdline, in stdargv.c. The rules are:

            2N backslashes + "      ==> N backslashes and begin/end quote
            2N+1 backslashes + "    ==> N backslashes + literal "
            N backslashes           ==> N backslashes

       so here we use the reverse:

            always surround each argument with doublequotes
            a literal " in the argument becomes 1 backslash + literal "
            N backslashes + literal " become 2N+1 backslashes + literal "
            N backslashes at the end of the argument become 2N backslashes (because we have the closing doublequote)
     */
    for each (String ^arg in arguments)
    {
        sb.Append(SPACE_SCHAR); /* insert a space between the args */
        sb.Append(DQUOTE_SCHAR); /* begin with a double quote */
        for (int i = 0; i < arg->Length; ++i)
        {
            if (arg[i] == DQUOTE_SCHAR)
            {
                sb.Append(SLASH_SCHAR);
                sb.Append(DQUOTE_SCHAR);
            }
            else if (arg[i] == SLASH_SCHAR)
            {
                int numSlash = 0;
                while (i < arg->Length && arg[i] == SLASH_SCHAR)
                {
                    ++numSlash;
                    ++i;
                }
                if (i < arg->Length)
                {
                    if (arg[i] == DQUOTE_SCHAR)
                    {
                        /* we have a 'literal' double quote preceeded by numSlash backslashes:
                        we will need to append 2 * numSlash backslashes + 1 backslash + 1 double quote */
                        sb.Append(SLASH_SCHAR, (2 * numSlash) + 1);
                        sb.Append(DQUOTE_SCHAR);
                    }
                    else
                    {
                        /* we have a non double quote char (arg[i]) preceeded by numSlash backslashes:
                        we will simply need to append numSlash backslashes + arg[i] */
                        sb.Append(SLASH_SCHAR, numSlash);
                        sb.Append(arg[i]);
                    }
                }
                else
                {
                    /* the string ends with numSlash backslashes: we need to append 2 * numSlash backslashes
                    and then add the final double quote (below) */
                    sb.Append(SLASH_SCHAR, 2 * numSlash);
                    break;
                }
            }
            else
            {
                /* normal char (not a double quote nor a backslash) */
                sb.Append(arg[i]);
            }
        }
        sb.Append(DQUOTE_SCHAR); /* end with a double quote */
    }

    size_t cchArguments = sb.Length;
#ifndef WPRFLAG
    cchArguments *= MB_CUR_MAX; /* for multibyte chars, assume that every char will use the maximum space (for this locale) */
#endif  /* WPRFLAG */
    size_t cmdlineSize =
        1 +             /* begin double quote for argv[0] */
        MAX_PATH +      /* argv[0] */
        1 +             /* end double quote for argv[0] */
        cchArguments +  /* argv[1..(argc - 1)] (contains initial space) */
        1;              /* terminating null */

    _TCHAR *tcmdline = (_TCHAR *)_malloc_crt(cmdlineSize * sizeof(_TCHAR));
    if (tcmdline == NULL)
    {
        return -1;
    }

    _TCHAR *p = tcmdline;

    *p++ = DQUOTE_TCHAR;
    int cch = GetModuleFileName(NULL, p, MAX_PATH);
    p += cch;
    *p++ = DQUOTE_TCHAR;

#ifndef WPRFLAG
    int bytesWritten = -1;
    errno_t saveErrno = errno;
#endif  /* WPRFLAG */
    for (int i = 0; i < sb.Length; ++i)
    {
        wchar_t wc = sb[i];
#ifdef WPRFLAG
        *p++ = wc;
#else  /* WPRFLAG */
        wctomb_s(&bytesWritten, p, cmdlineSize - (p - tcmdline), wc);
        if (bytesWritten == -1)
        {
            *p++ = QUESTIONMARK_TCHAR;
        }
        else
        {
            p += bytesWritten;
        }
#endif  /* WPRFLAG */
    }
    *p = 0;
#ifndef WPRFLAG
    errno = saveErrno;
#endif  /* WPRFLAG */

#ifdef WPRFLAG
    _wcmdln = tcmdline;
#else  /* WPRFLAG */
    _acmdln = tcmdline;
#endif  /* WPRFLAG */

    return 0;
}

#endif  /* !defined (_MANAGED_MAIN) && !defined (_WINMAIN_) */

#if defined (_MANAGED_MAIN)

static void __clrcall __set_managed_app_type(void)
{
        PIMAGE_DOS_HEADER pDOSHeader;
        PIMAGE_NT_HEADERS pPEHeader;
        PIMAGE_OPTIONAL_HEADER pNTHeader;

        pDOSHeader = (PIMAGE_DOS_HEADER)GetModuleHandleW(NULL);
        if ( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE )
            return;

        pPEHeader = (PIMAGE_NT_HEADERS)((char *)pDOSHeader +
                                        pDOSHeader->e_lfanew);
        if ( pPEHeader->Signature != IMAGE_NT_SIGNATURE )
            return;

        pNTHeader = (PIMAGE_OPTIONAL_HEADER)&pPEHeader->OptionalHeader;
        if ( pNTHeader->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC )
            return;

        switch (pNTHeader->Subsystem) {
        case IMAGE_SUBSYSTEM_WINDOWS_CUI:
            _set_app_type(_crt_console_app);
            break;
        default:
            _set_app_type(_crt_gui_app);
        }
}

#endif  /* defined (_MANAGED_MAIN) */

/*
 * This function is the shared initialization between MIXED and PURE.  In Mixed, this function
 * is called during _mixed_pre_c_init(), but in PURE, this function is called during
 * mainCRTStartup.
 */
_CRT_SECURITYCRITICAL_ATTRIBUTE
static void __CLRCALL_OR_CDECL _common_init()
{
        /*
         * Set __acrt_app_type properly
         */
#if defined (_MANAGED_MAIN)
        __set_managed_app_type();
#elif defined (_WINMAIN_)
        _set_app_type(_crt_gui_app);
#else  /* defined (_WINMAIN_) */
        _set_app_type(_crt_console_app);
#endif  /* defined (_WINMAIN_) */

        /*
         * Propagate the _fmode and _commode variables to the DLL
         */
        _set_fmode(_get_startup_file_mode());
        _commode = _get_startup_commit_mode();

        /* Enable per-thread locale if user asked for it */
        _configthreadlocale(_get_startup_thread_locale_mode());
}
#endif  /* MRTDLL */
