// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCCmdUsageCount : public CObject
{
public:
	CMFCCmdUsageCount();
	virtual ~CMFCCmdUsageCount();

	virtual void Serialize(CArchive& ar);

	void AddCmd(UINT uiCmd);
	void Reset();

	UINT GetCount(UINT uiCmd) const;
	BOOL HasEnoughInformation() const;

	BOOL IsFrequentlyUsedCmd(UINT uiCmd) const;

	static BOOL __stdcall SetOptions(UINT nStartCount, UINT nMinUsagePercentage);

protected:
	CMap<UINT, UINT, UINT, UINT> m_CmdUsage;
	UINT                         m_nTotalUsage;

	AFX_IMPORT_DATA static UINT m_nStartCount;
	AFX_IMPORT_DATA static UINT m_nMinUsagePercentage;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
