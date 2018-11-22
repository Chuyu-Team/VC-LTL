// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXSOCK_H__
#define __AFXSOCK_H__

#pragma once

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#include <atlsocket.h>

#ifndef _WINSOCK2API_
#ifdef _WINSOCKAPI_
	#error MFC requires use of Winsock2.h
#endif


#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef _AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////
// Win32 libraries

#pragma comment(lib, "wsock32.lib")

#endif //!_AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// AFXSOCK - MFC support for Windows Sockets

// Classes declared in this file

	// CObject
		class CAsyncSocket; // Async Socket implementation and
							// base class for Synchronous Socket
			class CSocket;  // Synchronous Socket

	// CFile
		class CSocketFile; // Used with CSocket and CArchive for
						   // streaming objects on sockets.

/////////////////////////////////////////////////////////////////////////////

// AFXDLL support
#undef AFX_DATA
#define AFX_DATA AFX_NET_DATA

/*============================================================================*/
// CSocketWnd -- internal use only
//  Implementation for sockets notification callbacks.
//  Future versions of MFC may or may not include this exact class.

class CSocketWnd : public CWnd
{
// Construction
public:
	CSocketWnd();

protected:
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnSocketDead(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CAsyncSocket

class CAsyncSocket : public CObject
{
	DECLARE_DYNAMIC(CAsyncSocket);
private:
	CAsyncSocket(const CAsyncSocket& rSrc);    // no implementation
	void operator=(const CAsyncSocket& rSrc);  // no implementation

// Construction
public:
	CAsyncSocket();
	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		LPCTSTR lpszSocketAddress = NULL);

	BOOL CreateEx(ADDRINFOT* pAI, 
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

// Attributes
public:
	SOCKET m_hSocket;

	operator SOCKET() const;
	BOOL Attach(SOCKET hSocket, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
	SOCKET Detach();

	BOOL GetPeerName(CString& rPeerAddress, UINT& rPeerPort);
	BOOL GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);
	BOOL GetPeerNameEx(CString& rPeerAddress, UINT& rPeerPort);	

	BOOL GetSockName(CString& rSocketAddress, UINT& rSocketPort);
	BOOL GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);
	BOOL GetSockNameEx(CString& rSocketAddress, UINT& rSocketPort);

	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,
		int nOptionLen, int nLevel = SOL_SOCKET);
	BOOL GetSockOpt(int nOptionName, void* lpOptionValue,
		int* lpOptionLen, int nLevel = SOL_SOCKET);

	static CAsyncSocket* PASCAL FromHandle(SOCKET hSocket);
	static int PASCAL GetLastError();

// Operations
public:

	virtual BOOL Accept(CAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);

	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind (const SOCKADDR* lpSockAddr, int nSockAddrLen);
	BOOL BindEx(ADDRINFOT* pAI);

	virtual void Close();

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	BOOL ConnectEx(ADDRINFOT* pAI);

	BOOL IOCtl(long lCommand, DWORD* lpArgument);

	BOOL Listen(int nConnectionBacklog=5);

	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

	int ReceiveFrom(void* lpBuf, int nBufLen,
		CString& rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	int ReceiveFrom(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);
	int ReceiveFromEx(void* lpBuf, int nBufLen,
		CString& rSocketAddress, UINT& rSocketPort, int nFlags = 0);

	enum { receives = 0, sends = 1, both = 2 };
	BOOL ShutDown(int nHow = sends);

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendTo(const void* lpBuf, int nBufLen,
		UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
	int SendTo(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);
	int SendToEx(const void* lpBuf, int nBufLen,
		UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);

	BOOL AsyncSelect(long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

// Overridable callbacks
protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

// Implementation
public:
	virtual ~CAsyncSocket();

	static CAsyncSocket* PASCAL LookupHandle(SOCKET hSocket, BOOL bDead = FALSE);
	static void PASCAL AttachHandle(SOCKET hSocket, CAsyncSocket* pSocket, BOOL bDead = FALSE);
	static void PASCAL DetachHandle(SOCKET hSocket, BOOL bDead = FALSE);
	static void PASCAL KillSocket(SOCKET hSocket, CAsyncSocket* pSocket);
	static void PASCAL DoCallBack(WPARAM wParam, LPARAM lParam);

	BOOL Socket(int nSocketType=SOCK_STREAM, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	friend class CSocketWnd;

	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);
};

/*============================================================================*/
// CSocket

class CSocket : public CAsyncSocket
{
	DECLARE_DYNAMIC(CSocket);
private:
	CSocket(const CSocket& rSrc);         // no implementation
	void operator=(const CSocket& rSrc);  // no implementation

// Construction
public:
	CSocket();
	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		LPCTSTR lpszSocketAddress = NULL);

// Attributes
public:
	BOOL IsBlocking();
	static CSocket* PASCAL FromHandle(SOCKET hSocket);
	BOOL Attach(SOCKET hSocket);

// Operations
public:
	void CancelBlockingCall();

// Overridable callbacks
protected:
	virtual BOOL OnMessagePending();

// Implementation
public:
	int m_nTimeOut;

	virtual ~CSocket();

	static int PASCAL ProcessAuxQueue();

	virtual BOOL Accept(CAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);
	virtual void Close();
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendChunk(const void* lpBuf, int nBufLen, int nFlags);

protected:
	friend class CSocketWnd;

	BOOL* m_pbBlocking;
	int m_nConnectError;

	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);

	static void PASCAL AuxQueueAdd(UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL PumpMessages(UINT uStopFlag);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/*============================================================================*/
// CSocketFile

class CSocketFile : public CFile
{
	DECLARE_DYNAMIC(CSocketFile)
public:
//Constructors
	explicit CSocketFile(CSocket* pSocket, BOOL bArchiveCompatible = TRUE);

// Implementation
public:
	CSocket* m_pSocket;
	BOOL m_bArchiveCompatible;

	virtual ~CSocketFile();

	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount, void** ppBufStart,
		void** ppBufMax);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual void Close();

// Unsupported APIs
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
	virtual CFile* Duplicate() const;
	virtual ULONGLONG GetPosition() const;
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual ULONGLONG GetLength() const;
	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void Flush();
	virtual void Abort();
};

/////////////////////////////////////////////////////////////////////////////
// Global functions

BOOL AFXAPI AfxSocketInit(WSADATA* lpwsaData = NULL);
void AFXAPI AfxSocketTerm();

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXSOCK_INLINE AFX_INLINE
#include <afxsock.inl>
#undef _AFXSOCK_INLINE
#endif

inline BOOL CAsyncSocket::CreateEx(ADDRINFOT* pAI, long lEvent)
{
	if (pAI == NULL)
	{
		WSASetLastError(WSAEINVAL);
		return FALSE;
	}
	return Socket(pAI->ai_socktype, lEvent, pAI->ai_protocol, pAI->ai_family);
}

inline BOOL CAsyncSocket::BindEx(ADDRINFOT* pAI)
{
	if (pAI == NULL)
	{
		WSASetLastError(WSAEINVAL);
		return FALSE;
	}
	return Bind((SOCKADDR*)pAI->ai_addr, (int)pAI->ai_addrlen);
}

inline BOOL CAsyncSocket::ConnectEx(ADDRINFOT* pAI)
{
	if (pAI == NULL)
	{
		WSASetLastError(WSAEINVAL);
		return FALSE;
	}
	return Connect((SOCKADDR*)pAI->ai_addr, (int)pAI->ai_addrlen);
}

inline BOOL CAsyncSocket::GetPeerNameEx(CString& rPeerAddress, UINT& rPeerPort)
{
	SOCKADDR_STORAGE sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = GetPeerName((SOCKADDR*)&sockAddr, &nSockAddrLen);
	if (bResult)
	{
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		wchar_t szName[NI_MAXHOST];
		BOOL bResult2 = GetNameInfoW((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#else
		char szName[NI_MAXHOST];
		BOOL bResult2 = getnameinfo((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		if (!bResult2)
		{
			rPeerAddress = szName;
			rPeerPort = ntohs(SS_PORT(&sockAddr));
		}
	}
	return bResult;
}

inline BOOL CAsyncSocket::GetSockNameEx(CString& rSocketAddress, UINT& rSocketPort)
{
	SOCKADDR_STORAGE sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = GetSockName((SOCKADDR*)&sockAddr, &nSockAddrLen);
	if (bResult)
	{
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		wchar_t szName[NI_MAXHOST];
		BOOL bResult2 = GetNameInfoW((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#else
		char szName[NI_MAXHOST];
		BOOL bResult2 = getnameinfo((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		if (!bResult2)
		{
			rSocketAddress = szName;
			rSocketPort = ntohs(SS_PORT(&sockAddr));
		}
	}
	return bResult;
}

inline int CAsyncSocket::ReceiveFromEx(void* lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort, int nFlags)
{
	SOCKADDR_STORAGE sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = ReceiveFrom(lpBuf, nBufLen, (SOCKADDR*)&sockAddr, &nSockAddrLen, nFlags);
	if (nResult != SOCKET_ERROR)
	{
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		wchar_t szName[NI_MAXHOST];
		BOOL bResult = GetNameInfoW((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#else
		char szName[NI_MAXHOST];
		BOOL bResult = getnameinfo((SOCKADDR*)&sockAddr, nSockAddrLen, szName, NI_MAXHOST, NULL, 0, 0);
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
		if (!bResult)
		{
			rSocketAddress = szName;
			rSocketPort = ntohs(SS_PORT(&sockAddr));
		}
	}
	return nResult;
}


inline int CAsyncSocket::SendToEx(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	if (lpszHostAddress == NULL)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}
	
	SOCKADDR_STORAGE sockAddrSelf;
	memset(&sockAddrSelf, 0, sizeof(sockAddrSelf));

	int nSockAddrSelfLen = sizeof(sockAddrSelf);

	BOOL bResult = GetSockName((SOCKADDR*)&sockAddrSelf, &nSockAddrSelfLen);
	if (!bResult)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;		
	}
	
	int nSocketType;
	int nSocketTypeLen = int(sizeof(int));
	if (!GetSockOpt(SO_TYPE, &nSocketType, &nSocketTypeLen))
	{
		return SOCKET_ERROR;
	}

	ATL::CSocketAddr sockAddr;
	int nRet = sockAddr.FindAddr(lpszHostAddress, nHostPort, 0, sockAddrSelf.ss_family, nSocketType, 0);
	if (nRet != 0)
	{
		WSASetLastError(nRet);
		return SOCKET_ERROR;
	}
	
	ADDRINFOT *p = sockAddr.GetAddrInfo();

	return SendTo(lpBuf, nBufLen, p->ai_addr, (int)p->ai_addrlen, nFlags);
}

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXSOCK_H__

/////////////////////////////////////////////////////////////////////////////
