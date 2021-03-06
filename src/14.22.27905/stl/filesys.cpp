// filesys.cpp -- filesystem support functions
#include <yvals.h>

#include <awint.h>
#include <direct.h>
#include <experimental/filesystem>
#include <io.h> // for _chmod/_wchmod
#include <string.h>

#include <Windows.h>

#define _FILE_ATTRIBUTE_REGULAR                                                                            \
    (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN \
        | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_OFFLINE              \
        | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY)

_STD_BEGIN
namespace experimental {
    namespace filesystem {
        inline namespace v1 {
            static file_type _Map_mode(int _Mode) { // map Windows file attributes to file_status
                if ((_Mode & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                    return file_type::directory;
                } else if ((_Mode & _FILE_ATTRIBUTE_REGULAR) != 0) {
                    return file_type::regular;
                } else {
                    return file_type::unknown;
                }
            }

            _FS_DLL void __CLRCALL_PURE_OR_CDECL _Close_dir(void* _Handle) { // close a directory
                FindClose((HANDLE) _Handle);
            }

#define FILESYS_WIDE 1
#include "filesys.h"

        } // namespace v1
    } // namespace filesystem
} // namespace experimental
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
