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

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CUserTool : public CObject
{
	friend class CUserToolsManager;

	DECLARE_SERIAL(CUserTool)

public:
// Construction
	CUserTool();
	virtual ~CUserTool();

// Overrides
	virtual void Serialize(CArchive& ar);
	virtual BOOL Invoke();
	virtual HICON SetToolIcon();

// Operations:
public:
	void DrawToolIcon(CDC* pDC, const CRect& rectImage);
	void SetCommand(LPCTSTR lpszCmd);
	BOOL CopyIconToClipboard();

protected:
	virtual HICON LoadDefaultIcon();
	virtual void DeleteIcon();

// Attributes:
public:
	UINT GetCommandId() const
	{
		return m_uiCmdId;
	}

	const CString& GetCommand() const
	{
		return m_strCommand;
	}

	CString m_strLabel;
	CString m_strArguments;
	CString m_strInitialDirectory;

protected:
	UINT    m_uiCmdId;
	CString m_strCommand;
	HICON   m_hIcon;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
