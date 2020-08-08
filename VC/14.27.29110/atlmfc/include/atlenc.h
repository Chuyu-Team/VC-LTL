// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLENC_H__
#define __ATLENC_H__

#pragma once

#include <atlbase.h>
#include <stdio.h>

#pragma pack(push,_ATL_PACKING)
namespace ATL {

#ifndef ATL_UUENCODE_END
#define ATL_UUENCODE_END "end\r\n"
#endif//ATL_UUENCODE_END

//Not including CRLFs
//NOTE: For BASE64 and UUENCODE, this actually
//represents the amount of unencoded characters
//per line
#define ATLSMTP_MAX_QP_LINE_LENGTH       76
#define ATLSMTP_MAX_BASE64_LINE_LENGTH   57
#define ATLSMTP_MAX_UUENCODE_LINE_LENGTH 45


//=======================================================================
// Base64Encode/Base64Decode
// compliant with RFC 2045
//=======================================================================
//
#define ATL_BASE64_FLAG_NONE	0
#define ATL_BASE64_FLAG_NOPAD	1
#define ATL_BASE64_FLAG_NOCRLF  2

inline int Base64EncodeGetRequiredLength(
	_In_ int nSrcLen,
	_In_ DWORD dwFlags = ATL_BASE64_FLAG_NONE)
{
	__int64 nSrcLen4=static_cast<__int64>(nSrcLen)*4;
	ATLENSURE(nSrcLen4 <= INT_MAX);

	int nRet = static_cast<int>(nSrcLen4/3);

	if ((dwFlags & ATL_BASE64_FLAG_NOPAD) == 0)
		nRet += nSrcLen % 3;

	int nCRLFs = nRet / 76 + 1;
	int nOnLastLine = nRet % 76;

	if (nOnLastLine)
	{
		if (nOnLastLine % 4)
			nRet += 4-(nOnLastLine % 4);
	}

	nCRLFs *= 2;

	if ((dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
		nRet += nCRLFs;

	return nRet;
}

inline int Base64DecodeGetRequiredLength(_In_ int nSrcLen) throw()
{
	return nSrcLen;
}

ATLPREFAST_SUPPRESS(6054)
_Success_(return != FALSE)
inline BOOL Base64Encode(
	_In_reads_(nSrcLen) const BYTE *pbSrcData,
	_In_ int nSrcLen,
	_When_(*pnDestLen > 0, _Out_writes_to_(*pnDestLen, *pnDestLen)) LPSTR szDest,
	_Inout_ int *pnDestLen,
	_In_ DWORD dwFlags = ATL_BASE64_FLAG_NONE) throw()
{
	static const char s_chBase64EncodingTable[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',	'h',
		'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
		'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

	if (!pbSrcData || !szDest || !pnDestLen)
	{
		return FALSE;
	}

	if(*pnDestLen < Base64EncodeGetRequiredLength(nSrcLen, dwFlags))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	int nWritten( 0 );
	int nLen1( (nSrcLen/3)*4 );
	int nLen2( nLen1/76 );
	int nLen3( 19 );

	for (int i=0; i<=nLen2; i++)
	{
		if (i==nLen2)
			nLen3 = (nLen1%76)/4;

		for (int j=0; j<nLen3; j++)
		{
			DWORD dwCurr(0);
			for (int n=0; n<3; n++)
			{
				dwCurr |= *pbSrcData++;
				dwCurr <<= 8;
			}
			for (int k=0; k<4; k++)
			{
				BYTE b = (BYTE)(dwCurr>>26);
				*szDest++ = s_chBase64EncodingTable[b];
				dwCurr <<= 6;
			}
		}
		nWritten+= nLen3*4;

		if ((dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
		{
			*szDest++ = '\r';
			*szDest++ = '\n';
			nWritten+= 2;
		}
	}

	if (nWritten && (dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
	{
		szDest-= 2;
		nWritten -= 2;
	}

	nLen2 = (nSrcLen%3) ? (nSrcLen%3 + 1) : 0;
	if (nLen2)
	{
		DWORD dwCurr(0);
		for (int n=0; n<3; n++)
		{
			if (n<(nSrcLen%3))
				dwCurr |= *pbSrcData++;
			dwCurr <<= 8;
		}
		for (int k=0; k<nLen2; k++)
		{
			BYTE b = (BYTE)(dwCurr>>26);
			*szDest++ = s_chBase64EncodingTable[b];
			dwCurr <<= 6;
		}
		nWritten+= nLen2;
		if ((dwFlags & ATL_BASE64_FLAG_NOPAD)==0)
		{
			nLen3 = nLen2 ? 4-nLen2 : 0;
			for (int j=0; j<nLen3; j++)
			{
				*szDest++ = '=';
			}
			nWritten+= nLen3;
		}
	}

	*pnDestLen = nWritten;
	return TRUE;
}
ATLPREFAST_UNSUPPRESS()

inline int DecodeBase64Char(_In_ unsigned int ch) throw()
{
	// returns -1 if the character is invalid
	// or should be skipped
	// otherwise, returns the 6-bit code for the character
	// from the encoding table
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 0;	// 0 range starts at 'A'
	if (ch >= 'a' && ch <= 'z')
		return ch - 'a' + 26;	// 26 range starts at 'a'
	if (ch >= '0' && ch <= '9')
		return ch - '0' + 52;	// 52 range starts at '0'
	if (ch == '+')
		return 62;
	if (ch == '/')
		return 63;
	return -1;
}

inline BOOL Base64Decode(
	_In_reads_z_(nSrcLen) LPCSTR szSrc,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) BYTE *pbDest,
	_Inout_ int *pnDestLen) throw()
{
	// walk the source buffer
	// each four character sequence is converted to 3 bytes
	// CRLFs and =, and any characters not in the encoding table
	// are skiped

	if (szSrc == NULL || pnDestLen == NULL)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	LPCSTR szSrcEnd = szSrc + nSrcLen;
	int nWritten = 0;

	BOOL bOverflow = (pbDest == NULL) ? TRUE : FALSE;

	while (szSrc < szSrcEnd &&(*szSrc) != 0)
	{
		DWORD dwCurr = 0;
		int i;
		int nBits = 0;
		for (i=0; i<4; i++)
		{
			if (szSrc >= szSrcEnd)
				break;
			int nCh = DecodeBase64Char(*szSrc);
			szSrc++;
			if (nCh == -1)
			{
				// skip this char
				i--;
				continue;
			}
			dwCurr <<= 6;
			dwCurr |= nCh;
			nBits += 6;
		}

		if(!bOverflow && nWritten + (nBits/8) > (*pnDestLen))
			bOverflow = TRUE;

		// dwCurr has the 3 bytes to write to the output buffer
		// left to right
		dwCurr <<= 24-nBits;
		for (i=0; i<nBits/8; i++)
		{
			if(!bOverflow)
			{
				*pbDest = (BYTE) ((dwCurr & 0x00ff0000) >> 16);
				pbDest++;
			}
			dwCurr <<= 8;
			nWritten++;
		}

	}

	*pnDestLen = nWritten;

	if(bOverflow)
	{
		if(pbDest != NULL)
		{
			ATLASSERT(FALSE);
		}

		return FALSE;
	}

	return TRUE;
}


//=======================================================================
// UUEncode/UUDecode
// compliant with POSIX P1003.2b/D11
//=======================================================================
//
//Flag to determine whether or not we should encode the header
#define ATLSMTP_UUENCODE_HEADER 1

//Flag to determine whether or not we should encode the end
#define ATLSMTP_UUENCODE_END    2

//Flag to determine whether or not we should do data stuffing
#define ATLSMTP_UUENCODE_DOT    4

//The the (rough) required length of the uuencoded stream based
//on input of length nSrcLen
inline int UUEncodeGetRequiredLength(_In_ int nSrcLen)
{
	__int64 nRet64=static_cast<__int64>(nSrcLen)*4/3;
	nRet64 += 3*(nSrcLen/ATLSMTP_MAX_UUENCODE_LINE_LENGTH);
	nRet64 += 12+MAX_PATH; // "begin" statement
	nRet64 += 8; // "end" statement
	nRet64 += 1; // dot-stuffing
	ATLENSURE(nRet64 <= INT_MAX && nRet64 >= INT_MIN);
	int nRet = static_cast<int>(nRet64);
	return nRet;
}

//Get the decode required length
inline int UUDecodeGetRequiredLength(_In_ int nSrcLen) throw()
{
	return nSrcLen;
}

#define UUENCODE(ch) ((ch) ? ((ch) & 0x3F ) + ' ' : '`')

#ifdef _DEBUG
inline void UUEncodeCheckFilename(_In_z_ const char* pcszFilename)
{
	// It's the callers responsibility to supply a filename that can readily be
	// used for the UUEncode header. We do a basic check here because there does
	// not appear to be a real specification.
	for (const char* pc = pcszFilename; *pc != 0; ++pc)
	{
		if (*pc >= 32 && *pc < 127)
			;
		else
		{
			ATLASSERT( "Is *pc a character suitable for filenames in UUEncode header" && 0 );
			break;
		}
	}
}
#endif // def _DEBUG

namespace Checked {
template <typename T>
inline
T* EnsureNotBeyond(T* p, T* end)
{
	ATLENSURE(p < end);
	return p;
}
} // namespace Checked

//encode a chunk of data
// this warning is bogus
// Invalid data: accessing 'szDest', the readable size is 'sizeof(("end??"))-1' bytes, but '15' bytes might be read
ATLPREFAST_SUPPRESS(6054 6385)
inline BOOL UUEncode(
	_In_reads_bytes_(nSrcLen) const BYTE* pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) LPSTR szDestBegin,
	_Inout_ int* pnDestLen,
	_In_opt_z_ LPCTSTR lpszFile = _T("file"),
	_In_ DWORD dwFlags = 0) throw()
{
	//The UUencode character set
	static const char s_chUUEncodeChars[64] = {
		'`','!','"','#','$','%','&','\'','(',')','*','+',',',
		'-','.','/','0','1','2','3','4','5','6','7','8','9',
		':',';','<','=','>','?','@','A','B','C','D','E','F',
		'G','H','I','J','K','L','M','N','O','P','Q','R','S',
		'T','U','V','W','X','Y','Z','[','\\',']','^','_'
	};

	if (!pbSrcData || !szDestBegin || !pnDestLen)
	{
		return FALSE;
	}

	ATLASSERT(*pnDestLen >= UUEncodeGetRequiredLength(nSrcLen));

	char* szDest = szDestBegin;
	char* const szDestEnd = szDestBegin + *pnDestLen;

	using Checked::EnsureNotBeyond;

	ATLENSURE(szDestEnd >= szDestBegin );

	BYTE ch1 = 0, ch2 = 0, ch3 = 0;
	int nTotal = 0, nCurr = 0, nWritten = 0, nCnt = 0;

	//if ATL_UUENCODE_HEADER
	//header
	if (dwFlags & ATLSMTP_UUENCODE_HEADER)
	{
		if (!lpszFile)
		{
			return FALSE;
		}

		{
			CT2CAEX<MAX_PATH+1> narrowFilename( lpszFile );
#ifdef _DEBUG
			UUEncodeCheckFilename(narrowFilename);
#endif
			//default permission is 666
			nWritten = sprintf_s(szDest, *pnDestLen, "begin 666 %s\r\n", static_cast<LPCSTR>(narrowFilename));
		}
		if (nWritten < 0)
		{
			if(*pnDestLen>0)
			{
				szDest[0]='\0';
			}
			return FALSE;
		}

		szDest += nWritten;
	}

	//while we haven't reached the end of the data
	while (nTotal < nSrcLen)
	{
		//If the amount of data is greater than MAX_UUENCODE_LINE_LENGTH
		//cut off at MAX_UUENCODE_LINE_LENGTH
		nCurr = __min(nSrcLen-nTotal, ATLSMTP_MAX_UUENCODE_LINE_LENGTH);
		*EnsureNotBeyond(szDest,szDestEnd) = UUENCODE((unsigned char)(nCurr));
		nCurr++;
		nCnt = 1;

		nWritten++;
		//if we need to stuff an extra dot (e.g. when we are sending via SMTP), do it
		if ((dwFlags & ATLSMTP_UUENCODE_DOT) && *szDest == '.')
		{
			*EnsureNotBeyond(++szDest,szDestEnd) = '.';
			nWritten++;
		}
		szDest++;
		while (nCnt < nCurr)
		{
			//Set to 0 in the uuencoding alphabet
			ch1 = ch2 = ch3 = ' ';
			ch1 = *pbSrcData++;
			nCnt++;
			nTotal++;
			if (nTotal < nSrcLen)
			{
				ch2 = *pbSrcData++;
				nCnt++;
				nTotal++;
			}
			if (nTotal < nSrcLen)
			{
				ch3 = *pbSrcData++;
				nCnt++;
				nTotal++;
			}

			//encode the first 6 bits of ch1
			*EnsureNotBeyond(szDest++,szDestEnd) = s_chUUEncodeChars[(ch1 >> 2) & 0x3F];
			//encode the last 2 bits of ch1 and the first 4 bits of ch2
			*EnsureNotBeyond(szDest++,szDestEnd)  = s_chUUEncodeChars[((ch1 << 4) & 0x30) | ((ch2 >> 4) & 0x0F)];
			//encode the last 4 bits of ch2 and the first 2 bits of ch3
			*EnsureNotBeyond(szDest++,szDestEnd)  = s_chUUEncodeChars[((ch2 << 2) & 0x3C) | ((ch3 >> 6) & 0x03)];
			//encode the last 6 bits of ch3
			*EnsureNotBeyond(szDest++,szDestEnd) = s_chUUEncodeChars[ch3 & 0x3F];
			nWritten += 4;
		}
		//output a CRLF
		*EnsureNotBeyond(szDest++,szDestEnd)  = '\r'; 
		*EnsureNotBeyond(szDest++,szDestEnd)  = '\n'; 
		nWritten += 2;
	}

	//if we need to encode the end, do it
	if (dwFlags & ATLSMTP_UUENCODE_END)
	{
		*EnsureNotBeyond(szDest++,szDestEnd) = '`'; 
		*EnsureNotBeyond(szDest++,szDestEnd) = '\r';
		*EnsureNotBeyond(szDest++,szDestEnd) = '\n';
		nWritten += 3;

		Checked::memcpy_s(szDest, *pnDestLen-nWritten, ATL_UUENCODE_END, sizeof(ATL_UUENCODE_END)-1);
		nWritten += sizeof("end\r\n")-1;
	}
	*pnDestLen = nWritten;
	return TRUE;
}
ATLPREFAST_UNSUPPRESS()

#define UUDECODE(ch) (((ch) == '`') ? '\0' : ((ch) - ' ') & 0x3F)

inline BOOL UUDecode(
	_In_reads_bytes_(nSrcLen) BYTE* pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) BYTE* pbDest,
	_Inout_ int* pnDestLen)
{
	if (!pbSrcData || !pbDest || !pnDestLen)
	{
		return FALSE;
	}

	BYTE* pbDestEnd=pbDest+*pnDestLen;
	int nConvert = 0;
	int nScan = 0;

	int nWritten = 0;
	int fSkipLine = 0;
	int nLineLen = 0;

	char chars[4];
	char ch;

	while (nSrcLen > 0)
	{
		if (*pbSrcData == '\r' || *pbSrcData == '\n')
		{
			fSkipLine = 0;
			pbSrcData++;
			nSrcLen--;
			nLineLen = 0;
			continue;
		}
		if (fSkipLine)
		{
			pbSrcData++;
			nSrcLen--;
			continue;
		}

		// skip begin/end lines
		if ((nSrcLen >= sizeof("begin")-1 && !_strnicmp((char *)pbSrcData, "begin", sizeof("begin")-1)) ||
			(nSrcLen >= sizeof("end")-1 && !_strnicmp((char *)pbSrcData, "end", sizeof("end")-1)))
		{
			fSkipLine = 1;
			continue;
		}

		// skip first character on line
		nLineLen = UUDECODE(*pbSrcData);
		pbSrcData++;
		nSrcLen--;

		nConvert = 0;
		nScan = 0;

		while (nScan < nLineLen)
		{
			if (nSrcLen > 0)
			{
				if (*pbSrcData == '\r' || *pbSrcData == '\n')
				{
					// invalid uuencoding
					return FALSE;
				}
				ch = *pbSrcData++;
				nSrcLen--;
			}
			else
			{
				ch = ' ';
			}

			chars[nConvert++] = UUDECODE(ch);

			if (nConvert == 4)
			{
				// write to destination buffer if it fits
				if (nScan < nLineLen && pbDest < pbDestEnd)
				{
					nScan++;
					*pbDest++ = (BYTE)(((chars[0] & 0x3F) << 2) | ((chars[1] & 0x3F) >> 4));
				}
				if (nScan < nLineLen && pbDest < pbDestEnd)
				{
					nScan++;
					*pbDest++ = (BYTE)(((chars[1] & 0x3F) << 4) | ((chars[2] & 0x3F) >> 2));
				}
				if (nScan < nLineLen && pbDest < pbDestEnd)
				{
					nScan++;
					*pbDest++ = (BYTE)(((chars[2] & 0x3F) << 6) | (chars[3] & 0x3F));
				}

				// if we are at the end of the buffer and there's still more to write, return error
				if (pbDest >= pbDestEnd && nScan < nLineLen)
				{
					// Estimating how much space we need
					// (this should always give you more than that is required, but give still
					// give better estimate than UUDecodeGetRequiredLength()
					// Math:
					//     nWritten - length we have decoded so far (not including the current line)
					//     nScan - length of the current line that we have decoded so far
					//     nSrcLen - length of the encoded stream that we haven't read
					//               (encoded length is always greater than decoded length)
					//
					*pnDestLen = nWritten + nScan + nSrcLen;
					return FALSE;
				}

				nConvert = 0;
			}
		}
		nWritten+= nLineLen;
	}

	*pnDestLen = nWritten;
	return TRUE;
}

//=======================================================================
// Quoted Printable encode/decode
// compliant with RFC 2045
//=======================================================================
//
inline int QPEncodeGetRequiredLength(_In_ int nSrcLen)
{
	__int64 nRet64 = 3*((3*static_cast<__int64>(nSrcLen))/(ATLSMTP_MAX_QP_LINE_LENGTH-8));
	nRet64 += 3*static_cast<__int64>(nSrcLen);
	nRet64 += 3;
	ATLENSURE(nRet64 <= INT_MAX && nRet64 >= INT_MIN);
	int nRet = static_cast<int>(nRet64);
	return nRet;
}

inline int QPDecodeGetRequiredLength(_In_ int nSrcLen) throw()
{
	return nSrcLen;
}


#define ATLSMTP_QPENCODE_DOT 1
#define ATLSMTP_QPENCODE_TRAILING_SOFT 2

_Success_(return != FALSE)
inline BOOL QPEncode(
	_In_reads_bytes_(nSrcLen) BYTE* pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) CHAR* szDest,
	_Inout_ int* pnDestLen,
	_In_ DWORD dwFlags = 0) throw()
{
	//The hexadecimal character set
	static const char s_chHexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
								'A', 'B', 'C', 'D', 'E', 'F'};

	if (!pbSrcData || !szDest || !pnDestLen)
	{
		return FALSE;
	}

	ATLASSERT(*pnDestLen >= QPEncodeGetRequiredLength(nSrcLen));

	int nRead = 0, nWritten = 0, nLineLen = 0;
	char ch;
	while (nRead < nSrcLen)
	{
		ch = *pbSrcData++;
		nRead++;
		if (nLineLen == 0 && ch == '.' && (dwFlags & ATLSMTP_QPENCODE_DOT))
		{
			*szDest++ = '.';
			nWritten++;
			nLineLen++;
		}
		if ((ch > 32 && ch < 61) || (ch > 61 && ch < 127))
		{
			*szDest++ = ch;
			nWritten++;
			nLineLen++;
		}
		else if ((ch == ' ' || ch == '\t') && (nLineLen < (ATLSMTP_MAX_QP_LINE_LENGTH-12)))
		{
			*szDest++ = ch;
			nWritten++;
			nLineLen++;
		}
		else
		{
			*szDest++ = '=';
			*szDest++ = s_chHexChars[(ch >> 4) & 0x0F];
			*szDest++ = s_chHexChars[ch & 0x0F];
			nWritten += 3;
			nLineLen += 3;
		}
		if (nLineLen >= (ATLSMTP_MAX_QP_LINE_LENGTH-11))
		{
			*szDest++ = '=';
			*szDest++ = '\r';
			*szDest++ = '\n';
			nLineLen = 0;
			nWritten += 3;
		}
	}
	if (dwFlags & ATLSMTP_QPENCODE_TRAILING_SOFT)
	{
		*szDest++ = '=';
		*szDest++ = '\r';
		*szDest++ = '\n';
		nWritten += 3;
	}

	*pnDestLen = nWritten;

	return TRUE;
}

_Success_(return != FALSE)
inline BOOL QPDecode(
	_In_reads_bytes_(nSrcLen) BYTE* pbSrcData,
	_In_ int nSrcLen,
	_When_(*pnDestLen > 0, _Out_writes_to_(*pnDestLen, *pnDestLen)) CHAR* szDest,
	_Inout_ int* pnDestLen,
	_In_ DWORD dwFlags = 0)
{
	if (!pbSrcData || !szDest || !pnDestLen)
	{
		return FALSE;
	}

	LPSTR szDestEnd=szDest + *pnDestLen;
	int nRead = 0, nWritten = 0, nLineLen = -1;
	char ch;
	while (nRead <= nSrcLen)
	{
		ch = *pbSrcData++;
		nRead++;
		nLineLen++;
		if (ch == '=')
		{
			//if the next character is a digit or a character, convert
			if ( ( nRead < ( nSrcLen - 1 ) ) && (isdigit(static_cast<unsigned char>(*pbSrcData)) || isalpha(static_cast<unsigned char>(*pbSrcData))))
			{
				char szBuf[5];
				szBuf[0] = *pbSrcData++;
				szBuf[1] = *pbSrcData++;
				szBuf[2] = '\0';
				ATLENSURE(szDest < szDestEnd);
				*szDest++ = (BYTE)strtoul(szBuf, nullptr, 16);
				nWritten++;
				nRead += 2;
				continue;
			}
			//if the next character is a carriage return or line break, eat it
			if (nRead < nSrcLen && *pbSrcData == '\r' && (nRead+1 < nSrcLen) && *(pbSrcData+1)=='\n')
			{
				pbSrcData++;
				nRead++;
				nLineLen = -1;
				continue;
			}
			return FALSE;
		}
		if (ch == '\r' || ch == '\n')
		{
			nLineLen = -1;
			continue;
		}
		if ((dwFlags & ATLSMTP_QPENCODE_DOT) && ch == '.' && nLineLen == 0)
		{
			continue;
		}
		ATLENSURE(szDest < szDestEnd);
		*szDest++ = ch;
		nWritten++;
	}

	*pnDestLen = (nWritten > 0 ) ? nWritten-1 : 0;
	return TRUE;
}

//=======================================================================
// Q and B encoding (for encoding MIME header information)
// compliant with RFC 2047
//=======================================================================

inline int IsExtendedChar(_In_ char ch) throw()
{
	return ((ch > 126 || ch < 32) && ch != '\t' && ch != '\n' && ch != '\r');
}

inline int GetExtendedChars(
	_In_reads_z_(nSrcLen) LPCSTR szSrc,
	_In_ int nSrcLen)
{
	ATLENSURE( szSrc );

	int nChars(0);

	for (int i=0; i<nSrcLen; i++)
	{
		if (IsExtendedChar(*szSrc++))
			nChars++;
	}

	return nChars;
}

#ifndef ATL_MAX_ENC_CHARSET_LENGTH
#define ATL_MAX_ENC_CHARSET_LENGTH 50
#endif

//Get the required length to hold this encoding based on nSrcLen
inline int QEncodeGetRequiredLength(
	_In_ int nSrcLen,
	_In_ int nCharsetLen) throw()
{
	return QPEncodeGetRequiredLength(nSrcLen)+7+nCharsetLen;
}

inline BOOL IsBufferWriteSafe(
	_In_ int nNumOfCharsAboutToWrite,
	_In_ int nBuffSize)
{
	if(nNumOfCharsAboutToWrite >= nBuffSize)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

//QEncode pbSrcData with the charset specified by pszCharSet
inline _Success_(return != FALSE) BOOL QEncode(
	_In_reads_bytes_(nSrcLen) BYTE* pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) LPSTR szDest,
	_Inout_ int* pnDestLen,
	_In_z_ LPCSTR pszCharSet,
	_Out_opt_ int* pnNumEncoded = NULL) throw()
{
	//The hexadecimal character set
	static const char s_chHexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
								'A', 'B', 'C', 'D', 'E', 'F'};

	if (!pbSrcData || !szDest || !pszCharSet || !pnDestLen)
	{
		return FALSE;
	}

	if(*pnDestLen < QEncodeGetRequiredLength(nSrcLen, ATL_MAX_ENC_CHARSET_LENGTH))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	int nRead = 0, nWritten = 0, nEncCnt = 0;
	char ch;

	if (!IsBufferWriteSafe(nWritten+2, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '=';
	*szDest++ = '?';
	nWritten = 2;

	//output the charset
	while(nWritten < *pnDestLen && *pszCharSet != '\0')
	{
		*szDest++ = *pszCharSet++;
		nWritten++;
	}

	if (!IsBufferWriteSafe(nWritten+3, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '?';
	*szDest++ = 'Q';
	*szDest++ = '?';
	nWritten += 3;

	while (nRead < nSrcLen)
	{
		ch = *pbSrcData++;
		nRead++;
		if (((ch > 32 && ch < 61) || (ch > 61 && ch < 127)) && ch != '?' && ch != '_')
		{
			if (!IsBufferWriteSafe(nWritten+1, *pnDestLen))
			{
					return FALSE;
			}
			*szDest++ = ch;
			nWritten++;
			continue;
					}
		//otherwise it is an unprintable/unsafe character
		if (!IsBufferWriteSafe(nWritten+3, *pnDestLen))
		{
			return FALSE;
		}
		*szDest++ = '=';
		*szDest++ = s_chHexChars[(ch >> 4) & 0x0F];
		*szDest++ = s_chHexChars[ch & 0x0F];
		if (ch < 32 || ch > 126)
			nEncCnt++;
		nWritten += 3;
	}
	if (!IsBufferWriteSafe(nWritten+2, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '?';
	*szDest++ = '=';
	*szDest = 0;
	nWritten += 2;

	*pnDestLen = nWritten;

	if (pnNumEncoded)
		*pnNumEncoded = nEncCnt;

	return TRUE;
}

#define BENCODE_ADDITION_SIZE 7 // size of prefix+suffix added by the encoding.

//Get the required length to hold this encoding based on nSrcLen
inline int BEncodeGetRequiredLength(
	_In_ int nSrcLen,
	_In_ int nCharsetLen) throw()
{
	return Base64EncodeGetRequiredLength(nSrcLen)+BENCODE_ADDITION_SIZE+nCharsetLen;
}

//BEncode pbSrcData with the charset specified by pszCharSet
_Success_(return != FALSE)
inline BOOL BEncode(
	_In_reads_bytes_(nSrcLen) BYTE* pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) LPSTR szDest,
	_Inout_ int* pnDestLen,
	_In_z_ LPCSTR pszCharSet) throw()
{
	if (!pbSrcData || !szDest || !pszCharSet || !pnDestLen)
	{
		return FALSE;
	}

	if(*pnDestLen < BEncodeGetRequiredLength(nSrcLen, ATL_MAX_ENC_CHARSET_LENGTH))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	int nWritten = 0;

	if (!IsBufferWriteSafe(nWritten+2, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '=';
	*szDest++ = '?';
	nWritten = 2;

	//output the charset
	while(nWritten < *pnDestLen && *pszCharSet != '\0')
	{
		*szDest++ = *pszCharSet++;
		nWritten++;
	}

	if (!IsBufferWriteSafe(nWritten+3, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '?';
	*szDest++ = 'B';
	*szDest++ = '?';
	nWritten += 3;

	// the buffer size is *pnDestLen - size of the header and the tail.
	int DataDestLen = *pnDestLen - BENCODE_ADDITION_SIZE;
	BOOL bRet = Base64Encode(pbSrcData, nSrcLen, szDest, &DataDestLen, ATL_BASE64_FLAG_NOCRLF);
	if (!bRet)
		return FALSE;

	nWritten += DataDestLen;
	szDest += DataDestLen;
	if (!IsBufferWriteSafe(nWritten+2, *pnDestLen))
	{
		return FALSE;
	}
	*szDest++ = '?';
	*szDest++ = '=';
	*szDest = 0;
	nWritten += 2;

	*pnDestLen = nWritten;
	return TRUE;
}

//=======================================================================
// AtlUnicodeToUTF8
//
// Support for converting UNICODE strings to UTF8
//
//=======================================================================
//

#define ATL_HIGH_SURROGATE_START  0xd800
#define ATL_HIGH_SURROGATE_END    0xdbff
#define ATL_LOW_SURROGATE_START   0xdc00
#define ATL_LOW_SURROGATE_END     0xdfff

ATL_NOINLINE inline
int AtlUnicodeToUTF8(
	_In_reads_(nSrc) LPCWSTR wszSrc,
	_In_ int nSrc,
	_Out_writes_to_opt_(nDest, return + 1) LPSTR szDest,
	_In_ int nDest)
{
	return(WideCharToMultiByte(CP_UTF8, 0, wszSrc, nSrc, szDest, nDest, NULL, NULL));
}


//=======================================================================
// EscapeXML
//
// Support for escaping strings for use in XML documents
//=======================================================================
//

#define ATL_ESC_FLAG_NONE 0
#define ATL_ESC_FLAG_ATTR 1 // escape for attribute values

ATLPREFAST_SUPPRESS(6054)
inline int _AtlCopyNCR(
	_In_ wchar_t wch,
	_Out_writes_z_(9) wchar_t *wszEsc) throw()
{
	wchar_t szHex[9];
	int nRet = swprintf_s(szHex, _countof(szHex), L"&#x%04X;", wch);
	Checked::memcpy_s(wszEsc, 9*sizeof(wchar_t), szHex, 8*sizeof(wchar_t));
	return nRet;
}
ATLPREFAST_UNSUPPRESS()

ATLPREFAST_SUPPRESS(6054)
inline int _AtlCopyNCRPair(
	_In_ DWORD dw,
	_Out_writes_z_(11) wchar_t *wszEsc) throw()
{
	wchar_t szHex[11];
	int nRet = swprintf_s(szHex, _countof(szHex), L"&#x%06X;", dw);
	Checked::memcpy_s(wszEsc, 11*sizeof(wchar_t), szHex, 10*sizeof(wchar_t));
	return nRet;
}
ATLPREFAST_UNSUPPRESS()

// wide-char version
_Success_(return != 0)
inline int EscapeXML(
	_In_reads_(nSrcLen) const wchar_t *szIn,
	_In_ int nSrcLen,
	_Out_writes_to_opt_(nDestLen, return + 1) wchar_t *szEsc,
	_In_ int nDestLen,
	_In_ DWORD dwFlags = ATL_ESC_FLAG_NONE)
{
	ATLENSURE( szIn != NULL );

	int nCnt(0);
	int nCurrLen(nDestLen);
	int nInc(0);
	wchar_t wchHighSurrogate = 0;
	BOOL bHandled;

	while (nSrcLen--)
	{
		bHandled = FALSE;
		nInc = 0;
		switch (*szIn)
		{
		case L'<': case L'>':
			if ((szEsc != NULL) && (3 < nCurrLen))
			{
				*szEsc++ = L'&';
				*szEsc++ = (*szIn==L'<' ? L'l' : L'g');
				*szEsc++ = L't';
				*szEsc++ = L';';
			}
			nInc = 4;
			break;

		case L'&':
			if ((szEsc != NULL) && (4 < nCurrLen))
			{
				Checked::memcpy_s(szEsc, nCurrLen*sizeof(wchar_t), L"&amp;", 5*sizeof(wchar_t));
				szEsc+= 5;
			}
			nInc = 5;
			break;

		case L'\'': case L'\"': // escaping for attribute values
			if (dwFlags & ATL_ESC_FLAG_ATTR)
			{
				if ((szEsc != NULL) && (5 < nCurrLen))
				{
					Checked::memcpy_s(szEsc, nCurrLen*sizeof(wchar_t), (*szIn == L'\'' ? L"&apos;" : L"&quot;"), 6*sizeof(wchar_t));
					szEsc+= 6;
				}
				nInc = 6;
				break;
			}
			// fall through

		default:
			// check if high surrogate is available
			if ((*szIn >= ATL_HIGH_SURROGATE_START) && (*szIn <= ATL_HIGH_SURROGATE_END))
			{
				// another high surrogate, then treat the 1st as normal unicode character
				if (wchHighSurrogate)
				{
					if ((szEsc != NULL) && (7 < nCurrLen))
					{
						_AtlCopyNCR(wchHighSurrogate, szEsc);
						szEsc+= 8;
					}
					nInc = 8;
				}
				wchHighSurrogate = *szIn;
				bHandled = TRUE;
			}
			if ((!bHandled) && (wchHighSurrogate))
			{
				if ((*szIn >= ATL_LOW_SURROGATE_START) && (*szIn <= ATL_LOW_SURROGATE_END))
				{
					// valid surrogate pairs
					DWORD dwSurrogateChar = (((wchHighSurrogate-0xD800) << 10) + (*szIn - 0xDC00) + 0x10000);
					if ((szEsc != NULL) && (9 < nCurrLen))
					{
						_AtlCopyNCRPair(dwSurrogateChar, szEsc);
						szEsc+= 10;
					}
					nInc = 10;
					bHandled = TRUE;
				}
				else
				{
					// bad surrogate pair: ERROR
					// just process wchHighSurrogate, and the code below will
					// process the current code point
					if ((szEsc != NULL) && (7 < nCurrLen))
					{
						_AtlCopyNCR(wchHighSurrogate, szEsc);
						szEsc+= 8;
					}
					nCurrLen-= 8;
					nCnt+= 8;
				}
				wchHighSurrogate = 0;
			}
			if (!bHandled)
			{
				if ((*szIn < 0x0020) || (*szIn > 0x007E))
				{
					if ((szEsc != NULL) && (7 < nCurrLen))
					{
						_AtlCopyNCR(*szIn, szEsc);
						szEsc+= 8;
					}
					nInc = 8;
				}
				else
				{
					if ((szEsc != NULL) && (0 < nCurrLen))
					{
						*szEsc++ = *szIn;
					}
					nInc = 1;
				}
			}
		}

		nCurrLen -= nInc;
		nCnt+= nInc;

		szIn++;
	}

	// If the last character was a high surrogate, then handle it as a normal unicode character.
	if (wchHighSurrogate != 0)
	{
		if ((wchHighSurrogate < 0x0020) || (wchHighSurrogate > 0x007E))
		{
			if ((szEsc != NULL) && (7 < nCurrLen))
			{
				_AtlCopyNCR(wchHighSurrogate, szEsc);
			}
			nCurrLen -= 8;
			nCnt+= 8;
		}
		else
		{
			if ((szEsc != NULL) && (0 < nCurrLen))
			{
				*szEsc = wchHighSurrogate;
			}
			nCurrLen--;
			nCnt++;
		}
	}


	if ((szEsc != NULL) && (nCurrLen < 0))
	{
		return 0;
	}

	return nCnt;
}

//=======================================================================
// HexEncode, HexDecode
//
// Support for encoding/decoding binary XML datatypes with hex encoding
//=======================================================================
//

inline int AtlHexEncodeGetRequiredLength(_In_ int nSrcLen)
{
	__int64 nRet64=2*static_cast<__int64>(nSrcLen)+1;
	ATLENSURE(nRet64 <= INT_MAX && nRet64 >= INT_MIN);
	int nRet = static_cast<int>(nRet64);
	return nRet;
}

inline int AtlHexDecodeGetRequiredLength(_In_ int nSrcLen) throw()
{
	return nSrcLen/2;
}

ATLPREFAST_SUPPRESS(6054)
inline BOOL AtlHexEncode(
	_In_reads_bytes_(nSrcLen) const BYTE *pbSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) LPSTR szDest,
	_Inout_ int *pnDestLen) throw()
{
	static const char s_chHexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
										  'A', 'B', 'C', 'D', 'E', 'F'};

	if (!pbSrcData || !szDest || !pnDestLen)
	{
		return FALSE;
	}

	if(*pnDestLen < AtlHexEncodeGetRequiredLength(nSrcLen))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	int nRead = 0;
	int nWritten = 0;
	BYTE ch;
	while (nRead < nSrcLen)
	{
		ch = *pbSrcData++;
		nRead++;
		*szDest++ = s_chHexChars[(ch >> 4) & 0x0F];
		*szDest++ = s_chHexChars[ch & 0x0F];
		nWritten += 2;
	}

	*pnDestLen = nWritten;

	return TRUE;
}
ATLPREFAST_UNSUPPRESS()

#ifdef _CHAR_UNSIGNED
#define ATL_HEX_INVALID CHAR_MAX
#else
/* In pre-V8 this was always the value, which meant we didn't compile clean with /J */
#define ATL_HEX_INVALID ((char)(-1))
#endif

//Get the decimal value of a hexadecimal character
inline char AtlGetHexValue(_In_ char ch) throw()
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 10);
	return ATL_HEX_INVALID;
}

inline BOOL AtlHexDecode(
	_In_reads_z_(nSrcLen) LPCSTR pSrcData,
	_In_ int nSrcLen,
	_Out_writes_to_(*pnDestLen, *pnDestLen) LPBYTE pbDest,
	_Inout_ int* pnDestLen) throw()
{
	if (!pSrcData || !pbDest || !pnDestLen)
	{
		return FALSE;
	}

	if(*pnDestLen < AtlHexDecodeGetRequiredLength(nSrcLen))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	int nRead = 0;
	int nWritten = 0;
	while (nRead < nSrcLen)
	{
		char ch1 = AtlGetHexValue((char)*pSrcData++);
		char ch2 = AtlGetHexValue((char)*pSrcData++);
		if ((ch1==ATL_HEX_INVALID) || (ch2==ATL_HEX_INVALID))
		{
			return FALSE;
		}
		*pbDest++ = (BYTE)(16*ch1+ch2);
		nWritten++;
		nRead += 2;
	}

	*pnDestLen = nWritten;
	return TRUE;
}

} // namespace ATL
#pragma pack(pop)

#endif // __ATLENC_H__
