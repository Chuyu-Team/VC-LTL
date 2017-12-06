// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXDLGS_H__
#define __AFXDLGS_H__

#pragma once

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#include <objbase.h>

#ifndef _INC_COMMDLG
	#include <commdlg.h>    // common dialog APIs
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
#else
#undef _WIN32_IE
#define _WIN32_IE 0x0700
#endif
#ifndef __shobjidl_h__
	#include <ShObjIdl.h>    // for IFileDialog/IFileOpenDialog/IFileSaveDialog
#endif

// Avoid mapping GetFileTitle to GetFileTitle[A/W]
#ifdef GetFileTitle
#undef GetFileTitle
AFX_INLINE short APIENTRY GetFileTitle(LPCTSTR lpszFile, LPTSTR lpszTitle, WORD cbBuf)
#ifdef UNICODE
	{ return ::GetFileTitleW(lpszFile, lpszTitle, cbBuf); }
#else
	{ return ::GetFileTitleA(lpszFile, lpszTitle, cbBuf); }
#endif
#endif

#ifndef _RICHEDIT_
	#include <Richedit.h>
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef _AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////
// Win32 libraries

#endif //!_AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// AFXDLGS - MFC Standard dialogs

// Classes declared in this file

	// CDialog
		class CCommonDialog;  // implementation base class

			// modeless dialogs
			class CFindReplaceDialog; // Find/FindReplace dialog

			// modal dialogs
			class CFileDialog;    // FileOpen/FileSaveAs dialogs
			class CColorDialog;   // Color picker dialog
			class CFontDialog;    // Font chooser dialog
			class CPrintDialog;   // Print/PrintSetup dialogs
			class CPrintDialogEx;   // Windows 2000 Print dialog
			class CPageSetupDialog; // Page Setup dialog

	// CWnd
	class CPropertySheet;     // implements tabbed dialogs

	// CDialog
		class CPropertyPage;  // Used with CPropertySheet for tabbed dialogs

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// CCommonDialog - base class for all common dialogs

class CCommonDialog : public CDialog
{
	DECLARE_DYNAMIC(CCommonDialog)

public:
	explicit CCommonDialog(CWnd* pParentWnd);

// Implementation
public:
    virtual ~CCommonDialog() {};

protected:
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg BOOL OnHelpInfo(HELPINFO*);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CFileDialog - used for FileOpen... or FileSaveAs...

class CFileDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CFileDialog)

public:
// Attributes
	__declspec(property(get=GetOFN)) OPENFILENAME m_ofn;
	const OPENFILENAME& GetOFN() const;
	OPENFILENAME& GetOFN();
	LPOPENFILENAME m_pOFN;

// Constructors
	explicit CFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0,
		BOOL bVistaStyle = TRUE);
	virtual ~CFileDialog();

// Operations
	virtual INT_PTR DoModal();

	/// <summary>
	/// Determines if the current dialog in folder picker mode.</summary>
	/// <returns>
	/// If the dialog in the folder picker mode, the return value is TRUE. Otherwise - FALSE</returns>
	BOOL IsPickFoldersMode() const { return m_bPickFoldersMode; }

	/// <summary>
	/// Determines if the current dialog in non-file system folder picker mode.</summary>
	/// <returns>
	/// If the dialog in the non-file system folder picker mode, the return value is TRUE. Otherwise - FALSE</returns>
	BOOL IsPickNonFileSysFoldersMode() const { return m_bPickNonFileSysFoldersMode; }

	// Helpers for parsing file name after successful return
	// or during Overridable callbacks if OFN_EXPLORER is set
	CString GetPathName() const;  // return full path and filename
	CString GetFileName() const;  // return only filename
	CString GetFileExt() const;   // return only ext
	CString GetFileTitle() const; // return file title
	BOOL GetReadOnlyPref() const; // return TRUE if readonly checked

	// Enumerating multiple file selections
	POSITION GetStartPosition() const;
	CString GetNextPathName(POSITION& pos) const;

	// Helpers for custom templates
	void SetTemplate(UINT nWin3ID, UINT nWin4ID);
	void SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID);

	// Other operations available while the dialog is visible
	CString GetFolderPath() const; // return full path
	void SetControlText(int nID, LPCTSTR lpsz);
	void HideControl(int nID);
	void SetDefExt(LPCTSTR lpsz);

	virtual void UpdateOFNFromShellDialog();
	void ApplyOFNToShellDialog();
	IFileOpenDialog* GetIFileOpenDialog() throw();
	IFileSaveDialog* GetIFileSaveDialog() throw();
	IFileDialogCustomize* GetIFileDialogCustomize() throw();

#if NTDDI_VERSION >= NTDDI_VISTA
	/// <summary>
	/// Adds a folder to the list of places available for the user to open or save items.</summary>
	/// <param name="lpszFolder">A path to the folder to be made available to the user. This can only be a folder.</param>
	/// <param name="fdap">Specifies where the folder is placed within the list.</param>
	void AddPlace(LPCWSTR lpszFolder, FDAP fdap = FDAP_TOP) throw();

	/// <summary>
	/// Adds a folder to the list of places available for the user to open or save items.</summary>
	/// <param name="psi">A pointer to an IShellItem that represents the folder to be made available to the user. This can only be a folder.</param>
	/// <param name="fdap">Specifies where the folder is placed within the list.</param>
	void AddPlace(IShellItem* psi, FDAP fdap = FDAP_TOP) throw();

	/// <summary>
	/// Gets the choice that the user made in the dialog.</summary>
	/// <returns>
	/// A pointer to an IShellItem that represents the user's choice</returns>
	IShellItem* GetResult() throw();

	/// <summary>
	/// Gets the user's choices in a dialog that allows multiple selection.</summary>
	/// <returns>
	/// A pointer to an IShellItemArray through which the items selected in the dialog can be accessed</returns>
	IShellItemArray* GetResults() throw();

	/// <summary>
	/// Provides a property store that defines the default values to be used for the item being saved.</summary>
	/// <returns>
	/// TRUE if successful, FALSE - otherwise</returns>
	/// <param name="lpszPropList">A list of predefined properties divided by ";"</param>
	BOOL SetProperties(LPCWSTR lpszPropList);

	// IFileDialogCustomize wrappers:

	/// <summary>
	/// Enables a drop-down list on the Open or Save button in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the drop-down list</param>
	HRESULT EnableOpenDropDown(DWORD dwIDCtl);

	/// <summary>
	/// Adds a menu to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the menu to add</param>
	/// <param name="strLabel">The menu name</param>
	HRESULT AddMenu(DWORD dwIDCtl, const CString& strLabel);

	/// <summary>
	/// Adds a button to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the button to add</param>
	/// <param name="strLabel">The button name</param>
	HRESULT AddPushButton(DWORD dwIDCtl, const CString& strLabel);

	/// <summary>
	/// Adds a combo box to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the combo box to add</param>
	HRESULT AddComboBox(DWORD dwIDCtl);

	/// <summary>
	/// Adds an option button (also known as radio button) group to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the option button group to add</param>
	HRESULT AddRadioButtonList(DWORD dwIDCtl);

	/// <summary>
	/// Adds a check button to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the check button to add</param>
	/// <param name="strLabel">The check button name</param>
	/// <param name="bChecked">A BOOL indicating the current state of the check button. TRUE if checked; FALSE otherwise</param>
	HRESULT AddCheckButton(DWORD dwIDCtl, const CString& strLabel, BOOL bChecked);

	/// <summary>
	/// Adds an edit box to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the edit box to add</param>
	/// <param name="strText">The edit box name</param>
	HRESULT AddEditBox(DWORD dwIDCtl, const CString& strText);

	/// <summary>
	/// Adds a separator to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the separator add</param>
	HRESULT AddSeparator(DWORD dwIDCtl);

	/// <summary>
	/// Adds text content to the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the text to add</param>
	/// <param name="strText">The text name</param>
	HRESULT AddText(DWORD dwIDCtl, const CString& strText);

	/// <summary>
	/// Sets the text associated with a control, such as button text or an edit box label.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the control</param>
	/// <param name="strLabel">The control name</param>
	HRESULT SetControlLabel(DWORD dwIDCtl, const CString& strLabel);

	/// <summary>
	/// Gets the current visibility and enabled states of a given control.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the control</param>
	/// <param name="dwState">A reference to a variable that receives one or more values from the CDCONTROLSTATE enumeration that indicate the current state of the control.</param>
	HRESULT GetControlState(DWORD dwIDCtl, CDCONTROLSTATEF& dwState);

	/// <summary>
	/// Sets the current visibility and enabled states of a given control.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the control</param>
	/// <param name="dwState"> One or more values from the CDCONTROLSTATE enumeration that indicate the current state of the control.</param>
	HRESULT SetControlState(DWORD dwIDCtl, CDCONTROLSTATEF dwState);

	/// <summary>
	/// Gets the current text in an edit box control.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the edit box</param>
	/// <param name="strText">The text value</param>
	HRESULT GetEditBoxText(DWORD dwIDCtl, CString& strText);

	/// <summary>
	/// Sets the current text in an edit box control.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the edit box</param>
	/// <param name="strText">The text value</param>
	HRESULT SetEditBoxText(DWORD dwIDCtl, const CString& strText);

	/// <summary>
	/// Gets the current state of a check button (check box) in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the check box</param>
	/// <param name="bChecked">check box state: TRUE means checked; FALSE, unchecked</param>
	HRESULT GetCheckButtonState(DWORD dwIDCtl, BOOL& bChecked);

	/// <summary>
	/// Sets the current state of a check button (check box) in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the check box</param>
	/// <param name="bChecked">check box state: TRUE means checked; FALSE, unchecked</param>
	HRESULT SetCheckButtonState(DWORD dwIDCtl, BOOL bChecked);

	/// <summary>
	/// Adds an item to a container control in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control to which the item is to be added</param>
	/// <param name="dwIDItem">The ID of the item</param>
	/// <param name="strLabel">Item's text</param>
	HRESULT AddControlItem(DWORD dwIDCtl, DWORD dwIDItem, const CString& strLabel);

	/// <summary>
	/// Removes an item from a container control in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control from which the item is to be removed</param>
	/// <param name="dwIDItem">The ID of the item</param>
	HRESULT RemoveControlItem(DWORD dwIDCtl, DWORD dwIDItem);

	/// <summary>
	/// Gets the current state of an item in a container control found in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item</param>
	/// <param name="dwState">A reference to a variable that receives one of more values from the CDCONTROLSTATE enumeration that indicate the current state of the control</param>
	HRESULT GetControlItemState(DWORD dwIDCtl, DWORD dwIDItem, CDCONTROLSTATEF& dwState);

	/// <summary>
	/// Sets the current state of an item in a container control found in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item</param>
	/// <param name="dwState">One or more values from the CDCONTROLSTATE enumeration that indicate the new state of the control</param>
	HRESULT SetControlItemState(DWORD dwIDCtl, DWORD dwIDItem, CDCONTROLSTATEF dwState);

	/// <summary>
	/// Gets a particular item from specified container controls in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item that the user selected in the control</param>
	HRESULT GetSelectedControlItem(DWORD dwIDCtl, DWORD& dwIDItem);

	/// <summary>
	/// Sets the selected state of a particular item in an option button group or a combo box found in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item that the user selected in the control</param>
	HRESULT SetSelectedControlItem(DWORD dwIDCtl, DWORD dwIDItem);

	/// <summary>
	/// Declares a visual group in the dialog. Subsequent calls to any "add" method add those elements to this group.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the visual group</param>
	/// <param name="strLabel">The group name</param>
	HRESULT StartVisualGroup(DWORD dwIDCtl, const CString& strLabel);

	/// <summary>
	/// Stops the addition of elements to a visual group in the dialog.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	HRESULT EndVisualGroup();

	/// <summary>
	/// Places a control in the dialog so that it stands out compared to other added controls.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the control</param>
	HRESULT MakeProminent(DWORD dwIDCtl);

	/// <summary>
	/// Sets the text of a control item. For example, the text that accompanies a radio button or an item in a menu.</summary>
	/// <returns>
	/// Returns S_OK if successful, or an error value otherwise.</returns>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item</param>
	/// <param name="strLabel">Item's text</param>
	HRESULT SetControlItemText(DWORD dwIDCtl, DWORD dwIDItem, const CString& strLabel);
#endif // NTDDI_VERSION >= NTDDI_VISTA

// Overridable callbacks
protected:

#if NTDDI_VERSION >= NTDDI_VISTA
	HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv);
#endif // NTDDI_VERSION >= NTDDI_VISTA

	friend UINT_PTR CALLBACK _AfxCommDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual UINT OnShareViolation(LPCTSTR lpszPathName);
	virtual BOOL OnFileNameOK();
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

	// only called back if OFN_EXPLORER is set
	virtual void OnInitDone();
	virtual void OnFileNameChange();
	virtual void OnFolderChange();
	virtual void OnTypeChange();

	/// <summary>
	/// Called when the container item is being selected.</summary>
	/// <param name="dwIDCtl">The ID of the container control</param>
	/// <param name="dwIDItem">The ID of the item</param>
	virtual void OnItemSelected(DWORD dwIDCtl, DWORD dwIDItem);

	/// <summary>
	/// Called when the button is clicked.</summary>
	/// <param name="dwIDCtl">The ID of the button</param>
	virtual void OnButtonClicked(DWORD dwIDCtl);

	/// <summary>
	/// Called when the check box is checked/unchecked.</summary>
	/// <param name="dwIDCtl">The ID of the check box</param>
	/// <param name="bChecked">checked/unchecked</param>
	virtual void OnCheckButtonToggled(DWORD dwIDCtl, BOOL bChecked);

	/// <summary>
	/// Called when the control is being active.</summary>
	/// <param name="dwIDCtl">The ID of the control</param>
	virtual void OnControlActivating(DWORD dwIDCtl);

// Implementation
#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	BOOL m_bVistaStyle;
	BOOL m_bPickFoldersMode;
	BOOL m_bPickNonFileSysFoldersMode;
	DWORD m_dwCookie;
	void* m_pIFileDialog;
	void* m_pIFileDialogCustomize;

	BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
	CString m_strFilter;          // filter string
						// separate fields with '|', terminate with '||\0'
	TCHAR m_szFileTitle[_MAX_FNAME];       // contains file title after return
	TCHAR m_szFileName[_MAX_PATH]; // contains full path name after return

	OPENFILENAME*  m_pofnTemp;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	BOOL m_bFileTypesSet;   // have file types already been set for Vista style dialog?

protected:
	DECLARE_INTERFACE_MAP()

#if NTDDI_VERSION >= NTDDI_VISTA
	BEGIN_INTERFACE_PART(FileDialogEvents, IFileDialogEvents)
		STDMETHOD(OnFileOk)(IFileDialog *);
		STDMETHOD(OnFolderChange)(IFileDialog *);
		STDMETHOD(OnFolderChanging)(IFileDialog *, IShellItem *);
		STDMETHOD(OnHelp)(IFileDialog *);
		STDMETHOD(OnSelectionChange)(IFileDialog *);
		STDMETHOD(OnShareViolation)(IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pResponse);
		STDMETHOD(OnTypeChange)(IFileDialog *);
		STDMETHOD(OnOverwrite)(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *);
	END_INTERFACE_PART_OPTIONAL(FileDialogEvents)

	BEGIN_INTERFACE_PART(FileDialogControlEvents, IFileDialogControlEvents)
		STDMETHOD(OnItemSelected)(IFileDialogCustomize *, DWORD, DWORD);
		STDMETHOD(OnButtonClicked)(IFileDialogCustomize *, DWORD);
		STDMETHOD(OnCheckButtonToggled)(IFileDialogCustomize *, DWORD, BOOL);
		STDMETHOD(OnControlActivating)(IFileDialogCustomize *, DWORD);
	END_INTERFACE_PART_OPTIONAL(FileDialogControlEvents)
#else
	BEGIN_INTERFACE_PART(FileDialogEvents, IUnknown)
	END_INTERFACE_PART_OPTIONAL(FileDialogEvents)

	BEGIN_INTERFACE_PART(FileDialogControlEvents, IUnknown)
	END_INTERFACE_PART_OPTIONAL(FileDialogControlEvents)
#endif // NTDDI_VERSION >= NTDDI_VISTA
};

/*============================================================================*/
// CFolderPickerDialog - used to select a folder

/// <summary>
/// CFolderPickerDialog class implements CFileDialog in the folder picker mode.</summary>
class CFolderPickerDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CFolderPickerDialog)

// Constructors

	/// <summary>
	/// CFolderPickerDialog constructor</summary>
	/// <param name="lpszFolder">Initial folder</param>
	/// <param name="dwFlags">A combination of one or more flags that allow you to customize the dialog box</param>
	/// <param name="pParentWnd">A pointer to the file dialog-box object's parent or owner window</param>
	/// <param name="dwSize">The size of the OPENFILENAME structure</param>
	explicit CFolderPickerDialog(LPCTSTR lpszFolder = NULL, DWORD dwFlags = 0, CWnd* pParentWnd = NULL, DWORD dwSize = 0, BOOL fNonFileSystemFolders = FALSE);

	/// <summary>
	/// CFolderPickerDialog destructor.</summary>
	virtual ~CFolderPickerDialog();
};

/*============================================================================*/
// CFontDialog - used to select a font

class CFontDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CFontDialog)

public:
// Attributes
	// font choosing parameter block
	CHOOSEFONT m_cf;

// Constructors
	CFontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
	CFontDialog(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);

// Operations
	virtual INT_PTR DoModal();

	// Get the selected font (works during DoModal displayed or after)
	void GetCurrentFont(LPLOGFONT lplf);

	// Helpers for parsing information after successful return
	CString GetFaceName() const;  // return the face name of the font
	CString GetStyleName() const; // return the style name of the font
	int GetSize() const;          // return the pt size of the font
	COLORREF GetColor() const;    // return the color of the font
	int GetWeight() const;        // return the chosen font weight
	BOOL IsStrikeOut() const;     // return TRUE if strikeout
	BOOL IsUnderline() const;     // return TRUE if underline
	BOOL IsBold() const;          // return TRUE if bold font
	BOOL IsItalic() const;        // return TRUE if italic font
	void GetCharFormat(CHARFORMAT& cf) const;

// Implementation
	LOGFONT m_lf; // default LOGFONT to store the info
	DWORD FillInLogFont(const CHARFORMAT& cf);

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	TCHAR m_szStyleName[64]; // contains style name after return
};

/*============================================================================*/
// CColorDialog - used to select a color

class CColorDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CColorDialog)

public:
// Attributes
	// color chooser parameter block
	CHOOSECOLOR m_cc;

// Constructors
	CColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0,
			CWnd* pParentWnd = NULL);

// Operations
	virtual INT_PTR DoModal();

	// Set the current color while dialog is displayed
	void SetCurrentColor(COLORREF clr);

	// Helpers for parsing information after successful return
	COLORREF GetColor() const;
	static COLORREF* PASCAL GetSavedCustomColors();

// Overridable callbacks
protected:
	friend UINT_PTR CALLBACK _AfxCommDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual BOOL OnColorOK();       // validate color

// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif
};

// for backward compatibility clrSavedCustom is defined as GetSavedCustomColors
#define clrSavedCustom GetSavedCustomColors()

/*============================================================================*/
// Page Setup dialog

class CPageSetupDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CPageSetupDialog)

public:
// Attributes
	PAGESETUPDLG m_psd;

// Constructors
	CPageSetupDialog(DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE,
		CWnd* pParentWnd = NULL);

// Attributes
	LPDEVMODE GetDevMode() const;   // return DEVMODE
	CString GetDriverName() const;  // return driver name
	CString GetDeviceName() const;  // return device name
	CString GetPortName() const;    // return output port name
	HDC CreatePrinterDC();
	CSize GetPaperSize() const;
	void GetMargins(LPRECT lpRectMargins, LPRECT lpRectMinMargins) const;

// Operations
	virtual INT_PTR DoModal();

// Overridables
	virtual UINT PreDrawPage(WORD wPaper, WORD wFlags, LPPAGESETUPDLG pPSD);
	virtual UINT OnDrawPage(CDC* pDC, UINT nMessage, LPRECT lpRect);

// Implementation
protected:
	static UINT CALLBACK PaintHookProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/*============================================================================*/
// CPrintDialog - used for Print... and PrintSetup...

class CPrintDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CPrintDialog)

public:
// Attributes
	// print dialog parameter block (note this is a reference)
	PRINTDLG& m_pd;

// Constructors
	CPrintDialog(BOOL bPrintSetupOnly,
		// TRUE for Print Setup, FALSE for Print Dialog
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
			| PD_HIDEPRINTTOFILE | PD_NOSELECTION,
		CWnd* pParentWnd = NULL);

// Operations
	virtual INT_PTR DoModal();

	// GetDefaults will not display a dialog but will get
	// device defaults
	BOOL GetDefaults();

	// Helpers for parsing information after successful return
	int GetCopies() const;          // num. copies requested
	BOOL PrintCollate() const;      // TRUE if collate checked
	BOOL PrintSelection() const;    // TRUE if printing selection
	BOOL PrintAll() const;          // TRUE if printing all pages
	BOOL PrintRange() const;        // TRUE if printing page range
	int GetFromPage() const;        // starting page if valid
	int GetToPage() const;          // starting page if valid
	LPDEVMODE GetDevMode() const;   // return DEVMODE
	CString GetDriverName() const;  // return driver name
	CString GetDeviceName() const;  // return device name
	CString GetPortName() const;    // return output port name
	HDC GetPrinterDC() const;       // return HDC (caller must delete)

	// This helper creates a DC based on the DEVNAMES and DEVMODE structures.
	// This DC is returned, but also stored in m_pd.hDC as though it had been
	// returned by CommDlg.  It is assumed that any previously obtained DC
	// has been/will be deleted by the user.  This may be
	// used without ever invoking the print/print setup dialogs.

	HDC CreatePrinterDC();

// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	PRINTDLG m_pdActual; // the Print/Print Setup need to share this
protected:
	// The following handle the case of print setup... from the print dialog
	CPrintDialog(PRINTDLG& pdInit);
	virtual CPrintDialog* AttachOnSetup();

	afx_msg void OnPrintSetup();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CPrintDialogEx - Windows 2000 Print Dialog

#ifndef __ocidl_h__
	#include <OCIdl.h>
#endif

class CPrintDialogEx :
			public CCommonDialog,
			public IPrintDialogCallback,
			public IObjectWithSite
{

	DECLARE_DYNAMIC(CPrintDialogEx)

public:
// Attributes
	// print dialog parameter block
	PRINTDLGEX m_pdex;

// Constructors
	CPrintDialogEx(DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
			| PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_NOCURRENTPAGE,
		CWnd* pParentWnd = NULL);

// Operations
	virtual INT_PTR DoModal();

	// GetDefaults will not display a dialog but will get
	// device defaults
	BOOL GetDefaults();

	// Helpers for parsing information after successful return
	int GetCopies() const;          // num. copies requested
	BOOL PrintCollate() const;      // TRUE if collate checked
	BOOL PrintSelection() const;    // TRUE if printing selection
	BOOL PrintCurrentPage() const;	// TRUE if printing current page
	BOOL PrintAll() const;          // TRUE if printing all pages
	BOOL PrintRange() const;        // TRUE if printing page range
	LPDEVMODE GetDevMode() const;   // return DEVMODE
	CString GetDriverName() const;  // return driver name
	CString GetDeviceName() const;  // return device name
	CString GetPortName() const;    // return output port name
	HDC GetPrinterDC() const;       // return HDC (caller must delete)

	// This helper creates a DC based on the DEVNAMES and DEVMODE structures.
	// This DC is returned, but also stored in m_pdex.hDC as though it had been
	// returned by CommDlg.  It is assumed that any previously obtained DC
	// has been/will be deleted by the user.  This may be
	// used without ever invoking the print dialog.

	HDC CreatePrinterDC();

// Implementation

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

// IPrintDialogCallback
	STDMETHOD(InitDone)();
	STDMETHOD(SelectionChange)();
	STDMETHOD(HandleMessage)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, void **ppvSite);

	IUnknown* m_pUnkSite;

	// implementation helpers
	HWND PreModal();
	void PostModal();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// Find/FindReplace modeless dialogs

class CFindReplaceDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CFindReplaceDialog)

public:
// Attributes
	FINDREPLACE m_fr;

// Constructors
	CFindReplaceDialog();
	// Note: you must allocate these on the heap.
	//  If you do not, you must derive and override PostNcDestroy()

	virtual BOOL Create(BOOL bFindDialogOnly, // TRUE for Find, FALSE for FindReplace
			LPCTSTR lpszFindWhat,
			LPCTSTR lpszReplaceWith = NULL,
			DWORD dwFlags = FR_DOWN,
			CWnd* pParentWnd = NULL);

	// find/replace parameter block
	static CFindReplaceDialog* PASCAL GetNotifier(LPARAM lParam);

// Operations
	// Helpers for parsing information after successful return
	CString GetReplaceString() const;// get replacement string
	CString GetFindString() const;   // get find string
	BOOL SearchDown() const;         // TRUE if search down, FALSE is up
	BOOL FindNext() const;           // TRUE if command is find next
	BOOL MatchCase() const;          // TRUE if matching case
	BOOL MatchWholeWord() const;     // TRUE if matching whole words only
	BOOL ReplaceCurrent() const;     // TRUE if replacing current string
	BOOL ReplaceAll() const;         // TRUE if replacing all occurrences
	BOOL IsTerminating() const;      // TRUE if terminating dialog

// Implementation
protected:
	virtual void PostNcDestroy();

#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	TCHAR m_szFindWhat[128];
	TCHAR m_szReplaceWith[128];
};

/*============================================================================*/
// CPropertyPage -- one page of a tabbed dialog

class CPropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CPropertyPage)

// Construction
public:
	// simple construction
	CPropertyPage();
	explicit CPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	explicit CPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	void Construct(UINT nIDTemplate, UINT nIDCaption = 0);
	void Construct(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

	// extended construction
	CPropertyPage(UINT nIDTemplate, UINT nIDCaption,
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	CPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption,
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	void Construct(UINT nIDTemplate, UINT nIDCaption,
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	void Construct(LPCTSTR lpszTemplateName, UINT nIDCaption,
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

// Attributes
	__declspec(property(get=GetPSP)) PROPSHEETPAGE m_psp;
	const PROPSHEETPAGE & GetPSP() const;
	PROPSHEETPAGE & GetPSP();
	LPPROPSHEETPAGE m_pPSP;

// Operations
	void CancelToClose();
	void SetModified(BOOL bChanged = TRUE);
	LRESULT QuerySiblings(WPARAM wParam, LPARAM lParam);

// Overridables
public:
	virtual BOOL OnApply();
	virtual void OnReset();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnQueryCancel();

	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual HWND OnWizardFinishEx();

	virtual CPropertySheet *GetParentSheet();

// Implementation
public:
	virtual ~CPropertyPage();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void EndDialog(int nEndID); // called for error scenarios

protected:
	// private implementation data
	CString m_strCaption;
	BOOL m_bFirstSetActive;
	CString m_strHeaderTitle;    // this is displayed in the header
	CString m_strHeaderSubTitle; //

	// implementation helpers
	void AllocPSP(DWORD dwSize);
	void CommonConstruct(LPCTSTR lpszTemplateName, UINT nIDCaption);
	void CommonConstruct(LPCTSTR lpszTemplateName, UINT nIDCaption,
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG*);
	LRESULT MapWizardResult(LRESULT lToMap);
	BOOL IsButtonEnabled(int iButton);

	void PreProcessPageTemplate(PROPSHEETPAGE& psp, BOOL bWizard);
	void Cleanup();
	const DLGTEMPLATE* InitDialogInfo(const DLGTEMPLATE* pTemplate);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

	friend class CPropertySheet;
};

#define CPropertyPageEx CPropertyPage

/*============================================================================*/
// CPropertySheet -- a tabbed "dialog" (really a popup-window)

class CTabCtrl; // forward reference (see afxcmn.h)

class CPropertySheet : public CWnd
{
	DECLARE_DYNAMIC(CPropertySheet)

// Construction
public:
	// simple construction
	CPropertySheet();
	explicit CPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);
	explicit CPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);
	void Construct(UINT nIDCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);
	void Construct(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);

	// extended construction
	CPropertySheet(UINT nIDCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	CPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	void Construct(UINT nIDCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	void Construct(LPCTSTR pszCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

	// for modeless creation
	virtual BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1,
		DWORD dwExStyle = 0);
	// the default style, expressed by passing -1 as dwStyle, is actually:
	// WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_CONTEXT_HELP | WS_VISIBLE

// Attributes
public:
	PROPSHEETHEADER m_psh;

	int GetPageCount() const;
	CPropertyPage* GetActivePage() const;
	int GetActiveIndex() const;
	CPropertyPage* GetPage(int nPage) const;
	int GetPageIndex(CPropertyPage* pPage);
	BOOL SetActivePage(int nPage);
	BOOL SetActivePage(CPropertyPage* pPage);
	void SetTitle(LPCTSTR lpszText, UINT nStyle = 0);
	CTabCtrl* GetTabControl() const;

	void SetWizardMode();
	void SetFinishText(LPCTSTR lpszText);
	void SetWizardButtons(DWORD dwFlags);

	void EnableStackedTabs(BOOL bStacked);

// Operations
public:
	virtual INT_PTR DoModal();
	void AddPage(CPropertyPage* pPage);
	void RemovePage(CPropertyPage* pPage);
	void RemovePage(int nPage);
	void EndDialog(int nEndID); // used to terminate a modal dialog
	void PressButton(int nButton);
	void MapDialogRect(LPRECT lpRect) const;

// Overridables (special message map entries)
	virtual BOOL OnInitDialog();

// Implementation
public:
	virtual ~CPropertySheet();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void CommonConstruct(CWnd* pParentWnd, UINT iSelectPage);
	void CommonConstruct(CWnd* pParentWnd, UINT iSelectPage,
		HBITMAP hbmWatermark, HPALETTE hpalWatermark, HBITMAP hbmHeader);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void BuildPropPageArray();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL ContinueModal();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	BOOL IsWizard() const;
	BOOL IsModeless() const;

protected:
	virtual BOOL IsLeftNavigationPane(HWND /*hWnd*/) const { return FALSE; }
	virtual BOOL CanAddPageToDynamicLayout() const { return TRUE; }

protected:
	CPtrArray m_pages;      // array of CPropertyPage pointers
	CString m_strCaption;   // caption of the pseudo-dialog
	CWnd* m_pParentWnd;     // parent window of property sheet
	BOOL m_bStacked;        // EnableStackedTabs sets this
	BOOL m_bModeless;       // TRUE when Create called instead of DoModal
	CSize m_sizeMin;		// Used in OnGetMinMaxInfo

	afx_msg BOOL OnNcCreate(LPCREATESTRUCT);
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM);
	afx_msg LRESULT OnSetDefID(WPARAM, LPARAM);
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);

	DECLARE_MESSAGE_MAP()

	friend class CPropertyPage;
};

#define CPropertySheetEx CPropertySheet

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXDLGS_INLINE AFX_INLINE
#include <afxdlgs.inl>
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif //__AFXDLGS_H__

/////////////////////////////////////////////////////////////////////////////
