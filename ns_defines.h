//===- ns_defines.h - The configuration macros. -----------------*- C++ -*-===//
//
//                     NotepadStarter
//
// This file is distributed under the New BSD License. See license.txt.
//
//===----------------------------------------------------------------------===//
//
// It's include version definition, Windows notification string. 
//
//===----------------------------------------------------------------------===//

#ifndef NS_DEFINES_H
#define NS_DEFINES_H

#include "VersionInfo.inc"
#define NS_VER_NAME TEXT("Version ")
#define NS_APP_TITLE TEXT("NotepadStarter")
#define NS_SPACE TEXT(" ")
#define NS_ID_FILE_OPEN   103500
#define NS_ID_FILE_CLOSED 103501
#define NS_ID_PROG_CLOSED 103502
#define NS_HEADLINE NS_APP_TITLE NS_SPACE NS_VERSION
#define NS_VERSION_INFO NS_VER_NAME NS_VERSION
#define NS_SPC TEXT(" ")

#define NS_PLGN_TITLE TEXT("NotepadStarter Plugin ")
#define NS_PLGN_HELP TEXT("\n\
Copyright: Yonggang Luo\n\
This plugin is the hook for NotepadStarter observing if an edit window\n\
becomes closed. If you close a window in NoptePad++ this plugin\n\
will send a release message to NotepadStarter systemtray App to release \n\
the blocked call.\n\
For further information see https://github.com/lygstate/NotepadStarter/")

#define NS_PLGN_HELP_TEXT NS_PLGN_TITLE NS_VERSION_INFO NS_PLGN_HELP

#define NS_PRG_HELP TEXT("\n\
Copyright: Yonggang Luo\n\
This program replaces the notepad.exe by a different editor like Notepad++\n\
It creates a blocking application in the systemtray and sends a request\n\
to open the text file in NotePad++.\n\
When the file is getting closed in NotePad++ it receives a close message\n\
and releases the blocking call.\n\
For further information see https://github.com/lygstate/NotepadStarter/")

#define NS_PRG_HELP_TEXT NS_APP_TITLE NS_SPC NS_VERSION_INFO NS_PRG_HELP

#endif