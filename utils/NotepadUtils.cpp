#include <NotepadUtils.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

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
	len = GetFullPathNameW(inPath.c_str(), (DWORD)p.size(), p.data(), NULL);
	wstring newStr(p.begin(), p.begin() + len);
	len = GetLongPathNameW(newStr.c_str(), p.data(), (DWORD)p.size());
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
			sz = GetModuleFileNameW(module, p.data(), (DWORD)p.size() - 1);
		}
		else {
			sz = GetModuleFileNameExW(process, module, p.data(), (DWORD)p.size() - 1);
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

bool ExistPath(wstring const& p) {
	return PathFileExistsW(p.c_str()) == TRUE;
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

HANDLE FoundProcessHandle(std::wstring const & processExecutable)
{
	std::vector<DWORD> processList;
	processList.resize(16);
	DWORD dw = 0;
	while (true) {
		DWORD pBytesReturned;
		if (EnumProcesses(processList.data(), (DWORD)(processList.size() * sizeof(DWORD)), &pBytesReturned) == FALSE) {
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

bool LaunchProcess(STARTUPINFO& si, PROCESS_INFORMATION& oProcessInfo, std::wstring cmd, bool wait, bool noWindow) {
	// preparation the notepad++ process launch information.
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&oProcessInfo, 0, sizeof(oProcessInfo));
	// launch the Notepad++
	std::vector<wchar_t> cmdStr(cmd.c_str(), cmd.c_str() + cmd.size() + 1);
	if (CreateProcessW(NULL, cmdStr.data(), NULL, NULL, FALSE, noWindow ? CREATE_NO_WINDOW : 0, NULL, NULL, &si, &oProcessInfo) == FALSE) {
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

std::wstring GetEnvironmentVariableValue(const std::wstring& name)
{
	DWORD bufferSize = 65535; //Limit according to http://msdn.microsoft.com/en-us/library/ms683188.aspx
	std::wstring buff;
	buff.resize(bufferSize);
	bufferSize = GetEnvironmentVariableW(name.c_str(), &buff[0], bufferSize);
	buff.resize(bufferSize);
	return std::move(buff);
}