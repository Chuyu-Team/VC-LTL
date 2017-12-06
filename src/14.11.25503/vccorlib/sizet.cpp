//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop

namespace Platform
{
	CPPCLI_FUNC SizeT::SizeT(void* pv)
	{
		_value = pv;
	}

	CPPCLI_FUNC SizeT::SizeT(int32 n)
	{
		_value = (void*)n;
	}
}
