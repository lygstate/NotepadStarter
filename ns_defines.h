/* -------------------------------------
This file is part of AnalysePlugin for NotePad++ 
Copyright (C)2013 Matthias H. mattesh(at)gmx.net

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
------------------------------------- */
#ifndef NS_DEFINES_H
#define NS_DEFINES_H

#include "VersionInfo.inc"
#define NS_VER_NAME TEXT("Version ")
#define NS_APP_TITLE TEXT("NotepadStarter")
#define NS_SPACE TEXT(" ")
#define NS_ID_FILE_CLOSED 103401
#define NS_ID_PROG_CLOSED 103402
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
For further information see https://github.com/lygstate/notepadstarter/")

#define NS_PLGN_HELP_TEXT NS_PLGN_TITLE NS_VERSION_INFO NS_PLGN_HELP

#define NS_PRG_HELP TEXT("\n\
Copyright: Yonggang Luo\n\
This program replaces the notepad.exe by a different editor like Notepad++\n\
It creates a blocking application in the systemtray and sends a request\n\
to open the text file in NotePad++.\n\
When the file is getting closed in NotePad++ it receives a close message\n\
and releases the blocking call.\n\
For further information see https://github.com/lygstate/notepadstarter/")

#define NS_PRG_HELP_TEXT NS_APP_TITLE NS_SPC NS_VERSION_INFO NS_PRG_HELP

#endif