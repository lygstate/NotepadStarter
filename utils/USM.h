#ifndef _USM_H
#define _USM_H
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
using std::wstring;
using std::vector;

/* http://www.codeproject.com/Articles/835818/Ultimate-Shared-Memory-A-flexible-class-for-interp */

class usm
{
private:
	struct USMHEADER
	{
	};

	struct USMTHREAD
	{
		DWORD id;
		int evidx;
	};

	// Strings of handle ids
	wstring cwmn;
	wstring fmn;
	wstring evrn;
	wstring evwn;

	wstring stringid;

	bool WasFirst = false;
	HANDLE hEventWrote = 0;
	HANDLE hMutexWriting = 0;
	HANDLE hEventMeReading = 0;
	HANDLE hFM = 0;
	unsigned long long ClientSZ = 0;
	DWORD MaxThreads = 0;
	PVOID  Buff = 0;

	HANDLE CreateEvR(int idx)
	{
		wchar_t n[1024] = { 0 };
		swprintf_s(n, L"%s%i", evrn.c_str(), idx);
		HANDLE hX = CreateEvent(0, TRUE, TRUE, n);
		return hX;
	}

	HANDLE CreateEvW()
	{
		wchar_t n[1024] = { 0 };
		swprintf_s(n, L"%s", evwn.c_str());
		HANDLE hX = CreateEventW(0, 0, 0, n);
		return hX;
	}

	HANDLE CreateCWM()
	{
		HANDLE hX = OpenMutexW(MUTEX_MODIFY_STATE | SYNCHRONIZE, false, cwmn.c_str());
		if (hX != 0)
			return hX;
		hX = CreateMutexW(0, 0, cwmn.c_str());
		return hX;
	}

	HANDLE CreateFM()
	{
		// Try to open the map , or else create it
		WasFirst = true;
		HANDLE hX = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, false, fmn.c_str());
		if (hX != 0)
		{
			WasFirst = false;
			return hX;
		}

		unsigned long long  FinalSize = ClientSZ*sizeof(uint8_t) + MaxThreads*sizeof(USMTHREAD) + sizeof(USMHEADER);
		ULARGE_INTEGER ulx = { 0 };
		ulx.QuadPart = FinalSize;
		hX = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, ulx.HighPart, ulx.LowPart, fmn.c_str());
		if (hX != 0)
		{
			LPVOID Buff = MapViewOfFile(hFM, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			if (Buff)
			{
				memset(Buff, 0, (size_t)FinalSize);
				UnmapViewOfFile(Buff);
			}
		}
		return hX;
	}


public:
	void End()
	{
		// Remove the ID from the thread
		if (Buff)
		{
			USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
			WaitForSingleObject(hMutexWriting, INFINITE);
			// Find 
			for (unsigned int y = 0; y < MaxThreads; y++)
			{
				USMTHREAD& tt = th[y];
				DWORD myid = GetCurrentThreadId();
				if (tt.id == myid)
				{
					tt.id = 0;
					tt.evidx = 0;
					break;
				}
			}
			ReleaseMutex(hMutexWriting);
		}


		if (hEventWrote)
			CloseHandle(hEventWrote);
		hEventWrote = 0;
		if (hFM)
			CloseHandle(hFM);
		hFM = 0;
		if (hEventMeReading)
			CloseHandle(hEventMeReading);
		hEventMeReading = 0;
		if (hMutexWriting)
			CloseHandle(hMutexWriting);
		hMutexWriting = 0;
	}


	bool IsFirst() { return WasFirst; }

	usm(const wchar_t* string_id = 0, bool Init = false, unsigned long long csz = 1048576, DWORD MaxTh = 100)
	{
		if (!string_id)
			return;
		CreateInit(string_id, Init, csz, MaxTh);
	}

	void operator =(const usm &x)
	{
		// Terminate current
		End();

		// Recreate
		CreateInit(x.stringid.c_str(), true, x.ClientSZ, x.MaxThreads);
	}

	usm(const usm& x)
	{
		operator=(x);
	}

	void CreateInit(const wchar_t* string_id, bool Init = false, unsigned long long csz = 1048576, DWORD MaxTh = 100)
	{
		if (!string_id)
			return;
		if (wcslen(string_id) == 0)
			return;

		wchar_t xup[1024] = { 0 };
		stringid = string_id;

		swprintf_s(xup, 1024, L"%s_cwmn", stringid.c_str());
		cwmn = xup;
		swprintf_s(xup, 1024, L"%s_evrn", stringid.c_str());
		evrn = xup;
		swprintf_s(xup, 1024, L"%s_evwn", stringid.c_str());
		evwn = xup;
		swprintf_s(xup, 1024, L"%s_fmn", stringid.c_str());
		fmn = xup;

		if (!csz)
			csz = 1048576;
		ClientSZ = csz;
		if (!MaxTh)
			MaxTh = 100;
		MaxThreads = MaxTh;
		if (Init)
		{
			int iv = Initialize();
			if (iv <= 0)
			{
				End();
				throw iv;
			}
		}
	}

	~usm()
	{
		End();
	}

	int Initialize()
	{
		hEventWrote = 0;
		hMutexWriting = 0;
		hFM = 0;
		Buff = 0;
		hEventMeReading = 0;

		if (hMutexWriting == 0)
			hMutexWriting = CreateCWM();
		if (hMutexWriting == 0)
			return -1;
		if (hFM == 0)
			hFM = CreateFM();
		if (hFM == 0)
			return -1;
		if (hEventWrote == 0)
			hEventWrote = CreateEvW();
		if (hEventWrote == 0)
			return -1;
		if (Buff == 0)
			Buff = MapViewOfFile(hFM, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (!Buff)
			return -1;

		// Acquire lock for Count variable
		// USMHEADER* h = (USMHEADER*)Buff;
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
		WaitForSingleObject(hMutexWriting, INFINITE);
		// Find 
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.id == 0)
			{
				tt.id = GetCurrentThreadId();
				tt.evidx = (y + 1);
				hEventMeReading = CreateEvR(y + 1);
				break;
			}
		}
		ReleaseMutex(hMutexWriting);

		if (!hEventMeReading)
			return -1;

		return 1;

	}

	const uint8_t* BeginRead(bool FailOnNotReady = false)
	{
		if (!Buff)
			return 0;

		// Is someone writing 
		if (FailOnNotReady)
		{
			DWORD x = WaitForSingleObject(hMutexWriting, 0);
			if (x != WAIT_OBJECT_0)
				return 0;
		}
		else
			WaitForSingleObject(hMutexWriting, INFINITE);

		// Reset our reading event
		ResetEvent(hEventMeReading);

		// Release the mutex, but now any writing thread that locks it must wait for is
		ReleaseMutex(hMutexWriting);

		// Return the pointer
		const uint8_t* a1 = (const  uint8_t*)Buff;
		a1 += sizeof(USMHEADER);
		a1 += sizeof(USMTHREAD)*MaxThreads;

		return (const uint8_t*)a1;
	}

	void EndRead()
	{
		SetEvent(hEventMeReading);
	}

	unsigned long long ReadData(uint8_t* b, size_t sz, size_t offset = 0, bool FailIfNotReady = false)
	{
		const uint8_t* ptr = BeginRead(FailIfNotReady);
		if (!ptr)
			return (unsigned long long) - 1;
		memcpy(b, ptr + offset, sz);
		EndRead();
		return sz;
	}


	DWORD NotifyOnRead(bool Wait)
	{
		// See if any thread is reading
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
		vector<HANDLE> evs;

		// Find 
		bool S = true;
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.evidx > 0)
			{
				// Open the event
				wchar_t n[1024] = { 0 };
				swprintf_s(n, L"%s%i", evrn.c_str(), tt.evidx);
				HANDLE hEv = OpenEvent(SYNCHRONIZE, 0, n);
				if (hEv == 0) // duh
				{
					S = false;
					break;
				}
				evs.push_back(hEv);
			}
		}
		DWORD fi = 0;
		if (!S)
			return (DWORD)-1;
		if (evs.empty())
			return (DWORD)-2;

		// Wait for any thread to terminate reading
		fi = WaitForMultipleObjects((DWORD)evs.size(), &evs[0], FALSE, Wait ? INFINITE : 0);

		// Cleanup
		for (unsigned int i = 0; i < evs.size(); i++)
			CloseHandle(evs[i]);
		evs.clear();

		return fi;
	}

	uint8_t* BeginWrite(bool FailOnNotReady = false)
	{
		// Lock the writing mutex
		if (FailOnNotReady)
		{
			DWORD x = WaitForSingleObject(hMutexWriting, 0);
			if (x != WAIT_OBJECT_0)
				return 0;
		}
		else
			WaitForSingleObject(hMutexWriting, INFINITE);

		// Having locked the writing mutex, no reading thread can start now
		// After that, no new threads can read
		vector<HANDLE> evs;
		evs.reserve(MaxThreads);

		// Wait for threads that are already in read state
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));

		// Find 
		bool S = true;
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.evidx > 0)
			{
				// Open the event
				wchar_t n[1024] = { 0 };
				swprintf_s(n, L"%s%i", evrn.c_str(), tt.evidx);
				HANDLE hEv = OpenEvent(SYNCHRONIZE, 0, n);
				if (hEv == 0) // duh
				{
					S = false;
					break;
				}
				evs.push_back(hEv);
			}
		}
		DWORD fi = 0;
		if (S)
		{
			// Wait for all these threads to terminate reading
			fi = WaitForMultipleObjects((DWORD)evs.size(), &evs[0], TRUE, FailOnNotReady ? 0 : INFINITE);
			if (fi == -1 || fi == WAIT_TIMEOUT)
				S = false;
		}
		else
		{
			fi = (DWORD)-1;
		}

		// Cleanup
		for (unsigned int i = 0; i < evs.size(); i++)
			CloseHandle(evs[i]);
		evs.clear();
		if (!S)
		{
			ReleaseMutex(hMutexWriting);
			return 0;
		}

		// Return the pointer
		char* a1 = (char*)Buff;
		a1 += sizeof(USMHEADER);
		a1 += sizeof(USMTHREAD)*MaxThreads;

		ResetEvent(hEventWrote);
		return (uint8_t*)a1;
	}

	void EndWrite()
	{
		ReleaseMutex(hMutexWriting);
		SetEvent(hEventWrote);
	}

	DWORD NotifyWrite(bool Wait)
	{
		// Wait for all these threads to terminate reading
		return WaitForSingleObject(hEventWrote, Wait ? INFINITE : 0);
	}

	unsigned long long WriteData(const uint8_t* b, size_t sz, size_t offset = 0, bool FailIfNotReady = false)
	{
		uint8_t* ptr = BeginWrite(FailIfNotReady);
		if (!ptr)
			return (unsigned long long) - 1;
		memcpy(ptr + offset, b, sz);
		EndWrite();
		return sz;
	}
};


#endif // USM_H