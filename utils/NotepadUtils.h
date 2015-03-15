//===- NotepadUtils.h - Utils for process and manipulate paths --*- C++ -*-===//
//
//                     NotepadStarter
//
// This file is distributed under the New BSD License. See license.txt.
//
//===----------------------------------------------------------------------===//
//
// This CSystemTray class provide a way to create a Windows notification icon
// to track whether NotepadStarter is terminated.
//
//===----------------------------------------------------------------------===//

#include <string>
#include <cwchar>
#include <vector>
#include <windows.h>

using std::wstring;
using std::vector;

bool QueryRegistryString(HKEY hKey, std::wstring key, std::wstring &value);
wstring FullPath(wstring const &inPath);
wstring GetModuleExecutable(HANDLE process, HMODULE module);
wstring GetThisExecutable();
wstring GetParentDir(std::wstring p);
bool ExistPath(wstring const& p);
std::wstring QueryErrorString(DWORD dw);
HANDLE FoundProcessHandle(std::wstring const & processExecutable);
bool LaunchProcess(STARTUPINFO& si, PROCESS_INFORMATION& oProcessInfo, std::wstring cmd, bool wait, bool noWindow = false);
std::wstring GetEnvironmentVariableValue(const std::wstring& name);
