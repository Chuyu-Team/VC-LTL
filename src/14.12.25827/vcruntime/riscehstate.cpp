/***
*riscehstate.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*/

#include <vcruntime_internal.h>
#include <eh.h>
#include <ehassert.h>
#include <ehdata.h>
#include <ehhooks.h>
#include <ehstate.h>
#include <trnsctrl.h>

__ehstate_t RENAME_EH_EXTERN(__StateFromIp)(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC,
    uintptr_t           Ip
) {
    unsigned int    index;          //  loop control variable
    unsigned int    nIPMapEntry;    //  # of IpMapEntry; must be > 0

#if defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

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

#endif

    DASSERT(pFuncInfo != nullptr);
    nIPMapEntry = FUNC_NIPMAPENT(*pFuncInfo);

    DASSERT(FUNC_IPMAP(*pFuncInfo, pDC->ImageBase) != nullptr);

    for (index = 0; index < nIPMapEntry; index++) {
        IptoStateMapEntry    *pIPtoStateMap = FUNC_PIPTOSTATE(*pFuncInfo, index, pDC->ImageBase);
        if( Ip < (uintptr_t)(pDC->ImageBase + pIPtoStateMap->Ip)) {
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

__ehstate_t RENAME_EH_EXTERN(__StateFromControlPc)(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC
) {
    uintptr_t Ip = pDC->ControlPc;

    return RENAME_EH_EXTERN(__StateFromIp)(pFuncInfo, pDC, Ip);
}

//
// This routine is a replacement for the corresponding macro in 'ehdata.h'
//

__ehstate_t RENAME_EH_EXTERN(__GetCurrentState)(
    EHRegistrationNode  *pFrame,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
) {
    if( UNWINDSTATE(*pFrame, FUNC_DISPUNWINDHELP(*pFuncInfo)) == -2 ) {
        return RENAME_EH_EXTERN(__StateFromControlPc)(pFuncInfo, pDC);
    }
    else {
        return UNWINDSTATE(*pFrame, FUNC_DISPUNWINDHELP(*pFuncInfo));
    }
}

VOID RENAME_EH_EXTERN(__SetState)(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    __ehstate_t          newState
){
    UNREFERENCED_PARAMETER(pDC);
    UNWINDSTATE(*pRN, FUNC_DISPUNWINDHELP(*pFuncInfo)) = (int)newState;
}

VOID RENAME_EH_EXTERN(__SetUnwindTryBlock)(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    INT                 curState
){
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *RENAME_EH_EXTERN(_GetEstablisherFrame)(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    if( curState > UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo)) ) {
        UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo)) = (int)curState;
    }
}

INT RENAME_EH_EXTERN(__GetUnwindTryBlock)(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
){
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *RENAME_EH_EXTERN(_GetEstablisherFrame)(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    return UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo));
}
