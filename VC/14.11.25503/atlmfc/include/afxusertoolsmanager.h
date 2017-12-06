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
#include "afxusertool.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CUserToolsManager : public CObject
{
public:
	//--------------
	// Construction:
	//--------------
	CUserToolsManager();
	CUserToolsManager(	const UINT uiCmdToolsDummy, const UINT uiCmdFirst, const UINT uiCmdLast,
		CRuntimeClass* pToolRTC = RUNTIME_CLASS(CUserTool), UINT uArgMenuID = 0, UINT uInitDirMenuID = 0);
	virtual ~CUserToolsManager();

	//------------
	// Operations:
	//------------
	BOOL LoadState(LPCTSTR lpszProfileName = NULL);
	BOOL SaveState(LPCTSTR lpszProfileName = NULL);

	CUserTool* CreateNewTool();
	BOOL RemoveTool(CUserTool* pTool);
	
	BOOL MoveToolUp(CUserTool* pTool);
	BOOL MoveToolDown(CUserTool* pTool);

	BOOL IsUserToolCmd(UINT uiCmdId) const { return uiCmdId >= m_uiCmdFirst && uiCmdId <= m_uiCmdLast; }
	UINT GetToolsEntryCmd() const { return m_uiCmdToolsDummy; }

	BOOL InvokeTool(UINT uiCmdId);
	CUserTool* FindTool(UINT uiCmdId) const;

	//------------
	// Attributes:
	//------------
public:
	const CObList& GetUserTools() const { return m_lstUserTools; }
	int GetMaxTools() const { return m_uiCmdLast - m_uiCmdFirst + 1; }

	void SetFilter(const CString& strFilter) { m_strFilter = strFilter; }
	const CString& GetFilter() const { return m_strFilter; }

	void SetDefExt(const CString& strDefExt) { m_strDefExt = strDefExt; }
	const CString& GetDefExt() const { return m_strDefExt; }

	UINT GetArgumentsMenuID() const { return m_uiArgumentsMenuID; }
	UINT GetInitialDirMenuID() const { return m_uiInitialDirMenuID; }

protected:
	CObList             m_lstUserTools;       // List of CUserTool-derived objects
	const UINT          m_uiCmdToolsDummy;    // Will be replaced by the tools list
	const UINT          m_uiCmdFirst;         // First user tool command
	const UINT          m_uiCmdLast;          // Last user tool command
	CRuntimeClass*      m_pToolRTC;           // User tool run-time class
	CString             m_strFilter;          // "Add tool" file dilaog filter
	CString             m_strDefExt;          // Default tool file extension
	const UINT          m_uiArgumentsMenuID;  // ID for arguments popup option menu
	const UINT          m_uiInitialDirMenuID; // ID for initial dir popup option menu
};

extern AFX_IMPORT_DATA CUserToolsManager* afxUserToolsManager;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
