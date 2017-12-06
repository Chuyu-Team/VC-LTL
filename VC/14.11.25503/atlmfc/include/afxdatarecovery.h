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

/////////////////////////////////////////////////////////////////////////////
// The CDataRecoveryHandler does the bulk of the work in all restart scenarios,
// whether the restart is due to the Restart Manager (like in a component upgrade
// scenario) or due to application recovery (like in a hang or crash scenario.
//
// The component upgrade scenario goes like this:
// The application is sent the WM_QUERYENDSESSION, WM_ENDSESSION and WM_CLOSE
// messages.  In the WM_CLOSE, the application calls the CDataRecoveryHandler
// to auto-save any dirty documents (via AutosaveAllDocumentInfo, which in turn
// calls AutosaveDocumentInfo for each open document).  This ensures both that
// the document is in the list of open documents and that the document is saved
// to a temporary auto-save location.  The list of open documents is then saved
// to the registry (via SaveOpenDocumentList), and if there is an auto-save 
// available this is associated with the registry entry for the document.  When
// the application is restarted, the list of documents is read from the registry
// (via ReadOpenDocumentList) and the documents are subsequently re-opened
// (via ReopenPreviousDocuments).  The user is then queried about restoring any
// available auto-saved documents (via QueryRestoreAutosavedDocuments) and if he
// indicates he wants this, the selected auto-saved documents are then opened and
// the original documents are closed (via RestoreAutosavedDocuments).
//
// The crash recovery scenario goes like this:
// A timer is set for the specified auto-save interval.  When the timer ticks, a
// flag is set which indicates that documents should be auto-saved on the next idle
// loop.  The application hangs or crashes and the application recovery callback is
// invoked.  The recovery callback calls the CDataRecoveryHandler to save the list
// of open documents to the registry (via SaveOpenDocumentList).  The behavior on
// restart is the same as in the above scenario.
//
/*============================================================================*/
// CDataRecoveryHandler class

class CDataRecoveryHandler : public CObject
{
// Construction
public:
	CDataRecoveryHandler(_In_ DWORD dwRestartManagerSupportFlags, _In_ int nAutosaveInterval);
	virtual ~CDataRecoveryHandler();

	virtual BOOL Initialize();

// Attributes
public:
	// These functions get and set the auto-save interval (in milliseconds).
	virtual int GetAutosaveInterval() const { return m_nAutosaveInterval; }
	virtual void SetAutosaveInterval(_In_ int nAutosaveInterval);

	// These functions get and set the auto-save directory where files will be auto-saved.
	virtual CString GetAutosavePath() const { return m_strAutosavePath; }
	virtual void SetAutosavePath(_In_ const CString &strAutosavePath) { m_strAutosavePath = strAutosavePath; }

	// These functions get and set the unique restart identifier which is used on the command line and in the registry.
	virtual CString GetRestartIdentifier() const { return m_strRestartIdentifier; }
	virtual void SetRestartIdentifier(_In_ const CString &strRestartIdentifier) { m_strRestartIdentifier = strRestartIdentifier; }

	// These functions get and set the flag that indicates that auto-save should be done in the current idle loop.
	virtual BOOL GetSaveDocumentInfoOnIdle() const { return m_bSaveDocumentInfoOnIdle; }
	virtual void SetSaveDocumentInfoOnIdle(_In_ BOOL bSaveOnIdle) { m_bSaveDocumentInfoOnIdle = bSaveOnIdle; }

	// These functions get and set the flag that indicates that the shut-down was invoked by the Restart Manager (WM_QUERYENDSESSION, etc).
	virtual BOOL GetShutdownByRestartManager() const { return m_bShutdownByRestartManager; }
	virtual void SetShutdownByRestartManager(_In_ BOOL bShutdownByRestartManager) { m_bShutdownByRestartManager = bShutdownByRestartManager; }

// Operations
public:
	// These functions save the open document list to the registry and read it back in from the registry.
	virtual BOOL SaveOpenDocumentList();  // save to registry
	virtual BOOL ReadOpenDocumentList();  // load from registry

	// This function ensures that the document is in the open document list and auto-saves the document if it is dirty.
	virtual BOOL AutosaveDocumentInfo(_In_ CDocument *pDocument, _In_ BOOL bResetModifiedFlag = TRUE);

	// This function iterates through all open documents and calls AutosaveDocumentInfo (above) on each.
	virtual BOOL AutosaveAllDocumentInfo();

	// This function creates information about the document in the open document list (usually after a document is opened or a new document is created).
	virtual BOOL CreateDocumentInfo(_In_ CDocument *pDocument);

	// This function updates information about the document in the open document list after the document is saved (may be a new filename).
	virtual BOOL UpdateDocumentInfo(_In_ CDocument *pDocument);

	// This function removes information about the document from the open document list (usually after the document is closed).
	virtual BOOL RemoveDocumentInfo(_In_ CDocument *pDocument);

	// This function attempts to delete the auto-saved file, and if unsuccessful, keeps the filename in a list for later deletion.
	virtual BOOL DeleteAutosavedFile(_In_ const CString &strAutosavedFile);

	// This function deletes all auto-saved files (usually on normal application close, when all documents have been properly saved).
	virtual BOOL DeleteAllAutosavedFiles();

	// These functions reopen the previously open documents, prompt for restoration and restore any associated auto-saved documents.
	virtual BOOL ReopenPreviousDocuments();
	virtual void QueryRestoreAutosavedDocuments();
	virtual BOOL RestoreAutosavedDocuments();

	// This function creates the document list name for the document.  This name is used in the m_map*DocName* maps.
	virtual CString GetDocumentListName(_In_ CDocument *pDocument) const;

	// This function creates the auto-save filename based on the document name.  This name is used in the m_mapDocNameToAutosaveName map.
	virtual CString GenerateAutosaveFileName(_In_ const CString &strDocumentName) const;

	// This function creates the title for the recovered document.  This title is used as the title in the UI until the document is saved.
	virtual CString GetRecoveredDocumentTitle(_In_ const CString &strDocumentTitle) const;

	// This function obtains the normal title for the recovered document.  This title is used in the UI (prompt for save, etc.).
	virtual CString GetNormalDocumentTitle(_In_ CDocument *pDocument) const;

protected:
	CMap<CString, LPCTSTR, CString, LPCTSTR>        m_mapDocNameToAutosaveName;   // maps document name to auto-save filename: list of currently open documents and associated auto-save file (if any)
	CMap<CString, LPCTSTR, CDocument *, CDocument*> m_mapDocNameToDocumentPtr;    // maps document name to CDocument *: list of documents to close after auto-saved versions are opened)
	CMap<CString, LPCTSTR, bool, bool>              m_mapDocNameToRestoreBool;    // maps document name to true/false: list of documents and whether to restore auto-save or not
	CMap<CDocument *, CDocument*, CString, LPCTSTR> m_mapDocumentPtrToDocName;    // maps CDocument * to document name: associates document with name used in other maps
	CMap<CDocument *, CDocument*, CString, LPCTSTR> m_mapDocumentPtrToDocTitle;   // maps CDocument * to document title (without "recovered"): for use in save prompt, etc.

	CStringList        m_lstAutosavesToDelete;  // list of auto-saved files which could not be deleted; retry delete on this list at shutdown time.

	CString m_strAutosavePath;       // path on disk where auto-saved documents will be stored
	CString m_strRestartIdentifier;  // string representation of GUID (used for storing data in registry)

	DWORD   m_dwRestartManagerSupportFlags; // what aspects of Restart Manager support does the application want?  (See AFX_RESTART_MANAGER_ flags in afxwin.h.)
	int     m_nAutosaveInterval;            // timer interval between auto-saves (in milliseconds)
	BOOL    m_bSaveDocumentInfoOnIdle;      // should auto-save be performed in the next idle loop?
	BOOL    m_bShutdownByRestartManager;    // is the application being shut down by the Restart Manager?
	BOOL    m_bRestoringPreviousOpenDocs;   // is the data recovery handler currently reopening previously opened documents?

	UINT_PTR m_nTimerID;                    // periodic auto-save timer identifier
};
