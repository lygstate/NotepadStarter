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
#include <errno.h>
#include "SystemTraySDK.h"
#include "resource.h"
#include <assert.h>
#include "ns_defines.h"
#define MDBG_COMP "NS:"
#include "NotepadDebug.h"
#include "NotepadUtils.h"

#define CAPTION L"NS-" NS_VERSION  L":" // must be 9 chars long to fit to current copyName()
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
std::wstring g_NppFilepath;
DWORD  g_Pid = 0xFFFFFFFF;

void StopNotepad() {
	_TrayIcon.RemoveIcon();
	ExitProcess(0);
}

VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UnregisterWait(m_hRegisterWait);
	CloseHandle(g_hMonitorProcess);
	StopNotepad();
}



UINT timer;

VOID CALLBACK Timer(HWND hwnd,
	UINT uMsg,
	UINT_PTR idEvent,
	DWORD dwTime
	)
{
	HANDLE m_hTempProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, g_Pid);
	if (m_hTempProcess == NULL) {
		StopNotepad();
	}
	std::wstring thisNppFilePath = GetModuleExecutable(g_hMonitorProcess, NULL);
	if (thisNppFilePath != g_NppFilepath) {
		StopNotepad();
	}
	CloseHandle(m_hTempProcess);
}

void MonitorPid(DWORD pid) {
	g_Pid = pid;
	g_hMonitorProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (g_hMonitorProcess == NULL) {
		StopNotepad();
	}
	g_NppFilepath = GetModuleExecutable(g_hMonitorProcess, NULL);
	if (g_NppFilepath == L"") {
		StopNotepad();
	}
	if (FALSE == RegisterWaitForSingleObject(
		&m_hRegisterWait,
		g_hMonitorProcess,
		WaitOrTimerCallback,
		0,
		INFINITE,
		WT_EXECUTEONLYONCE))
	{
		CloseHandle(g_hMonitorProcess);
		g_hMonitorProcess = NULL;
		StopNotepad();
	}
/* TODO: to start heart beat at all, and using 
nanomsg to send/recv heartbeat.
*/
	timer = SetTimer(0, // window handle
		0, // id of the timer message, leave 0 in this case
		500, // millis
		Timer // callback
		);
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
					PostQuitMessage(0);
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

std::wstring QueryNotepadPlusPlusCommand() {
	HKEY hKey;
	LSTATUS errorCode = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\notepad.exe",
		0,
		KEY_READ,
		&hKey);
	std::wstring NotepadPlusPlusExecutable = L"";
	std::wstring NotepadStarter;
	bool hasNpp = false;
	if (errorCode == ERROR_SUCCESS)
	{
		hasNpp = QueryRegistryString(hKey, L"Notepad++", NotepadPlusPlusExecutable);
		NotepadPlusPlusExecutable = FullPath(NotepadPlusPlusExecutable);
		RegCloseKey(hKey);
	}
	if (!hasNpp || !ExistPath(NotepadPlusPlusExecutable)) {
		MessageBoxW(NULL, L"Please reinstall NotepadStarter", TEXT("NotepadStarter Error"), MB_OK);
		ExitProcess(0);
	}
	return NotepadPlusPlusExecutable;
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
	filename = StripFilename(arguments.substr(i));
	if (filename[0] == L':') {
		return std::move(filename);
	}
	filename = FullPath(filename);
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

BOOL IsProcessRunning(HANDLE hProcess)
{
	DWORD ret = WaitForSingleObject(hProcess, 0);
	return ret == WAIT_TIMEOUT;
}

void testCommandLineParse() {
	std::wstring name = GetFilenameParameter(L"D:\\CI-Tools\\IDE\\npp\\NotepadStarter.exe notepad test.txt");
}


void enableDebugToFile() {
	char name[1024];
	DWORD pid = GetCurrentProcessId();
	sprintf(name, "D:\\CI\\ides\\NotepadStarter\\log.%d.txt", pid);
	freopen(name, "w", stderr);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
}

int DoCommand(wstring cmd) {
	wstring executableScript = GetParentDir(GetThisExecutable()) + L"\\";
	if (cmd == L":install-registry")
	{
		executableScript += L"NotepadStarterInstall.bat";
	}
	else if (cmd == L":install-replace")
	{
		executableScript += L"NotepadStarterReplacer.bat";
	}
	else if (cmd == L":uninstall")
	{
		executableScript += L"NotepadStarterUninstall.bat";
	}
	else
	{
		return -1;
	}

	PVOID OldValue = NULL;
	Wow64DisableWow64FsRedirection(&OldValue);

	STARTUPINFO si;
	PROCESS_INFORMATION oProcessInfo;
	LaunchProcess(si, oProcessInfo, executableScript, false, true);
	return 0;
}

// main 
int WINAPI wWinMain(
	HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
	)
{
	//enableDebugToFile();

	std::wstring  fullCommandLine = GetCommandLineW();
	filename = GetFilenameParameter(fullCommandLine);
	//testCommandLineParse();

	if (filename[0] == L':') {
		return DoCommand(filename);
	}

	if (bDebug) {
		if (IDCANCEL == MessageBoxW(NULL, fullCommandLine.c_str(), L"NotepadStarter initial command line", MB_OKCANCEL))
			return -1;
	}

	MyRegisterClass(hThisInstance);

	// Flag for waiting or note waiting for notepad++ to exit. "yes" | "no"
	bool bWaitForNotepadClose = true;
	/* */
	std::wstring cmd = QueryNotepadPlusPlusCommand();

	if (filename.size() > 0) {
		cmd += L" \"" + filename + L"\"";
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
		std::wstring ballonMsg = std::wstring(CAPTION) + filename;
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
			<< dw << L": " << errorMsg << L"\nPlease reinstall NotepadStarter";
		MessageBoxW(NULL, msg.str().c_str(), TEXT("NotepadStarter Error"), MB_OK);
	} else if (bWaitForNotepadClose) {
		// message loop for tray icon:
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		_TrayIcon.RemoveIcon();
		CloseHandle(oProcessInfo.hProcess);
		CloseHandle(oProcessInfo.hThread);
	}
	return 0;
}
