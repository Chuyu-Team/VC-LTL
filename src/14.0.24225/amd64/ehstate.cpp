/***
*ehstate.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
****/

#include <eh.h>
#include <ehassert.h>
#include <ehdata.h>
#include <ehhooks.h>
#include <ehstate.h>
#include <trnsctrl.h>

#include <windows.h>

__ehstate_t __StateFromIp(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC,
    __int64             Ip
) {
    unsigned int    index;          //  loop control variable
    unsigned int    nIPMapEntry;    //  # of IpMapEntry; must be > 0

    DASSERT(pFuncInfo != nullptr);
    nIPMapEntry = FUNC_NIPMAPENT(*pFuncInfo);

    DASSERT(FUNC_IPMAP(*pFuncInfo, pDC->ImageBase) != nullptr);

    for (index = 0; index < nIPMapEntry; index++) {
        IptoStateMapEntry    *pIPtoStateMap = FUNC_PIPTOSTATE(*pFuncInfo, index, pDC->ImageBase);
        if( Ip < (pDC->ImageBase + pIPtoStateMap->Ip) ) {
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

__ehstate_t __StateFromControlPc(
    FuncInfo            *pFuncInfo,
    DispatcherContext   *pDC
) {
    return __StateFromIp(pFuncInfo, pDC, pDC->ControlPc);
}


//
// This routine is a replacement for the corresponding macro in 'ehdata.h'
//

__ehstate_t __GetCurrentState(
    EHRegistrationNode  *pFrame,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
) {
    if( UNWINDSTATE(*pFrame, FUNC_DISPUNWINDHELP(*pFuncInfo)) == -2 ) {
        return __StateFromControlPc(pFuncInfo, pDC);
    }
    else {
        return UNWINDSTATE(*pFrame, FUNC_DISPUNWINDHELP(*pFuncInfo));
    }
}

VOID __SetState(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    __ehstate_t          newState
){
    UNREFERENCED_PARAMETER(pDC);

    UNWINDSTATE(*pRN, FUNC_DISPUNWINDHELP(*pFuncInfo)) = (int)newState;
}

VOID __SetUnwindTryBlock(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    INT                 curState
){
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *_GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    if( curState > UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo)) ) {
        UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo)) = (int)curState;
    }
}

INT __GetUnwindTryBlock(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
){
    EHRegistrationNode EstablisherFramePointers;
    EstablisherFramePointers = *_GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
    return UNWINDTRYBLOCK(EstablisherFramePointers, FUNC_DISPUNWINDHELP(*pFuncInfo));
}
