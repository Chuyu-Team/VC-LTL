// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//

#pragma once

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#include "afxcontrolbarutil.h" // Library global definitions.

//----------------------------
// Control bars:
//----------------------------
#include "afxtoolbar.h"
#include "afxstatusbar.h"
#include "afxmenubar.h"
#include "afxpanedialog.h"
#include "afxoutlookbar.h"
#include "afxoutlookbartabctrl.h"
#include "afxcolorbar.h"
#include "afxcaptionbar.h"
#include "afxoutlookbarpane.h"
#include "afxtabbedpane.h"
#include "afxtaskspane.h"

//--------------------------
// Ribbon bar:
//--------------------------
#include "afxribbonbar.h"
#include "afxribboncustomizedialog.h"
#include "afxribbonkeyboardcustomizedialog.h"
#include "afxribbonpanel.h"
#include "afxribbonmainpanel.h"
#include "afxribbonpanelmenu.h"
#include "afxribbonminitoolbar.h"
#include "afxribboncategory.h"
#include "afxribbonbutton.h"
#include "afxribbonlabel.h"
#include "afxbaseribbonelement.h"
#include "afxribbonbuttonsgroup.h"
#include "afxribboncolorbutton.h"
#include "afxribbonedit.h"
#include "afxribboncombobox.h"
#include "afxribboncheckbox.h"
#include "afxribboncommandslistbox.h"
#include "afxribbonpalettegallery.h"
#include "afxribbonstatusbar.h"
#include "afxribbonstatusbarpane.h"
#include "afxribbonslider.h"
#include "afxribbonprogressbar.h"
#include "afxribbonlinkctrl.h"
#include "afxribbonundobutton.h"

//-----------------------------------
// Control bar buttons:
//-----------------------------------
#include "afxtoolbarbutton.h"
#include "afxtoolbarcomboboxbutton.h"
#include "afxtoolbarmenubutton.h"
#include "afxtoolbareditboxbutton.h"
#include "afxtoolbarspineditboxbutton.h"
#include "afxtoolbarsystemmenubutton.h"
#include "afxtoolbardatetimectrl.h"
#include "afxdropdowntoolbar.h"
#include "afxcolormenubutton.h"
#include "afxtoolbarfontcombobox.h"
#include "afxoutlookbarpanebutton.h"
#include "afxdropdownlistbox.h"

//-------------------------
// Autohide bars
//-------------------------
#include "afxautohidebutton.h"
#include "afxautohidebar.h"

//-------------------------------------------------------------------
// Control Bar frame and miniframe windows (replaces CFrameWnd, 
// CMDIFrameWnd, CMDIChildWnd, COleIPFrameWnd and COleDocIPFrameWnd):
//-------------------------------------------------------------------
#include "afxframewndex.h"
#include "afxmdiframewndex.h"
#include "afxmdichildwndex.h"
#include "afxoleipframewndex.h"
#include "afxoledocipframewndex.h"
#include "afxolecntrframewndex.h"

#include "afxpaneframewnd.h"
#include "afxmultipaneframewnd.h"

//-----------------------------------
// Control Bar customization stuff:
//-----------------------------------
#include "afxtoolbarscustomizedialog.h"
#include "afxcustomizemenubutton.h"

#include "afxcontextmenumanager.h"
#include "afxkeyboardmanager.h"
#include "afxmousemanager.h"

#include "afxusertool.h"
#include "afxacceleratorkey.h"

//--------------------------------
// Control Bar workspace manager
//--------------------------------
#include "afxwinappex.h"
#include "afxsettingsstore.h"
#include "afxrebarstate.h"

//-----------------------
// Control Bar helpers:
//-----------------------
#include "afxmenutearoffmanager.h"
#include "afxdrawmanager.h"
#include "afxdockingpanesrow.h"
#include "afxtooltipmanager.h"
#include "afxcontrolrenderer.h"

//---------------------------------
// Control Bar menu replacements:
//---------------------------------
#include "afxpopupmenu.h"
#include "afxpopupmenubar.h"
#include "afxtoolbarimages.h"

//----------------------------
// Control Bar docking bars:
//----------------------------
#include "afxdockablepane.h"
#include "afxtabctrl.h"
#include "afxdockablepaneadapter.h"
#include "afxdocksite.h"

//------------------------
// Control Bar controls:
//------------------------
#include "afxbutton.h"
#include "afxeditbrowsectrl.h"
#include "afxcolorbutton.h"
#include "afxmenubutton.h"
#include "afxlinkctrl.h"
#include "afxvslistbox.h"
#include "afxcolorpickerctrl.h"
#include "afxfontcombobox.h"
#include "afxrebar.h"
#include "afxshelltreectrl.h"
#include "afxshelllistctrl.h"
#include "afxpropertygridctrl.h"
#include "afxmaskededit.h"
#include "afxsplitterwndex.h"
#include "afxspinbuttonctrl.h"
#include "afxdesktopalertwnd.h"
#include "afxdesktopalertdialog.h"
#include "afxtoolbarslistcheckbox.h"
#include "afxtooltipctrl.h"

//-----------------------
// Control Bar dialogs:
//-----------------------
#include "afxdialogex.h"
#include "afxwindowsmanagerdialog.h"
#include "afxpreviewviewex.h"
#include "afximageeditordialog.h"
#include "afxcolordialog.h"
#include "afxkeymapdialog.h"
#include "afxpropertysheet.h"
#include "afxpropertypage.h"

//---------------------
// Control Bar views:
//---------------------
#include "afxtabview.h"

//--------------------
// Visualization stuff:
//--------------------
#include "afxvisualmanager.h"
#include "afxvisualmanagerofficexp.h"
#include "afxvisualmanagerwindows.h"
#include "afxvisualmanageroffice2003.h"
#include "afxvisualmanagervs2005.h"
#include "afxvisualmanagervs2008.h"
#include "afxvisualmanageroffice2007.h"
#include "afxvisualmanagerwindows7.h"

//-------------------------
// OLE Full Server Support:
//-------------------------
#include "afxoleserverdocex.h"

//-------------------------
// Global Utils:
//-------------------------
#include "afxglobalutils.h"
#include "afxaccessibility.h"

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif
