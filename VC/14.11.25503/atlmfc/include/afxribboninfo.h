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

#include "afxtoolbarimages.h"
#include "afxribbonbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonInfo
{
public:
	enum XImages
	{
		e_ImagesFirst   = 0,
		e_ImagesSmall   = e_ImagesFirst,
		e_ImagesLarge   = 1,
		e_ImagesLast    = e_ImagesLarge
	};

	enum XElementType
	{
		e_TypeFirst              = -1,
		e_TypeUnknown            =  e_TypeFirst,
		e_TypeButton             =  0,
		e_TypeButton_Check       =  1,
		e_TypeButton_Color       =  2,
		e_TypeButton_Undo        =  3,
		e_TypeButton_Gallery     =  4,
		e_TypeButton_LinkCtrl    =  5,
		e_TypeButton_Application =  6,
		e_TypeButton_MainPanel   =  7,
		e_TypeButton_Launch      =  8,
		e_TypeLabel              =  9,
		e_TypeEdit               = 10,
		e_TypeComboBox           = 11,
		e_TypeComboBox_Font      = 12,
		e_TypeSlider             = 13,
		e_TypeProgress           = 14,
		e_TypeSeparator          = 15,
		e_TypeGroup              = 16,
		e_TypePanel              = 17,
		e_TypeCategory           = 18,
		e_TypeContext            = 19,
		e_TypeCategoryMain       = 20,
		e_TypeQAT                = 21,
		e_TypeRibbonBar          = 22,
		e_TypeLast               = e_TypeRibbonBar
	};

public:
	class XRibbonInfoParserCollection;

	class XInfoError
	{
	public:
		enum XError
		{
			e_ErrorFirst         = 0,
			e_ErrorNone          =  e_ErrorFirst,       // none
			e_ErrorFile          = 1,                   // parsing error, use GetDescription();
			e_ErrorInvalidRoot   = 2,                   // root element is invalid or missing
			e_ErrorInvalidHeader = 3,                   // header element is invalid or missing
			e_ErrorInvalidRibbon = 4,                   // ribbon element is invalid or missing
			e_ErrorLast          = e_ErrorInvalidRibbon
		};

	public:
		XInfoError();
		~XInfoError();

		void Empty();
		void SetError(XError error, const CString& strDescription = CString(), long nLine = -1, long nLinePos = -1);

		XError GetError() const { return m_Error; }
		const CString& GetDescription() const { return m_strDescription; }
		long GetLine() const { return m_nLine; }
		long GetLinePos() const { return m_nLinePos; }

	protected:
		XError  m_Error;
		CString m_strDescription;
		long    m_nLine;
		long    m_nLinePos;
	};

	class XRibbonInfoParser
	{
	protected:
		XRibbonInfoParser();

	public:
		virtual ~XRibbonInfoParser();

		virtual BOOL Add(const CString& strName, XRibbonInfoParser** val) = 0;

		virtual BOOL Read(const CString& strName, XRibbonInfoParser** val) = 0;
		virtual BOOL ReadCollection(const CString& strName, XRibbonInfoParserCollection** val) = 0;

		virtual BOOL ReadString(const CString& strName, CString& val) = 0;
		virtual BOOL ReadColor(const CString& strName, COLORREF& val);
		virtual BOOL ReadBool(const CString& strName, BOOL& val);
		virtual BOOL ReadSize(const CString& strName, CSize& val);
		virtual BOOL ReadInt(const CString& strName, int& val);
		virtual BOOL ReadUInt(const CString& strName, UINT& val);
		virtual BOOL ReadValue(CString& val) = 0;

		virtual BOOL WriteString(const CString& strName, const CString& val, const CString& valDefault = CString()) = 0;
		virtual BOOL WriteColor(const CString& strName, COLORREF val, COLORREF valDefault);
		virtual BOOL WriteBool(const CString& strName, BOOL val, BOOL valDefault);
		virtual BOOL WriteSize(const CString& strName, const CSize& val, const CSize& valDefault);
		virtual BOOL WriteInt(const CString& strName, int val, int valDefault);
		virtual BOOL WriteUInt(const CString& strName, UINT val, UINT valDefault);
	};

	class XRibbonInfoParserCollection
	{
	protected:
		XRibbonInfoParserCollection();

	public:
		virtual ~XRibbonInfoParserCollection();

		virtual UINT GetCount() const = 0;
		virtual XRibbonInfoParser* GetItem(UINT nIndex) = 0;
	};

	class XRibbonInfoParserRoot
	{
	protected:
		XRibbonInfoParserRoot();

	public:
		virtual ~XRibbonInfoParserRoot();

		virtual BOOL GetRoot(const CString& strName, XRibbonInfoParser** pParser) = 0;
		virtual BOOL Load(LPBYTE lpBuffer, UINT size) = 0;
		virtual BOOL Save(LPBYTE* lpBuffer, UINT& size) = 0;
	};

public:
	class XID
	{
	public:
		XID();
		~XID();

		BOOL IsEmpty() const { return m_Name.IsEmpty() && m_Value == 0; }

		BOOL Read(XRibbonInfoParser& rParser);
		BOOL Write(XRibbonInfoParser& rParser);

	public:
		CString m_Name;
		UINT    m_Value;
	};

	class XImage
	{
	public:
		XImage();
		~XImage();

		BOOL IsEmpty() const { return m_ID.IsEmpty() || !m_Image.IsValid(); }

		BOOL Read(XRibbonInfoParser& rParser);
		BOOL Write(XRibbonInfoParser& rParser);

	public:
		XID                 m_ID;
		XID                 m_ID_HDPI;
		CMFCToolBarImages   m_Image;
	};
	typedef CArray<XImage*, XImage*> XArrayImages;

	class XBase
	{
	protected:
		XBase(XElementType type);

	public:
		virtual ~XBase();

		static XBase* __stdcall CreateFromType(XElementType type);
		static XBase* __stdcall CreateFromTag(XRibbonInfoParser& rParser);

		virtual BOOL Read(XRibbonInfoParser& rParser) = 0;
		virtual BOOL Write(XRibbonInfoParser& rParser);

		XElementType GetElementType() const { return m_ElementType; }

	private:
		XElementType            m_ElementType;
	};

	class XElement: public XBase
	{
	protected:
		XElement(XElementType type);

	public:
		virtual ~XElement();

		static XElement* __stdcall CreateFromType(XElementType type);

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		XID      m_ID;
		CString  m_strText;
		CString  m_strToolTip;
		CString  m_strDescription;
		CString  m_strKeys;
		CString  m_strMenuKeys;
		BOOL     m_bIsOnPaletteTop;
		BOOL     m_bIsAlwaysLarge;
	};
	typedef CArray<XElement*, XElement*> XArrayElement;

	class XElementSeparator: public XElement
	{
	public:
		XElementSeparator();
		virtual ~XElementSeparator();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

		BOOL     m_bIsHoriz;
	};

	class XElementGroup: public XElement
	{
	public:
		XElementGroup();
		virtual ~XElementGroup();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		XImage          m_Images;
		XArrayElement   m_arButtons;
	};

	class XElementButton: public XElement
	{
	protected:
		XElementButton(XElementType type);

	public:
		XElementButton();
		virtual ~XElementButton();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		int             m_nSmallImageIndex;
		int             m_nLargeImageIndex;
		BOOL            m_bIsDefaultCommand;
		BOOL            m_bIsAlwaysShowDescription;
		XArrayElement   m_arSubItems;
	};

	class XElementLabel: public XElementButton
	{
	public:
		XElementLabel();
		virtual ~XElementLabel();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);
	};

	class XElementButtonCheck: public XElementButton
	{
	public:
		XElementButtonCheck();
		virtual ~XElementButtonCheck();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);
	};

	class XElementButtonLinkCtrl: public XElementButton
	{
	public:
		XElementButtonLinkCtrl();
		virtual ~XElementButtonLinkCtrl();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		CString  m_strLink;
	};

	class XElementEdit: public XElementButton
	{
	protected:
		XElementEdit(XElementType type);

	public:
		XElementEdit();
		virtual ~XElementEdit();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		int   m_nWidth;
		int   m_nWidthFloaty;
		BOOL  m_bHasSpinButtons;
		int   m_nMin;
		int   m_nMax;
		CString m_strValue;
	};

	class XElementComboBox: public XElementEdit
	{
	protected:
		XElementComboBox(XElementType type);

	public:
		XElementComboBox();
		virtual ~XElementComboBox();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		BOOL            m_bHasEditBox;
		BOOL            m_bHasDropDownList;
		BOOL            m_bResizeDropDownList;
		CStringArray    m_arItems;
		CString m_strValue;
	};

	class XElementFontComboBox: public XElementComboBox
	{
	public:
		XElementFontComboBox();
		virtual ~XElementFontComboBox();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		int   m_nFontType;
		BYTE  m_nCharSet;
		BYTE  m_nPitchAndFamily;
	};

	class XElementButtonGallery: public XElementButton
	{
	public:
		class XGalleryGroup
		{
		public:
			XGalleryGroup();
			~XGalleryGroup();

			BOOL Read(XRibbonInfoParser& rParser);
			BOOL Write(XRibbonInfoParser& rParser);

		public:
			CString      m_strName;
			int          m_nItems;
		};
		typedef CArray<XGalleryGroup*, XGalleryGroup*> XArrayGalleryGroup;

	protected:
		XElementButtonGallery(XElementType type);

	public:
		XElementButtonGallery();
		virtual ~XElementButtonGallery();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		BOOL               m_bIsButtonMode;
		BOOL               m_bEnableMenuResize;
		BOOL               m_bMenuResizeVertical;
		int                m_nIconsInRow;
		CSize              m_sizeIcon;
		XImage             m_Images;
		XArrayGalleryGroup m_arGroups;
	};

	class XElementButtonColor: public XElementButtonGallery
	{
	public:
		XElementButtonColor();
		virtual ~XElementButtonColor();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		COLORREF  m_clrColor;
		BOOL      m_bSimpleButtonLook;

		CString   m_strAutomaticBtnLabel;
		CString   m_strAutomaticBtnToolTip;
		COLORREF  m_clrAutomaticBtnColor;
		BOOL      m_bAutomaticBtnOnTop;
		BOOL      m_bAutomaticBtnBorder;

		CString   m_strOtherBtnLabel;
		CString   m_strOtherBtnToolTip;
	};

	class XElementButtonUndo: public XElementButtonGallery
	{
	public:
		XElementButtonUndo();
		virtual ~XElementButtonUndo();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);
	};

	class XElementButtonLaunch: public XElementButton
	{
	public:
		XElementButtonLaunch();
		virtual ~XElementButtonLaunch();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);
	};

	class XElementButtonApplication: public XElementButton
	{
	public:
		XElementButtonApplication();
		virtual ~XElementButtonApplication();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		XImage  m_Image;
	};

	class XElementButtonMainPanel: public XElementButton
	{
	public:
		XElementButtonMainPanel();
		virtual ~XElementButtonMainPanel();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);
	};

	class XElementSlider: public XElement
	{
	public:
		XElementSlider();
		virtual ~XElementSlider();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		DWORD  m_dwStyle;
		int    m_nWidth;
		int    m_nMin;
		int    m_nMax;
		int    m_nPos;
		BOOL   m_bZoomButtons;
	};

	class XElementProgressBar: public XElement
	{
	public:
		XElementProgressBar();
		virtual ~XElementProgressBar();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		int    m_nWidth;
		int    m_nHeight;
		int    m_nMin;
		int    m_nMax;
		int    m_nPos;
		BOOL   m_bInfinite;
	};

	class XPanel: public XBase
	{
	public:
		XPanel();
		virtual ~XPanel();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		CString              m_strName;
		CString              m_strKeys;
		int                  m_nImageIndex;
		BOOL                 m_bJustifyColumns;
		BOOL                 m_bCenterColumnVert;
		XElementButtonLaunch m_btnLaunch;
		XArrayElement        m_arElements;
	};
	typedef CArray<XPanel*, XPanel*> XArrayPanel;

	class XCategory: public XBase
	{
	public:
		XCategory();
		virtual ~XCategory();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		CString        m_strName;
		CString        m_strKeys;
		XImage         m_SmallImages;
		XImage         m_LargeImages;
		XArrayPanel    m_arPanels;
		XArrayElement  m_arElements;
	};
	typedef CArray<XCategory*, XCategory*> XArrayCategory;

	class XContext: public XBase
	{
	public:
		XContext();
		virtual ~XContext();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		XID                     m_ID;
		CString                 m_strText;
		AFX_RibbonCategoryColor m_Color;
		XArrayCategory          m_arCategories;
	};
	typedef CArray<XContext*, XContext*> XArrayContext;

	class XCategoryMain: public XBase
	{
	public:
		XCategoryMain();
		virtual ~XCategoryMain();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		CString        m_strName;
		XImage         m_SmallImages;
		XImage         m_LargeImages;
		XArrayElement  m_arElements;
		BOOL           m_bRecentListEnable;
		CString        m_strRecentListLabel;
		int            m_nRecentListWidth;
	};

	class XQAT: public XBase
	{
	public:
		class XQATItem
		{
		public:
			XQATItem();
			~XQATItem();

			BOOL Read(XRibbonInfoParser& rParser);
			BOOL Write(XRibbonInfoParser& rParser);

		public:
			XID     m_ID;
			BOOL    m_bVisible;
		};
		typedef CArray<XQATItem, XQATItem> XArrayQATItem;

	public:
		XQAT();
		virtual ~XQAT();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

		INT_PTR AddItem(const XQATItem& rItem);

	public:
		XArrayQATItem   m_arItems;
		BOOL            m_bOnTop;
	};

	class XRibbonBar: public XBase
	{
	public:
		XRibbonBar();
		virtual ~XRibbonBar();

		virtual BOOL Read(XRibbonInfoParser& rParser);
		virtual BOOL Write(XRibbonInfoParser& rParser);

	public:
		BOOL  m_bToolTip;
		BOOL  m_bToolTipDescr;
		BOOL  m_bKeyTips;
		BOOL  m_bPrintPreview;
		BOOL  m_bDrawUsingFont;

		XImage          m_Images;
		XCategoryMain*  m_MainCategory;
		XQAT            m_QAT;
		XElementGroup   m_TabElements;
		XArrayCategory  m_arCategories;
		XArrayContext   m_arContexts;

		XElementButtonApplication*  m_btnMain;
	};

public:
	CMFCRibbonInfo();
	virtual ~CMFCRibbonInfo();

	static LPCTSTR __stdcall ElementNameFromType(XElementType type);
	static XElementType __stdcall ElementTypeFromName(const CString& name);

	virtual BOOL Read(IStream* pStream);
	virtual BOOL Read(LPBYTE lpBuffer, UINT nSize);
	virtual BOOL Write(IStream* pStream);
	virtual BOOL Write(LPBYTE* lpBuffer, UINT& nSize);

	void GetArrayImages(XArrayImages& images);

	CSize& GetImageSize(XImages image)
	{
		ASSERT(e_ImagesFirst <= image && image <= e_ImagesLast);

		return m_sizeImage[image];
	}
	const CSize& GetImageSize(XImages image) const
	{
		ASSERT(e_ImagesFirst <= image && image <= e_ImagesLast);

		return m_sizeImage[image];
	}

	DWORD GetVersion() const { return m_dwVersion; }
	XRibbonBar& GetRibbonBar() { return m_RibbonBar; }
	const XRibbonBar& GetRibbonBar() const { return m_RibbonBar; }

	const XInfoError& GetError() const { return m_Error; };

protected:
	virtual BOOL Read(XRibbonInfoParser& rParser);
	virtual BOOL Write(XRibbonInfoParser& rParser);

	void AddElementImages(XElement& info, XArrayImages& images);

private:
	CSize       m_sizeImage[e_ImagesLast + 1];
	XRibbonBar  m_RibbonBar;
	DWORD       m_dwVersion;
	XInfoError  m_Error;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
