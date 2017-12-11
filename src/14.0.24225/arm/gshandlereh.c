/***
*gshandlereh.c - Defines __GSHandlerCheck_EH for X64
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines __GSHandlerCheck_EH, the X64 exception handler for functions
*       with /GS-protected buffers as well as C++ EH constructs.
*******************************************************************************/

#include <winternl.h>
#include <vcwininternls.h>

void
__GSHandlerCheckCommon (
    IN PVOID EstablisherFrame,
    IN PDISPATCHER_CONTEXT DispatcherContext,
    IN PULONG GSHandlerData
    );

EXCEPTION_DISPOSITION
__CxxFrameHandler3 (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    );

/***
*__GSHandlerCheck_EH - Check local security cookie during C++ EH on ARM
*
*Purpose:
*   Functions which have a local security cookie, as well as C++ EH, will
*   register this routine for exception handling.  It checks the validity of
*   the local security cookie during exception dispatching and unwinding.  This
*   helps defeat buffer overrun attacks that trigger an exception to avoid the
*   normal end-of-function cookie check.  If the security check succeeds,
*   control passes to __CxxFrameHandler3 to perform normal C++ EH processing.
*
*   Note that this routine must be statically linked into any module that uses
*   it, since it needs access to the global security cookie of that function's
*   image.
*
*Entry:
*   ExceptionRecord - Supplies a pointer to an exception record.
*   EstablisherFrame - Supplies a pointer to frame of the establisher function.
*   ContextRecord - Supplies a pointer to a context record.
*   DispatcherContext - Supplies a pointer to the exception dispatcher or
*       unwind dispatcher context.
*
*Return:
*   If the security cookie check fails, the process is terminated.  Otherwise,
*   return the result of __CxxFrameHandler3.
*
*******************************************************************************/

EXCEPTION_DISPOSITION
__GSHandlerCheck_EH (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    )
{
    PULONG GSHandlerData;

    //
    // Retrieve a pointer to the start of that part of the handler data used
    // to locate the local security cookie in the local frame.  That is found
    // following the image-relative offset to the FuncInfo table used by
    // __CxxFrameHandler3.
    //

    GSHandlerData = (PULONG)DispatcherContext->HandlerData + 1;

    //
    // Perform the actual cookie check.
    //

    __GSHandlerCheckCommon(
        EstablisherFrame,
        DispatcherContext,
        GSHandlerData
        );

    //
    // If the cookie check succeeds, call the normal C++ EH handler if we're
    // supposed to on this exception pass.
    //

    return __CxxFrameHandler3(
                ExceptionRecord,
                EstablisherFrame,
                ContextRecord,
                DispatcherContext
                );
}
