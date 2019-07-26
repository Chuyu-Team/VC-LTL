// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXCMN_H__
#define __AFXCMN_H__

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#pragma once

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef IMAGE_BITMAP
#define IMAGE_BITMAP 0
#endif

#ifndef HDSIL_NORMAL
#define HDSIL_NORMAL 0
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#include <Uxtheme.h>

#ifndef _RICHEDIT_
	#include <Richedit.h>
#endif
#ifdef __AFXOLE_H__  // only include richole if OLE support is included
	#ifndef _RICHOLE_
		#include <RichOle.h>
		#define _RICHOLE_
	#endif
#else
	struct IRichEditOle;
	struct IRichEditOleCallback;
#endif

#ifdef _AFX_ALL_WARNINGS
#pragma warning(push)
#endif

#pragma warning(disable: 4263 4264)  // base class method is hidden

/////////////////////////////////////////////////////////////////////////////
// AFXCMN - MFC COMCTL32 Control Classes

// Classes declared in this file

//TOOLINFO
	class CToolInfo;

//CObject
	//CCmdTarget;
		//CWnd
			// class CListBox;
				class CDragListBox;
			class CStatusBarCtrl;
			class CListCtrl;
			class CTreeCtrl;
			class CSpinButtonCtrl;
			class CSliderCtrl;
			class CProgressCtrl;
			// class CComboBox;
				class CComboBoxEx;
			class CHeaderCtrl;
			class CHotKeyCtrl;
			class CToolTipCtrl;
			class CTabCtrl;
			class CAnimateCtrl;
			class CToolBarCtrl;
			class CReBarCtrl;
			class CRichEditCtrl;
			class CIPAddressCtrl;
			class CPagerCtrl;
			class CLinkCtrl;

#if defined(UNICODE)
			class CNetAddressCtrl;
#endif

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// CToolInfo

#ifdef _UNICODE
class CToolInfo : public tagTOOLINFOW
#else
class CToolInfo : public tagTOOLINFOA
#endif
{
public:
	TCHAR szText[256];
};

/*============================================================================*/
// CDragListBox

class CDragListBox : public CListBox
{
	DECLARE_DYNAMIC(CDragListBox)

// Constructors
public:
	CDragListBox();

// Attributes

	// find item index at given point
	int ItemFromPt(_In_ CPoint pt, _In_ BOOL bAutoScroll = TRUE) const;

// Operations

#pragma push_macro("DrawInsert")
#undef DrawInsert
	// draws insertion line
	virtual void DrawInsert(_In_ int nItem);
#pragma pop_macro("DrawInsert")

// Overridables

	// Override to respond to beginning of drag event.
	virtual BOOL BeginDrag(_In_ CPoint pt);

	// Overrdie to react to user cancelling drag.
	virtual void CancelDrag(_In_ CPoint pt);

	// Called as user drags. Return constant indicating cursor.
	virtual UINT Dragging(_In_ CPoint pt);

	// Called when user releases mouse button to end drag event.
	virtual void Dropped(_In_ int nSrcIndex, _In_ CPoint pt);

// Implementation
public:
	int m_nLast;
	void DrawSingle(_In_ int nIndex);
	virtual void PreSubclassWindow();
	virtual ~CDragListBox();

protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

/*============================================================================*/
// CStatusBarCtrl

// NOTE: This class must remain a binary-compatible subset
// of CStatusBar. Do not add data members or virtual functions
// directly to this class.
class CStatusBarCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual OK here - CWnd has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CStatusBarCtrl)

// Constructors
public:
	CStatusBarCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes

	// Sets text in a particular pane.
	BOOL SetText(_In_z_ LPCTSTR lpszText, _In_ int nPane, _In_ int nType);

	// Retrieves text from a particular pane.
	CString GetText(_In_ int nPane, _In_ int* pType = NULL) const;
	int GetText(_Pre_notnull_ _Post_z_ LPTSTR lpszText, _In_ int nPane, _Out_ int* pType = NULL) const;

	// Gets the legnth of text in a particular pane without
	// retreiving the actual text.
	int GetTextLength(_In_ int nPane, _Out_ int* pType = NULL) const;

	// Sets size of individual panes within the control.
	BOOL SetParts(_In_ int nParts, _In_ int* pWidths);

	// Returns sizes of each pane within the control.
	int GetParts(_In_ int nParts, _Out_ int* pParts) const;

	// Get sizes of all borders in on control.
	BOOL GetBorders(_Out_ int* pBorders) const;
	BOOL GetBorders(_Out_ int& nHorz, _Out_ int& nVert, _Out_ int& nSpacing) const;

	// Sets the minimum allowable height for the control.
	void SetMinHeight(_In_ int nMin);

	// Sets the control to have one single pane and no subdivisions.
	BOOL SetSimple(_In_ BOOL bSimple = TRUE);

	// Gets the bounding rectangle of a specific pane.
	BOOL GetRect(_In_ int nPane, _Out_ LPRECT lpRect) const;

	// Determines if the control has one single pane.
	BOOL IsSimple() const;

	// Returns tooltip text for the given pane.
	CString GetTipText(_In_ int nPane) const;

	// Sets tooltip text for the given pane.
	void SetTipText(_In_ int nPane, _In_z_ LPCTSTR pszTipText);

	// Sets background color for all panes in the control.
	COLORREF SetBkColor(_In_ COLORREF cr);

	// Sets icon for the given part.
	BOOL SetIcon(_In_ int iPart, _In_ HICON hIcon);

	// Retrieves icon for the given part.
	HICON GetIcon(_In_ int iPart) const;

// Overridables
	// Override to perform owner draw. Control must
	// have the owner draw style.
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	// virtual OK here - ~CWnd is virtual
	virtual ~CStatusBarCtrl();
protected:
	// virtual OK here - CWnd makes this virtual
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

/*============================================================================*/
// CListCtrl

// NOTE: This class must remain a binary-compatible subset
// of CListView. Do not add data members or virtual functions
// directly to this class.
class CListCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual is OK here - CWnd has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CListCtrl)

// Constructors
public:
	CListCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the background color for the control.
	COLORREF GetBkColor() const;

	// Sets background color for the control.
	BOOL SetBkColor(_In_ COLORREF cr);

	// Retrieves the image list associated with the control.
	CImageList* GetImageList(_In_ int nImageList) const;

	// Sets the image list associated with this control.
	CImageList* SetImageList(_In_ CImageList* pImageList, _In_ int nImageList);

	// Retrieves the tool tip control associated with this control.
	CToolTipCtrl* GetToolTips() const;

	// Sets the tool tip control to be used by this control.
	CToolTipCtrl* SetToolTips(_In_ CToolTipCtrl* pWndTip);

	// Retrieves the number of items in the control.
	int GetItemCount() const;

	// Retrieves a description of a particular item in the control.
	BOOL GetItem(_Out_ LVITEM* pItem) const;

	// Sets information to an existing item in the control.
	BOOL SetItem(_In_ const LVITEM* pItem);
	BOOL SetItem(_In_ int nItem, _In_ int nSubItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem,
		_In_ int nImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam);
	BOOL SetItem(_In_ int nItem, _In_ int nSubItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem,
		_In_ int nImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam, _In_ int nIndent);

	// Determines which item attributes are maintained by the application
	// instead of the control itself.
	UINT GetCallbackMask() const;

	// Specifies which item attributes are maintained by the application
	// instead of the control itself.
	BOOL SetCallbackMask(_In_ UINT nMask);

	// Get the next item after nItem matching flags in nFlags.
	int GetNextItem(_In_ int nItem, _In_ int nFlags) const;

	// Gets first item selected in the control and prepares for
	// finding other selected items (if the control has the multiple
	// selection style).
	POSITION GetFirstSelectedItemPosition() const;

	// Finds the next selected item, after a previous call
	// to GetFirstSelectedItemPosition().
	int GetNextSelectedItem(_Inout_ POSITION& pos) const;

	// Retrieves the bounding rectangle for a particular item.
	BOOL GetItemRect(_In_ int nItem, _Out_ LPRECT lpRect, _In_ UINT nCode) const;

	// Find the location of a particular item in the control,
	// relative to the control's client area.
	BOOL SetItemPosition(_In_ int nItem, _In_ POINT pt);
	BOOL GetItemPosition(_In_ int nItem, _Out_ LPPOINT lpPoint) const;

	// Determines the width of a string as displayed in report mode.
	int GetStringWidth(_In_z_ LPCTSTR lpsz) const;

	// Retrieves the edit control associated with the currently edited
	// item in the control.
	CEdit* GetEditControl() const;

	// Retrieves information about a column in a report-mode control.
	BOOL GetColumn(_In_ int nCol, _Out_ LVCOLUMN* pColumn) const;

	// Sets information about a column in a report-mode control.
	BOOL SetColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn);

	// Retrieves the width of a column in a report-mode control.
	int GetColumnWidth(_In_ int nCol) const;

	// Sets the width of a column in a report-mode control.
	BOOL SetColumnWidth(_In_ int nCol, _In_ int cx);

	BOOL GetViewRect(_Out_ LPRECT lpRect) const;

	// Retrieves foreground color of text labels in the control.
	COLORREF GetTextColor() const;

	// Sets foreground color of text labels in the control.
	BOOL SetTextColor(_In_ COLORREF cr);

	// Retrieves background color of text labels in the control.
	COLORREF GetTextBkColor() const;

	// Sets background color of text labels in the control.
	BOOL SetTextBkColor(_In_ COLORREF cr);

	// Retrieves the index of the topmost visible item in the control.
	int GetTopIndex() const;

	// Retrieves the number of items displayed at one time
	// the control's client area.
	int GetCountPerPage() const;

	// Retrieves the current origin of the client area.
	BOOL GetOrigin(_Out_ LPPOINT lpPoint) const;

	// Sets the state of a particular item.
	BOOL SetItemState(_In_ int nItem, _In_ LVITEM* pItem);
	BOOL SetItemState(_In_ int nItem, _In_ UINT nState, _In_ UINT nMask);

	// Retrieves the state of a particular item.
	UINT GetItemState(_In_ int nItem, _In_ UINT nMask) const;

	// Retrieves the text associated with a particular item.
	CString GetItemText(_In_ int nItem, _In_ int nSubItem) const;
	int GetItemText(_In_ int nItem, _In_ int nSubItem, _Out_writes_to_(nLen, return + 1) LPTSTR lpszText, _In_ int nLen) const;

	// Sets the text associated with a particular item.
	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText);

	// Sets the count of items in the control. The control will use
	// this value to preallocate memory for its own storage; you may
	// exceed the item count at any time, but accurate preallocation
	// can help performance.
	void SetItemCount(_In_ int nItems);

	// Sets the data (lParam) associated with a particular item.
	BOOL SetItemData(_In_ int nItem, _In_ DWORD_PTR dwData);

	// Retrieves the data (lParam) associated with a particular item.
	DWORD_PTR GetItemData(_In_ int nItem) const;

	// Retrieves the number of selected items in the control.
	UINT GetSelectedCount() const;

	// Retrieves the spacing between items in the control.
	BOOL GetItemSpacing(_In_ BOOL fSmall, _Out_ int* pnHorzSpacing, _Out_ int* pnVertSpacing) const;

	BOOL SetColumnOrderArray(_In_ int iCount, _In_ LPINT piArray);
	BOOL GetColumnOrderArray(_Out_ LPINT piArray, _In_ int iCount = -1) const;

	// Sets minimum spacing of items in the icon view.
	CSize SetIconSpacing(_In_ CSize size);
	CSize SetIconSpacing(_In_ int cx, _In_ int cy);

	// Retrieves a reference to the header control in
	// a report-mode control.
	CHeaderCtrl* GetHeaderCtrl() const;

	// Retrieves the cursor used over hot items. (Only valid for
	// controls with the LVS_EX_TRACKSELECT style.)
	HCURSOR GetHotCursor() const;

	// Sets cursor to be used over hot items. (Only used in controls
	// with the LVS_EX_TRACKSELECT style.)
	HCURSOR SetHotCursor(_In_ HCURSOR hc);

	BOOL GetSubItemRect(_In_ int iItem, _In_ int iSubItem, _In_ int nArea, _Out_ CRect& ref) const;

	// Retrieves the item currently hot-selected, or -1 if none.
	int GetHotItem() const;

	// Sets the item to be currently hot-selected.
	int SetHotItem(_In_ int iIndex);

	// Retrieves the item with the selection mark, or -1 if none.
	int GetSelectionMark() const;

	// Sets the item with the selection mark.
	int SetSelectionMark(_In_ int iIndex);

	// Retrieves the control-specific extended style bits.
	DWORD GetExtendedStyle() const;

	// Sets the control-specific extended style bits.
	DWORD SetExtendedStyle(_In_ DWORD dwNewStyle);

	// Determines the visual feature of a subitem control under
	// the specified point.
	int SubItemHitTest(_In_ LPLVHITTESTINFO pInfo);

	// Sets up virtual work areas within the control.
	void SetWorkAreas(_In_ int nWorkAreas, _In_ LPRECT lpRect);

	// Updates expected item count for a virtual control.
	BOOL SetItemCountEx(_In_ int iCount, _In_ DWORD dwFlags = LVSICF_NOINVALIDATEALL);

	// Calculates the approximate minimum size required to
	// display the passed number of items.
	CSize ApproximateViewRect(_In_ CSize sz = CSize(-1, -1),
		_In_ int iCount = -1) const;

	// Retrieves information about the background image in the control.
	BOOL GetBkImage(_Out_ LVBKIMAGE* plvbkImage) const;

	// Retrieves the delay (in milliseconds) for the mouse to hover
	// over an item before it is selected.
	DWORD GetHoverTime() const;

	// Retrieves the rectangles defining the workareas in the control.
	void GetWorkAreas(_In_ int nWorkAreas, _Out_ LPRECT prc) const;

	// Sets the image used in the background of the control.
   BOOL SetBkImage(_In_ HBITMAP hbm, _In_ BOOL fTile = TRUE,
		_In_ int xOffsetPercent = 0, _In_ int yOffsetPercent = 0);
   BOOL SetBkImage(_In_z_ LPTSTR pszUrl, _In_ BOOL fTile = TRUE,
		_In_ int xOffsetPercent = 0, _In_ int yOffsetPercent = 0);
   BOOL SetBkImage(_In_ LVBKIMAGE* plvbkImage);

	// Sets the delay (in milliseconds) for the mouse to hover
	// over an item before it is selected.
	DWORD SetHoverTime(_In_ DWORD dwHoverTime = (DWORD)-1);

	// Returns the number of work areas in the control.
	UINT GetNumberOfWorkAreas() const;

	// Retrieves the checked state of a particular item. Only useful
	// on controls with the LVS_EX_CHECKBOXES style.
	BOOL GetCheck(_In_ int nItem) const;

	// Sets the checked state of a particular item. Only useful
	// on controls with the LVS_EX_CHECKBOXES style.
	BOOL SetCheck(_In_ int nItem, _In_ BOOL fCheck = TRUE);

// Operations

	// Adds an item to the control.
	int InsertItem(_In_ const LVITEM* pItem);
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem);
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage);

	// Removes a single item from the control.
	BOOL DeleteItem(_In_ int nItem);

	// Removes all items from the control.
	BOOL DeleteAllItems();

	// Finds an item in the control matching the specified criteria.
	int FindItem(_In_ LVFINDINFO* pFindInfo, _In_ int nStart = -1) const;

	// Determines the visual feature of the control under
	// the specified point.
	int HitTest(_In_ LVHITTESTINFO* pHitTestInfo) const;
	int HitTest(_In_ CPoint pt, _In_opt_ UINT* pFlags = NULL) const;

	// Causes the control to scroll its content so the specified item
	// is completely (or at least partially, depending on the
	// bPartialOK parameter) visible.
	BOOL EnsureVisible(_In_ int nItem, _In_ BOOL bPartialOK);

	// Forces the control to scroll its client area
	// by the specified amount.
	BOOL Scroll(_In_ CSize size);

	// Forces the control to repaint a specific range of items.
	BOOL RedrawItems(_In_ int nFirst, _In_ int nLast);

	// Causes the control to rearrange items within its client area.
	BOOL Arrange(_In_ UINT nCode);

	// Causes the control to enter edit mode on the specified item.
	CEdit* EditLabel(_In_ int nItem);

	// Inserts a column into a report-mode control.
	int InsertColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn);
	int InsertColumn(_In_ int nCol, _In_z_ LPCTSTR lpszColumnHeading,
		_In_ int nFormat = LVCFMT_LEFT, _In_ int nWidth = -1, _In_ int nSubItem = -1);

	// Deletes a column from a report-mode control.
	BOOL DeleteColumn(_In_ int nCol);

	// Creates a drag-time image from a particular item in the control.
	CImageList* CreateDragImage(_In_ int nItem, _In_ LPPOINT lpPoint);

	// Forces the control to repaint a specific item.
	BOOL Update(_In_ int nItem);

	// Call to sort items using a custom comparison function.
	BOOL SortItems(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData);
	BOOL SortItemsEx(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData);

	// Sets the selected column in a report-mode control.
	AFX_ANSI_DEPRECATED void SetSelectedColumn(_In_ int iCol);

	AFX_ANSI_DEPRECATED DWORD SetView(_In_ int iView);
	AFX_ANSI_DEPRECATED DWORD GetView() const;

	// Adds a group to the control.
	AFX_ANSI_DEPRECATED int InsertGroup(_In_ int index, _In_ PLVGROUP pgrp);

	// Sets information about the specified group (by ID) in the control.
	AFX_ANSI_DEPRECATED int SetGroupInfo(_In_ int iGroupId, _In_ PLVGROUP pGroup);

	// Retrieves information for the specified group in the control.
	AFX_ANSI_DEPRECATED int GetGroupInfo(_In_ int iGroupId, _Out_ PLVGROUP pgrp) const;

	// Removes a group from the control.
	AFX_ANSI_DEPRECATED int RemoveGroup(_In_ int iGroupId);

	// Moves a group in the control.
	AFX_ANSI_DEPRECATED void MoveGroup(_In_ int iGroupId, _In_ int toIndex);

	// Moves an item to a different group in the control.
	AFX_ANSI_DEPRECATED void MoveItemToGroup(_In_ int idItemFrom, _In_ int idGroupTo);

	// Sets group metric information to a group in the control.
	AFX_ANSI_DEPRECATED void SetGroupMetrics(_In_ PLVGROUPMETRICS pGroupMetrics);

	// Retrieves group metric information for a group in the control.
	AFX_ANSI_DEPRECATED void GetGroupMetrics(_Out_ PLVGROUPMETRICS pGroupMetrics) const;

	// Enables group view in the control.
	AFX_ANSI_DEPRECATED int EnableGroupView(_In_ BOOL fEnable);

	// Uses an application-defined comparison function to sort groups by ID within the list-view control.
	AFX_ANSI_DEPRECATED BOOL SortGroups(_In_ PFNLVGROUPCOMPARE _pfnGroupCompare, _In_ LPVOID _plv);

	// Inserts a group into an ordered list of groups in the control.
	AFX_ANSI_DEPRECATED void InsertGroupSorted(_In_ PLVINSERTGROUPSORTED pStructInsert);

	// Removes all groups from the control.
	AFX_ANSI_DEPRECATED void RemoveAllGroups();

	// Determines if the control has a group with the specified ID.
	AFX_ANSI_DEPRECATED BOOL HasGroup(_In_ int iGroupId) const;

	// Sets information that the list-view control uses in tile view.
	AFX_ANSI_DEPRECATED BOOL SetTileViewInfo(_In_ PLVTILEVIEWINFO ptvi);

	// Retrieves information about the list-view control in tile view.
	AFX_ANSI_DEPRECATED BOOL GetTileViewInfo(_Out_ PLVTILEVIEWINFO ptvi) const;

	// Sets information for a tile of the list-view control.
	AFX_ANSI_DEPRECATED BOOL SetTileInfo(_In_ PLVTILEINFO pti);

	// Retrieves information about a tile in the list-view control.
	AFX_ANSI_DEPRECATED BOOL GetTileInfo(_Out_ PLVTILEINFO pti) const;

	// Sets the insertion point in the control to the defined position.
	AFX_ANSI_DEPRECATED BOOL SetInsertMark(_In_ LPLVINSERTMARK lvim);

	// Retrieves the position of the insertion point in the control.
	AFX_ANSI_DEPRECATED BOOL GetInsertMark(_Out_ LPLVINSERTMARK lvim) const;

	// Retrieves the insertion point closest to a specified point.
	AFX_ANSI_DEPRECATED int InsertMarkHitTest(_In_ LPPOINT pPoint, _In_ LPLVINSERTMARK lvim) const;

	// Retrieves the rectangle that bounds the insertion point in the control.
	AFX_ANSI_DEPRECATED int GetInsertMarkRect(_Out_ LPRECT pRect) const;

	// Sets the color of the insertion point in the control.
	AFX_ANSI_DEPRECATED COLORREF SetInsertMarkColor(_In_ COLORREF color);

	// Retrieves the color of the insertion point in the control.
	AFX_ANSI_DEPRECATED COLORREF GetInsertMarkColor() const;

	// Sets ToolTip text for the control.
	AFX_ANSI_DEPRECATED BOOL SetInfoTip(_In_ PLVSETINFOTIP plvInfoTip);

	// Retrieves the selected column in a report-mode control.
	AFX_ANSI_DEPRECATED UINT GetSelectedColumn() const;

	// Determines if group view is enabled for the control.
	AFX_ANSI_DEPRECATED BOOL IsGroupViewEnabled() const;

	// Retrieves the color of the border of the list-view control.
	AFX_ANSI_DEPRECATED COLORREF GetOutlineColor() const;

	// Sets the color of the border of the list-view control.
	AFX_ANSI_DEPRECATED COLORREF SetOutlineColor(_In_ COLORREF color);

	// Cancels an item text editing operation in the control.
	AFX_ANSI_DEPRECATED void CancelEditLabel();

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	// REVIEW: Retrieves the string displayed when the list-view is empty.
	CString GetEmptyText() const;

	// REVIEW: Retrieves the rectangle for the specified item in the list-view (by group/item index).
	BOOL GetItemIndexRect(_In_ PLVITEMINDEX pItemIndex, _In_ int iColumn, _In_ int rectType, _Out_ LPRECT pRect) const;

	// REVIEW: Sets state for the specified item in the list-view (by group/item index).
	HRESULT SetItemIndexState(_In_ PLVITEMINDEX pItemIndex, _In_ DWORD dwState, _In_ DWORD dwMask);

	// REVIEW: Retrieves the next item in the list-view (by group/item index) based on the flags.
	BOOL GetNextItemIndex(_In_ PLVITEMINDEX pItemIndex, _In_ int nFlags) const;

	// Retrieves the number of groups in the control.
	int GetGroupCount() const;

	// REVIEW: Retrieves information about the specified group (by index) in the control.
	BOOL GetGroupInfoByIndex(_In_ int iIndex, _Out_ PLVGROUP pGroup) const;

	// Sets state information to the specified group in the control.
	void SetGroupState(_In_ int iGroupId, _In_ DWORD dwMask, _In_ DWORD dwState);

	// Retrieves state information for the specified group in the control.
	UINT GetGroupState(_In_ int iGroupId, _In_ DWORD dwMask) const;

	// REVIEW: Gets the group that has the focus in the control.
	int GetFocusedGroup() const;

	// REVIEW: Gets the rectangle for the specified group in the control.
	BOOL GetGroupRect(_In_ int iGroupId, _Out_ LPRECT lpRect, _In_ int iType = LVGGR_GROUP) const;

	// Maps the index of an item in the control to a unique ID.
	UINT MapIndexToID(_In_ UINT index) const;

	// Maps the unique ID of an item in the control to an index.
	UINT MapIDToIndex(_In_ UINT id) const;

	// Indicates if an item in the list-view control is visible.
	BOOL IsItemVisible(_In_ int index) const;

#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

// Overridables
	// Override to perform owner draw. Control must
	// have the owner draw style.
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	int InsertItem(_In_ UINT nMask, _In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ UINT nState,
		_In_ UINT nStateMask, _In_ int nImage, _In_ LPARAM lParam);
	// virtual OK here - ~CWnd is virtual
	virtual ~CListCtrl();
protected:
	void RemoveImageList(_In_ int nImageList);
	// virtual OK here - CWnd makes this virtual
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
protected:
	afx_msg void OnNcDestroy();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CTreeCtrl

// NOTE: This class must remain a binary-compatible subset
// of CTreeView. Do not add data members or virtual functions
// directly to this class.
class CTreeCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual OK - CWnd has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CTreeCtrl)

// Constructors
public:
	CTreeCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes

	// Retrieves the bounding rectangle for the specified item.
	BOOL GetItemRect(_In_ HTREEITEM hItem, _Out_ LPRECT lpRect, _In_ BOOL bTextOnly) const;

	// Gets the count of items in the control.
	UINT GetCount() const;

	// Returns the level-to-level indentation (in pixels).
	UINT GetIndent() const;

	// Sets the level-to-level indentation (in pixels).
	void SetIndent(_In_ UINT nIndent);

	// Retrieves the image list associated with the control.
	CImageList* GetImageList(_In_ int nImageList) const;

	// Sets the image list associated with this control.
	CImageList* SetImageList(_In_opt_ CImageList* pImageList, _In_ int nImageList);

	// Retrieves the next item having the given relationship with the
	// specified item.
	HTREEITEM GetNextItem(_In_ HTREEITEM hItem, _In_ UINT nCode) const;

	// Retrieves the next child item after the specified item.
	HTREEITEM GetChildItem(_In_ HTREEITEM hItem) const;

	// Retrieves the next sibling item after the specified item.
	HTREEITEM GetNextSiblingItem(_In_ HTREEITEM hItem) const;

	// Retrieves the previous sibling item of the specified item.
	HTREEITEM GetPrevSiblingItem(_In_ HTREEITEM hItem) const;

	// Retrieves the parent of the specified item.
	HTREEITEM GetParentItem(_In_ HTREEITEM hItem) const;

	// Retrieves the topmost visible item in the control.
	HTREEITEM GetFirstVisibleItem() const;

	// Retrieves the next visible item after the specified item.
	HTREEITEM GetNextVisibleItem(_In_ HTREEITEM hItem) const;

	// Retrieves the previous visible item before the specified item.
	HTREEITEM GetPrevVisibleItem(_In_ HTREEITEM hItem) const;

	// Retrieves the last expanded item in the tree. This does not retrieve the last item visible in the tree-view window.
	HTREEITEM GetLastVisibleItem() const;

	// Retrieves the higlighted item, NULL if none.
	HTREEITEM GetSelectedItem() const;

	// Retrieves the currently drop-highlighted item, NULL if none.
	HTREEITEM GetDropHilightItem() const;

	// Retrieves the root item of the control, NULL if none.
	HTREEITEM GetRootItem() const;

	// Retrieves information about a particular item in the control.
	BOOL GetItem(_Out_ TVITEM* pItem) const;

	// Retrieves the text associated with the given item.
	CString GetItemText(_In_ HTREEITEM hItem) const;

	// Retrieves the images associated with the given item.
	BOOL GetItemImage(_In_ HTREEITEM hItem, _Out_ int& nImage,
		_Out_ int& nSelectedImage) const;

	// Retrieves the state of the given item.
	UINT GetItemState(_In_ HTREEITEM hItem, _In_ UINT nStateMask) const;

	// Retrieves the user-supplied data associated with the given item.
	DWORD_PTR GetItemData(_In_ HTREEITEM hItem) const;

	// Sets the state of the an item.
	BOOL SetItem(_In_ TVITEM* pItem);
	BOOL SetItem(_In_ HTREEITEM hItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam);

	BOOL SetItemEx(_In_ HTREEITEM hItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam,
		_In_ UINT uStateEx, _In_opt_ HWND hWnd, _In_ int iExpandedImage);

	// Sets the text of the specified item.
	BOOL SetItemText(_In_ HTREEITEM hItem, _In_z_ LPCTSTR lpszItem);

	// Sets the image on the specified item.
	BOOL SetItemImage(_In_ HTREEITEM hItem, _In_ int nImage, _In_ int nSelectedImage);

	// Sets the state of the specified item.
	BOOL SetItemState(_In_ HTREEITEM hItem, _In_ UINT nState, _In_ UINT nStateMask);

	// Sets the user data on the specified item.
	BOOL SetItemData(_In_ HTREEITEM hItem, _In_ DWORD_PTR dwData);

	// Determines if the specified item has children.
	BOOL ItemHasChildren(_In_ HTREEITEM hItem) const;

	// Retrieves the edit control used to perform in-place editing.
	CEdit* GetEditControl() const;

	// Gets the count of items presently visible in the control.
	UINT GetVisibleCount() const;

	// Retrieves the tool tip control associated with this control.
	CToolTipCtrl* GetToolTips() const;

	// Sets the tool tip control to be used by this control.
	CToolTipCtrl* SetToolTips(_In_ CToolTipCtrl* pWndTip);

	// Retrieves the background colour used throughout the control.
	COLORREF GetBkColor() const;

	// Sets the background color to be used throughout the control.
	COLORREF SetBkColor(_In_ COLORREF clr);

	// Retrieves the height of items in the control.
	SHORT GetItemHeight() const;

	// Sets the height of items in the control.
	SHORT SetItemHeight(_In_ SHORT cyHeight);

	// Retrieves the text color used for all items in the control.
	COLORREF GetTextColor() const;

	// Sets the text color used for all items in the control.
	COLORREF SetTextColor(_In_ COLORREF clr);

	// Sets the insertion mark to the specified item in the control.
	BOOL SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter = TRUE);

	// Gets the checked state of the specified item in the control.
	// (Only useful on a control with the TVS_CHECKBOXES style.)
	BOOL GetCheck(_In_ HTREEITEM hItem) const;

	// Sets the checked state of the specified item in the control.
	// (Only useful on a control with the TVS_CHECKBOXES style.)
	BOOL SetCheck(_In_ HTREEITEM hItem, _In_ BOOL fCheck = TRUE);

	COLORREF GetInsertMarkColor() const;
	COLORREF SetInsertMarkColor(_In_ COLORREF clrNew);

	// Sets the maximum time (in milliseconds) the control will
	// spend smooth scrolling its content.
	UINT SetScrollTime(_In_ UINT uScrollTime);

	// Retrieves the maximum time (in milliseconds) the control will
	// spend smooth scrolling its content.
	UINT GetScrollTime() const;

	COLORREF GetLineColor() const;
	COLORREF SetLineColor(_In_ COLORREF clrNew = CLR_DEFAULT);

#if defined(UNICODE)
	// Maps treeview item id to accessibility identifier.
	UINT MapItemToAccId(_In_ HTREEITEM hItem) const;

	// Maps accessibility identifier id to treeview item.
	HTREEITEM MapAccIdToItem(_In_ UINT uAccId) const;

	// Set autoscrolling rate and delay (?).
	BOOL SetAutoscrollInfo(_In_ UINT uPixelsPerSec, _In_ UINT uUpdateTime);
#endif

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	// Get count of selected items in the tree control.
	UINT GetSelectedCount();

	// Get rectangle for the specified part of the specified item.
	BOOL GetItemPartRect(_In_ HTREEITEM hItem, _In_ TVITEMPART nPart, _Out_ LPRECT lpRect);

	// Get the extended state of the item
	UINT GetItemStateEx(_In_ HTREEITEM hItem) const;

	// Set the extended state of the item
	BOOL SetItemStateEx(_In_ HTREEITEM hItem, _In_ UINT uStateEx);

	// Get the expanded image index for the specified item
	int GetItemExpandedImageIndex(_In_ HTREEITEM hItem) const;

	// Set the expanded image index for the specified item
	BOOL SetItemExpandedImageIndex(_In_ HTREEITEM hItem, _In_ int nIndex);

	// Get extended styles for the tree control.
	DWORD GetExtendedStyle() const;

	// Set extended styles on the tree control.
	DWORD SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles);
#endif //  (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

// Operations

	// Inserts a new item to the control.
	HTREEITEM InsertItem(_In_ LPTVINSERTSTRUCT lpInsertStruct);
	HTREEITEM InsertItem(_In_ UINT nMask, _In_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam,
		_In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(_In_z_ LPCTSTR lpszItem, _In_ HTREEITEM hParent = TVI_ROOT,
		_In_ HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(_In_z_ LPCTSTR lpszItem, _In_ int nImage, _In_ int nSelectedImage,
		_In_ HTREEITEM hParent = TVI_ROOT, _In_ HTREEITEM hInsertAfter = TVI_LAST);

	// Removes the specified item from the control.
	BOOL DeleteItem(_In_ HTREEITEM hItem);

	// Removes all items from the control.
	BOOL DeleteAllItems();

	// Expands the children of the specified item.
	BOOL Expand(_In_ HTREEITEM hItem, _In_ UINT nCode);

	// Selects the specified item.
	BOOL Select(_In_ HTREEITEM hItem, _In_ UINT nCode);

	// Selects the specified item.
	BOOL SelectItem(_In_opt_ HTREEITEM hItem);

	// Selects an item to be the drop target in the control.
	BOOL SelectDropTarget(_In_opt_ HTREEITEM hItem);

	// Draws the specified item as the drop target for the control.
	BOOL SelectSetFirstVisible(_In_ HTREEITEM hItem);

	// Begins editing the label of the specified item.
	CEdit* EditLabel(_In_ HTREEITEM hItem);

	// Determines the visual feature of the control under
	// the specified point.
	HTREEITEM HitTest(_In_ CPoint pt, _In_opt_ UINT* pFlags = NULL) const;
	HTREEITEM HitTest(_In_ TVHITTESTINFO* pHitTestInfo) const;

	// Create a drag image for the specified item.
	CImageList* CreateDragImage(_In_ HTREEITEM hItem);

	// Sorts all children of the specified item.
	BOOL SortChildren(_In_opt_ HTREEITEM hItem);

	// Scrolls the control to ensure the specified item is visible.
	BOOL EnsureVisible(_In_ HTREEITEM hItem);

	// Sorts items in the control using the provided callback function.
	BOOL SortChildrenCB(_In_ LPTVSORTCB pSort);

	// Terminates label editing operation.
	BOOL EndEditLabelNow(_In_ BOOL fCancelWithoutSave);

#if defined(UNICODE)
	// Shows information tooltip on the specified item.
	void ShowInfoTip(_In_ HTREEITEM hItem);
#endif

// Implementation
protected:
	void RemoveImageList(_In_ int nImageList);

public:
	// virtual OK here - ~CWnd is virtual
	virtual ~CTreeCtrl();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CSpinButtonCtrl

class CSpinButtonCtrl : public CWnd
{
	DECLARE_DYNAMIC(CSpinButtonCtrl)

// Constructors
public:
	CSpinButtonCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Sets acceleration information for the up-down control.
	BOOL SetAccel(_In_ int nAccel, _In_ UDACCEL* pAccel);

	// Retrieves acceleration information for the up-down control.
	UINT GetAccel(_In_ int nAccel, _Out_ UDACCEL* pAccel) const;

	// Sets the radix base for the up-down control.
	int SetBase(_In_ int nBase);

	// Retrieves the current radix base for the up-down control.
	UINT GetBase() const;

	// Sets the up-down control's buddy window.
	CWnd* SetBuddy(_In_ CWnd* pWndBuddy);

	// Retrieves the up-down control's current buddy window.
	CWnd* GetBuddy() const;

	// Sets the current position of an up-down control with 16-bit precision.
	int SetPos(_In_ int nPos);

	// Retrieves the current position of an up-down control with 16-bit precision.
	int GetPos() const;

	// Sets the minimum and maximum positions (range) for the up-down control.
	void SetRange(_In_ short nLower, _In_ short nUpper);
	void SetRange32(_In_ int nLower, _In_ int nUpper);

	// Retrieves the minimum and maximum positions (range) for the up-down control.
	DWORD GetRange() const;
	void GetRange(_Out_ int &lower, _Out_ int& upper) const;
	void GetRange32(_Out_ int &lower, _Out_ int& upper) const;

	// Retrieves the current position of the up-down control with 32-bit precision.
	int GetPos32(_Out_opt_ LPBOOL lpbError = NULL) const;

	// Sets the current position of an up-down control with 32-bit precision.
	int SetPos32(_In_ int nPos);

// Implementation
public:
	virtual ~CSpinButtonCtrl();
};

/*============================================================================*/
// CSliderCtrl

class CSliderCtrl : public CWnd
{
	DECLARE_DYNAMIC(CSliderCtrl)

// Constructors
public:
	CSliderCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the number of logical positions the trackbar control's slider
	// moves in response to keyboard input from the up/down arrow keys.
	int GetLineSize() const;

	// Sets the number of logical positions the trackbar control's slider
	// moves in response to keyboard input from the up/down arrow keys.
	int SetLineSize(_In_ int nSize);

	// Retrieves the number of logical positions the trackbar control's slider
	// moves in response to keyboard input from the PgUp/PgDn keys.
	int GetPageSize() const;

	// Sets the number of logical positions the trackbar control's slider
	// moves in response to keyboard input from the PgUp/PgDn keys.
	int SetPageSize(_In_ int nSize);

	// Retrieves the maximum position for the slider in the trackbar control.
	int GetRangeMax() const;

	// Retrieves the minimum position for the slider in the trackbar control.
	int GetRangeMin() const;

	// Retrieves the minimum and maximum positions for the slider in the trackbar control.
	void GetRange(_Out_ int& nMin, _Out_ int& nMax) const;

	// Sets the minimum position for the slider in the trackbar control.
	void SetRangeMin(_In_ int nMin, _In_ BOOL bRedraw = FALSE);

	// Sets the maximum position for the slider in the trackbar control.
	void SetRangeMax(_In_ int nMax, _In_ BOOL bRedraw = FALSE);

	// Sets the minimum and maximum positions for the slider in the trackbar control.
	void SetRange(_In_ int nMin, _In_ int nMax, _In_ BOOL bRedraw = FALSE);

	// Retrieves the starting and ending positions of the selection in the trackbar control.
	void GetSelection(_Out_ int& nMin, _Out_ int& nMax) const;

	// Sets the starting and ending positions of the selection in the trackbar control.
	void SetSelection(_In_ int nMin, _In_ int nMax);

	// Retrieves the bounding rectangle for the trackbar control's channel.
	void GetChannelRect(_Out_ LPRECT lprc) const;

	// Retrieves the bounding rectangle for the slider in the trackbar control.
	void GetThumbRect(_Out_ LPRECT lprc) const;

	// Retrieves the current logical position of the slider in the trackbar control.
	int GetPos() const;

	// Sets the current logical position of the slider in the trackbar control.
	void SetPos(_In_ int nPos);

	// Retrieves the number of tick marks in a trackbar control.
	UINT GetNumTics() const;

	// Retrieves an array of positions of tick marks in the trackbar control.
	DWORD* GetTicArray() const;

	// Retrieves the logical position of the specified tick mark in the trackbar control.
	int GetTic(_In_ int nTic) const;

	// Retrieves the current physical position of the specified tick mark in the trackbar control.
	int GetTicPos(_In_ int nTic) const;

	// Sets a tick mark in the trackbar control at the specified logical position.
	BOOL SetTic(_In_ int nTic);

	// Sets the interval frequency for tick marks in the trackbar control.
	void SetTicFreq(_In_ int nFreq);

	// Retrieves the trackbar control buddy window at the specified location.
	CWnd* GetBuddy(_In_ BOOL fLocation = TRUE) const;

	// Assigns the specified window as the buddy window for the trackbar control.
	CWnd* SetBuddy(_In_ CWnd* pWndBuddy, _In_ BOOL fLocation = TRUE);

	// Retrieves the ToolTip control assigned to the trackbar control.
	CToolTipCtrl* GetToolTips() const;

	// Assigns a ToolTip control to the trackbar control.
	void SetToolTips(_In_ CToolTipCtrl* pWndTip);

	// Positions the ToolTip control used by the trackbar control.
	int SetTipSide(_In_ int nLocation);

	// Sets the length of the slider in the trackbar control.
	void SetThumbLength(_In_ int nLength);

	// Retrieves the length of the slider in the trackbar control.
	int GetThumbLength() const;

// Operations
	// Clears the current selection range in the trackbar control.
	void ClearSel(_In_ BOOL bRedraw = FALSE);

	// Removes the current tick marks from the trackbar control.
	void ClearTics(_In_ BOOL bRedraw = FALSE);

// Implementation
public:
	virtual ~CSliderCtrl();
};

/*============================================================================*/
// CProgressCtrl

class CProgressCtrl : public CWnd
{
	DECLARE_DYNAMIC(CProgressCtrl)

// Constructors
public:
	CProgressCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes

	// Sets range of values for the control. (16-bit limit)
	void SetRange(_In_ short nLower, _In_ short nUpper);

	// Sets range of values for the control. (32-bit limit)
	void SetRange32(_In_ int nLower, _In_ int nUpper);

	// Retrieves range of values for the control. (32-bit limit)
	void GetRange(_Out_ int& nLower, _Out_ int& nUpper) const;

	// Gets the current position within the set range of the control.
	int GetPos() const;

	// Sets the current position within the set range of the control.
	int SetPos(_In_ int nPos);

	// Displaces the current position within the set range of the
	// control by the passed value.
	int OffsetPos(_In_ int nPos);

	// Sets the step by which increments happen with a call to StepIt().
	int SetStep(_In_ int nStep);

	// Sets the control's background color.
	COLORREF SetBkColor(_In_ COLORREF clrNew);

	// Sets the color of the progress indicator bar in the progress bar control.
	COLORREF SetBarColor(_In_ COLORREF clrBar);

#if defined(UNICODE)
	// Sets the progress bar control to marquee mode.
	BOOL SetMarquee(_In_ BOOL fMarqueeMode, _In_ int nInterval);
#endif	// defined(UNICODE)

#if defined(UNICODE)
	// REVIEW: Retrieves the step increment for the progress bar control.
	int GetStep() const;

	// REVIEW: Retrieves the background color of the progress bar control.
	COLORREF GetBkColor() const;

	// REVIEW: Retrieves the color of the progress bar control.
	COLORREF GetBarColor() const;

	// REVIEW: Sets the state of the progress bar.
	int SetState(_In_ int iState);

	// REVIEW: Retrieves the state of the progress bar.
	int GetState() const;
#endif	// defined(UNICODE)

// Operations

	// Steps the control by the value set with SetStep().
	int StepIt();

// Implementation
public:
	virtual ~CProgressCtrl();
};

/*============================================================================*/
// CComboBoxEx

class CComboBoxEx : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxEx)

// Constructors
public:
	CComboBoxEx();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Operations
	using CComboBox::DeleteItem;
	int DeleteItem(_In_ int iIndex);
	BOOL GetItem(_Out_ COMBOBOXEXITEM* pCBItem);
	int InsertItem(_In_ const COMBOBOXEXITEM* pCBItem);
	BOOL SetItem(_In_ const COMBOBOXEXITEM* pCBItem);

// Attributes
	BOOL HasEditChanged();
	DWORD GetExtendedStyle() const;
	DWORD SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles);
	CEdit* GetEditCtrl() const;
	CComboBox* GetComboBoxCtrl() const;
	CImageList* GetImageList() const;
	CImageList* SetImageList(_In_ CImageList* pImageList);

	AFX_ANSI_DEPRECATED HRESULT SetWindowTheme(_In_z_ LPCWSTR pszSubAppName);

	// These functions are supported by the Windows ComboBox control,
	// but not supported by the Windows ComboBoxEx control.

	int Dir(_In_ UINT attr, _In_z_ LPCTSTR lpszWildCard);
	int FindString(_In_ int nIndexStart, _In_z_ LPCTSTR lpszFind) const;
	int AddString(_In_z_ LPCTSTR lpszString);
	BOOL SetEditSel(_In_ int nStartChar, _In_ int nEndChar);
	int InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString);

// Implementation
public:
	virtual ~CComboBoxEx();
};

/*============================================================================*/
// CHeaderCtrl

class CHeaderCtrl : public CWnd
{
	DECLARE_DYNAMIC(CHeaderCtrl)

// Constructors
public:
	CHeaderCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves a count of the items in the header control.
	int GetItemCount() const;

	// Retrieves information about an item in the header control.
	BOOL GetItem(_In_ int nPos, _Inout_ HDITEM* pHeaderItem) const;

	// Sets the attributes of the specified item in the header control.
	BOOL SetItem(_In_ int nPos, _In_ HDITEM* pHeaderItem);

	// Retrieves the image list assigned to the header control.
	CImageList* GetImageList(_In_ int nImageList = HDSIL_NORMAL) const;

	// Assigns an image list to the header control.
	CImageList* SetImageList(_In_ CImageList* pImageList, _In_ int nImageList = HDSIL_NORMAL);

	// Retrieves the bounding rectangle for the specified item in the header control.
	BOOL GetItemRect(_In_ int nIndex, _Out_ LPRECT lpRect) const;

	// Retrieves the current left-to-right order of items in the header control.
	BOOL GetOrderArray(_In_ LPINT piArray, _In_ int iCount) const;

	// Sets the left-to-right order of items in the header control.
	BOOL SetOrderArray(_In_ int iCount, _In_ LPINT piArray);

	// Retrieves an index value for an item based on its order in the header control.
	int OrderToIndex(_In_ int nOrder) const;

	// Determines which header item, if any, is at the specified point.
	int HitTest(_Inout_ LPHDHITTESTINFO pHeaderHitTestInfo);

	// Retrieves the width of the bitmap margin for the header control.
	int GetBitmapMargin() const;

	// Sets the width of the bitmap margin for the header control.
	int SetBitmapMargin(_In_ int nWidth);

#if defined(UNICODE)
	// REVIEW:
	BOOL GetItemDropDownRect(_In_ int iItem, _Out_ LPRECT lpRect) const;

	// REVIEW:
	BOOL GetOverflowRect(_Out_ LPRECT lpRect) const;

	// REVIEW: Gets the item in the header control that has the focus.
	int GetFocusedItem() const;

	// Sets the focus to the specified item in the header control.
	BOOL SetFocusedItem(_In_ int iItem);
#endif // defined(UNICODE)

// Operations
	// Inserts a new item into the header control.
	int InsertItem(_In_ int nPos, _In_ HDITEM* phdi);

	// Deletes an item from a header control.
	BOOL DeleteItem(_In_ int nPos);

	// Retrieves the correct size and position of the header control within the parent window.
	BOOL Layout(_In_ HDLAYOUT* pHeaderLayout);

	// Creates a semi-transparent version of an item's image for use as a dragging image.
	CImageList* CreateDragImage(_In_ int nIndex);

	// Changes the color of a divider between header items to indicate
	// the destination of an external drag-and-drop operation.
	int SetHotDivider(_In_ CPoint pt);
	int SetHotDivider(_In_ int nIndex);

	// Sets the timeout interval between the time a change takes place in the
	// filter attributes and the posting of an HDN_FILTERCHANGE notification.
	int SetFilterChangeTimeout(_In_ DWORD dwTimeOut);

	// Starts editing the filter for the specified column.
	int EditFilter(_In_ int nColumn, _In_ BOOL bDiscardChanges);

	// Clears the filter for the specified column in the header control.
	BOOL ClearFilter(_In_ int nColumn);

	// Clears all filters for all columns in the header control.
	BOOL ClearAllFilters();

// Overridables
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CHeaderCtrl();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);

};

/*============================================================================*/
// CHotKeyCtrl

class CHotKeyCtrl : public CWnd
{
	DECLARE_DYNAMIC(CHotKeyCtrl)

// Constructors
public:
	CHotKeyCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Sets the hot key combination for the hot key control.
	void SetHotKey(_In_ WORD wVirtualKeyCode, _In_ WORD wModifiers);

	// Retrieves the virtual key code and modifier flags of the hot key from the hot key control.
	DWORD GetHotKey() const;

	// Retrieves the virtual key code and modifier flags of the hot key from the hot key control.
	void GetHotKey(_Out_ WORD &wVirtualKeyCode, _Out_ WORD &wModifiers) const;

	// Retrieves a string representation of the hot key code and flags.
	CString GetHotKeyName() const;

// Operations
	// Defines the invalid combinations and the default modifier combination for the hot key control.
	void SetRules(_In_ WORD wInvalidComb, _In_ WORD wModifiers);

	// Retrieves a string representation of the virtual key.
	static CString GetKeyName(_In_ UINT vk, _In_ BOOL fExtended);

// Implementation
public:
	virtual ~CHotKeyCtrl();

};

/*============================================================================*/
// CToolTipCtrl

class CToolTipCtrl : public CWnd
{
	DECLARE_DYNAMIC(CToolTipCtrl)

// Constructors
public:
	CToolTipCtrl();

	// Generic creator
	virtual BOOL Create(_In_ CWnd* pParentWnd, _In_ DWORD dwStyle = 0);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ CWnd* pParentWnd, _In_ DWORD dwStyle = 0, _In_ DWORD dwStyleEx = 0);

// Attributes
	// Retrieves the text for a tool in the ToolTip control .
	void GetText(_Out_ CString& str, _In_ CWnd* pWnd, _In_ UINT_PTR nIDTool = 0) const;

	// Retrieves the information that the ToolTip control maintains about a tool.
	BOOL GetToolInfo(_Out_ CToolInfo& ToolInfo, _In_ CWnd* pWnd, _In_ UINT_PTR nIDTool = 0) const;

	// Sets the information that the ToolTip control maintains about a tool.
	void SetToolInfo(_In_ LPTOOLINFO lpToolInfo);

	// Sets a new bounding rectangle for a tool.
	void SetToolRect(_In_ CWnd* pWnd, _In_ UINT_PTR nIDTool, _In_ LPCRECT lpRect);

	// Retrieves a count of the tools maintained by the ToolTip control.
	int GetToolCount() const;

	// Retrieves the initial, pop-up, or reshow duration currently set for the ToolTip control.
	int GetDelayTime(_In_ DWORD dwDuration) const;

	// Sets the initial, pop-up, or reshow duration for the ToolTip control.
	void SetDelayTime(_In_ DWORD dwDuration, _In_ int iTime);

	// Retrieves the top, left, bottom, and right margins set for the ToolTip window.
	void GetMargin(_Out_ LPRECT lprc) const;

	// Sets the top, left, bottom, and right margins for the ToolTip window.
	void SetMargin(_In_ LPRECT lprc);

	// Retrieves the maximum width for the ToolTip window.
	int GetMaxTipWidth() const;

	// Sets the maximum width for the ToolTip window.
	int SetMaxTipWidth(_In_ int iWidth);

	// Retrieves the background color in the ToolTip window.
	COLORREF GetTipBkColor() const;

	// Sets the background color in the ToolTip window.
	void SetTipBkColor(_In_ COLORREF clr);

	// Retrieves the text color in the ToolTip window.
	COLORREF GetTipTextColor() const;

	// Sets the text color in the ToolTip window.
	void SetTipTextColor(_In_ COLORREF clr);

	// Retrieves the information for the current tool in the ToolTip control.
	BOOL GetCurrentTool(_Out_ LPTOOLINFO lpToolInfo) const;

	// Returns the width and height of the ToolTip control.
	CSize GetBubbleSize(_In_ LPTOOLINFO lpToolInfo) const;

	// Sets the visual style of the ToolTip control.
	AFX_ANSI_DEPRECATED HRESULT SetWindowTheme(_In_z_ LPCWSTR pszSubAppName);

// Operations
	// Activates or deactivates the ToolTip control.
	void Activate(_In_ BOOL bActivate);

	// Registers a tool with the ToolTip control.
	BOOL AddTool(_In_ CWnd* pWnd, _In_ UINT nIDText, _In_opt_ LPCRECT lpRectTool = NULL,
		_In_ UINT_PTR nIDTool = 0);
	BOOL AddTool(_In_ CWnd* pWnd, _In_z_ LPCTSTR lpszText = LPSTR_TEXTCALLBACK,
		_In_opt_ LPCRECT lpRectTool = NULL, _In_ UINT_PTR nIDTool = 0);

	// Removes a tool from the ToolTip control.
	void DelTool(_In_ CWnd* pWnd, _In_ UINT_PTR nIDTool = 0);

	// Tests a point to determine whether it is within the bounding rectangle of
	// the specified tool and, if it is, retrieves information about the tool.
	BOOL HitTest(_In_ CWnd* pWnd, _In_ CPoint pt, _In_ LPTOOLINFO lpToolInfo) const;

	// Passes a mouse message to the ToolTip control for processing.
	void RelayEvent(_In_ LPMSG lpMsg);

	void SetDelayTime(_In_ UINT nDelay);

	// Sets the ToolTip text for a tool.
	void UpdateTipText(_In_z_ LPCTSTR lpszText, _In_ CWnd* pWnd, _In_ UINT_PTR nIDTool = 0);
	void UpdateTipText(_In_ UINT nIDText, _In_ CWnd* pWnd, _In_ UINT_PTR nIDTool = 0);

	// Forces the current tool to be redrawn.
	void Update();

	// Removes the displayed ToolTip window from view.
	void Pop();

	// Calculates a ToolTip control's text display rectangle
	// from its window rectangle, or the ToolTip window rectangle
	// needed to display a specified text display rectangle.
	BOOL AdjustRect(_Out_ LPRECT lprc, _In_ BOOL bLarger = TRUE);

	// Adds a standard icon and title string to the ToolTip.
	BOOL SetTitle(_In_ UINT uIcon, _In_z_ LPCTSTR lpstrTitle);

#if defined(UNICODE)
	// Causes the ToolTip to display at the coordinates of the last mouse message.
	void Popup();

	// Retrieves information concerning the title of a tooltip control.
	void GetTitle(_Out_ PTTGETTITLE pTTGetTitle) const;
#endif

// Implementation
public:
	void FillInToolInfo(_Out_ TOOLINFO& ti, _In_ CWnd* pWnd, _In_ UINT_PTR nIDTool) const;
	virtual ~CToolTipCtrl();
#ifndef _AFXDLL
	virtual BOOL DestroyToolTipCtrl();
#else
	BOOL DestroyToolTipCtrl();
#endif

protected:
	afx_msg LRESULT OnDisableModal(WPARAM, LPARAM);
	afx_msg LRESULT OnWindowFromPoint(WPARAM, LPARAM);
	afx_msg LRESULT OnAddTool(WPARAM, LPARAM);
	afx_msg void OnEnable(BOOL bEnable);

	DECLARE_MESSAGE_MAP()

	CMapStringToPtr m_mapString;

	friend class CWnd;
	friend class CToolBar;
};

/*============================================================================*/
// CTabCtrl

class CTabCtrl : public CWnd
{
	DECLARE_DYNAMIC(CTabCtrl)

// Constructors
public:
	CTabCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the image list associated with the tab control.
	CImageList* GetImageList() const;

	// Assigns an image list to the tab control.
	CImageList* SetImageList(_In_ CImageList* pImageList);

	// Retrieves the number of tabs in the tab control.
	int GetItemCount() const;

	// Retrieves information about the specified tab in the tab control.
	BOOL GetItem(_In_ int nItem, _Out_ TCITEM* pTabCtrlItem) const;

	// Sets some or all attributes of the specified tab in the tab control.
	BOOL SetItem(_In_ int nItem, _In_ TCITEM* pTabCtrlItem);

	// Sets the number of bytes per tab reserved for application-defined data in the tab control.
	BOOL SetItemExtra(_In_ int nBytes);

	// Retrieves the bounding rectangle for the specified tab in the tab control.
	BOOL GetItemRect(_In_ int nItem, _Out_ LPRECT lpRect) const;

	// Determines the currently selected tab in the tab control.
	int GetCurSel() const;

	// Selects the specified tab in the tab control.
	int SetCurSel(_In_ int nItem);

	// Sets the focus to the specified tab in the tab control.
	void SetCurFocus(_In_ int nItem);

	// Sets the width and height of tabs in a fixed-width or owner-drawn tab control.
	CSize SetItemSize(_In_ CSize size);

	// Sets the amount of space (padding) around each tab's icon and label in the tab control.
	void SetPadding(_In_ CSize size);

	// Retrieves the current number of rows of tabs in the tab control.
	int GetRowCount() const;

	// Retrieves the ToolTip control associated with the tab control.
	CToolTipCtrl* GetToolTips() const;

	// Assigns a ToolTip control to the tab control.
	void SetToolTips(_In_ CToolTipCtrl* pWndTip);

	// Returns the index of the tab that has the focus in a tab control.
	int GetCurFocus() const;

	// Sets the minimum width of tabs in the tab control.
	int SetMinTabWidth(_In_ int cx);

	// Retrieves the extended styles that are currently in use for the tab control.
	DWORD GetExtendedStyle() const;

	// Sets the extended styles that the tab control will use.
	DWORD SetExtendedStyle(_In_ DWORD dwNewStyle, _In_ DWORD dwExMask = 0);

	// Retrieves state of a tab in the tab control.
	DWORD GetItemState(_In_ int nItem, _In_ DWORD dwMask) const;

	// Sets state for a tab in the tab control.
	BOOL SetItemState(_In_ int nItem, _In_ DWORD dwMask, _In_ DWORD dwState);

// Operations
	// Inserts a new tab in the tab control.
	LONG InsertItem(_In_ int nItem, _In_ TCITEM* pTabCtrlItem);
	LONG InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem);
	LONG InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage);
	LONG InsertItem(_In_ UINT nMask, _In_ int nItem, _In_z_ LPCTSTR lpszItem,
		_In_ int nImage, _In_ LPARAM lParam);
	LONG InsertItem(_In_ UINT nMask, _In_ int nItem, _In_z_ LPCTSTR lpszItem,
		_In_ int nImage, _In_ LPARAM lParam, _In_ DWORD dwState, _In_ DWORD dwStateMask);

	// Removes a tab from the tab control.
	BOOL DeleteItem(_In_ int nItem);

	// Removes all tabs from the tab control.
	BOOL DeleteAllItems();

	// Calculates the tab control's display area given a window rectangle.
	void AdjustRect(_In_ BOOL bLarger, _Inout_ LPRECT lpRect);

	// Removes an image from the tab control's image list.
	void RemoveImage(_In_ int nImage);

	// Determines which tab, if any, is at a specified screen position.
	int HitTest(_In_ TCHITTESTINFO* pHitTestInfo) const;

	// Resets tabs in the tab control, clearing any that were in the pressed state.
	void DeselectAll(_In_ BOOL fExcludeFocus);

	// Sets the highlight state of a tab in the tab control.
	BOOL HighlightItem(_In_ int idItem, _In_ BOOL fHighlight = TRUE);

// Overridables
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CTabCtrl();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CAnimateCtrl

class CAnimateCtrl : public CWnd
{
	DECLARE_DYNAMIC(CAnimateCtrl)

// Constructors
public:
	CAnimateCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Operations
	// Opens an AVI clip and displays its first frame in the animation control.
	BOOL Open(_In_z_ LPCTSTR lpszFileName);
	BOOL Open(_In_ UINT nID);

	// Plays the AVI clip opened in the animation control.
	BOOL Play(_In_ UINT nFrom, _In_ UINT nTo, _In_ UINT nRep);

	// Stops playing the AVI clip opened in the animation control.
	BOOL Stop();

	// Closes the AVI clip opened in the animation control.
	BOOL Close();

	// Displays the specified frame of the AVI clip opened in the animation control.
	BOOL Seek(_In_ UINT nTo);

#if defined(UNICODE)
	// REVIEW: Determines whether the animation control is playing a clip.
	BOOL IsPlaying() const;
#endif

// Implementation
public:
	virtual ~CAnimateCtrl();
};

/*============================================================================*/
// CToolBarCtrl

struct IDropTarget; // forward declaration

// NOTE: This class must remain a binary-compatible subset
// of CToolBar. Do not add data members or virtual functions
// directly to this class.
class CToolBarCtrl : public CWnd
{
	// DECLARE_DYNAMIC OK here - CWnd already has virtual from DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CToolBarCtrl)

// Construction
public:
	CToolBarCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
public:
	// Determines whether the specified button in the toolbar is enabled.
	BOOL IsButtonEnabled(_In_ int nID) const;

	// Determines whether the specified button in the toolbar is checked.
	BOOL IsButtonChecked(_In_ int nID) const;

	// Determines whether the specified button in the toolbar is pressed.
	BOOL IsButtonPressed(_In_ int nID) const;

	// Determines whether the specified button in the toolbar is hidden.
	BOOL IsButtonHidden(_In_ int nID) const;

	// Determines whether the specified button in the toolbar is indeterminate.
	BOOL IsButtonIndeterminate(_In_ int nID) const;

	// Sets the state for the specified button in the toolbar.
	BOOL SetState(_In_ int nID, _In_ UINT nState);

	// Retrieves the state of the specified button in the toolbar.
	int GetState(_In_ int nID) const;

	// Retrieves information about the specified button in the toolbar.
	BOOL GetButton(_In_ int nIndex, _Out_ LPTBBUTTON lpButton) const;

	// Retrieves a count of the buttons currently in the toolbar control.
	int GetButtonCount() const;

	// Retrieves the bounding rectangle of the specified button in the toolbar (by index).
	BOOL GetItemRect(_In_ int nIndex, _Out_ LPRECT lpRect) const;

	// Retrieves the bounding rectangle for the specified button in the toolbar (by command).
	BOOL GetRect(_In_ int nID, _Out_ LPRECT lpRect) const;

	// Specifies the size of the TBBUTTON structure to the toolbar control.
	void SetButtonStructSize(_In_ int nSize);

	// Retrieves the current width and height of toolbar buttons, in pixels.
	DWORD GetButtonSize() const;

	// Sets the size of the buttons to be added to the toolbar.
	BOOL SetButtonSize(_In_ CSize size);

	// Sets the size of the bitmapped images to be added to the toolbar.
	BOOL SetBitmapSize(_In_ CSize size);

	// Retrieves the ToolTip control associated with the toolbar.
	CToolTipCtrl* GetToolTips() const;

	// Associates a ToolTip control with the toolbar.
	void SetToolTips(_In_ CToolTipCtrl* pTip);

	// Sets the window to which the toolbar control sends notification messages.
	void SetOwner(_In_ CWnd* pWnd);

	// Sets the number of rows of buttons in the toolbar.
	void SetRows(_In_ int nRows, _In_ BOOL bLarger, _In_ LPRECT lpRect);

	// Retrieves the number of rows of buttons in the toolbar.
	int GetRows() const;

	// Sets the command identifier of the specified toolbar button.
	BOOL SetCmdID(_In_ int nIndex, _In_ UINT nID);

	// Retrieves the flags that describe the type of bitmap to be used.
	UINT GetBitmapFlags() const;

	// Retrieves the index of the bitmap associated with the specified button in the toolbar.
	int GetBitmap(_In_ int nID) const;

	// Replaces one bitmap in the toolbar control with another bitmap.
	BOOL ReplaceBitmap(_In_ LPTBREPLACEBITMAP pReplaceBitmap);

	// Changes the bitmap for the specified button in the toolbar control.
	BOOL ChangeBitmap(_In_ int idButton, _In_ int iBitmap);

	// Retrieves the image list that the toolbar control uses to display inactive buttons.
	CImageList* GetDisabledImageList() const;

	// Retrieves the image list that the toolbar control uses to display hot buttons.
	CImageList* GetHotImageList() const;

	// Retrieves the image list that the toolbar control uses to display buttons in their default state
	CImageList* GetImageList() const;

	// Retrieves the styles currently in use for the toolbar control.
	DWORD GetStyle() const;

	// Retrieves the maximum number of text rows that can be displayed on a toolbar button.
	int GetMaxTextRows() const;

	// Determines whether the specified button in the toolbar is highlighted.
	BOOL IsButtonHighlighted(_In_ int nID) const;

	// Sets the minimum and maximum button widths in the toolbar control.
	BOOL SetButtonWidth(_In_ int cxMin, _In_ int cxMax);

	// Sets the image list that the toolbar control will use to display disabled buttons.
	CImageList* SetDisabledImageList(_In_ CImageList* pImageList);

	// Sets the image list that the toolbar control will use to display hot buttons.
	CImageList* SetHotImageList(_In_ CImageList* pImageList);

	// Sets the image list that the toolbar control will use to display buttons in their default state.
	CImageList* SetImageList(_In_ CImageList* pImageList);

	// Retrieves the IDropTarget for the toolbar control.
	HRESULT GetDropTarget(_Out_ IDropTarget** ppDropTarget) const;

	// Sets the indentation for the first button in the toolbar control.
	BOOL SetIndent(_In_ int iIndent);

	// Sets the maximum number of text rows that can be displayed on a toolbar button.
	BOOL SetMaxTextRows(_In_ int iMaxRows);

	// Sets the styles for the toolbar control.
	void SetStyle(_In_ DWORD dwStyle);

	// Retrieves extended information for the specified button in the toolbar.
	int GetButtonInfo(_In_ int nID, _Out_ TBBUTTONINFO* ptbbi) const;

	// Sets the information for the specified button in the toolbar.
	BOOL SetButtonInfo(_In_ int nID, _In_ TBBUTTONINFO* ptbbi);

	// Sets the text drawing flags for the toolbar.
	DWORD SetDrawTextFlags(_In_ DWORD dwMask, _In_ DWORD dwDTFlags);

	// Retrieves the anchor highlight setting for the toolbar.
	BOOL GetAnchorHighlight() const;

	// Sets the anchor highlight setting for the toolbar.
	BOOL SetAnchorHighlight(_In_ BOOL fAnchor = TRUE);

	// Retrieves the index of the hot item in the toolbar.
	int GetHotItem() const;

	// Sets the index of the hot item in the toolbar.
	int SetHotItem(_In_ int nHot);

	// Retrieves the current insertion mark for the toolbar.
	void GetInsertMark(_Out_ TBINSERTMARK* ptbim) const;

	// Sets the current insertion mark for the toolbar.
	void SetInsertMark(_In_ TBINSERTMARK* ptbim);

	// Retrieves the total size of all of the visible buttons and separators in the toolbar.
	BOOL GetMaxSize(_Out_ LPSIZE pSize) const;

	// Retrieves the insertion mark information for a point in the toolbar.
	BOOL InsertMarkHitTest(_In_ LPPOINT ppt, _In_ LPTBINSERTMARK ptbim) const;

	// Retrieves the extended styles for the toolbar control.
	DWORD GetExtendedStyle() const;

	// Sets the extended styles for the toolbar control.
	DWORD SetExtendedStyle(_In_ DWORD dwExStyle);

	// Retrieves the padding for the toolbar control.
	void GetPadding(_Out_ int& nHorzPadding, _Out_ int& nVertPadding) const;

	// Sets the padding for the toolbar control.
	DWORD SetPadding(_In_ int nHorzPadding, _In_ int nVertPadding);

	// Retrieves the color used to draw the insertion mark for the toolbar.
	COLORREF GetInsertMarkColor() const;

	// Sets the color used to draw the insertion mark for the toolbar.
	COLORREF SetInsertMarkColor(_In_ COLORREF clrNew);

	// Retrieves the color scheme information from the toolbar control.
	BOOL GetColorScheme(_Out_ COLORSCHEME* lpcs) const;

	// Sets the color scheme information for the toolbar control.
	void SetColorScheme(_In_ const COLORSCHEME* lpcs);

	// Retrieves the metrics of the toolbar control.
	AFX_ANSI_DEPRECATED void GetMetrics(_Out_ LPTBMETRICS ptbm) const;

	// Sets the metrics of the toolbar control.
	AFX_ANSI_DEPRECATED void SetMetrics(_In_ LPTBMETRICS ptbm);

	// Sets the visual style of the toolbar control.
	AFX_ANSI_DEPRECATED HRESULT SetWindowTheme(_In_z_ LPCWSTR pszSubAppName);

#if defined(UNICODE)
	// REVIEW: Sets the image list that the toolbar control uses to display buttons in a pressed state.
	CImageList* SetPressedImageList(_In_ int iImageID, _In_ CImageList* pImageList);

	// REVIEW: Retrieves the image list that the toolbar control uses to display buttons in a pressed state.
	CImageList* GetPressedImageList() const;
#endif // defined(UNICODE)

// Operations
public:
	// Enables or disables the specified button in the toolbar.
	BOOL EnableButton(_In_ int nID, _In_ BOOL bEnable = TRUE);

	// Checks or unchecks the specified button in the toolbar.
	BOOL CheckButton(_In_ int nID, _In_ BOOL bCheck = TRUE);

	// Presses or releases the specified button in the toolbar.
	BOOL PressButton(_In_ int nID, _In_ BOOL bPress = TRUE);

	// Hides or shows the specified button in the toolbar.
	BOOL HideButton(_In_ int nID, _In_ BOOL bHide = TRUE);

	// Sets or clears the indeterminate state of the specified button in the toolbar.
	BOOL Indeterminate(_In_ int nID, _In_ BOOL bIndeterminate = TRUE);

	// Adds one or more images to the list of button images available for the toolbar control.
	int AddBitmap(_In_ int nNumButtons, _In_ UINT nBitmapID);
	int AddBitmap(_In_ int nNumButtons, _In_ CBitmap* pBitmap);

	// Adds one or more buttons to the toolbar control.
	BOOL AddButtons(_In_ int nNumButtons, _In_ LPTBBUTTON lpButtons);

	// Inserts a button into the toolbar control.
	BOOL InsertButton(_In_ int nIndex, _In_ LPTBBUTTON lpButton);

	// Deletes a button from the toolbar control.
	BOOL DeleteButton(_In_ int nIndex);

	// Retrieves the zero-based index for the button associated with the specified command identifier.
	UINT CommandToIndex(_In_ UINT nID) const;

	// Saves state for the toolbar control.
	void SaveState(_In_ HKEY hKeyRoot, _In_z_ LPCTSTR lpszSubKey,
		_In_z_ LPCTSTR lpszValueName);

	// Restores state for the toolbar control.
	void RestoreState(_In_ HKEY hKeyRoot, _In_z_ LPCTSTR lpszSubKey,
		_In_z_ LPCTSTR lpszValueName);

	// Loads system-defined button images into the toolbar control's image list.
	void LoadImages(_In_ int iBitmapID, _In_ HINSTANCE hinst);

	// Determines the ID of the button that corresponds to the specified accelerator character.
	BOOL MapAccelerator(_In_ TCHAR chAccel, _In_ UINT* pIDBtn);

	// Sets the highlight state of the specified button in the toolbar control.
	BOOL MarkButton(_In_ int nID, _In_ BOOL fHighlight = TRUE);

	// Moves a button from one index to another in the toolbar control.
	BOOL MoveButton(_In_ UINT nOldPos, _In_ UINT nNewPos);

	// Determines where a point lies in the toolbar control..
	int HitTest(_In_ LPPOINT ppt) const;

	// Displays the Customize Toolbar dialog box.
	void Customize();

	// Adds one or more strings to the toolbar control's string pool.
	int AddString(_In_ UINT nStringID);
	int AddStrings(_In_z_ LPCTSTR lpszStrings);

	// Causes the toolbar control to be resized.
	void AutoSize();

	int GetString(_In_ int nString, _Out_writes_to_(cchMaxLen, return + 1) LPTSTR lpstrString, _In_ size_t cchMaxLen) const;
	int GetString(_In_ int nString, _Out_ CString& str) const;

// Implementation
public:
	// virtual OK here: ~CWnd() is already virtual
	virtual ~CToolBarCtrl();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CReBarCtrl

// NOTE: This class must remain a binary-compatible subset
// of CReBar. Do not add data members or virtual functions
// directly to this class.
class CReBarCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual is OK here - CWnd already has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CReBarCtrl)

// Construction
public:
	CReBarCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
public:
	// Retrieves the count of bands currently in the rebar control.
	UINT GetBandCount() const;

	// Retrieves information about the specified band in the rebar control.
	BOOL GetBandInfo(_In_ UINT uBand, _Out_ REBARBANDINFO* prbbi) const;

	// Retrieves the height of the rebar control.
	UINT GetBarHeight() const;

	// Retrieves information about the rebar control and the image list it uses.
	BOOL GetBarInfo(_Out_ REBARINFO* prbi) const;

	// Retrieves the rebar control's default background color.
	COLORREF GetBkColor() const;

	// Retrieves the rebar control's IDropTarget interface pointer.
	IDropTarget* GetDropTarget() const;

	// Retrieves the bounding rectangle for the specified band in the rebar control.
	BOOL GetRect(_In_ UINT uBand, _Out_ LPRECT prc) const;

	// Retrieves the number of rows of bands in the rebar control.
	UINT GetRowCount() const;

	// Retrieves the height of the specified row in the rebar control.
	UINT GetRowHeight(_In_ UINT uRow) const;

	// Retrieves the rebar control's default text color.
	COLORREF GetTextColor() const;

	// Retrieves the ToolTip control associated with the rebar control.
	CToolTipCtrl* GetToolTips() const;

	// Converts the specified band identifier to a band index in the rebar control.
	int IDToIndex(_In_ UINT uBandID) const;

	// Sets characteristics of the specified band in the rebar control.
	BOOL SetBandInfo(_In_ UINT uBand, _In_ REBARBANDINFO* prbbi);

	// Sets the characteristics of the rebar control.
	BOOL SetBarInfo(_In_ REBARINFO* prbi);

	// Sets the rebar control's default background color.
	COLORREF SetBkColor(_In_ COLORREF clr);

	// Sets the rebar control's parent window.
	CWnd* SetOwner(_In_ CWnd* pWnd);

	// Sets the rebar control's default text color.
	COLORREF SetTextColor(_In_ COLORREF clr);

	// Associates the ToolTip control with the rebar control.
	void SetToolTips(_In_ CToolTipCtrl* pToolTip);

	// Retrieves the image list associated with the rebar control.
	CImageList* GetImageList() const;

	// Associates an image list with the rebar control.
	BOOL SetImageList(_In_opt_ CImageList* pImageList);

	// Retrieves the borders of a band in the rebar control.
	void GetBandBorders(_In_ UINT uBand, _Out_ LPRECT prc) const;

	// Retrieves the rebar control's current palette.
	CPalette* GetPalette() const;

	// Sets the rebar control's current palette.
	CPalette* SetPalette(_In_ HPALETTE hPal);

	// Retrieves the color scheme information from the rebar control.
	BOOL GetColorScheme(_Out_ COLORSCHEME* lpcs) const;

	// Sets the color scheme information for the rebar control.
	void SetColorScheme(_In_ const COLORSCHEME* lpcs);

	// Retrieves the margins of a band in the rebar control.
	AFX_ANSI_DEPRECATED void GetBandMargins(_Out_ PMARGINS pMargins) const;

	// Sets the visual style of the rebar control.
	AFX_ANSI_DEPRECATED HRESULT SetWindowTheme(_In_z_ LPCWSTR pszSubAppName);

#if defined(UNICODE)
	// REVIEW: Sets the width for a docked band in the rebar control.
	BOOL SetBandWidth(_In_ UINT uBand, _In_ int cxWidth);

	// REVIEW: Sets the extended style of the rebar control.
	DWORD SetExtendedStyle(_In_ DWORD dwMask, _In_ DWORD dwStyleEx);

	// REVIEW: Retrieves the extended style of the rebar control.
	DWORD GetExtendedStyle() const;
#endif // defined(UNICODE)

// Operations
public:
	// Puts the rebar control in drag-and-drop mode.
	void BeginDrag(_In_ UINT uBand, _In_ DWORD dwPos = (DWORD)-1);

	// Deletes a band from the rebar control.
	BOOL DeleteBand(_In_ UINT uBand);

	// Updates the drag position in the rebar control.
	void DragMove(_In_ DWORD dwPos = (DWORD)-1);

	// Terminates the rebar control's drag-and-drop operation.
	void EndDrag();

	// Determines which portion of a rebar band is at a given
	// point on the screen, if a rebar band exists at that point.
	int HitTest(_In_ RBHITTESTINFO* prbht);

	// Inserts a new band in the rebar control.
	BOOL InsertBand(_In_ UINT uIndex, _In_ REBARBANDINFO* prbbi);

	// Resizes a band in the rebar control to its largest size.
	void MaximizeBand(_In_ UINT uBand);

	// Resizes a band in the rebar control to its smallest size.
	void MinimizeBand(_In_ UINT uBand);

	// Resizes a band in the rebar control to its ideal size.
	void RestoreBand(_In_ UINT uBand);

	// Shows or hides the specified band in the rebar control.
	BOOL ShowBand(_In_ UINT uBand, _In_ BOOL fShow = TRUE);

	// Attempts to find the best layout of the bands for the given rectangle.
	BOOL SizeToRect(_Out_ CRect& rect);

	// Moves the specified band from one index to another in the rebar control.
	BOOL MoveBand(_In_ UINT uFrom, _In_ UINT uTo);

	// Programmatically push a chevron in the rebar control.
	void PushChevron(_In_ UINT uBand, _In_ LPARAM lAppValue);

// Implementation
public:
	// virtual OK here: ~CWnd() is already virtual
	virtual ~CReBarCtrl();
};

/*============================================================================*/
// CRichEditCtrl

// NOTE: This class must remain a binary-compatible subset
// of CRichEditView. Do not add data members or virtual functions
// directly to this class.
class CRichEditCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual OK here - CWnd has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CRichEditCtrl)

// Constructors
public:
	CRichEditCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	BOOL CanUndo() const;
	BOOL CanRedo() const;
	UNDONAMEID GetUndoName() const;
	UNDONAMEID GetRedoName() const;
	int GetLineCount() const;
	BOOL GetModify() const;
	void SetModify(_In_ BOOL bModified = TRUE);
	BOOL SetTextMode(_In_ UINT fMode);
	UINT GetTextMode() const;
	void GetRect(_Out_ LPRECT lpRect) const;
	CPoint GetCharPos(_In_ long lChar) const;
	UINT GetOptions() const;
	void SetOptions(_In_ WORD wOp, _In_ DWORD dwFlags);
	BOOL SetAutoURLDetect(_In_ BOOL bEnable = TRUE);
	UINT GetWordWrapMode() const;
	UINT SetWordWrapMode(_In_ UINT uFlags) const;
	BOOL GetPunctuation(_In_ UINT fType, _Out_ PUNCTUATION* lpPunc) const;
	BOOL SetPunctuation(_In_ UINT fType, _In_ PUNCTUATION* lpPunc);

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	// NOTE: Copied line does not contain null character!
	int GetLine(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszBuffer) const;
	// NOTE: Copied line does not contain null character!
	int GetLine(_In_ int nIndex, _Out_writes_to_(nMaxLength, return) LPTSTR lpszBuffer, _In_ int nMaxLength) const;

	BOOL CanPaste(_In_ UINT nFormat = 0) const;
	void GetSel(_Out_ long& nStartChar, _Out_ long& nEndChar) const;
	void GetSel(_Out_ CHARRANGE &cr) const;
	void LimitText(_In_ long nChars = 0);
	long LineFromChar(_In_ long nIndex) const;
	CPoint PosFromChar(_In_ UINT nChar) const;
	int CharFromPos(_In_ CPoint pt) const;

	void SetSel(_In_ long nStartChar, _In_ long nEndChar);
	void SetSel(_In_ CHARRANGE &cr);
	DWORD GetDefaultCharFormat(_Out_ CHARFORMAT &cf) const;
	DWORD GetDefaultCharFormat(_Out_ CHARFORMAT2 &cf) const;
	DWORD GetSelectionCharFormat(_Out_ CHARFORMAT &cf) const;
	DWORD GetSelectionCharFormat(_Out_ CHARFORMAT2 &cf) const;
	long GetEventMask() const;
	long GetLimitText() const;
	DWORD GetParaFormat(_Out_ PARAFORMAT &pf) const;
	DWORD GetParaFormat(_Out_ PARAFORMAT2 &pf) const;
	// richedit EM_GETSELTEXT is ANSI
	_AFX_INSECURE_DEPRECATE("GetSelText(char *) is unsafe. Instead, use GetSelText(void) returning CString")
	long GetSelText(_Pre_notnull_ _Post_z_ LPSTR lpBuf) const;
	int GetTextRange(_In_ int nFirst, _In_ int nLast, _Out_ CString& refString) const;
	CString GetSelText() const;
	WORD GetSelectionType() const;
	COLORREF SetBackgroundColor(_In_ BOOL bSysColor, _In_ COLORREF cr);
	BOOL SetDefaultCharFormat(_In_ CHARFORMAT &cf);
	BOOL SetDefaultCharFormat(_In_ CHARFORMAT2 &cf);
	BOOL SetSelectionCharFormat(_In_ CHARFORMAT &cf);
	BOOL SetSelectionCharFormat(_In_ CHARFORMAT2 &cf);
	BOOL SetWordCharFormat(_In_ CHARFORMAT &cf);
	BOOL SetWordCharFormat(_In_ CHARFORMAT2 &cf);
	DWORD SetEventMask(_In_ DWORD dwEventMask);
	BOOL SetParaFormat(_In_ PARAFORMAT &pf);
	BOOL SetParaFormat(_In_ PARAFORMAT2 &pf);
	BOOL SetTargetDevice(_In_ HDC hDC, _In_ long lLineWidth);
	BOOL SetTargetDevice(_In_ CDC &dc, _In_ long lLineWidth);
	long GetTextLength() const;
	long GetTextLengthEx(_In_ DWORD dwFlags, _In_ UINT uCodePage = -1) const;
	BOOL SetReadOnly(_In_ BOOL bReadOnly = TRUE);
	int GetFirstVisibleLine() const;

// Operations
	void EmptyUndoBuffer();
	void StopGroupTyping();
	UINT SetUndoLimit(_In_ UINT nLimit);

	int LineIndex(_In_ int nLine = -1) const;
	int LineLength(_In_ int nLine = -1) const;
	void LineScroll(_In_ int nLines, _In_ int nChars = 0);
	void ReplaceSel(_In_z_ LPCTSTR lpszNewText, _In_ BOOL bCanUndo = FALSE);
	void SetRect(_In_ LPCRECT lpRect);

	BOOL DisplayBand(_In_ LPRECT pDisplayRect);
#pragma push_macro("FindTextA")
#pragma push_macro("FindTextW")
#undef FindTextA
#undef FindTextW
	long FindText(_In_ DWORD dwFlags, _Out_ FINDTEXTEX* pFindText) const;
#pragma pop_macro("FindTextA")
#pragma pop_macro("FindTextW")
	DWORD FindWordBreak(_In_ UINT nCode, _In_ DWORD nStart) const;
	long FormatRange(_In_opt_ FORMATRANGE* pfr, _In_ BOOL bDisplay = TRUE);
	void HideSelection(_In_ BOOL bHide, _In_ BOOL bPerm);
	void PasteSpecial(_In_ UINT nClipFormat, _In_ DWORD dvAspect = 0, _In_ HMETAFILE hMF = 0);
	void RequestResize();
	long StreamIn(_In_ int nFormat, EDITSTREAM &es);
	long StreamOut(_In_ int nFormat, EDITSTREAM &es);

	// Clipboard operations
	BOOL Undo();
	BOOL Redo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();

// OLE support
	IRichEditOle* GetIRichEditOle() const;
	BOOL SetOLECallback(_In_ IRichEditOleCallback* pCallback);

// Implementation
public:
	// virtual OK here - ~CWnd is already virtual
	virtual ~CRichEditCtrl();
};



/*============================================================================*/
// CIPAddressCtrl

class CIPAddressCtrl : public CWnd
{
	DECLARE_DYNAMIC(CIPAddressCtrl)

public:
// Constructors
	CIPAddressCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Determines if all fields in the IP address control are blank.
	BOOL IsBlank() const;

	// Clears the contents of the IP address control.
	void ClearAddress();

	// Retrieves the address values for all four fields in the IP address control.
	int GetAddress(_Out_ BYTE& nField0, _Out_ BYTE& nField1, _Out_ BYTE& nField2, _Out_ BYTE& nField3) const;
	int GetAddress(_Out_ DWORD& dwAddress) const;

	// Sets the address values for all four fields in the IP address control.
	void SetAddress(_In_ DWORD dwAddress);
	void SetAddress(_In_ BYTE nField0, _In_ BYTE nField1, _In_ BYTE nField2, _In_ BYTE nField3);

	// Sets the keyboard focus to the specified field in the IP address control.
	void SetFieldFocus(_In_ WORD nField);

	// Sets the valid range for the specified field in the IP address control.
	void SetFieldRange(_In_ int nField, _In_ BYTE nLower, _In_ BYTE nUpper);

// Implementation
public:
	virtual ~CIPAddressCtrl();
};

/*============================================================================*/
// CPagerCtrl

class CPagerCtrl : public CWnd
{
	DECLARE_DYNAMIC(CPagerCtrl)

// Constructors
public:
	CPagerCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Sets the contained window for the pager control.
	void SetChild(_In_ HWND hwndChild);

	// Sets the current background color for the pager control.
	COLORREF SetBkColor(_In_ COLORREF clrBk);

	// Retrieves the current background color for the pager control.
	COLORREF GetBkColor() const;

	// Sets the current border size for the pager control.
	int SetBorder(_In_ int iBorder);

	// Retrieves the current border size for the pager control.
	int GetBorder() const;

	// Sets the current scroll position for the pager control.
	void SetScrollPos(_In_ int iPos);

	// Retrieves the current scroll position of the pager control.
	int GetScrollPos() const;

	// Sets the current button size for the pager control.
	int SetButtonSize(_In_ int iButtonSize);

	// Retrieves the current button size for the pager control.
	int GetButtonSize() const;

	// Determines whether the specified button in the pager control is invisible.
	BOOL IsButtonInvisible(_In_ int iButton) const;

	// Determines whether the specified button in the pager control is normal.
	BOOL IsButtonNormal(_In_ int iButton) const;

	// Determines whether the specified button in the pager control is grayed.
	BOOL IsButtonGrayed(_In_ int iButton) const;

	// Determines whether the specified button in the pager control is depressed.
	BOOL IsButtonDepressed(_In_ int iButton) const;

	// Determines whether the specified button in the pager control is hot.
	BOOL IsButtonHot(_In_ int iButton) const;

	// Retrieves the state of the specified button in a pager control.
	DWORD GetButtonState(_In_ int iButton) const;

	// Retrieves a pager control's IDropTarget interface pointer.
	IDropTarget* GetDropTarget() const;

// Operations

	// Forces the pager control to recalculate the size of the contained window.
	void RecalcSize();

	// Enables or disables mouse forwarding for the pager control.
	void ForwardMouse(_In_ BOOL bForward);

// Implementation
public:
	virtual ~CPagerCtrl();
};

/*============================================================================*/
// CLinkCtrl

AFX_ANSI_DEPRECATED class CLinkCtrl : public CWnd
{
	DECLARE_DYNAMIC(CLinkCtrl)

// Constructors
public:
	CLinkCtrl();

	// Generic creator
	AFX_DEPRECATED("CLinkCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) is no longer supported as the Link Control does not get initialized correctly using this method. Instead, use CLinkCtrl::Create(LPCTSTR lpszLinkMarkup, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)")
		virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
	virtual BOOL Create(_In_z_ LPCTSTR lpszLinkMarkup, _In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	AFX_DEPRECATED("CLinkCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) is no longer supported as the Link Control does not get initialized correctly using this method. Instead, use CLinkCtrl::CreateEx(LPCTSTR lpszLinkMarkup, DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)")
		virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
			_In_ CWnd* pParentWnd, _In_ UINT nID);
	virtual BOOL CreateEx(_In_z_ LPCTSTR lpszLinkMarkup, _In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the preferred height of a link for the control's current width.
	int GetIdealHeight() const;

#if defined(UNICODE)
	// REVIEW: Retrieves the preferred size of a link for the specified width.
	int GetIdealSize(_In_ int cxMaxWidth, _Out_ SIZE* pSize) const;
#endif // defined(UNICODE)

// Operations
	// Set the states and attributes of an item.
	BOOL SetItem(_In_ PLITEM pItem);
	BOOL SetItemID(_In_ int iLink, _In_z_ LPCWSTR szID);
	BOOL SetItemUrl(_In_ int iLink, _In_z_ LPCWSTR szUrl);
	BOOL SetItemState(_In_ int iLink, _In_ UINT state, _In_ UINT stateMask = LIS_FOCUSED | LIS_ENABLED | LIS_VISITED);

	// Retrieve the states and attributes of an item.
	BOOL GetItem(_Out_ PLITEM pItem) const;
	_Success_(return != FALSE) BOOL GetItemID(_In_ int iLink, _Out_ CString& strID) const;
	BOOL GetItemID(_In_ int iLink, _Out_writes_z_(cchID) LPWSTR szID, _In_ UINT cchID) const;
	_Success_(return != FALSE) BOOL GetItemUrl(_In_ int iLink, _Out_ CString& strUrl) const;
	BOOL GetItemUrl(_In_ int iLink, _Out_writes_z_(cchUrl) LPWSTR szUrl, _In_ UINT cchUrl) const;
	_Success_(return != FALSE) BOOL GetItemState(_In_ int iLink, _Out_ UINT *pnState, _In_ UINT stateMask = LIS_FOCUSED | LIS_ENABLED | LIS_VISITED) const;

	// Determines whether the user clicked the specified link.
	BOOL HitTest(_In_ PLHITTESTINFO phti) const;

// Implementation
public:
	virtual ~CLinkCtrl();
};

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/*============================================================================*/
// CNetAddressCtrl

class CNetAddressCtrl : public CEdit
{
	DECLARE_DYNAMIC(CNetAddressCtrl)

public:
// Constructors
	CNetAddressCtrl();

	// Generic creator
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the address in the control.
	HRESULT GetAddress(_Out_ PNC_ADDRESS pAddress) const;

	// Sets the allowed address types for the control.
	HRESULT SetAllowType(_In_ DWORD dwAddrMask);

	// Retrieves the allowed address types for the control.
	DWORD GetAllowType() const;

// Operations
	// Displays the error tip for the control.
	HRESULT DisplayErrorTip();

// Implementation
public:
	virtual ~CNetAddressCtrl();
};

#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_ALL_WARNINGS
#pragma warning(pop)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXCMN_INLINE AFX_INLINE
#include <afxcmn.inl>
#include <afxcmn2.inl>
#undef _AFXCMN_INLINE
#endif
#include <afxcmn3.inl>

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif //__AFXCMN_H__

/////////////////////////////////////////////////////////////////////////////
