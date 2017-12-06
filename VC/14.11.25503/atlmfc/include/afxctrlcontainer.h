// CWnd support for MFC Control containment (Feature Pack controls)
//

#pragma once

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/

void AfxRegisterMFCCtrlClasses();

/*============================================================================*/
// CMFCControlContainer

class CMFCControlContainer : public CObject
{
public:
// Constructor
	CMFCControlContainer(CWnd* pWnd);
	virtual ~CMFCControlContainer();

// Operations
	BOOL SubclassDlgControls();
	void FreeSubclassedControls();

	BOOL IsSubclassedFeaturePackControl(HWND hWndCtrl);
	BOOL ReSubclassControl(HWND hWndCtrl, WORD nIDC, CWnd& control);

	void SetControlData(WORD nIDC, DWORD dwSize, BYTE* pbData);
	BOOL GetControlData(WORD nIDC, DWORD& dwSize, BYTE*& pbData);
	void ClearControlData();

	static int __stdcall UTF8ToString(LPCSTR lpSrc, CString& strDst, int nLength);

// Attributes
	CWnd* m_pWnd;
	CObArray m_arSubclassedCtrls;

protected:
	CMapWordToOb m_mapControlData;

// Implementation
protected:
	CWnd* CreateDlgControl(LPCTSTR lpszClassName);
	void PreUnsubclassControl(CWnd* pControl);
};

/*============================================================================*/

// wParam - DWORD dwSize
// lParam - BYTE* pbInitData

#define MFC_BTN_STYLE_3D            4
#define MFC_BTN_STYLE_FLAT          5
#define MFC_BTN_STYLE_NOBORDERS     6
#define MFC_BTN_MODE_STYLE_SEMIFLAT 7
#define MFC_BTN_IMAGETYPE_NONE      8
#define MFC_BTN_IMAGETYPE_ICON      9
#define MFC_BTN_IMAGETYPE_BITMAP    10
#define MFC_BTN_CURSORTYPE_NONE     11
#define MFC_BTN_CURSORTYPE_HAND     12
#define MFC_BTN_CURSORTYPE_CUSTOM   13

#define MFC_EB_MODE_NONE    0
#define MFC_EB_MODE_FILE    1
#define MFC_EB_MODE_FOLDER  2
#define MFC_EB_MODE_CUSTOM  3

// String constants to identify property
#define PS_True                                     _T("TRUE")
#define PS_False                                    _T("FALSE")
#define PS_MFCLink_Url                              _T("MFCLink_Url")
#define PS_MFCLink_UrlPrefix                        _T("MFCLink_UrlPrefix")
#define PS_MFCLink_Tooltip                          _T("MFCLink_Tooltip")
#define PS_MFCLink_FullTextTooltip                  _T("MFCLink_FullTextTooltip")
#define PS_MFCComboBox_DrawUsingFont                _T("MFCComboBox_DrawUsingFont")
#define PS_MFCComboBox_ShowTrueTypeFonts            _T("MFCComboBox_ShowTrueTypeFonts")
#define PS_MFCComboBox_ShowRasterTypeFonts          _T("MFCComboBox_ShowRasterTypeFonts")
#define PS_MFCComboBox_ShowDeviceTypeFonts          _T("MFCComboBox_ShowDeviceTypeFonts")
#define PS_MFCShellListCtrl_EnableShellContextMenu  _T("MFCShellListCtrl_EnableShellContextMenu")
#define PS_MFCShellTreeCtrl_EnableShellContextMenu  _T("MFCShellTreeCtrl_EnableShellContextMenu")
#define PS_MFCColorButton_EnableOtherButton         _T("MFCColorButton_EnableOtherButton")
#define PS_MFCColorButton_EnableAutomaticButton     _T("MFCColorButton_EnableAutomaticButton")
#define PS_MFCColorButton_ColumnsCount              _T("MFCColorButton_ColumnsCount")

#define PS_MFCMenuButton_OSMenu                     _T("MFCMenuButton_OSMenu")
#define PS_MFCMenuButton_RightArrow                 _T("MFCMenuButton_RightArrow")
#define PS_MFCMenuButton_StayPressed                _T("MFCMenuButton_StayPressed")
#define PS_MFCMenuButton_DefaultClick               _T("MFCMenuButton_DefaultClick")
#define PS_MFCMenuButton_Autosize                   _T("MFCMenuButton_Autosize")

#define PS_MFCPropertyGrid_DescriptionArea          _T("MFCPropertyGrid_DescriptionArea")
#define PS_MFCPropertyGrid_DescriptionRows          _T("MFCPropertyGrid_DescriptionRows")
#define PS_MFCPropertyGrid_HeaderCtrl               _T("MFCPropertyGrid_HeaderCtrl")
#define PS_MFCPropertyGrid_AlphabeticMode           _T("MFCPropertyGrid_AlphabeticMode")
#define PS_MFCPropertyGrid_ModifiedProperties       _T("MFCPropertyGrid_ModifiedProperties")
#define PS_MFCPropertyGrid_VSDotNetLook             _T("MFCPropertyGrid_VSDotNetLook")

#define PS_MFCEditBrowse_BrowseMode                 _T("MFCEditBrowse_Mode")

#define PS_MFCButton_Style                          _T("MFCButton_Style")
#define PS_MFCButton_Autosize                       _T("MFCButton_Autosize")
#define PS_MFCButton_Tooltip                        _T("MFCButton_Tooltip")
#define PS_MFCButton_FullTextTooltip                _T("MFCButton_FullTextTool")
#define PS_MFCButton_CursorType                     _T("MFCButton_CursorType")
#define PS_MFCButton_Cursor                         _T("MFCButton_Cursor")
#define PS_MFCButton_CursorID                       _T("MFCButton_CursorID")
#define PS_MFCButton_ImageType                      _T("MFCButton_ImageType")
#define PS_MFCButton_Image                          _T("MFCButton_Image")
#define PS_MFCButton_ImageID                        _T("MFCButton_ImageID")
#define PS_MFCButton_ImageOnTop                     _T("MFCButton_ImageOnTop") 
#define PS_MFCButton_ImageOnRight                   _T("MFCButton_ImageOnRight")

#define PS_MFCMaskedEdit_SelectByGroup              _T("MFCMaskedEdit_SelectByGroup")
#define PS_MFCMaskedEdit_Mask                       _T("MFCMaskedEdit_Mask")
#define PS_MFCMaskedEdit_InputTemplate              _T("MFCMaskedEdit_InputTemplate")
#define PS_MFCMaskedEdit_DefaultChar                _T("MFCMaskedEdit_DefaultChar")
#define PS_MFCMaskedEdit_ValidChars                 _T("MFCMaskedEdit_ValidChars")

#define PS_MFCVSListbox_BrowseButton                _T("MFCVSListbox_BrowseButton")
#define PS_MFCVSListbox_NewButton                   _T("MFCVSListbox_NewButton")
#define PS_MFCVSListbox_RemoveButton                _T("MFCVSListbox_RemoveButton")
#define PS_MFCVSListbox_UpButton                    _T("MFCVSListbox_UpButton")
#define PS_MFCVSListbox_DownButton                  _T("MFCVSListbox_DownButton")

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
