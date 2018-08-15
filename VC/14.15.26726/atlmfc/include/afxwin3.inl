// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines that reference NT5-only DLLs 

#pragma once

#ifdef _AFXWIN_INLINE

_AFXWIN_INLINE BOOL CDC::AlphaBlend(int xDest, int yDest, int nDestWidth, int nDestHeight,
   CDC* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BLENDFUNCTION blend)
   { ASSERT(m_hDC != NULL); return ::AlphaBlend(m_hDC, xDest, yDest, 
	  nDestWidth, nDestHeight, pSrcDC->GetSafeHdc(), xSrc, ySrc, nSrcWidth, 
	  nSrcHeight, blend); }
_AFXWIN_INLINE BOOL CDC::TransparentBlt(int xDest, int yDest, int nDestWidth, 
   int nDestHeight, CDC* pSrcDC, int xSrc, int ySrc, int nSrcWidth, 
   int nSrcHeight, UINT crTransparent)
   { ASSERT(m_hDC != NULL); return ::TransparentBlt(m_hDC, xDest, yDest, 
	  nDestWidth, nDestHeight, pSrcDC->GetSafeHdc(), xSrc, ySrc, nSrcWidth, 
	  nSrcHeight, crTransparent); }
_AFXWIN_INLINE BOOL CDC::GradientFill(TRIVERTEX* pVertices, ULONG nVertices,
   void* pMesh, ULONG nMeshElements, DWORD dwMode)
   { ASSERT(m_hDC != NULL); return ::GradientFill(m_hDC, pVertices, nVertices,
	  pMesh, nMeshElements, dwMode); }

#endif //_AFXWIN_INLINE
