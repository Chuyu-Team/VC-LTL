// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxcontrolbarutil.h"
#include "afxusertool.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMouseManager;
class CContextMenuManager;
class CKeyboardManager;
class CUserToolsManager;
class CFrameImpl;
class CMDIFrameWndEx;
class CFrameWndEx;
class COleIPFrameWndEx;
class CShellManager;
class CWinAppEx;
class CTooltipManager;

class CWinAppEx : public CWinApp
{
	friend class CMFCPopupMenu;
	friend class CMFCRibbonRecentFilesList;
	friend class CMFCTasksPane;
	friend class CMFCFileDialog;

	DECLARE_DYNAMIC(CWinAppEx)
public:
	CWinAppEx(BOOL bResourceSmartUpdate = TRUE);
	virtual ~CWinAppEx();

	virtual int ExitInstance();

	LPCTSTR SetRegistryBase(LPCTSTR lpszSectionName = NULL);
	LPCTSTR	GetRegistryBase() { return m_strRegSection; }

	// Saved data version:
	int GetDataVersionMajor() const { return m_iSavedVersionMajor; }
	int GetDataVersionMinor() const { return m_iSavedVersionMinor; }
	int GetDataVersion() const;

	BOOL InitMouseManager();
	BOOL InitContextMenuManager();
	BOOL InitKeyboardManager();
	BOOL InitShellManager();
	BOOL InitTooltipManager();

	BOOL EnableUserTools(const UINT uiCmdToolsDummy, const UINT uiCmdFirst, const UINT uiCmdLast,
		CRuntimeClass* pToolRTC = RUNTIME_CLASS(CUserTool), UINT uArgMenuID = 0, UINT uInitDirMenuID = 0);
	BOOL EnableTearOffMenus(LPCTSTR lpszRegEntry, const UINT uiCmdFirst, const UINT uiCmdLast);

	BOOL IsResourceSmartUpdate() const { return m_bResourceSmartUpdate; }
	void EnableLoadWindowPlacement(BOOL bEnable = TRUE) { m_bLoadWindowPlacement = bEnable; }

	CMouseManager* GetMouseManager();
	CContextMenuManager* GetContextMenuManager();
	CKeyboardManager* GetKeyboardManager();
	CUserToolsManager* GetUserToolsManager();
	CShellManager* GetShellManager();
	CTooltipManager* GetTooltipManager();

	// Call one of these in CMyApp::InitInstance just after ProcessShellCommand() and before pMainFrame->ShowWindow().
	BOOL LoadState(CMDIFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);
	BOOL LoadState(CFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);
	BOOL LoadState(COleIPFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);
	virtual BOOL LoadState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);

	virtual BOOL CleanState(LPCTSTR lpszSectionName = NULL);
	virtual BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);

	BOOL SaveState(CMDIFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);
	BOOL SaveState(CFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);
	BOOL SaveState(COleIPFrameWndEx* pFrame, LPCTSTR lpszSectionName = NULL);

	BOOL IsStateExists(LPCTSTR lpszSectionName /*=NULL*/);

	virtual BOOL OnViewDoubleClick(CWnd* pWnd, int iViewId);
	virtual BOOL ShowPopupMenu(UINT uiMenuResId, const CPoint& point, CWnd* pWnd);

	CString GetRegSectionPath(LPCTSTR szSectionAdd = _T(""));

	// These functions load and store values from the "Custom" subkey
	// To use subkeys of the "Custom" subkey use GetSectionInt() etc. instead
	int GetInt(LPCTSTR lpszEntry, int nDefault = 0);
	CString GetString(LPCTSTR lpszEntry, LPCTSTR lpzDefault = _T(""));
	BOOL GetBinary(LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
	BOOL GetObject(LPCTSTR lpszEntry, CObject& obj);
	BOOL WriteInt(LPCTSTR lpszEntry, int nValue );
	BOOL WriteString(LPCTSTR lpszEntry, LPCTSTR lpszValue );
	BOOL WriteBinary(LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
	BOOL WriteObject(LPCTSTR lpszEntry, CObject& obj);

	// These functions load and store values from a given subkey
	// of the "Custom" subkey. For simpler access you may use GetInt() etc.
	int GetSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nDefault = 0);
	CString GetSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = _T(""));
	BOOL GetSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
	BOOL GetSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj);
	BOOL WriteSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nValue );
	BOOL WriteSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszValue );
	BOOL WriteSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
	BOOL WriteSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj);

	// WinHelp override:
	virtual void OnAppContextHelp(CWnd* pWndControl, const DWORD dwHelpIDArray []);

	// Idle processing override:
	virtual BOOL OnWorkspaceIdle(CWnd* /*pWnd*/) { return FALSE; }

public:
	BOOL m_bLoadUserToolbars;
	BOOL m_bExitingFullScreenMode;

protected:
	friend class CFrameImpl;

	// Overidables for customization
	virtual void OnClosingMainFrame(CFrameImpl* pFrameImpl);
	
	virtual void PreLoadState() {}    // called before anything is loaded
	virtual void LoadCustomState() {} // called after everything is loaded
	virtual void PreSaveState() {}    // called before anything is saved
	virtual void SaveCustomState() {} // called after everything is saved

	virtual BOOL LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd);
	virtual BOOL StoreWindowPlacement(const CRect& rectNormalPosition, int nFflags, int nShowCmd);
	virtual BOOL ReloadWindowPlacement(CFrameWnd* pFrame);

protected:
	CString m_strRegSection;

	BOOL m_bKeyboardManagerAutocreated;
	BOOL m_bContextMenuManagerAutocreated;
	BOOL m_bMouseManagerAutocreated;
	BOOL m_bUserToolsManagerAutoCreated;
	BOOL m_bTearOffManagerAutoCreated;
	BOOL m_bShellManagerAutocreated;
	BOOL m_bTooltipManagerAutocreated;
	BOOL m_bForceDockStateLoad; // Load dock bars state even it's not valid
	BOOL m_bLoadSaveFrameBarsOnly;
	BOOL m_bSaveState;          // Automatically save state when the main frame is closed.
	BOOL m_bForceImageReset;    // Force image reset every time when the frame is loaded
	BOOL m_bLoadWindowPlacement;
	BOOL m_bDeferShowOnFirstWindowPlacementLoad;

	const BOOL m_bResourceSmartUpdate; // Automatic toolbars/menu resource update

	int m_iSavedVersionMajor;
	int m_iSavedVersionMinor;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
