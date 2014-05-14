/* -------------------------------------
This file is part of NotepadStarterPlugin for NotePad++ 
Copyright (C)2013 Matthias H. mattesh(at)gmx.net
partly copied from the NotePad++ project from 
Don HO donho(at)altern.org 

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
#include "Windows.h"
#include "PluginInterface.h"
#include "NotepadDebug.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "ns_defines.h"
#define MNU_ABOUT 0

const int nbFunc = 1;
FuncItem _funcItem[nbFunc];
NppData _nppData;
HINSTANCE _hModule;

const TCHAR NPP_PLUGIN_NAME[] = TEXT("NotepadStarterPlugin");
enum teNppWindows {
   scnMainHandle,
   scnSecondHandle,
   nppHandle,
   scnActiveHandle // used to take the one being active regardless if main or second
};

HWND getCurrentHScintilla(teNppWindows which) 
{  
   if(_nppData._nppHandle==0) {
      return 0;	
   }
   int activView=0;
   switch(which) 
   {
   case scnMainHandle:
      return _nppData._scintillaMainHandle;
   case scnSecondHandle:
      return _nppData._scintillaSecondHandle;
   case nppHandle:
      return _nppData._nppHandle;
   case scnActiveHandle:
      ::SendMessage(_nppData._nppHandle, 
         NPPM_GETCURRENTSCINTILLA,
         (WPARAM)0,
         (LPARAM)&activView);
      return (activView==0)?_nppData._scintillaMainHandle:_nppData._scintillaSecondHandle;
   default :
      return 0;
   } // switch
}

LRESULT execute(teNppWindows window, UINT Msg, WPARAM wParam=0, LPARAM lParam=0) 
{
   HWND hCurrentEditView = getCurrentHScintilla(window); // it's 0 for first = find text		
   return ::SendMessage(hCurrentEditView, Msg, wParam, lParam);
}

std::string wstring2string(const std::wstring & rwString, UINT codepage)
{
   int len = WideCharToMultiByte(codepage, 0, rwString.c_str(), -1, NULL, 0, NULL, NULL);
   if(len > 0) {		
      std::vector<char> vw(len);
      WideCharToMultiByte(codepage, 0, rwString.c_str(), -1, &vw[0], len, NULL, NULL);
      return &vw[0];
   } else {
      return "";
   }
}

extern "C" __declspec(dllexport) void doAboutDialog() 
{
   ::MessageBox(NULL, NS_PLGN_HELP_TEXT, NS_HEADLINE, MB_OK);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  reasonForCall, 
                      LPVOID lpReserved )
{
   switch (reasonForCall) {
      case DLL_PROCESS_ATTACH:
         _hModule = (HINSTANCE)hModule;
         memset(_funcItem, 0, sizeof(_funcItem));
         _funcItem[MNU_ABOUT]._pFunc = doAboutDialog;
         lstrcpy(_funcItem[MNU_ABOUT]._itemName, TEXT("About..."));
         break;
      case DLL_PROCESS_DETACH:break;
      case DLL_THREAD_ATTACH:break;
      case DLL_THREAD_DETACH:break;
      default:break;
   }
   return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
   _nppData = notpadPlusData;
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
   return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
   *nbF = nbFunc;
   return _funcItem;
}

struct EnumMessage {
	UINT msgID;
	WPARAM wparam;
	LPARAM lparam;
};

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	std::wstring str;
	str.resize(1024);
	int sz = GetClassNameW(hWnd, (LPWSTR)str.data(), str.size());
	if (sz <= 0) {
		return TRUE;
	}
	str[sz] = 0;
	str.resize(sz);
	if (str == NS_APP_TITLE) {
		EnumMessage *msg = (EnumMessage *)lParam;
		if (msg != NULL) {
			::SendMessageW(hWnd, msg->msgID, msg->wparam, (msg->lparam));
		}
	}
	return TRUE;
}

std::wstring getCurrentFilename(uptr_t bufferId) {
	int len = (int)execute(nppHandle, NPPM_GETFULLPATHFROMBUFFERID, bufferId);
	if (len <= 0) {
		return L"";
	}
	std::wstring str;
	str.resize(len + 1);
	len = (int)execute(nppHandle, NPPM_GETFULLPATHFROMBUFFERID, bufferId, (LPARAM)str.data());
	if (len <= 0) {
		return L"";
	}
	str[len] = 0;
	str.resize(len + 1);
	return std::move(str);
}

void reportMessage(std::wstring const &savedPath) {
	COPYDATASTRUCT cpst;
	cpst.dwData = NS_ID_FILE_CLOSED;
	cpst.cbData = savedPath.size() * sizeof(wchar_t);
	cpst.lpData = (PVOID)savedPath.c_str();
	DBGW1("beNotified() sending closed message for %s", savedPath.c_str());
	//DBG1("beNotified() sending closed message for %s", s.c_str());
	EnumMessage msg;
	msg.msgID = WM_COPYDATA;
	msg.wparam = (WPARAM)_nppData._nppHandle;
	msg.lparam = (LPARAM)&cpst;
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&msg);
}

std::wstring savedPath;

extern "C" __declspec(dllexport) void beNotified(SCNotification *notification)
{
	static uptr_t CloseId = 0;

	switch (notification->nmhdr.code)
	{
	case NPPN_FILEBEFORESAVE: {
		uptr_t SaveId = notification->nmhdr.idFrom;
		DBGW2("beNotified() NPPN_FILEBEFORESAVE hwndNpp = %d SaveId = %d",
			notification->nmhdr.hwndFrom, SaveId);
		savedPath = getCurrentFilename(SaveId);
		break;
	}
	case NPPN_FILEBEFORECLOSE:
	{
		CloseId = notification->nmhdr.idFrom;
		DBGW2("beNotified() NPPN_FILEBEFORECLOSE hwndNpp = %d CloseId = %d",
			notification->nmhdr.hwndFrom, CloseId);
		savedPath = getCurrentFilename(CloseId);
		DBGW2("beNotified() NPPN_FILEBEFORECLOSE CloseId = %d Path '%s' ",
			notification->nmhdr.idFrom, savedPath.c_str());
		break;
	}
	case NPPN_FILESAVED: {
		std::wstring currentPath = getCurrentFilename(notification->nmhdr.idFrom);
		if (currentPath != savedPath) { // It's save as
			reportMessage(savedPath);
		}
		break;
	}
	case NPPN_FILECLOSED:
	{
		if (CloseId == notification->nmhdr.idFrom) {
			int pos = (int)execute(nppHandle, NPPM_GETPOSFROMBUFFERID, CloseId);
			DBG2("beNotified() NPPN_FILECLOSED CloseId = %d pos %d ", CloseId, pos);
			if (pos == -1) {
				reportMessage(savedPath);
			}
		}
		CloseId = 0;
		break;
	}
	case NPPN_SHUTDOWN:
	{
		EnumMessage msg;
		msg.msgID = NS_ID_PROG_CLOSED;
		msg.wparam = 0;
		msg.lparam = 0;
		EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&msg);
		break;
	}

	default:
		return;
	}
}

// http://sourceforge.net/forum/forum.php?forum_id=482781
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
   return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
   return TRUE;
}
#endif //UNICODE
