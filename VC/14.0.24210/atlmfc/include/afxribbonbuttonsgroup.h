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
#include "afxbaseribbonelement.h"
#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonButtonsGroup : public CMFCRibbonBaseElement
{
	friend class CMFCRibbonBar;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonButtonsGroup)

// Construction
public:
	CMFCRibbonButtonsGroup();
	CMFCRibbonButtonsGroup(CMFCRibbonBaseElement* pButton);

	virtual ~CMFCRibbonButtonsGroup();

// Attributes
public:
	void SetImages(CMFCToolBarImages* pImages, CMFCToolBarImages* pHotImages, CMFCToolBarImages* pDisabledImages);

	BOOL HasImages() const { return m_Images.GetCount() > 0; }
	const CSize GetImageSize() const;
	int GetCount() const { return (int) m_arButtons.GetSize(); }

	CMFCRibbonBaseElement* GetButton(int i) const
	{
		ASSERT_VALID(m_arButtons [i]);
		return m_arButtons [i];
	}

	int GetButtonIndex(const CMFCRibbonBaseElement* pButton) const;

// Operations
public:
	void AddButton(CMFCRibbonBaseElement* pButton);
	void AddButtons(const CList<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& lstButtons);

	void RemoveAll();

// Overrides
public:
	virtual CSize GetRegularSize(CDC* pDC);

	virtual void OnDrawImage(CDC* pDC, CRect rectImage, CMFCRibbonBaseElement* pButton, int nImageIndex);
	virtual void SetParentCategory(CMFCRibbonCategory* pCategory);

protected:
	virtual void OnDraw(CDC* pDC);
	virtual void OnUpdateCmdUI(CMFCRibbonCmdUI* pCmdUI, CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void OnAfterChangeRect(CDC* pDC);
	virtual void OnShow(BOOL bShow);

	virtual CMFCRibbonBaseElement* HitTest(CPoint point);

	virtual BOOL CanBeStretched() { return FALSE; }
	virtual BOOL IsAlignByColumn() const { return FALSE; }
	virtual BOOL IsQuickAccessToolBar() const { return FALSE; }

	virtual CMFCRibbonBaseElement* Find(const CMFCRibbonBaseElement* pElement);
	virtual CMFCRibbonBaseElement* FindByID(UINT uiCmdID);
	virtual CMFCRibbonBaseElement* FindByData(DWORD_PTR dwData);
	virtual CMFCRibbonBaseElement* FindByOriginal(CMFCRibbonBaseElement* pOriginal);
	virtual CMFCRibbonBaseElement* GetPressed();
	virtual CMFCRibbonBaseElement* GetDroppedDown();
	virtual CMFCRibbonBaseElement* GetHighlighted();
	/// <summary>
	/// Returns a focused element. </summary>
	/// <returns> A pointer to a focused element or NULL.</returns>
	virtual CMFCRibbonBaseElement* GetFocused();

	virtual BOOL ReplaceByID(UINT uiCmdID, CMFCRibbonBaseElement* pElem);
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void SetParentMenu(CMFCRibbonPanelMenuBar* pMenuBar);
	virtual void SetOriginal(CMFCRibbonBaseElement* pOriginal);

	virtual void GetElementsByID(UINT uiCmdID, CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons);
	virtual void GetItemIDsList(CList<UINT,UINT>& lstItems) const;

	virtual int AddToListBox(CMFCRibbonCommandsListBox* pWndListBox, BOOL bDeep);
	virtual void AddToKeyList(CArray<CMFCRibbonKeyTip*,CMFCRibbonKeyTip*>& arElems);

	virtual void OnRTLChanged(BOOL bIsRTL);
	virtual void CleanUpSizes();

	virtual void SetParentRibbonBar(CMFCRibbonBar* pRibbonBar);

	virtual CMFCRibbonBaseElement* GetFirstTabStop();
	virtual CMFCRibbonBaseElement* GetLastTabStop();

	virtual void GetVisibleElements(CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);

// Attributes
protected:
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arButtons;

	CMFCToolBarImages m_Images;
	CMFCToolBarImages m_HotImages;
	CMFCToolBarImages m_DisabledImages;
};

class CMFCRibbonTabsGroup : public CMFCRibbonButtonsGroup  
{
	friend class CMFCRibbonBar;

	DECLARE_DYNCREATE(CMFCRibbonTabsGroup)

	// Construction
public:
	CMFCRibbonTabsGroup();
	CMFCRibbonTabsGroup(CMFCRibbonBaseElement* pButton);

	~CMFCRibbonTabsGroup();

protected:
	// Accessibility:
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
	virtual BOOL OnSetAccData(long lVal);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

public:
	void UpdateTabs(CArray<CMFCRibbonCategory*,CMFCRibbonCategory*>& arCategories);
};


#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
