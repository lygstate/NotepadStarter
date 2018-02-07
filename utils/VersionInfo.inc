//===- VersionInfo.inc - The NotepadStarter version macros. -----*- C++ -*-===//
//
//                     NotepadStarter
//
// This file is distributed under the New BSD License. See license.txt.
//
//===----------------------------------------------------------------------===//
//
// It's defines the NS_VERSION_NUMBER for latter usage
//
//===----------------------------------------------------------------------===//

#ifndef VERSIONINFO_INC
#define VERSIONINFO_INC

#define NS_VERSION_MAJOR 2
#define NS_VERSION_MINOR 3
#define NS_VERSION_MAINTENANCE 1
#define NS_VERSION_REVISION 0

#define _NS_STR_HELPER(x) L#x
#define _NS_STR(x) _NS_STR_HELPER(x)
#define NS_VERSION _NS_STR(NS_VERSION_MAJOR) L"." _NS_STR(NS_VERSION_MINOR)  L"." _NS_STR(NS_VERSION_MAINTENANCE)  L"." _NS_STR(NS_VERSION_REVISION)

#endif

