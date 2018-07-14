#include "msvcrt_IAT.h"


#ifdef _ATL_XP_TARGETING

#ifdef _M_IX86
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_seh_longjmp_unwind4_downlevel@4);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_local_unwind4_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_except_handler4_common_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_sse_excpt_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_sse_downlevel);
#elif defined _M_AMD64
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(RTCastToVoid_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(RTDynamicCast_downlevel);
#endif

#endif



//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_msize_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_offset_recalloc_downlevel);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_recalloc_downlevel);
