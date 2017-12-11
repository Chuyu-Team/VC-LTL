// filesys.h -- filesystem support functions for Windows
// NOTE: no include guard

#define READONLY_PERMS	\
	_Perms(perms::all & ~WRITE_PERMS)
#define WRITE_PERMS	\
	_Perms(perms::owner_write | perms::group_write | perms::others_write)

  #define TCHAR		wchar_t
  #define TCHMOD	_wchmod
  #define TFUN(x)	x##W
  #define TLIT(x)	L##x
  #define TSTRING	wstring

	// CRT functions
  #define TCHDIR	_wchdir
  #define TGETCWD	_wgetcwd
  #define TMKDIR	_wmkdir
  #define TSTRCPY	::wcscpy_s
  #define TRMDIR	_wrmdir
  #define TREMOVE	_wremove
  #define TRENAME	_wrename

 #if !defined(_CRT_APP) && !defined(_KERNELX)
  #define _HAS_HARDLINKS	1
  #define _HAS_SYMLINKS		1
 #endif /* !defined(_CRT_APP) && !defined(_KERNELX) */

		// DIRECTORY FUNCTIONS
static TCHAR *_Strcpy(TCHAR (&_Dest)[_MAX_FILESYS_NAME], const TCHAR *_Src)
	{	// copy an NTBS
	TSTRCPY(_Dest, _MAX_FILESYS_NAME, _Src);
	return (_Dest);
	}

static HANDLE _FilesysOpenFile(const TCHAR *_Fname, DWORD _Desired_access,
	DWORD _Flags)
	{
  #if defined(_CRT_APP)
	CREATEFILE2_EXTENDED_PARAMETERS _Create_file_parameters = {};
	_Create_file_parameters.dwSize = sizeof(_Create_file_parameters);
	_Create_file_parameters.dwFileFlags = _Flags;

	return (CreateFile2(_Fname, _Desired_access,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		OPEN_EXISTING, &_Create_file_parameters));
  #else /* defined(_CRT_APP) */
	return (TFUN(CreateFile)(_Fname, _Desired_access,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		0, OPEN_EXISTING, _Flags, 0));
  #endif /* defined(_CRT_APP) */
	}

_FS_DLL TCHAR *__CLRCALL_PURE_OR_CDECL _Read_dir(
	TCHAR (&_Dest)[_MAX_FILESYS_NAME], void *_Handle, _File_type& _Ftype)
	{	// read a directory entry
	TFUN(WIN32_FIND_DATA) _Dentry;

	for (; ; )
		if (TFUN(FindNextFile)((HANDLE)_Handle, &_Dentry) == 0)
			{	// fail
			_Ftype = _File_type::unknown;
			return (_Strcpy(_Dest, TLIT("")));
			}
		else if (_Dentry.cFileName[0] == TLIT('.')
			&& (_Dentry.cFileName[1] == TLIT('\0')
				|| (_Dentry.cFileName[1] == TLIT('.')
					&& _Dentry.cFileName[2] == TLIT('\0'))))
			;	// skip "." and ".."
		else
			{	// get file type and return name
			_Ftype = _Map_mode(_Dentry.dwFileAttributes);
			return (_Strcpy(_Dest, &_Dentry.cFileName[0]));
			}
	}

static unsigned int _Filesys_code_page()
	{	/* determine appropriate code page */
 #if (defined(_ONECORE) || defined(_KERNELX))
	return (CP_ACP);
 #else /* (defined(_ONECORE) || defined(_KERNELX)) */
	if (AreFileApisANSI())
		return (CP_ACP);
	else
		return (CP_OEMCP);
 #endif /* (defined(_ONECORE) || defined(_KERNELX)) */
	}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _To_wide(const char *_Bsrc, wchar_t *_Wdest)
	{	// return nonzero on success
	return (MultiByteToWideChar(_Filesys_code_page(),
			0, _Bsrc, -1, _Wdest, _MAX_FILESYS_NAME));
	}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _To_byte(const wchar_t *_Wsrc, char *_Bdest)
	{	// return nonzero on success
	return (WideCharToMultiByte(_Filesys_code_page(),
			0, _Wsrc, -1, _Bdest, _MAX_FILESYS_NAME, NULL, NULL));
	}


_FS_DLL void *__CLRCALL_PURE_OR_CDECL _Open_dir(
	TCHAR (&_Dest)[_MAX_FILESYS_NAME], const TCHAR *_Dirname, int& _Errno,
	_File_type& _Ftype)
	{	// open a directory for reading
	TFUN(WIN32_FIND_DATA) _Dentry;
	TSTRING _Wildname(_Dirname);
	if (!_Wildname.empty())
		_Wildname.append(TLIT("\\*"));

	void *_Handle = TFUN(FindFirstFileEx)(_Wildname.c_str(),
		FindExInfoStandard, &_Dentry, FindExSearchNameMatch, NULL, 0);
	if (_Handle == INVALID_HANDLE_VALUE)
		{	// report failure
		_Errno = ERROR_BAD_PATHNAME;
		*_Dest = TLIT('\0');
		return (0);
		}
	else
		{	// success, get first directory entry
		_Errno = 0;
		if (_Dentry.cFileName[0] == TLIT('.')
			&& (_Dentry.cFileName[1] == TLIT('\0')
				|| _Dentry.cFileName[1] == TLIT('.')
					&& _Dentry.cFileName[2] == TLIT('\0')))
			{	// skip "." and ".."
			_Read_dir(_Dest, _Handle, _Ftype);
			if (_Dest[0] != TLIT('\0'))
				return (_Handle);
			else
				{	// no entries, release handle
				_Close_dir(_Handle);
				return (0);
				}
			}
		else
			{	// get file type and return handle
			_Strcpy(_Dest, &_Dentry.cFileName[0]);
			_Ftype = _Map_mode(_Dentry.dwFileAttributes);
			return (_Handle);
			}
		}
	}


_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Current_get(
	TCHAR (&_Dest)[_MAX_FILESYS_NAME])
	{	// get current working directory
	_Strcpy(_Dest, TLIT(""));
 #if defined(_CRT_APP)
	return (false);	// no support
 #else /* defined(_CRT_APP) */
	return (TGETCWD(_Dest, _MAX_FILESYS_NAME) != 0);
 #endif /* defined(_CRT_APP) */

	}

_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Current_set(const TCHAR *_Dirname)
	{	// set current working directory
 #if defined (_CRT_APP)
	(void)(_Dirname);
	return (false);	// no support
 #else /* defined(_CRT_APP) */
	return (TCHDIR(_Dirname) == 0);
 #endif /* defined(_CRT_APP) */

	}

_FS_DLL TCHAR *__CLRCALL_PURE_OR_CDECL _Symlink_get(
	TCHAR (&_Dest)[_MAX_FILESYS_NAME], const TCHAR *)
	{	// get symlink -- DUMMY
	_Dest[0] = TCHAR(0);
	return (&_Dest[0]);
	}

_FS_DLL TCHAR *__CLRCALL_PURE_OR_CDECL _Temp_get(
	TCHAR (&_Dest)[_MAX_FILESYS_NAME])
	{	// get temp directory
	TCHAR _Dentry[MAX_PATH];
	return (_Strcpy(_Dest,
		TFUN(GetTempPath)(MAX_PATH, &_Dentry[0]) == 0
			? TLIT(".") : &_Dentry[0]));
	}


_FS_DLL int __CLRCALL_PURE_OR_CDECL _Make_dir(const TCHAR *_Fname,
	const TCHAR *)
	{	// make a new directory (ignore attributes)
	int _Ans = TFUN(CreateDirectory)(_Fname, 0);

	if (_Ans != 0)
		return (1);
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
		return (0);
	else
		return (-1);
	}

_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Remove_dir(const TCHAR *_Fname)
	{	// remove a directory
	return (TRMDIR(_Fname) != -1);

	}

		// FILE STATUS FUNCTIONS
_FS_DLL _File_type __CLRCALL_PURE_OR_CDECL _Stat(const TCHAR *_Fname, _Perms *_Pmode)
	{	// get file status
	WIN32_FILE_ATTRIBUTE_DATA _Data;

	if (TFUN(GetFileAttributesEx)(_Fname, GetFileExInfoStandard, &_Data))
		{	// get file type and return permissions
		if (_Pmode != 0)
			*_Pmode = _Data.dwFileAttributes & FILE_ATTRIBUTE_READONLY
				? READONLY_PERMS : perms::all;
		return (_Map_mode(_Data.dwFileAttributes));
		}
	else
		{	// invalid, get error code
		int _Errno = GetLastError();

		if (_Errno == ERROR_BAD_NETPATH
			|| _Errno == ERROR_BAD_PATHNAME
			|| _Errno == ERROR_FILE_NOT_FOUND
			|| _Errno == ERROR_INVALID_DRIVE
			|| _Errno == ERROR_INVALID_NAME
			|| _Errno == ERROR_INVALID_PARAMETER
			|| _Errno == ERROR_PATH_NOT_FOUND)
			return (_File_type::not_found);
		else
			return (_File_type::unknown);
		}
	}


_FS_DLL _File_type __CLRCALL_PURE_OR_CDECL _Lstat(const TCHAR *_Fname, _Perms *_Pmode)
	{	// get symlink file status
	return (_Stat(_Fname, _Pmode));	// symlink not supported
	}

_FS_DLL _ULonglong __CLRCALL_PURE_OR_CDECL _Hard_links(const TCHAR *_Fname)
	{	// get hard link count
	HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_READ_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);

  #if defined (_CRT_APP)
	FILE_STANDARD_INFO _Info = {0};
	bool _Ok = false;

	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file info
		_Ok = GetFileInformationByHandleEx(_Handle, FileStandardInfo,
			&_Info, sizeof(_Info)) != 0;
		CloseHandle(_Handle);
		}
	return (_Ok ? _Info.NumberOfLinks : (_ULonglong)-1);

  #else /* defined(_CRT_APP) */
	BY_HANDLE_FILE_INFORMATION _Info = {0};
	bool _Ok = false;

	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file info
		_Ok = GetFileInformationByHandle(_Handle, &_Info) != 0;
		CloseHandle(_Handle);
		}
	return (_Ok ? _Info.nNumberOfLinks : (_ULonglong)-1);
  #endif /* defined(_CRT_APP) */
	}


_FS_DLL _ULonglong __CLRCALL_PURE_OR_CDECL _File_size(const TCHAR *_Fname)
	{	// get file size
	WIN32_FILE_ATTRIBUTE_DATA _Data;

	if (!TFUN(GetFileAttributesEx)(_Fname, GetFileExInfoStandard, &_Data))
		return ((_ULonglong)-1);
	else
		return ((_ULonglong)_Data.nFileSizeHigh << 32 | _Data.nFileSizeLow);
	}


// 3 centuries with 24 leap years each:
//		1600 is excluded, 1700/1800 are not leap years
// 1 partial century with 17 leap years:
//		1900 is not a leap year
//		1904 is leap year #1
//		1908 is leap year #2
//		1968 is leap year #17

 #define WIN_TICKS_PER_SECOND	10000000ULL

 #define WIN_TICKS_FROM_EPOCH \
	(((1970 - 1601) * 365 + 3 * 24 + 17) * 86400ULL * WIN_TICKS_PER_SECOND)

_FS_DLL int64_t __CLRCALL_PURE_OR_CDECL _Last_write_time(const TCHAR *_Fname)
	{	// get last write time
	WIN32_FILE_ATTRIBUTE_DATA _Data;

	if (!TFUN(GetFileAttributesEx)(_Fname, GetFileExInfoStandard, &_Data))
		return (-1);
	else
		{	// success, convert time
		_ULonglong _Wtime =
			(_ULonglong)_Data.ftLastWriteTime.dwHighDateTime << 32
				| _Data.ftLastWriteTime.dwLowDateTime;
		return ((int64_t)(_Wtime - WIN_TICKS_FROM_EPOCH));
		}
	}


_FS_DLL int __CLRCALL_PURE_OR_CDECL _Set_last_write_time(const TCHAR *_Fname,
	int64_t _When)
	{	// set last write time
	HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_WRITE_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);

	if (_Handle == INVALID_HANDLE_VALUE)
		return (0);

	//FILE_BASIC_INFO _Finfo;
	//if (!__crtGetFileInformationByHandleEx(_Handle, FileBasicInfo,
	//	&_Finfo, sizeof(_Finfo)))
	//	{	// can't get file info, fail
	//	CloseHandle(_Handle);
	//	return (0);
	//	}
	// convert to FILETIME and set
	_ULonglong _Wtime = (_ULonglong)_When + WIN_TICKS_FROM_EPOCH;
	//_Finfo.LastWriteTime.QuadPart = _Wtime;

	//if (!__crtSetFileInformationByHandle(_Handle, FileBasicInfo,
	//	&_Finfo, sizeof(_Finfo)))
	//	{	// can't set file info, fail
	//	CloseHandle(_Handle);
	//	return (0);
	//	}
	int _Result = SetFileTime(_Handle, nullptr, nullptr, (FILETIME*)&_Wtime);

	CloseHandle(_Handle);
	return _Result;

	}


_FS_DLL space_info __CLRCALL_PURE_OR_CDECL _Statvfs(const TCHAR *_Fname)
	{	// get space information for volume
	space_info _Ans = {(uintmax_t)-1, (uintmax_t)-1, (uintmax_t)-1};
	TSTRING _Devname = _Fname;

	if (_Devname.empty()
		|| _Devname.back() != TLIT('/') && _Devname.back() != TLIT('\\'))
		_Devname.append(TLIT("/"));
	_ULARGE_INTEGER _Available, _Capacity, _Free;

	if (TFUN(GetDiskFreeSpaceEx)(_Devname.c_str(),
		&_Available, &_Capacity, &_Free))
		{	// convert values
		_Ans.capacity = _Capacity.QuadPart;
		_Ans.free = _Free.QuadPart;
		_Ans.available = _Available.QuadPart;
		}
	return (_Ans);
	}


_FS_DLL int __CLRCALL_PURE_OR_CDECL _Equivalent(const TCHAR *_Fname1, const TCHAR *_Fname2)
	{	// test for equivalent file names
  #if defined (_CRT_APP)
	_FILE_ID_INFO _Info1 = {0};
	_FILE_ID_INFO _Info2 = {0};
	bool _Ok1 = false;
	bool _Ok2 = false;

	HANDLE _Handle = _FilesysOpenFile(_Fname1, FILE_READ_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);
	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file1 info
		_Ok1 = GetFileInformationByHandleEx(_Handle, FileIdInfo,
			&_Info1, sizeof(_Info1)) != 0;
		CloseHandle(_Handle);
		}

	_Handle = _FilesysOpenFile(_Fname2, FILE_READ_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);
	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file2 info
		_Ok2 = GetFileInformationByHandleEx(_Handle, FileIdInfo,
			&_Info2, sizeof(_Info2)) != 0;
		CloseHandle(_Handle);
		}

	if (!_Ok1 && !_Ok2)
		return (-1);
	else if (!_Ok1 || !_Ok2)
		return (0);
	else
		{	// test existing files for equivalence
		return (_Info1.VolumeSerialNumber != _Info2.VolumeSerialNumber
			|| memcmp(&_Info1.FileId, &_Info2.FileId, sizeof(_Info1.FileId)) != 0
				? 0 : 1);
		}

  #else /* defined(_CRT_APP) */
	BY_HANDLE_FILE_INFORMATION _Info1 = {0};
	BY_HANDLE_FILE_INFORMATION _Info2 = {0};
	bool _Ok1 = false;
	bool _Ok2 = false;

	HANDLE _Handle = _FilesysOpenFile(_Fname1, FILE_READ_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);
	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file1 info
		_Ok1 = GetFileInformationByHandle(_Handle, &_Info1) != 0;
		CloseHandle(_Handle);
		}

	_Handle = _FilesysOpenFile(_Fname2, FILE_READ_ATTRIBUTES,
		FILE_FLAG_BACKUP_SEMANTICS);
	if (_Handle != INVALID_HANDLE_VALUE)
		{	// get file2 info
		_Ok2 = GetFileInformationByHandle(_Handle, &_Info2) != 0;
		CloseHandle(_Handle);
		}

	if (!_Ok1 && !_Ok2)
		return (-1);
	else if (!_Ok1 || !_Ok2)
		return (0);
	else
		{	// test existing files for equivalence
		return (_Info1.dwVolumeSerialNumber != _Info2.dwVolumeSerialNumber
			|| _Info1.nFileIndexHigh != _Info2.nFileIndexHigh
			|| _Info1.nFileIndexLow != _Info2.nFileIndexLow
				? 0 : 1);
		}
  #endif /* defined(_CRT_APP) */
	}


		// FILE LINKAGE FUNCTIONS
 #if _HAS_HARDLINKS
_FS_DLL int __CLRCALL_PURE_OR_CDECL _Link(const TCHAR *_Fname1, const TCHAR *_Fname2)
	{	// link _Fname2 to _Fname1
	return (CreateHardLinkW(_Fname2, _Fname1, 0) != 0
		? 0 : GetLastError());
	}


 #else /* _HAS_HARDLINKS */
_FS_DLL int __CLRCALL_PURE_OR_CDECL _Link(const TCHAR *, const TCHAR *)
	{	// link _Fname2 to _Fname1
	return (errno = EDOM);	// hardlinks not supported
	}
 #endif /* _HAS_HARDLINKS */

 #if _HAS_SYMLINKS
_FS_DLL int __CLRCALL_PURE_OR_CDECL _Symlink(const TCHAR *_Fname1, const TCHAR *_Fname2)
	{	// link _Fname2 to _Fname1
	return (__crtCreateSymbolicLinkW(_Fname2, _Fname1, 0) != 0
		? 0 : GetLastError());
	}


 #else /* _HAS_SYMLINKS */
_FS_DLL int __CLRCALL_PURE_OR_CDECL _Symlink(const TCHAR *, const TCHAR *)
	{	// link _Fname2 to _Fname1
	return (errno = EDOM);	// symlinks not supported
	}
 #endif /* _HAS_SYMLINKS */

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Rename(const TCHAR *_Fname1, const TCHAR *_Fname2)
	{	// rename _Fname1 as _Fname2
	return (TRENAME(_Fname1, _Fname2) == 0
		? 0 : GetLastError());

	}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Resize(const TCHAR *_Fname, uintmax_t _Newsize)
	{	// change file size
	bool _Ok = false;

	HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_GENERIC_WRITE,
		0);

	if (_Handle != INVALID_HANDLE_VALUE)
		{	// set file pointer to new size and trim
		LARGE_INTEGER _Large;
		_Large.QuadPart = _Newsize;
		_Ok = SetFilePointerEx(_Handle, _Large, 0, FILE_BEGIN) != 0
			&& SetEndOfFile(_Handle) != 0;
		CloseHandle(_Handle);
		}
	return (_Ok ? 0 : GetLastError());
	}


_FS_DLL int __CLRCALL_PURE_OR_CDECL _Unlink(const TCHAR *_Fname)
	{	// unlink _Fname
	return (TREMOVE(_Fname) == 0
		? 0 : GetLastError());

	}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Copy_file(const TCHAR *_Fname1, const TCHAR *_Fname2)
	{	// copy _Fname1 to _Fname2
 #if defined(_ONECORE) || defined(_KERNELX)
	COPYFILE2_EXTENDED_PARAMETERS _Params = {0};
	_Params.dwSize = sizeof(COPYFILE2_EXTENDED_PARAMETERS);
	_Params.dwCopyFlags = 0;

	return (SUCCEEDED(CopyFile2(_Fname1, _Fname2, &_Params))
		? 0 : GetLastError());

 #else /* defined(_ONECORE) || defined(_KERNELX) */
	return (TFUN(CopyFile)(_Fname1, _Fname2, 0) != 0
		? 0 : GetLastError());
 #endif /* defined(_ONECORE) || defined(_KERNELX) */
	}


_FS_DLL int __CLRCALL_PURE_OR_CDECL _Chmod(const TCHAR *_Fname, _Perms _Newmode)
	{	// change file mode to _Newmode
	WIN32_FILE_ATTRIBUTE_DATA _Data;

	if (!TFUN(GetFileAttributesEx)(_Fname, GetFileExInfoStandard, &_Data))
		return (-1);
	else
		{	// got mode, alter readonly bit
		DWORD _Oldmode = _Data.dwFileAttributes;
		DWORD _Mode = _Oldmode & ~FILE_ATTRIBUTE_READONLY;
		if ((_Newmode & WRITE_PERMS) == perms::none)
			_Mode |= FILE_ATTRIBUTE_READONLY;
		return (_Mode == _Oldmode ? 0
			: TFUN(SetFileAttributes)(_Fname, _Mode) != 0 ? 0 : -1);
		}
	}

  #undef FILESYS_WIDE
  #undef TCHAR
  #undef TCHMOD
  #undef TFUN
  #undef TLIT
  #undef TSTRING

  #undef TCHDIR
  #undef TGETCWD
  #undef TMKDIR
  #undef TSTRCPY
  #undef TRMDIR
  #undef TREMOVE
  #undef TRENAME

  #undef _HAS_HARDLINKS
  #undef _HAS_SYMLINKS

  #undef READONLY_PERMS
  #undef WRITE_PERMS

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
