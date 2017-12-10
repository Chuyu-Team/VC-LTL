// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_HH_H__
#define __AFX_HH_H__

#pragma once

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

// Non-Client HitTest help IDs
#define HID_HT_NOWHERE                          0x40000
#define HID_HT_CAPTION                          0x40002
#define HID_HT_HSCROLL                          0x40006
#define HID_HT_VSCROLL                          0x40007
#define HID_HT_MINBUTTON                        0x40008
#define HID_HT_MAXBUTTON                        0x40009
#define HID_HT_SIZE                             0x40012 // alias: ID_HT_BORDER
#define HID_HT_OBJECT							0x40013
#define HID_HT_CLOSE							0x40014
#define HID_HT_HELP								0x40015

// WM_SYSCOMMAND help IDs
#define HID_SC_SIZE                             0x1EF00
#define HID_SC_MOVE                             0x1EF01
#define HID_SC_MINIMIZE                         0x1EF02
#define HID_SC_MAXIMIZE                         0x1EF03
#define HID_SC_NEXTWINDOW                       0x1EF04
#define HID_SC_PREVWINDOW                       0x1EF05
#define HID_SC_CLOSE                            0x1EF06
#define HID_SC_RESTORE                          0x1EF12
#define HID_SC_TASKLIST                         0x1EF13

// File MRU and aliases
#define HID_FILE_MRU_FILE1                      0x1E11F

// Window menu list
#define HID_WINDOW_ALL                          0x1EF1F

// OLE menu and aliases
#define HID_OLE_VERB_1                          0x1E21F

// Commands (HID_*) 
#define HID_FILE_NEW                            0x1E100
#define HID_FILE_OPEN                           0x1E101
#define HID_FILE_CLOSE                          0x1E102
#define HID_FILE_SAVE                           0x1E103
#define HID_FILE_SAVE_AS                        0x1E104
#define HID_FILE_PAGE_SETUP                     0x1E105
#define HID_FILE_PRINT_SETUP                    0x1E106
#define HID_FILE_PRINT                          0x1E107
#define HID_FILE_PRINT_DIRECT                   0x1E108
#define HID_FILE_PRINT_PREVIEW                  0x1E109
#define HID_FILE_UPDATE                         0x1E10A
#define HID_FILE_SAVE_COPY_AS                   0x1E10B
#define HID_FILE_SEND_MAIL                      0x1E10C
#define HID_EDIT_CLEAR                          0x1E120
#define HID_EDIT_CLEAR_ALL                      0x1E121
#define HID_EDIT_COPY                           0x1E122
#define HID_EDIT_CUT                            0x1E123
#define HID_EDIT_FIND                           0x1E124
#define HID_EDIT_PASTE                          0x1E125
#define HID_EDIT_PASTE_LINK                     0x1E126
#define HID_EDIT_PASTE_SPECIAL                  0x1E127
#define HID_EDIT_REPEAT                         0x1E128
#define HID_EDIT_REPLACE                        0x1E129
#define HID_EDIT_SELECT_ALL                     0x1E12A
#define HID_EDIT_UNDO                           0x1E12B
#define HID_EDIT_REDO                           0x1E12C
#define HID_WINDOW_NEW                          0x1E130
#define HID_WINDOW_ARRANGE                      0x1E131
#define HID_WINDOW_CASCADE                      0x1E132
#define HID_WINDOW_TILE_HORZ                    0x1E133
#define HID_WINDOW_TILE_VERT                    0x1E134
#define HID_WINDOW_SPLIT                        0x1E135
#define HID_APP_ABOUT                           0x1E140
#define HID_APP_EXIT                            0x1E141
#define HID_HELP_INDEX                          0x1E142
#define HID_HELP_FINDER                         0x1E143
#define HID_HELP_USING                          0x1E144
#define HID_CONTEXT_HELP                        0x1E145
#define HID_NEXT_PANE                           0x1E150
#define HID_PREV_PANE                           0x1E151
#define HID_FORMAT_FONT                         0x1E160
#define HID_OLE_INSERT_NEW                      0x1E200
#define HID_OLE_EDIT_LINKS                      0x1E201
#define HID_OLE_EDIT_CONVERT                    0x1E202
#define HID_OLE_EDIT_CHANGE_ICON                0x1E203
#define HID_OLE_EDIT_PROPERTIES                 0x1E204
#define HID_VIEW_TOOLBAR                        0x1E800
#define HID_VIEW_STATUS_BAR                     0x1E801
#define HID_RECORD_FIRST                        0x1E900
#define HID_RECORD_LAST                         0x1E901
#define HID_RECORD_NEXT                         0x1E902
#define HID_RECORD_PREV                         0x1E903
#define HID_WIZBACK                             0x13023
#define HID_WIZNEXT                             0x13024
#define HID_WIZFINISH                           0x13025

// Dialogs (AFX_HIDD_*)
#define AFX_HIDD_FILEOPEN                       0x27004
#define AFX_HIDD_FILESAVE                       0x27005
#define AFX_HIDD_FONT                           0x27006
#define AFX_HIDD_COLOR                          0x27007
#define AFX_HIDD_PRINT                          0x27008
#define AFX_HIDD_PRINTSETUP                     0x27009
#define AFX_HIDD_FIND                           0x2700A
#define AFX_HIDD_REPLACE                        0x2700B
#define AFX_HIDD_NEWTYPEDLG                     0x27801
#define AFX_HIDD_PRINTDLG                       0x27802
#define AFX_HIDD_PREVIEW_TOOLBAR                0x27803
#define AFX_HIDD_PREVIEW_SHORTTOOLBAR           0x2780B
#define AFX_HIDD_INSERTOBJECT                   0x27804
#define AFX_HIDD_CHANGEICON                     0x27805
#define AFX_HIDD_CONVERT                        0x27806
#define AFX_HIDD_PASTESPECIAL                   0x27807
#define AFX_HIDD_EDITLINKS                      0x27808
#define AFX_HIDD_FILEBROWSE                     0x27809
#define AFX_HIDD_BUSY                           0x2780A
#define AFX_HIDD_OBJECTPROPERTIES               0x2780C
#define AFX_HIDD_CHANGESOURCE                   0x2780D

// Prompts/Errors (AFX_HIDP_*)
#define AFX_HIDP_NO_ERROR_AVAILABLE             0x3F020
#define AFX_HIDP_INVALID_FILENAME               0x3F100
#define AFX_HIDP_FAILED_TO_OPEN_DOC             0x3F101
#define AFX_HIDP_FAILED_TO_SAVE_DOC             0x3F102
#define AFX_HIDP_ASK_TO_SAVE                    0x3F103
#define AFX_HIDP_FAILED_TO_CREATE_DOC           0x3F104
#define AFX_HIDP_FILE_TOO_LARGE                 0x3F105
#define AFX_HIDP_FAILED_TO_START_PRINT          0x3F106
#define AFX_HIDP_FAILED_TO_LAUNCH_HELP          0x3F107
#define AFX_HIDP_INTERNAL_FAILURE               0x3F108
#define AFX_HIDP_COMMAND_FAILURE                0x3F109
#define AFX_HIDP_FAILED_MEMORY_ALLOC            0x3F10A
#define AFX_HIDP_UNREG_DONE                     0x3F10B
#define AFX_HIDP_UNREG_FAILURE                  0x3F10C
#define AFX_HIDP_DLL_LOAD_FAILED                0x3F10D
#define AFX_HIDP_DLL_BAD_VERSION                0x3F10E
#define AFX_HIDP_PARSE_INT                      0x3F110
#define AFX_HIDP_PARSE_REAL                     0x3F111
#define AFX_HIDP_PARSE_INT_RANGE                0x3F112
#define AFX_HIDP_PARSE_REAL_RANGE               0x3F113
#define AFX_HIDP_PARSE_STRING_SIZE              0x3F114
#define AFX_HIDP_PARSE_RADIO_BUTTON             0x3F115
#define AFX_HIDP_PARSE_BYTE                     0x3F116
#define AFX_HIDP_PARSE_UINT                     0x3F117
#define AFX_HIDP_PARSE_DATETIME                 0x3F118
#define AFX_HIDP_PARSE_CURRENCY                 0x3F119
#define AFX_HIDP_FAILED_INVALID_FORMAT          0x3F120
#define AFX_HIDP_FAILED_INVALID_PATH            0x3F121
#define AFX_HIDP_FAILED_DISK_FULL               0x3F122
#define AFX_HIDP_FAILED_ACCESS_READ             0x3F123
#define AFX_HIDP_FAILED_ACCESS_WRITE            0x3F124
#define AFX_HIDP_FAILED_IO_ERROR_READ           0x3F125
#define AFX_HIDP_FAILED_IO_ERROR_WRITE          0x3F126
#define AFX_HIDP_STATIC_OBJECT                  0x3F180
#define AFX_HIDP_FAILED_TO_CONNECT              0x3F181
#define AFX_HIDP_SERVER_BUSY                    0x3F182
#define AFX_HIDP_BAD_VERB                       0x3F183
#define AFX_HIDP_FAILED_TO_NOTIFY               0x3F185
#define AFX_HIDP_FAILED_TO_LAUNCH               0x3F186
#define AFX_HIDP_ASK_TO_UPDATE                  0x3F187
#define AFX_HIDP_FAILED_TO_UPDATE               0x3F188
#define AFX_HIDP_FAILED_TO_REGISTER             0x3F189
#define AFX_HIDP_FAILED_TO_AUTO_REGISTER        0x3F18A
#define AFX_HIDP_FAILED_TO_CONVERT              0x3F18B
#define AFX_HIDP_GET_NOT_SUPPORTED              0x3F18C
#define AFX_HIDP_SET_NOT_SUPPORTED              0x3F18D
#define AFX_HIDP_ASK_TO_DISCARD                 0x3F18E
#define AFX_HIDP_FAILED_TO_CREATE               0x3F18F
#define AFX_HIDP_FAILED_MAPI_LOAD               0x3F190
#define AFX_HIDP_INVALID_MAPI_DLL               0x3F191
#define AFX_HIDP_FAILED_MAPI_SEND               0x3F192
#define AFX_HIDP_FILE_NONE                      0x3F1A0
#define AFX_HIDP_FILE_GENERIC                   0x3F1A1
#define AFX_HIDP_FILE_NOT_FOUND                 0x3F1A2
#define AFX_HIDP_FILE_BAD_PATH                  0x3F1A3
#define AFX_HIDP_FILE_TOO_MANY_OPEN             0x3F1A4
#define AFX_HIDP_FILE_ACCESS_DENIED             0x3F1A5
#define AFX_HIDP_FILE_INVALID_FILE              0x3F1A6
#define AFX_HIDP_FILE_REMOVE_CURRENT            0x3F1A7
#define AFX_HIDP_FILE_DIR_FULL                  0x3F1A8
#define AFX_HIDP_FILE_BAD_SEEK                  0x3F1A9
#define AFX_HIDP_FILE_HARD_IO                   0x3F1AA
#define AFX_HIDP_FILE_SHARING                   0x3F1AB
#define AFX_HIDP_FILE_LOCKING                   0x3F1AC
#define AFX_HIDP_FILE_DISKFULL                  0x3F1AD
#define AFX_HIDP_FILE_EOF                       0x3F1AE
#define AFX_HIDP_ARCH_NONE                      0x3F1B0
#define AFX_HIDP_ARCH_GENERIC                   0x3F1B1
#define AFX_HIDP_ARCH_READONLY                  0x3F1B2
#define AFX_HIDP_ARCH_ENDOFFILE                 0x3F1B3
#define AFX_HIDP_ARCH_WRITEONLY                 0x3F1B4
#define AFX_HIDP_ARCH_BADINDEX                  0x3F1B5
#define AFX_HIDP_ARCH_BADCLASS                  0x3F1B6
#define AFX_HIDP_ARCH_BADSCHEMA                 0x3F1B7
#define AFX_HIDP_SQL_CONNECT_FAIL               0x3F281
#define AFX_HIDP_SQL_RECORDSET_FORWARD_ONLY     0x3F282
#define AFX_HIDP_SQL_EMPTY_COLUMN_LIST          0x3F283
#define AFX_HIDP_SQL_FIELD_SCHEMA_MISMATCH      0x3F284
#define AFX_HIDP_SQL_ILLEGAL_MODE               0x3F285
#define AFX_HIDP_SQL_MULTIPLE_ROWS_AFFECTED     0x3F286
#define AFX_HIDP_SQL_NO_CURRENT_RECORD          0x3F287
#define AFX_HIDP_SQL_NO_ROWS_AFFECTED           0x3F288
#define AFX_HIDP_SQL_RECORDSET_READONLY         0x3F289
#define AFX_HIDP_SQL_SQL_NO_TOTAL               0x3F28A
#define AFX_HIDP_SQL_ODBC_LOAD_FAILED           0x3F28B
#define AFX_HIDP_SQL_DYNASET_NOT_SUPPORTED      0x3F28C
#define AFX_HIDP_SQL_SNAPSHOT_NOT_SUPPORTED     0x3F28D
#define AFX_HIDP_SQL_API_CONFORMANCE            0x3F28E
#define AFX_HIDP_SQL_SQL_CONFORMANCE            0x3F28F
#define AFX_HIDP_SQL_NO_DATA_FOUND              0x3F290
#define AFX_HIDP_SQL_ROW_UPDATE_NOT_SUPPORTED   0x3F291
#define AFX_HIDP_SQL_ODBC_V2_REQUIRED           0x3F292
#define AFX_HIDP_SQL_NO_POSITIONED_UPDATES      0x3F293
#define AFX_HIDP_SQL_LOCK_MODE_NOT_SUPPORTED    0x3F294
#define AFX_HIDP_SQL_DATA_TRUNCATED             0x3F295
#define AFX_HIDP_SQL_ROW_FETCH                  0x3F296
#define AFX_HIDP_SQL_INCORRECT_ODBC             0x3F297
#define AFX_HIDP_SQL_UPDATE_DELETE_FAILED       0x3F298
#define AFX_HIDP_SQL_DYNAMIC_CURSOR_NOT_SUPPORTED	0x3F299
#define AFX_HIDP_SQL_FIELD_NOT_FOUND            0x3F29A
#define AFX_HIDP_SQL_BOOKMARKS_NOT_SUPPORTED    0x3F29B
#define AFX_HIDP_SQL_BOOKMARKS_NOT_ENABLED      0x3F29C
#define AFX_HIDP_DAO_ENGINE_INITIALIZATION      0x3F2B0
#define AFX_HIDP_DAO_DFX_BIND                   0x3F2B1
#define AFX_HIDP_DAO_OBJECT_NOT_OPEN            0x3F2B2
#define AFX_HIDP_DAO_ROWTOOSHORT                0x3F2B3
#define AFX_HIDP_DAO_BADBINDINFO                0x3F2B4
#define AFX_HIDP_DAO_COLUMNUNAVAILABLE          0x3F2B5

// Frame Controls (AFX_HIDW_*)
#define AFX_HIDW_TOOLBAR                        0x5E800
#define AFX_HIDW_STATUS_BAR                     0x5E801
#define AFX_HIDW_PREVIEW_BAR                    0x5E802
#define AFX_HIDW_RESIZE_BAR                     0x5E803
#define AFX_HIDW_DOCKBAR_TOP                    0x5E81B
#define AFX_HIDW_DOCKBAR_LEFT                   0x5E81C
#define AFX_HIDW_DOCKBAR_RIGHT                  0x5E81D
#define AFX_HIDW_DOCKBAR_BOTTOM                 0x5E81E
#define AFX_HIDW_DOCKBAR_FLOAT                  0x5E81F

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFX_HH_H__
