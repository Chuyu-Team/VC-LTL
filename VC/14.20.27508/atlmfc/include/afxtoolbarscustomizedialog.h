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
#include "afxtoolbarslistpropertypage.h"
#include "afxtoolbarstoolspropertypage.h"
#include "afxtoolbarsmenupropertypage.h"
#include "afxmousepropertypage.h"
#include "afxtoolbarskeyboardpropertypage.h"
#include "afxtoolbarsoptionspropertypage.h"
#include "afxbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarImages;
class CMFCToolBarButton;
class CMFCToolBar;
class CUserTool;

//---------------------
// Customization flags:
//---------------------
#define AFX_CUSTOMIZE_MENU_SHADOWS    0x0001 // Allow chnage menu shadow appearance
#define AFX_CUSTOMIZE_TEXT_LABELS     0x0002 // Allow chnage toolbar text lables below the image
#define AFX_CUSTOMIZE_MENU_ANIMATIONS 0x0004 // Allow chnage menu animations
#define AFX_CUSTOMIZE_NOHELP          0x0008 // Remove help button from the customization dialog
#define AFX_CUSTOMIZE_CONTEXT_HELP    0x0010 // Add '?' to caption and "What's This" context menu
#define AFX_CUSTOMIZE_NOTOOLS         0x0020 // Remove tools page from the customization dialog
#define AFX_CUSTOMIZE_MENUAMPERS      0x0040 // Allow buttons text contain '&'
#define AFX_CUSTOMIZE_NO_LARGE_ICONS  0x0080 // Remove "Large Icons" option from the customization dialog

/*============================================================================*/
// CMFCToolBarsCustomizeDialog
//
// CMFCToolBarsCustomizeDialog is a modeless property sheet that is  created once and not destroyed
// until the application closes.  It is initialized and controlled from CPropertyFrame.

class CMFCToolBarsCustomizeDialog : public CPropertySheet
{
	friend class CMFCToolBar;
	friend class CMFCToolBarsToolsPropertyPage;
	friend class CMFCToolBarsKeyboardPropertyPage;
	friend class CVSToolsListBox;

	DECLARE_DYNAMIC(CMFCToolBarsCustomizeDialog)

// Construction
public:
	CMFCToolBarsCustomizeDialog(CFrameWnd* pWndParentFrame, BOOL bAutoSetFromMenus = FALSE,
		UINT uiFlags = (AFX_CUSTOMIZE_MENU_SHADOWS | AFX_CUSTOMIZE_TEXT_LABELS | AFX_CUSTOMIZE_MENU_ANIMATIONS | AFX_CUSTOMIZE_NOHELP),
		CList <CRuntimeClass*, CRuntimeClass*>* plistCustomPages = NULL);

// Attributes
public:
	UINT GetFlags() const { return m_uiFlags; }

protected:
	//------------------------------------------
	// Toolbar+menu items divided by categories:
	//------------------------------------------
	CMap<CString, LPCTSTR, CObList*, CObList*> m_ButtonsByCategory;
	CStringList m_strCategoriesList; // Need for order!
	//----------------
	// Property pages:
	//----------------
	CMFCToolBarsCommandsPropertyPage* m_pCustomizePage;
	CMFCToolBarsListPropertyPage*  m_pToolbarsPage;
	CMFCToolBarsKeyboardPropertyPage*  m_pKeyboardPage;
	CMFCToolBarsMenuPropertyPage*      m_pMenuPage;
	CMFCMousePropertyPage*     m_pMousePage;
	CMFCToolBarsOptionsPropertyPage*   m_pOptionsPage;
	CMFCToolBarsToolsPropertyPage*     m_pToolsPage;
	
	//-------------------------------------
	// Support for additional custom pages:
	//-------------------------------------
	CList<CPropertyPage*,CPropertyPage*> m_listCustomPages;
	CString    m_strAllCommands;
	CFrameWnd* m_pParentFrame;
	BOOL m_bAutoSetFromMenus;
	UINT m_uiFlags;
	BOOL m_bSaveMenuAmps;
	CMFCButton m_btnHelp;
	UINT m_nPaneMenuEntryID;

// Operations
public:

	void AddButton(UINT uiCategoryId, const CMFCToolBarButton& button, int iInsertBefore = -1);
	void AddButton(LPCTSTR lpszCategory, const CMFCToolBarButton& button, int iInsertBefore = -1);

	int RemoveButton(UINT uiCategoryId, UINT uiCmdId);
	int RemoveButton(LPCTSTR lpszCategory, UINT uiCmdId);

	void ReplaceButton(UINT uiCmd, const CMFCToolBarButton& button);

	BOOL AddToolBar(UINT uiCategoryId, UINT uiToolbarResId);
	BOOL AddToolBar(LPCTSTR lpszCategory, UINT uiToolbarResId);
	BOOL AddMenu(UINT uiMenuResId);
	void AddMenuCommands(const CMenu* pMenu, BOOL bPopup, LPCTSTR lpszCategory = NULL, LPCTSTR lpszMenuPath = NULL /* e.g. "ViewToolbars"*/);

	BOOL RenameCategory(LPCTSTR lpszCategoryOld, LPCTSTR lpszCategoryNew);

	BOOL SetUserCategory(LPCTSTR lpszCategory);

	void EnableUserDefinedToolbars(BOOL bEnable = TRUE);
	void EnableTools(CObList* pToolsList); // List of CUserTool-derived objects

	void FillCategoriesComboBox(CComboBox& wndCategory, BOOL bAddEmpty = TRUE) const;
	void FillCategoriesListBox(CListBox& wndCategory, BOOL bAddEmpty = TRUE) const;
	
	virtual void FillAllCommandsList(CListBox& wndListOfCommands) const;

	LPCTSTR GetCommandName(UINT uiCmd) const;

	int GetCountInCategory(LPCTSTR lpszItemName, const CObList& lstCommands) const; // Return number of items with the same name

protected:
	void SetFrameCustMode(BOOL bCustMode);
	void ShowToolBar(CMFCToolBar* pToolBar, BOOL bShow);
	void SetupFromMenus();
	void AddUserTools(LPCTSTR lpszCategory);

// Overrides
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//------------------
	// Tools page hooks:
	//------------------
	virtual void OnInitToolsPage() {}
	virtual void OnBeforeChangeTool(CUserTool* /*pSelTool*/) {}
	virtual void OnAfterChangeTool(CUserTool* /*pSelTool*/) {}
	virtual BOOL CheckToolsValidity(const CObList& /*lstTools*/) { return TRUE; }

	//---------------------
	// Keyboard page hooks:
	//---------------------
	virtual BOOL OnAssignKey(ACCEL* /*pAccel*/) { return TRUE; }
	virtual BOOL OnRemoveKey(ACCEL* /*pAccel*/) { return TRUE; }

public:
	virtual BOOL Create();
	virtual BOOL OnEditToolbarMenuImage(CWnd* pWndParent, CBitmap& bitmap, int nBitsPerPixel);

// Implementation
public:
	virtual ~CMFCToolBarsCustomizeDialog();
	virtual void PostNcDestroy();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
