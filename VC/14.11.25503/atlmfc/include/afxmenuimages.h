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
#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMenuImages  
{
public:
	enum IMAGES_IDS
	{
		IdArrowDown,
		IdArrowRight,
		IdCheck,
		IdMinimize,
		IdRestore,
		IdClose,
		IdMaximize,
		IdArrowUp,
		IdArrowShowAll,
		IdArrowLeft,
		IdCloseSmall,
		IdMoreButtons,
		IdRadio,
		IdArrowDownLarge,
		IdArrowRightLarge,
		IdPinHorz,
		IdPinVert,
		IdArrowLeftLarge,
		IdArrowFirst,
		IdArrowLast,
		IdArrowRightTab3d,
		IdArrowLeftTab3d,
		IdArrowRightDsbldTab3d,
		IdArrowLeftDsbldTab3d,
		IdArrowUpLarge,
		IdArrowPageLeft,
		IdArrowPageRight,
		IdArrowBack,
		IdArrowForward,
		IdCustomizeArrowDown,
		IdCustomizeArrowLeft,
		IdCustomizeMoreButtonsHorz,
		IdCustomizeMoreButtonsVert,
		IdCustomizeArrowDownBold,
		IdCloseBold,
		IdLaunchArrow,
	};

	enum IMAGE_STATE
	{
		ImageBlack,
		ImageGray,
		ImageLtGray,
		ImageWhite,
		ImageDkGray,
		ImageBlack2,
	};

	static void __stdcall Draw(CDC* pDC, CMenuImages::IMAGES_IDS id, const CPoint& ptImage, CMenuImages::IMAGE_STATE state = CMenuImages::ImageBlack, const CSize& sizeImage = CSize(0, 0));
	static void __stdcall Draw(CDC* pDC, CMenuImages::IMAGES_IDS id, const CRect& rectImage, CMenuImages::IMAGE_STATE state = CMenuImages::ImageBlack, const CSize& sizeImage = CSize(0, 0));
	static CSize __stdcall Size();
	static void __stdcall SetColor(CMenuImages::IMAGE_STATE state, COLORREF color);	// color == -1: default
	static void __stdcall CleanUp();

protected:
	static BOOL __stdcall Initialize();
	static void __stdcall CreateCopy(CMFCToolBarImages& images, COLORREF clr);

	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesBlack;
	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesDkGray;
	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesGray;
	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesLtGray;
	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesWhite;
	AFX_IMPORT_DATA static CMFCToolBarImages m_ImagesBlack2;

	static BOOL m_bInitializing;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
