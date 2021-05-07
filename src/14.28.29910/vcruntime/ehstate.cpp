/***
* ehstate.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*Purpose:
*       Contains state management code for all platforms.
*
*/

#include <vcruntime_internal.h>
#include <eh.h>
#include <ehassert.h>
#include <ehdata.h>
#include <ehdata4.h>
#include <ehhooks.h>
#include <trnsctrl.h>
#include <type_traits>
#include "ehhelpers.h"

#if _EH_RELATIVE_FUNCINFO
#if defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)
static uintptr_t adjustIp(DispatcherContext *pDC, uintptr_t Ip)
{
    //
    // If this context came from an unwind to a call, then the ControlPc points
    // to a return address, which could put us at the start of a neighboring
    // scope. To correct for this, back the PC up by the minimum instruction
    // size to ensure we are in the same scope as the original call opcode.
    //

    if (pDC->ControlPcIsUnwound) {

#if defined(_M_ARM_NT)

        Ip -= 2;

#else

        Ip -= 4;

#endif

    }

    return Ip;
}

#else
static uintptr_t adjustIp(DispatcherContext* /*pDC*/, uintptr_t Ip)
{
    return Ip;
}
#endif

__ehstate_t RENAME_EH_EXTERN(__FrameHandler4)::StateFromIp(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC,
    uintptr_t           Ip
)
{
    unsigned int        index;          //  loop control variable
    unsigned int        nIPMapEntry;    //  # of IpMapEntry; must be > 0

    Ip = adjustIp(pDC, Ip);

    if (pFuncInfo->dispIPtoStateMap == 0)
    {
        return EH_EMPTY_STATE;
    }

    PBYTE buffer = (PBYTE)__RVAtoRealOffset(pDC, pFuncInfo->dispIPtoStateMap);

    nIPMapEntry = FH4::ReadUnsigned(&buffer);

    __ehstate_t prevState = EH_EMPTY_STATE;
    unsigned int funcRelIP = 0;
    for (index = 0; index < nIPMapEntry; index++) {
        // States are delta-encoded relative to start of the function
        funcRelIP += FH4::ReadUnsigned(&buffer);
        if (Ip < __FuncRelToRealOffset(pDC, funcRelIP)) {
            break;
        }
        // States are encoded +1 so as to not encode a negative
        prevState = FH4::ReadUnsigned(&buffer) - 1;
    }

    if (index == 0) {
        // We are at the first entry, could be an error
        return EH_EMPTY_STATE;
    }

    // We over-shot one iteration; return state from the previous slot
    return prevState;
}

__ehstate_t RENAME_EH_EXTERN(__FrameHandler3)::StateFromIp(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC,
    uintptr_t           Ip
)
{

    unsigned int        index;          //  loop control variable
    unsigned int        nIPMapEntry;    //  # of IpMapEntry; must be > 0

    Ip = adjustIp(pDC, Ip);

    _VCRT_VERIFY(pFuncInfo);
    nIPMapEntry = FUNC_NIPMAPENT(*pFuncInfo);

    _VCRT_VERIFY(FUNC_IPMAP(*pFuncInfo, pDC->ImageBase));

    for (index = 0; index < nIPMapEntry; index++) {
        IptoStateMapEntry *pIPtoStateMap = FUNC_PIPTOSTATE(*pFuncInfo, index, pDC->ImageBase);
        if (Ip < (uintptr_t)__RVAtoRealOffset(pDC, pIPtoStateMap->Ip)) {
            break;
        }
    }

    if (index == 0) {
        // We are at the first entry, could be an error

        return EH_EMPTY_STATE;
    }

    // We over-shot one iteration; return state from the previous slot

    return FUNC_IPTOSTATE(*pFuncInfo, index - 1, pDC->ImageBase).State;

}

__ehstate_t RENAME_EH_EXTERN(__FrameHandler3)::StateFromControlPc(
    FuncInfo           *pFuncInfo,
    DispatcherContext  *pDC
)
{
    uintptr_t Ip = pDC->ControlPc;

    return StateFromIp(pFuncInfo, pDC, Ip);
}

__ehstate_t RENAME_EH_EXTERN(__FrameHandler4)::StateFromControlPc(
    FuncInfo           *pFuncInfo,
    DispatcherContext  *pDC
)
{
    uintptr_t Ip = pDC->ControlPc;

    return StateFromIp(pFuncInfo, pDC, Ip);
}

void RENAME_EH_EXTERN(__FrameHandler3)::SetUnwindTryBlock(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    int                 curState
)
{
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *RENAME_EH_EXTERN(__FrameHandler3)::GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    if (curState > UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo))) {
        UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo)) = (int)curState;
    }
}

__ehstate_t RENAME_EH_EXTERN(__FrameHandler3)::GetUnwindTryBlock(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
)
{
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *RENAME_EH_EXTERN(__FrameHandler3)::GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    return UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo));
}

__ehstate_t RENAME_EH_EXTERN(__FrameHandler3)::GetCurrentState(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
)
{
    if (UNWINDSTATE(*pRN, FUNC_DISPUNWINDHELP(*pFuncInfo)) == -2) {
        return RENAME_EH_EXTERN(__FrameHandler3)::StateFromControlPc(pFuncInfo, pDC);
    }
    else {
        return UNWINDSTATE(*pRN, FUNC_DISPUNWINDHELP(*pFuncInfo));
    }
}

void RENAME_EH_EXTERN(__FrameHandler3)::SetState(
    EHRegistrationNode  *pRN,
    FuncInfo            *pFuncInfo,
    __ehstate_t          newState
)
{
    UNWINDSTATE(*pRN, FUNC_DISPUNWINDHELP(*pFuncInfo)) = newState;
}
#else
__ehstate_t RENAME_EH_EXTERN(__FrameHandler3)::GetCurrentState(
    EHRegistrationNode  *pRN,
    DispatcherContext*  /*pDC*/,
    FuncInfo            *pFuncInfo
)
{
    // In the initial implementation, the state is simply stored in the registration node.
    // Added support for byte states when max state <= 128. Note that max state is 1+real max state
    if (pFuncInfo->maxState <= 128)
    {
        return (__ehstate_t)(signed char)((pRN)->state & 0xff);
    }
    else {
        return (pRN)->state;
    }
}

void RENAME_EH_EXTERN(__FrameHandler3)::SetState(
    EHRegistrationNode  *pRN,
    FuncInfo*           /*pFuncInfo*/,
    __ehstate_t         newState
)
{
    pRN->state = newState;
}
#endif
