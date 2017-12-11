// xlocale -- class locale exotic char member functions
#include <istream>
#include <locale>
_STD_BEGIN

 #pragma warning(disable: 4786)

 #pragma warning(disable: 4074)
 #pragma init_seg(compiler)

typedef char_traits<char> _Traits;
typedef istreambuf_iterator<char, _Traits> _Initer;
typedef ostreambuf_iterator<char, _Traits> _Outiter;

	// facets associated with C categories
#define ADDFAC(Facet, cat, ptrimp, ptrloc) \
	if ((_CATMASK(Facet::_Getcat()) & cat) == 0) \
		; \
	else if (ptrloc == 0) \
		ptrimp->_Addfac(new Facet(lobj), Facet::id); \
	else \
		ptrimp->_Addfac((locale::facet *)&_USE(*ptrloc, Facet), Facet::id);

// moved from locale to ease subsetting
typedef collate<char> _T5;
typedef messages<char> _T6;
typedef money_get<char, _Initer> _T7;
typedef money_put<char, _Outiter> _T9;
typedef moneypunct<char, false> _T11;
typedef moneypunct<char, true> _T12;
typedef time_get<char, _Initer> _T13;
typedef time_put<char, _Outiter> _T14;


void __CLRCALL_OR_CDECL locale::_Locimp::_Makexloc(const _Locinfo& lobj,
	locale::category cat, _Locimp *ptrimp, const locale *ptrloc)
	{	// setup exotic char part of a new locale
	ADDFAC(_T5, cat, ptrimp, ptrloc);
	ADDFAC(_T6, cat, ptrimp, ptrloc);
	ADDFAC(_T7, cat, ptrimp, ptrloc);
	ADDFAC(_T9, cat, ptrimp, ptrloc);
	ADDFAC(_T11, cat, ptrimp, ptrloc);
	ADDFAC(_T12, cat, ptrimp, ptrloc);
	ADDFAC(_T13, cat, ptrimp, ptrloc);
	ADDFAC(_T14, cat, ptrimp, ptrloc);
	}
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
