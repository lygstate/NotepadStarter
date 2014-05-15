//===- NotepadStarter.cpp: notepad++ launcher -------------------*- C++ -*-===//
//
//                     NotepadStarter
//
// This file is distributed under the New BSD License. See license.txt.
//
//===----------------------------------------------------------------------===//
//
// NotepadStarter replaces the original Windows notepad with additional effort
// was done to suspend the calling task as long as the notepad++ window with the
// edited text is open. We use the Windows file path to identify if the path we
// required is corresponding this NotepadStarter instance.
//
//===----------------------------------------------------------------------===//

#include <windows.h>
#include <winreg.h>
#include <stdio.h>
#include <string.h>
#include <wctype.h>
#include <sstream>

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#endif

#include <shellapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ERRNO.H>
#include "SystemTraySDK.h"
#include "resource.h"
#include <assert.h>
#include "ns_defines.h"
#define MDBG_COMP "NS:"
#include "NotepadDebug.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#define CAPTION_POST TEXT(" ")
#define CAPTION_PRE TEXT("NS ")
#define CAPTION CAPTION_PRE NS_VERSION CAPTION_POST // must be 9 chars long to fit to current copyName()
#define WM_ICON_NOTIFY WM_APP+10
#define MAX_TOOLTIP_LEN 64
#define MAX_TITLE_LEN 100

static CSystemTray _TrayIcon;
bool bDebug = false;
static std::wstring filename;
#ifndef length_of
#define length_of(x) (sizeof(x)/sizeof((x)[0]))
#endif

HANDLE g_hMonitorProcess = NULL;
HANDLE m_hRegisterWait;
VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	if( FALSE == TimerOrWaitFired )
	{
		UnregisterWait(m_hRegisterWait);
		CloseHandle(g_hMonitorProcess);
		ExitProcess(0);
	}
}

void MonitorPid(DWORD pid) {
	g_hMonitorProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);

	if (g_hMonitorProcess != NULL
		&& FALSE == RegisterWaitForSingleObject(
		&m_hRegisterWait,
		g_hMonitorProcess,
		WaitOrTimerCallback,
		0,
		INFINITE,
		WT_EXECUTEONLYONCE))
	{
		CloseHandle(g_hMonitorProcess);
		g_hMonitorProcess = NULL;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COPYDATA:
		if (lParam){
			COPYDATASTRUCT * pcds = (COPYDATASTRUCT *)lParam;
			LPCWSTR lpszString = (LPCWSTR)(pcds->lpData);
			switch (pcds->dwData) {
			default:break;
			case NS_ID_FILE_OPEN: {
				DBGW1("WndProc: got message NS_ID_FILE_OPEN: %s", lpszString);
				if (filename == lpszString && g_hMonitorProcess == NULL) { //TODO: more robust filename compare
					MonitorPid((DWORD)wParam);
				}
				break;
			}
			case NS_ID_FILE_CLOSED: {
				DBGW1("WndProc: got message NS_ID_FILE_CLOSED: %s", lpszString);
				if (filename == lpszString) {
					ExitProcess(0);
				} else {
					if (bDebug) {
						std::wstring msg = filename + L"!=" + lpszString;
						MessageBox(NULL, msg.c_str(), NS_APP_TITLE L" Not mine...", MB_OK);
					}
				}
				break;
			}
			case NS_ID_PROG_CLOSED: {
				DBGW1("WndProc: got message NS_ID_PROG_CLOSED: %s", lpszString);
				break;
			}
			}
		} else {
			DBG1("WndProc: got message WM_COPYDATA 0x%x", lParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_ICON_NOTIFY:
		return _TrayIcon.OnTrayNotification(wParam, lParam);

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			::MessageBox(hWnd, NS_PRG_HELP_TEXT, NS_HEADLINE, MB_OK);
			break;
		case IDM_EXIT:
			// cause call the close the program
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// register my application window class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	TCHAR szTitle[MAX_TITLE_LEN] = NS_APP_TITLE;

	// Initialize global strings
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, (LPCTSTR)IDI_NOTEPAD_STARTER_ICON);
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_CNTXT_MENU);  // TODO: check if this is correct
	wcex.lpszClassName = szTitle;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, (LPCTSTR)IDI_NOTEPAD_STARTER_ICON);

	return ::RegisterClassEx(&wcex);
}

// initialize my instance and show the tray icon
BOOL InitInstance(HINSTANCE hInstance, std::wstring arg/*, int nCmdShow*/)
{
	HWND hWnd;
	TCHAR szTitle[MAX_TITLE_LEN];

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, _countof(szTitle));

	hWnd = CreateWindow(szTitle, szTitle, WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	if (!_TrayIcon.Create(hInstance,
		hWnd,                            // Parent window
		WM_ICON_NOTIFY,                  // Icon notify message to use
		arg.c_str(),                     // tooltip
		::LoadIcon(hInstance, (LPCTSTR)IDI_NOTEPAD_STARTER_ICON),
		IDR_CNTXT_MENU))
	{
		// Create the icon failed.
		return FALSE;
	}

	_TrayIcon.SetMenuDefaultItem(IDM_EXIT);

	return TRUE;
}

std::wstring QueryErrorString(DWORD dw) {
	// Retrieve the system error message for the last-error code

	LPWSTR lpMsgBuf = 0;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0, NULL);
	std::wstring ret = lpMsgBuf;
	LocalFree(lpMsgBuf);
	return std::move(ret);
}

bool QueryRegistryString(HKEY hKey, std::wstring key, std::wstring &value) {
	DWORD iType = REG_NONE;
	DWORD iDataSize = 0;
	LSTATUS errorCode = ::RegQueryValueExW(hKey, key.c_str(), NULL, &iType, NULL, &iDataSize);
	if (iType != REG_SZ)
	{
		value.resize(0);
		return false;
	}
	if (errorCode == ERROR_SUCCESS && iDataSize > 0) {
		if (iDataSize & 1) ++iDataSize;
		DWORD sz = iDataSize >> 1;
		value.reserve(sz + 1);
		value.resize(sz);
		value[sz] = 0;
		errorCode = ::RegQueryValueExW(hKey, key.c_str(), NULL, &iType, (LPBYTE)value.data(), &iDataSize);
	}
	if (errorCode != ERROR_SUCCESS) {
		value.resize(0);
		return false;
	}
	return true;
}

wstring FullPath(wstring const &inPath) {
	std::vector<wchar_t> p;
	DWORD len = 0;
	p.resize(32768);
	len = GetFullPathNameW(inPath.c_str(), p.size(), p.data(), NULL);
	wstring newStr(p.begin(), p.begin() + len);
	len = GetLongPathNameW(newStr.c_str(), p.data(), p.size());
	if (len > 0) {
		return wstring(p.begin(), p.begin() + len);
	}
	return std::move(newStr);
}

wstring GetModuleExecutable(HANDLE process, HMODULE module) {
	std::vector<wchar_t> p;
	p.resize(8192);
	while (true) {
		DWORD sz;
		if (process == NULL) {
			sz = GetModuleFileNameW(module, p.data(), p.size() - 1);
		}
		else {
			sz = GetModuleFileNameExW(process, module, p.data(), p.size() - 1);
		}
		p.resize(sz + 1);
		p.data()[sz] = 0;
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER || p.size() <= sz){
			p.resize(p.size() << 1);
			continue;
		}
		break;
	}
	if (p.size() == 1) {
		return L"";
	}
	return std::move(FullPath(p.data()));
}

wstring GetThisExecutable()
{
	return GetModuleExecutable(NULL, NULL);
}


wstring GetParentDir(std::wstring p) {
	wchar_t *ptr = (wchar_t*)p.c_str();
	PathRemoveFileSpecW(ptr);
	return ptr;
}

bool LaunchProcess(STARTUPINFO& si, PROCESS_INFORMATION& oProcessInfo, std::wstring cmd, bool wait);

std::wstring QueryNotepadCommand() {
	HKEY hKey;
	LSTATUS errorCode = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\notepad.exe",
		0,
		KEY_READ,
		&hKey);
	std::wstring NotepadPlusPlusFolder = L"";
	if (errorCode == ERROR_SUCCESS)
	{
		bool hasNpp = QueryRegistryString(hKey, L"Notepad++", NotepadPlusPlusFolder);
		NotepadPlusPlusFolder = FullPath(NotepadPlusPlusFolder);
		if (!hasNpp) {
			std::wstring NotepadStarter;
			QueryRegistryString(hKey, L"Debugger", NotepadStarter);
			NotepadStarter = FullPath(NotepadStarter);
			std::wstring NotepadStarterCurrent = GetThisExecutable();
			NotepadPlusPlusFolder = GetParentDir(NotepadStarterCurrent);
			if (NotepadStarter != NotepadStarterCurrent) {
				STARTUPINFO si;
				PROCESS_INFORMATION oProcessInfo;
				LaunchProcess(si, oProcessInfo, NotepadPlusPlusFolder + L"\\NotepadStarterInstall.bat", true);
			}

		}
		RegCloseKey(hKey);
	} else {
		if (bDebug) {
			std::wstring msg = L"Open registry caused error" + QueryErrorString(GetLastError());
			MessageBoxW(NULL, msg.c_str(), L"Error opening registry", MB_OK);
		}
	}
	return NotepadPlusPlusFolder + L"\\notepad++.exe";
}

// read the parameters from registry how to behave
bool ReadOptions(std::wstring& notepadCmd, bool& bWaitForNotepadClose, /*DWORD& uWaitTime,*/ bool& bDebug)
{
	notepadCmd = QueryNotepadCommand(); // The commands path is relative to this executable, test if the executable exist, if doesn't ,then return false
	bWaitForNotepadClose = true; // Flag for waiting or note waiting for notepad++ to exit. "yes" | "no"
	//bDebug = false; // default not to debug "yes" | "no"

	return true;
}

std::wstring StripFilename(std::wstring const& arg) {
	size_t pos = 0;
	size_t end = arg.size() - 1;

	while (pos <= end) {
		if (iswspace(arg[pos]) || arg[pos] == L'\"') {
			++pos;
			continue;
		}
		if (iswspace(arg[end]) || arg[end] == L'\"') {
			--end;
			continue;
		}
		break;
	}
	return std::move(arg.substr(pos, end + 1 - pos));
}

std::wstring GetFilenameParameter(wstring const& arguments) {
	// different calling conventions
	static wstring const notepadNames[] = {
		L"\\NOTEPADSTARTER.EXE",
		L"\\NOTEPADSTARTER",
		L"\\NOTEPAD.EXE",
		L"\\NOTEPAD",
	};

	std::wstring filename = L"";

	size_t i = 0;
	while (i < arguments.size()) {
		if (iswspace(arguments[i])) {
			++i;
			continue;
		}
		size_t j = i;
		if (arguments[i] == L'\"') {
			++j;
			while (j < arguments.size()) {
				++j;
				if (arguments[j - 1] != L'\"') {
					continue;
				}
				if (j >= arguments.size()) {
					break;
				}
				if (iswspace(arguments[j])) {
					break;
				}
				++j;
			}
			++j;
		}
		else
		{
			while (j < arguments.size() && !iswspace(arguments[j])) {
				++j;
			}
		}
		std::wstring arg = StripFilename(arguments.substr(i, j - i));
		std::wstring upperApp = FullPath(arg);
		for (size_t k = 0; k < upperApp.size(); ++k) {
			upperApp[k] = towupper(upperApp[k]);
		}
		size_t idx = std::wstring::npos;
		for (int k = 0; k < length_of(notepadNames); ++k) {
			idx = upperApp.rfind(notepadNames[k]);
			if (idx != std::wstring::npos) {
				std::wstring notepad = notepadNames[k];
				idx += notepad.size();
				if (idx == upperApp.size())
					break;
				idx = std::wstring::npos;
			}
		}

		if (idx == std::wstring::npos) {
			break;
		}
		i = j;
	}

	if (i >= arguments.size()) {
		return std::move(filename);
	}

	filename = FullPath(StripFilename(arguments.substr(i)));
	if (*filename.rbegin() == '\\') filename.resize(filename.size() - 1);

	struct _stat st;
	memset(&st, 0, sizeof(st));
	int ret = _wstat(filename.c_str(), &st);

	if (!ret && (st.st_mode & _S_IFDIR) != 0) { //If this file is a directory, try to append \.txt to it.
		if (bDebug) {
			MessageBoxW(NULL, arguments.c_str(), L"NotepadStarter is opening the directory as txt.", MB_OK);
		}
		filename += L"\\.txt";
	}

	return std::move(filename);
}

// create from lpszArgument a valid arguments list for program to be called 
// this is especially to remove the notepad.exe name from the line
// cmd: the final running cmd
// filename: the file that notepad will open
bool CreateCommandLine(std::wstring& cmd, std::wstring& filename, boolean const& bDebug, wstring const& arguments)
{
	std::wstring upperApp;
	std::wstring  fullCommandLine = GetCommandLineW();
	if (bDebug) {
		if (IDCANCEL == MessageBoxW(NULL, fullCommandLine.c_str(), L"NotepadStarter initial command line", MB_OKCANCEL))
			return false;
	}

	filename = GetFilenameParameter(fullCommandLine);

	if (filename.size() > 0) {
		cmd = cmd + L" \"" + filename + L"\"";
	}
	return true;
}

bool LaunchProcess(STARTUPINFO& si, PROCESS_INFORMATION& oProcessInfo, std::wstring cmd, bool wait) {
	// preparation the notepad++ process launch information.
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&oProcessInfo, 0, sizeof(oProcessInfo));
	// launch the Notepad++
	std::vector<wchar_t> cmdStr(cmd.c_str(), cmd.c_str() + cmd.size() + 1);
	if (CreateProcessW(NULL, cmdStr.data(), NULL, NULL, false, 0, NULL, NULL, &si, &oProcessInfo) == FALSE) {
		CloseHandle(oProcessInfo.hProcess);
		CloseHandle(oProcessInfo.hThread);
		return false;
	}
	if (wait) {
		// Wait until child process exits.
		WaitForSingleObject(oProcessInfo.hProcess, INFINITE);
		CloseHandle(oProcessInfo.hProcess);
		CloseHandle(oProcessInfo.hThread);
	}
	return true;
}


HANDLE FoundProcessHandle(std::wstring const & processExecutable)
{
	std::vector<DWORD> processList;
	processList.resize(16);
	DWORD dw = 0;
	while (true) {
		DWORD pBytesReturned;
		if (EnumProcesses(processList.data(), processList.size() * sizeof(DWORD), &pBytesReturned) == FALSE) {
			return 0;
		}
		DWORD count = pBytesReturned / sizeof(DWORD);
		if (count < processList.size()) {
			processList.resize(count);
			break;
		}
		processList.resize(processList.size() << 1);
	}
	for (size_t i = 0; i < processList.size() && dw == 0; ++i) {
		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			processList[i]);
		HMODULE hMod;
		DWORD cbNeeded;
		if (hProcess == NULL) {
			continue;
		}
		if (EnumProcessModules(
			hProcess,
			&hMod,
			sizeof(hMod),
			&cbNeeded))
		{
			std::wstring currentFilepath = GetModuleExecutable(hProcess, hMod);
			if (currentFilepath == processExecutable) {
				return hProcess;
			}
		}
	}
	return NULL;
}


BOOL IsProcessRunning(HANDLE hProcess)
{
	DWORD ret = WaitForSingleObject(hProcess, 0);
	return ret == WAIT_TIMEOUT;
}

void testCommandLineParse() {
	std::wstring name = GetFilenameParameter(L"D:\\CI-Tools\\IDE\\npp\\NotepadStarter.exe notepad test.txt");
}

// main 
int WINAPI wWinMain(
	HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
	)
{
	std::wstring commandline;
	std::wstring cmd;
	bool bWaitForNotepadClose = true;
	//testCommandLineParse();
	MyRegisterClass(hThisInstance);

	if (!ReadOptions(commandline, bWaitForNotepadClose, bDebug))
	{
		return -1; // error case
	}

	cmd = commandline;
	if (!CreateCommandLine(cmd, filename, bDebug, lpCmdLine))
	{
		return -1; // error case
	}

	if (bDebug) {
		if (IDCANCEL == MessageBox(NULL, cmd.c_str(), TEXT("Command to be called"), MB_OKCANCEL))
			return 0;
	}

	if (FALSE == InitInstance(hThisInstance, filename))
	{
		return -1; // error case
	}
	if (filename.size() == 0) { // No need to wait for newly created files.
		bWaitForNotepadClose = false;
	}
	if (bWaitForNotepadClose) {
		// Wait until child process to exit.

		std::wstring ballonMsg = std::wstring(CAPTION) + L":" + filename;
		_TrayIcon.ShowBalloon(TEXT("Double click this icon when editing is finished..."), ballonMsg.c_str(), 1);
	}
	//MessageBoxW(NULL, cmd.c_str(), L"NotepadStarter initial command line", MB_OK);
	STARTUPINFO si;
	PROCESS_INFORMATION oProcessInfo;
	if (!LaunchProcess(si, oProcessInfo, cmd, false)) {
		DWORD dw = GetLastError();
		std::wstring errorMsg = QueryErrorString(dw);
		std::wstringstream msg;
		msg << L"CreateProcess() failed with error "
			<< dw << L": " << errorMsg << L"\nPlease consider checking the configurations in NotepadStarter.ini";
		MessageBoxW(NULL, msg.str().c_str(), TEXT("NotepadStarter Error"), MB_OK);
	} else if (bWaitForNotepadClose) {
		// message loop for tray icon:
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		CloseHandle(oProcessInfo.hProcess);
		CloseHandle(oProcessInfo.hThread);
	}
	return 0;
}
