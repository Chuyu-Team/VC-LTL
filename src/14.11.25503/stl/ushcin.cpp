// wcin -- initialize standard wide error stream
// This wcin is unsigned short version of wcin

 #ifdef _NATIVE_WCHAR_T_DEFINED
  #include <fstream>

  #ifndef wistream
  #define wistream    ushistream
  #define wostream    ushostream
  #define wfilebuf    ushfilebuf
  #define _Init_wcerr _Init_ushcerr
  #define _Init_wcout _Init_ushcout
  #define _Init_wclog _Init_ushclog
  #define _Init_wcin  _Init_ushcin
  #define _Winit      _UShinit
  #define init_wcerr  init_ushcerr
  #define init_wcout  init_ushcout
  #define init_wclog  init_ushclog
  #define init_wcin   init_ushcin
  #endif

  #include <iostream>
  #include "wcin.cpp"
 #endif

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
