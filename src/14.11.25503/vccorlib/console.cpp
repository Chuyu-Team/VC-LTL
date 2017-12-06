//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop


namespace Platform { namespace Details
{
	CPPCLI_FUNC void Console::WriteLine(String^ s)
	{
		printf("%S\r\n", s->Data());
	}

	CPPCLI_FUNC void Console::WriteLine(Object^ o)
	{
		::Platform::String^ s = o->ToString();
		printf("%S\r\n", s->Data());
	}

	CPPCLI_FUNC void Console::WriteLine()
	{
		printf("\r\n");
	}
}} // Platform::Details
