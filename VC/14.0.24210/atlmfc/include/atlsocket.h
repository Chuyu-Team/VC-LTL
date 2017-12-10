// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATL_SOCKET__
#define __ATL_SOCKET__

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#pragma warning(push)
#pragma warning(disable: 4191) // unsafe conversion from 'functionptr1' to 'functionptr2'

#include <winsock2.h>
#include <mswsock.h>
#include <wtypes.h>
#include <atlconv.h>
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable : 4127 4706)
#if !defined(_WIN32_WINNT)
/* psdk prefast noise */
#pragma warning(disable : 6011)
#endif
#include <ws2tcpip.h>
#pragma warning(pop)

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#if (NTDDI_VERSION < NTDDI_WINXPSP2) || (_WIN32_WINNT < 0x0501)
#define ADDRINFOT addrinfo
#define GetAddrInfo getaddrinfo
#define FreeAddrInfo freeaddrinfo
#endif

#pragma pack(push,_ATL_PACKING)
namespace ATL
{
class CSocketAddr;

////////////////////////////////////////////////////////////////////////
// class CSocketAddr
//
// Description:
// This class provides an abstraction over an internet address. It provides
// an IP version agnostic way to look up network addresses for use with
// Windows sockets API functions and Socket wrappers in libraries
// The members of this class that are used to look up network addresses
// use the getaddrinfo Win32 API, which is an IP version agnostic function
// for retrieving network addresses. This class can find both IPv4 and
// IPv6 network addresses.
////////////////////////////////////////////////////////////////////////
class CSocketAddr
{
public:
	// Construction/Destruction
	CSocketAddr() throw();
	virtual ~CSocketAddr() throw();

	// Operations
	int FindAddr(
		_In_z_ LPCTSTR szHost, 				// Host name or dotted IP address
		_In_z_ LPCTSTR szPortOrServiceName,	// Port number or name of service on host
		_In_ int flags,						// 0 or combination of AI_PASSIVE, AI_CANONNAME or AI_NUMERICHOST
		_In_ int addr_family,				// Address family (such as PF_INET)
		_In_ int sock_type,					// Socket type (such as SOCK_STREAM)
		_In_ int ai_proto);					// Protocol (such as IPPROTO_IP or IPPROTO_IPV6)

	int FindAddr(
		_In_z_ LPCTSTR szHost, 			// Host name or dotted IP address
		_In_ int nPortNo,				// Port number
		_In_ int flags,					// 0 or combination of AI_PASSIVE, AI_CANONNAME or AI_NUMERICHOST
		_In_ int addr_family,			// Address family (such as PF_INET)
		_In_ int sock_type,				// Socket type (such as SOCK_STREAM)
		_In_ int ai_proto) throw();		// Protocol (such as IPPROTO_IP or IPPROTO_IPV6)

	int FindINET4Addr(
		_In_z_ LPCTSTR szHost, 						// Host name
		_In_ int nPortNo, 							// Port number
		_In_ int flags = 0, 						// 0 or combination of AI_PASSIVE, AI_CANONNAME or AI_NUMERICHOST
		_In_ int sock_type = SOCK_STREAM) throw(); 	// Socket type (such as SOCK_STREAM or SOCK_DGRAM)

	int FindINET6Addr(
		_In_z_ LPCTSTR szHost, 						// Host name
		_In_ int nPortNo, 							// Port number
		_In_ int flags = 0, 						// 0 or combination of AI_PASSIVE, AI_CANONNAME or AI_NUMERICHOST
		_In_ int sock_type = SOCK_STREAM) throw(); 	// Socket type (such as SOCK_STREAM or SOCK_DGRAM)

	ADDRINFOT* const GetAddrInfoList() const;
	ADDRINFOT* const GetAddrInfo(_In_ int nIndex = 0) const;

	// Implementation
private:
	ADDRINFOT *m_pAddrs;
};


////////////////////////////////////////////////////////////////////////////
// CSocketAddr implmenetation.
////////////////////////////////////////////////////////////////////////////

inline CSocketAddr::CSocketAddr() throw()
{
	m_pAddrs = NULL;
}

inline CSocketAddr::~CSocketAddr() throw()
{
	if (m_pAddrs != NULL)
	{
		FreeAddrInfo(m_pAddrs);
		m_pAddrs = NULL;
	}
}

inline int CSocketAddr::FindAddr(
	_In_z_ LPCTSTR szHost,
	_In_z_ LPCTSTR szPortOrServiceName,
	_In_ int flags,
	_In_ int addr_family,
	_In_ int sock_type,
	_In_ int ai_proto) throw()
{
	if (m_pAddrs)
	{
		FreeAddrInfo(m_pAddrs);
		m_pAddrs = NULL;
	}

	ADDRINFOT hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_flags = flags;
	hints.ai_family = addr_family;
	hints.ai_socktype = sock_type;
	hints.ai_protocol = ai_proto;
#if _WIN32_WINNT < 0x0502
#ifdef _UNICODE
	USES_CONVERSION_EX;
	const char * pszHost = W2CA_EX(szHost, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
	const char * pszPortOrServiceName = W2CA_EX(szPortOrServiceName, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
	if(pszHost == NULL || pszPortOrServiceName == NULL )
	{
		WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
		return SOCKET_ERROR;
	}
#else
	const char * pszHost = szHost;
	const char * pszPortOrServiceName = szPortOrServiceName;
#endif
	return ::GetAddrInfo(pszHost, pszPortOrServiceName, &hints, &m_pAddrs);
#else
	return ::GetAddrInfo(szHost, szPortOrServiceName, &hints, &m_pAddrs);
#endif
}

inline int CSocketAddr::FindAddr(
	_In_z_ LPCTSTR szHost,
	_In_ int nPortNo,
	_In_ int flags,
	_In_ int addr_family,
	_In_ int sock_type,
	_In_ int ai_proto) throw()
{
	// convert port number to string
	TCHAR szPort[12];
	if(::_itot_s(nPortNo, szPort, _countof(szPort), 10))
	{
		WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
		return SOCKET_ERROR;
	}

	return FindAddr(szHost, szPort, flags, addr_family, sock_type, ai_proto);
}

inline int CSocketAddr::FindINET4Addr(
	_In_z_ LPCTSTR szHost,
	_In_ int nPortNo,
	_In_ int flags /* = 0 */,
	_In_ int sock_type /* = SOCK_STREAM */) throw()
{
	// convert port number to string
	TCHAR szPort[12];
	if(::_itot_s(nPortNo, szPort, _countof(szPort), 10))
	{
		WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
		return SOCKET_ERROR;
	}
	return FindAddr(szHost, szPort, flags, PF_INET, sock_type, IPPROTO_IP);
}

inline int CSocketAddr::FindINET6Addr(
	_In_z_ LPCTSTR szHost,
	_In_ int nPortNo,
	_In_ int flags /* = 0 */,
	_In_ int sock_type /* = SOCK_STREAM */) throw()
{
	// convert port number to string
	TCHAR szPort[12];
	if(::_itot_s(nPortNo, szPort, _countof(szPort), 10))
	{
		WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
		return SOCKET_ERROR;
	}

	// TEMP (alecont): With the new PSDK, IPPROTO_IPV6 is available only if _WIN32_WINNT >= 0x0501
	return FindAddr(szHost, szPort, flags, PF_INET6, sock_type, /*IPPROTO_IPV6*/ 41);
}

inline ADDRINFOT* const CSocketAddr::GetAddrInfoList() const
{
	return m_pAddrs;
}

inline ADDRINFOT* const CSocketAddr::GetAddrInfo(_In_ int nIndex /* = 0 */) const
{
	if (!m_pAddrs)
		return NULL;
	ADDRINFOT *pAI = m_pAddrs;
	while (nIndex > 0 && pAI != NULL)
	{
		pAI = pAI->ai_next;
		nIndex --;
	}
	return pAI;
}


}; // namespace ATL

#pragma pack(pop)
#pragma warning(pop)

#endif __ATL_SOCKET__

