// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Note: This header file contains useful classes that are documented only
//  in the MFC Technical Notes.  These classes may change from version to
//  version, so be prepared to change your code accordingly if you utilize
//  this header.  In the future, commonly used portions of this header
//  may be moved and officially documented.

#ifndef __AFXADV_H__
#define __AFXADV_H__

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#ifndef __AFXTEMPL_H__
	#include <afxtempl.h>
#endif

#ifndef __objectarray_h__
	#include <ObjectArray.h>
#endif

#ifndef __shobjidl_h__
	#include <ShObjIdl.h>
#endif

#pragma once

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// AFXADV - MFC Advanced Classes

// Classes declared in this file

//CObject
	//CFile
		//CMemFile
			class CSharedFile;          // Shared memory file

	class CRecentFileList;              // used in CWinApp for MRU list
	class CDockState;                   // state of docking toolbars
	class CJumpList;                    // Windows7 custom jump list implementation
	class CAppDestinations;             // Windows7 IApplicationDestinations implementation
/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// Shared file support

class CSharedFile : public CMemFile
{
	DECLARE_DYNAMIC(CSharedFile)

public:
// Constructors
	explicit CSharedFile(UINT nAllocFlags = GMEM_MOVEABLE,
		UINT nGrowBytes = 4096);

// Attributes
	HGLOBAL Detach();
	void SetHandle(HGLOBAL hGlobalMemory, BOOL bAllowGrow = TRUE);

// Implementation
public:
	virtual ~CSharedFile();
protected:
	virtual BYTE* Alloc(SIZE_T nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, SIZE_T nBytes);
	virtual void Free(BYTE* lpMem);

	UINT m_nAllocFlags;
	HGLOBAL m_hGlobalMemory;
	BOOL m_bAllowGrow;
};

/*============================================================================*/
// CRecentFileList

#define AFX_ABBREV_FILENAME_LEN 30

class CRecentFileList
{
// Constructors
public:
	CRecentFileList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

// Attributes
	int GetSize() const;
	CString& operator[](int nIndex);

// Operations
	virtual void Remove(int nIndex);
	virtual void Add(LPCTSTR lpszPathName);

	/// <summary>
	/// Adds a file to the MRU file list.</summary>
	/// <param name="lpszPathName"> Pathname to be added to the list.</param>
	/// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
	virtual void Add(LPCTSTR lpszPathName, LPCTSTR lpszAppID);
	virtual BOOL GetDisplayName(CString& strName, int nIndex,
		LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;
	virtual void UpdateMenu(CCmdUI* pCmdUI);
	virtual void ReadList();    // reads from registry or ini file
	virtual void WriteList();   // writes to registry or ini file

	/// <summary>
	/// Adds a file to the MRU file list.</summary>
	/// <param name="pItem"> Specifies a pointer to Shell Item to be added to the list.</param>
	/// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
	void Add(IShellItem* pItem, LPCTSTR lpszAppID);

	/// <summary>
	/// Adds a file to the MRU file list.</summary>
	/// <param name="pLink"> Specifies a pointer to Shell Link to be added to the list.</param>
	/// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
	void Add(IShellLink* pLink, LPCTSTR lpszAppID);

	/// <summary>
	/// Adds a file to the MRU file list.</summary>
	/// <param name="pidl"> Specifies the IDLIST for the shell item that should be added to the recent docs folder.</param>
	/// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
	void Add(PIDLIST_ABSOLUTE pidl, LPCTSTR lpszAppID);

// Implementation
	virtual ~CRecentFileList();

	int m_nSize;                // contents of the MRU list
	CString* m_arrNames;
	CString m_strSectionName;   // for saving
	CString m_strEntryFormat;
	UINT m_nStart;              // for displaying
	int m_nMaxDisplayLength;
	CString m_strOriginal;      // original menu item contents
};

AFX_INLINE int CRecentFileList::GetSize() const
	{ return m_nSize; }
AFX_INLINE CString& CRecentFileList::operator[](int nIndex)
{ ENSURE_ARG(nIndex >=0 && nIndex < m_nSize); return m_arrNames[nIndex]; }

/*============================================================================*/
// CDockState - used for docking serialization

class CDockState : public CObject
{
	DECLARE_SERIAL(CDockState)
	CDockState();

public:
// Attributes
	CPtrArray m_arrBarInfo;

public:
// Operations
	void LoadState(LPCTSTR lpszProfileName);
	void SaveState(LPCTSTR lpszProfileName);
	void Clear(); //deletes all the barinfo's
	DWORD GetVersion();

// Implementation
protected:
	BOOL m_bScaling;
	CRect m_rectDevice;
	CRect m_rectClip;
	CSize m_sizeLogical;
	DWORD m_dwVersion;

public:
	~CDockState();
	virtual void Serialize(CArchive& ar);

	// scaling implementation
	void ScalePoint(CPoint& pt);
	void ScaleRectPos(CRect& rect);
	CSize GetScreenSize();
	void SetScreenSize(CSize& size);
};

#if (NTDDI_VERSION >= NTDDI_WIN7)
class CJumpList
{
public:
	/// <summary>
	/// Constructs a CJumpList object.  </summary>
	/// <param name="bAutoCommit"> If this parameter is FALSE the list is not automatically committed in
	/// destructor. </param>
	CJumpList(BOOL bAutoCommit = TRUE);
	~CJumpList();

	/// <summary>
	/// Begins a list-building transaction.  </summary>
	/// <remarks>
	/// You don't need to call this method explicitly unless you wish to retrieve a pointer to
	/// ICustomDestinationList using GetDestinationList, the number of available slots using GetMaxSlots, or
	/// list of removed items using GetRemovedItems.</remarks>
	BOOL InitializeList();

	/// <summary>
	/// Sets the Application User Model ID for the list that will be built. </summary>
	/// <param name="strAppID"> A string that specifies the Application User Model ID./param>
	void SetAppID(LPCTSTR strAppID)
	{
		m_strAppID = strAppID;
		if (m_bInitialized)
		{
			TRACE0("Jump list has been initialized, therefore AppID setting has no effect.");
		}
	}

	/// <summary>
	/// Retrieves the maximum number of items, including category headers that can display in
	/// the calling application's destination menu. </summary>
	/// <remarks>
	/// Applications may only report a number of items and category headers combined up to this value.
	/// If calls to AppendCategory, AppendKnownCategory, or AddUserTasks exceed this number, they will
	/// return failure. </remarks>
	UINT GetMaxSlots() const;

	/// <summary>
	/// Retrieves an interface pointer to destination list.</summary>
	/// <remarks>
	/// If the jump list has not been initialized, or has been committed or aborted, the
	/// returned value will be NULL. </remarks>
	ICustomDestinationList* GetDestinationList()
	{
		return m_destListPtr;
	}

	/// <summary>
	/// Returns array of items that represent removed destinations.
	/// </summary>
	/// <remarks>
	/// The removed destinations are retrieved during initialization of jump list.
	/// When generating a new destination list, applications are expected to first process the removed
	/// destinations list, clearing their tracking data for any item returned by the removed list enumerator.
	/// If an application attempts to provide an item that was just removed in the transaction that the
	/// current call to BeginList started, the method call that re-added that item will fail,
	/// to ensure that applications are respecting the removed list.  </remarks>
	IObjectArray* GetRemovedItems()
	{
		return m_removedItemsPtr;
	}

	/// <summary>
	/// Aborts a list-building transaction without committing. </summary>
	/// <remarks>
	/// Calling this method has the same effect as destroying CJumpList without calling CommitList. </remarks>
	void AbortList()
	{
		if (m_bInitialized)
		{
			ASSERT(m_destListPtr != NULL);
			m_destListPtr->AbortList();
			ClearAll();
			m_bInitialized = FALSE;
		}
	}

	/// <summary>
	/// Adds destination to the list.</summary>
	/// <remarks>
	/// The instance of CJumpList internally accumulates added destinations and then commits them
	/// in CommitList.</remarks>
	/// <param name="lpcszCategoryName"> Specifies a category name. If the specified category does not exist, it will be created. </param>
	/// <param name="strDestinationPath"> Specifies a path to destination file. </param>
	BOOL AddDestination(LPCTSTR lpcszCategoryName, LPCTSTR strDestinationPath);

	/// <summary>
	/// Adds destination to the list.</summary>
	/// <remarks>
	/// The instance of CJumpList internally accumulates added destinations and then commits them
	/// in CommitList.</remarks>
	/// <param name="lpcszCategoryName"> Specifies a category name. If the specified category does not exist, it will be created. </param>
	/// <param name="pShellItem"> Specifies a Shell Item representing the destination being added. </param>
	BOOL AddDestination(LPCTSTR strCategoryName, IShellItem* pShellItem);


	/// <summary>
	/// Adds destination to the list.</summary>
	/// <remarks>
	/// The instance of CJumpList internally accumulates added destinations and then commits them
	/// in CommitList.</remarks>
	/// <param name="lpcszCategoryName"> Specifies a category name. If the specified category does not exist, it will be created. </param>
	/// <param name="pShellLink"> Specifies a Shell Link representing the destination being added. </param>
	BOOL AddDestination(LPCTSTR strCategoryName, IShellLink* pShellLink);

	/// <summary>
	/// Appends a Known Category to the list.</remarks>
	/// <remarks>
	/// Known Categories are the Frequent and Recent categories that we will automatically calculate for every
	/// application that utilizes SHAddToRecentDocs (or indirectly uses it as the shell will call it on the
	/// application's behalf in some scenarios).</remarks>
	/// <param name="category"> Specifies a known category type. Can be either KDC_RECENT, or KDC_KNOWN. </param>
	BOOL AddKnownCategory(KNOWNDESTCATEGORY category);

	/// <summary>
	/// Adds items to the canonical Tasks category. </summary>
	/// <remarks>
	/// The instance of CJumpList accumulates specified tasks and adds them to the Destination List
	/// during CommitList.
	/// Task items will appear in a category at the bottom of the application's destination menu.
	/// This category takes precedence over all other categories when it is filled in the UI. </remarks>
	/// <param name="strTargetExecutablePath"> Specifies the target task path.</param>
	/// <param name="strCommandLineArgs"> Specifies command line arguments of the executable specified by strTargetExecutablePath.</param>
	/// <param name="strTitle"> Task name that will be displayed in the Destination List.</param>
	/// <param name="strIconLocation"> Location of icon that will be displayed in the Destination List along with the title.</param>
	/// <param name="iIconIndex"> Icon index.</param>
	BOOL AddTask(LPCTSTR strTargetExecutablePath, LPCTSTR strCommandLineArgs, LPCTSTR strTitle, LPCTSTR strIconLocation, int iIconIndex);
	/// <summary>
	/// Adds items to the canonical Tasks category. </summary>
	/// <remarks>
	/// The instance of CJumpList accumulates specified tasks and adds them to the Destination List
	/// during CommitList.
	/// Task items will appear in a category at the bottom of the application's destination menu.
	/// This category takes precedence over all other categories when it is filled in the UI. </remarks>
	/// <param name="pShellLink"> Shell Link that represents a task to be added.</param>
	BOOL AddTask(IShellLink* pShellLink);

	/// <summary>
	/// Adds items to the canonical Tasks category. </summary>
	/// <remarks>
	/// The instance of CJumpList accumulates specified tasks and adds them to the Destination List
	/// during CommitList.
	/// Task items will appear in a category at the bottom of the application's destination menu.
	/// This category takes precedence over all other categories when it is filled in the UI. </remarks>
	/// <param name="pObjectCollection"> A collection of tasks to be added.</param>
	BOOL AddTasks(IObjectArray* pObjectCollection);

	/// <summary>
	/// Adds a separator between tasks. </summary>
	BOOL AddTaskSeparator();

	/// <summary>
	/// Removes all destinations that have been added to the current instance of CJumpList so far.</summary>
	/// <remarks>
	/// Call this function if you need to remove all destinations that have been added so far in the current session
	/// of destination list building and add other destinations again. If the internal ICustomDestinationList
	/// has been initialized, it's left alive.</remarks>
	void ClearAllDestinations();

	/// <summary>
	/// Removes all tasks and destinations that have been added to the current instance of CJumpList so far.</summary>
	/// <remarks>
	/// This method clears and releases all data and internal interfaces. </remarks>
	void ClearAll();

	/// <summary>
	/// Ends a list-building transaction and commits the reported list to the associated
	/// store (the registry in this case.) </summary>
	/// <remarks>
	/// The commit is atomic.  An error will be returned if the commit fails.  When CommitList is called,
	/// the current list of removed items will be cleaned up.
	/// Calling this method resets the object so that it does not have an active list-building transaction.
	/// To update the list, BeginList needs to be called again. </remarks>
	BOOL CommitList();

protected:
	IObjectCollection* CheckRemovedItems(IObjectCollection* pColl);
	void ReleaseObjectArray(IObjectArray* pColl, BOOL bReleaseItemsOnly = FALSE);
	IObjectCollection* GetObjectCollection(LPCTSTR lpcszCategoryName);

protected:
	CString		m_strAppID;
	BOOL		m_bInitialized;
	BOOL		m_bIsSupported;
	UINT		m_nMaxSlots;
	BOOL		m_bAutoCommit; // Automatically call commit in destructor

	::ATL::CComPtr<IObjectArray>           m_removedItemsPtr;
	::ATL::CComPtr<ICustomDestinationList> m_destListPtr;
	::ATL::CComPtr<IObjectCollection>      m_tasksPtr;

	CMap<CString, LPCTSTR, IObjectCollection*, IObjectCollection*> m_mapDestinations;
};

class CAppDestinations
{
public:
	/// <summary>
	/// The default constructor. </summary>
	CAppDestinations();

	/// <summary>
	/// The constructor. </summary>
	/// <param name="strAppID"> A string that specifies the Application User Model ID.</param>
	CAppDestinations(LPCTSTR lpszAppID);

	// operations

	/// <summary>
	/// Sets the Application User Model ID for the list that will be built. </summary>
	/// <param name="strAppID"> A string that specifies the Application User Model ID.</param>
	void SetAppID(LPCTSTR lpszAppID);

	/// <summary>
	/// Removes a destination. </summary>
	/// <param name="pUnk"> IShellItem or IShellLink that specifies a destination to remove.</param>
	BOOL RemoveDestination(IUnknown* pUnk);

	/// <summary>
	/// Removes all destinations. </summary>
	BOOL RemoveAllDestinations();

protected:
	BOOL Init();

protected:
	::ATL::CComPtr<IApplicationDestinations> m_appDests;
	CStringW  m_strAppID;
};

#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXADV_H__

/////////////////////////////////////////////////////////////////////////////
