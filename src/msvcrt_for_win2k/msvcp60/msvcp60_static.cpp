
#include "..\ThunksHelper.h"
#include <winapi_thunks.h>

#define _cpp_isleadbyte(c) (__pctype_func()[static_cast<unsigned char>(c)] & _LEADBYTE)

#include "xwctomb.hpp"
#include "_tolower.hpp"
#include "_toupper.hpp"
#include "xmbtowc.hpp"
#include "xcosh.hpp"
#include "xdnorm.hpp"
#include "xfexp.hpp"
#include "xfdtest.hpp"
#include "xfdscale.hpp"
#include "xfdnorm.hpp"
#include "xdtest.hpp"
#include "xexp.hpp"
#include "xdscale.hpp"
#include "xfcosh.hpp"
#include "xfsinh.hpp"
#include "xlcosh.hpp"
#include "xldscale.hpp"
#include "xldtest.hpp"
#include "xlexp.hpp"
#include "xlsinh.hpp"
#include "xsinh.hpp"
#include "xstrcoll.hpp"
#include "xstrxfrm.hpp"
#include "xlpoly.hpp"
#include "xpoly.hpp"

#ifdef _ATL_XP_TARGETING
#include "UCRT/mbrtowc.hpp"
#include "UCRT/wcrtomb.hpp"
#endif

#include "UCRT/wctrans.hpp"
#include "UCRT/wctype.hpp"