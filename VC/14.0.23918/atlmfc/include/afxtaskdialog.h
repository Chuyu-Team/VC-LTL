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

#if !defined(_UNICODE) // Unicode required
#error CTaskDialog requires _UNICODE to be defined.
#endif

#if (NTDDI_VERSION < NTDDI_VISTA) // min Windows Vista required
#error CTaskDialog is not supported on Windows versions prior to Vista.
#endif

#include <commctrl.h>
#include <afxtempl.h>

class CTaskDialog : public CObject
{
	DECLARE_DYNAMIC(CTaskDialog);

public:
	enum
	{
		PROGRESS_BAR_MIN = 0,
		PROGRESS_BAR_MAX = 100
	};

	CTaskDialog(_In_ const CString& strContent, _In_ const CString& strMainInstruction, _In_ const CString& strTitle,
		_In_ int nCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, _In_ int nTaskDialogOptions = TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS,
		_In_ const CString& strFooter = CString());

	CTaskDialog(_In_ const CString& strContent, _In_ const CString& strMainInstruction, _In_ const CString& strTitle,
		_In_ int nIDCommandControlsFirst, _In_ int nIDCommandControlsLast, _In_ int nCommonButtons,
		_In_ int nTaskDialogOptions = TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS, _In_ const CString& strFooter = CString());

	virtual ~CTaskDialog();

	void LoadCommandControls(_In_ int nIDCommandControlsFirst, _In_ int nIDCommandControlsLast);
	void LoadRadioButtons(_In_ int nIDRadioButtonsFirst, _In_ int nIDRadioButtonsLast);

	void SetWindowTitle(_In_ const CString& strWindowTitle);
	void SetMainInstruction(_In_ const CString& strInstructions);
	void SetContent(_In_ const CString& strContent);
	void SetFooterText(_In_ const CString& strFooterText);
	void SetExpansionArea(_In_ const CString& strExpandedInformation, _In_ const CString& strCollapsedLabel = CString(), _In_ const CString& strExpandedLabel = CString());

	void SetMainIcon(_In_ HICON hMainIcon);
	void SetFooterIcon(_In_ HICON hFooterIcon);

	void SetMainIcon(_In_ LPCWSTR lpszMainIcon);
	void SetFooterIcon(_In_ LPCWSTR lpszFooterIcon);

	void SetDialogWidth(_In_ int nWidth = 0);

	void SetOptions(_In_ int nOptionFlag);
	int GetOptions() const;

	void SetCommonButtons(_In_ int nButtonMask, _In_ int nDisabledButtonMask = 0, _In_ int nElevationButtonMask = 0);
	void SetCommonButtonOptions(_In_ int nDisabledButtonMask, _In_ int nElevationButtonMask = 0);

	void AddCommandControl(_In_ int nCommandControlID, _In_ const CString& strCaption, _In_ BOOL bEnabled = TRUE, _In_ BOOL bRequiresElevation = FALSE);
	void SetCommandControlOptions(_In_ int nCommandControlID, _In_ BOOL bEnabled, _In_ BOOL bRequiresElevation = FALSE);
	int GetSelectedCommandControlID() const;
	BOOL IsCommandControlEnabled(_In_ int nCommandControlID) const;
	void SetDefaultCommandControl(_In_ int nCommandControlID);
	void RemoveAllCommandControls();

	void AddRadioButton(_In_ int nRadioButtonID, _In_ const CString& strCaption, _In_ BOOL bEnabled = TRUE);
	void SetRadioButtonOptions(_In_ int nRadioButtonID, _In_ BOOL bEnabled);
	int GetSelectedRadioButtonID() const;
	BOOL IsRadioButtonEnabled(_In_ int nRadioButtonID) const;
	void SetDefaultRadioButton(_In_ int nRadioButtonID);
	void RemoveAllRadioButtons();

	void SetVerificationCheckboxText(_In_ const CString& strVerificationText);
	void SetVerificationCheckbox(_In_ BOOL bChecked);
	BOOL GetVerificationCheckboxState() const;

	void SetProgressBarRange(_In_ int nRangeMin, _In_ int nRangeMax);
	void SetProgressBarPosition(_In_ int nProgressPos);
	void SetProgressBarState(_In_ int nState = PBST_NORMAL);
	void SetProgressBarMarquee(_In_ BOOL bEnabled = TRUE, _In_ int nMarqueeSpeed = 0);

	INT_PTR DoModal(_In_ HWND hParent = ::GetActiveWindow());

	static BOOL __stdcall IsSupported();
	static INT_PTR __stdcall ShowDialog(_In_ const CString& strContent, _In_ const CString& strMainInstruction, _In_ const CString& strTitle, _In_ int nIDCommandControlsFirst, _In_ int nIDCommandControlsLast,
		_In_ int nCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, _In_ int nTaskDialogOptions = TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS, _In_ const CString& strFooter = CString());

	friend HRESULT CALLBACK TaskDialogCallback(_In_ HWND hwnd, _In_ UINT uNotification, _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ LONG_PTR dwRefData);

protected:
	void ClickRadioButton(_In_ int nRadioButtonID) const;
	void ClickCommandControl(_In_ int nCommandControlID) const;
	void NavigateTo(_In_ CTaskDialog& oTaskDialog) const;

	virtual HRESULT OnCreate();
	virtual HRESULT OnInit();
	virtual HRESULT OnDestroy();
	virtual HRESULT OnCommandControlClick(_In_ int nCommandControlID);
	virtual HRESULT OnRadioButtonClick(_In_ int nRadioButtonID);
	virtual HRESULT OnVerificationCheckboxClick(_In_ BOOL bChecked);
	virtual HRESULT OnExpandButtonClick(_In_ BOOL bExpanded);
	virtual HRESULT OnHyperlinkClick(_In_ const CString& strHref);
	virtual HRESULT OnHelp();
	virtual HRESULT OnTimer(_In_ long lTime);
	virtual HRESULT OnNavigatePage();

	virtual int GetCommonButtonId(_In_ int nFlag) const;
	virtual int GetCommonButtonFlag(_In_ int nButtonId) const;
	virtual int GetCommonButtonCount() const;

private:
	CTaskDialog& operator=(const CTaskDialog&);

	enum
	{
			BUTTON_ENABLED = 0x01,
			BUTTON_ELEVATION = 0x02
	};

	typedef struct _CTaskDialogButton
	{
		int nId;
		CString strCaption;
		unsigned char cState;

		_CTaskDialogButton(_In_ int nIdp = 0, _In_ const CString& strCaptionParam = CString(), _In_ unsigned char cStateParam = 0)
			: nId ( nIdp )
			, strCaption ( strCaptionParam )
			, cState ( cStateParam )
		{}

	} CTaskDialogButton;

	typedef union
	{
		HICON hIcon;
		PCWSTR pszIcon;
	} CTaskDialogIcon;

	typedef CArray<CTaskDialogButton> CTaskDialogButtonArray;
	typedef HRESULT (WINAPI *CTaskDialogIndirectFunc)(const TASKDIALOGCONFIG *pTaskConfig, int *pnButton, int *pnRadioButton, BOOL *pfVerificationFlagChecked);

	TASKDIALOG_BUTTON* GetButtonData(_In_ const CTaskDialogButtonArray& oButtonArr) const;
	INT_PTR GetButtonIndex(_In_ int nId, _In_ const CTaskDialogButtonArray& oButtonArr) const;
	void Notify(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) const;
	void FillStruct(_Inout_ TASKDIALOGCONFIG &configTaskDialog);
	void FreeStruct(_Inout_ TASKDIALOGCONFIG &configTaskDialog);

	HWND m_hWnd;

	int m_nCommonButton;
	int m_nButtonDisabled;
	int m_nButtonElevation;

	int m_nFlags;
	int m_nWidth;
	int m_nDefaultCommandControl;
	int m_nDefaultRadioButton;

	CTaskDialogIcon m_mainIcon;
	CTaskDialogIcon m_footerIcon;

	CTaskDialogButtonArray m_aButtons;
	CTaskDialogButtonArray m_aRadioButtons;

	CString m_strContent;
	CString m_strMainInstruction;
	CString m_strTitle;
	CString m_strFooter;
	CString m_strVerification;
	CString m_strInformation;
	CString m_strExpand;
	CString m_strCollapse;

	int m_nProgressRangeMin;
	int m_nProgressRangeMax;
	int m_nProgressState; //ProgressBar or Marquee state
	int m_nProgressPos;   //ProgressBar pos or Marquee speed

	BOOL m_bVerified;
	int m_nRadioId;
	int m_nButtonId;
};
