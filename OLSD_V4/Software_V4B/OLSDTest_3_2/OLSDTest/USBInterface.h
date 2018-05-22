
#ifndef __USBINTERFACE_H__
#define __USBINTEFFACE_H__

#include "ftd2xx.h"

class CUSBInterface
{
private:

	HMODULE m_hModule;

	void LoadDLL();

public:
   BOOL IsDLLFound();

	typedef FT_HANDLE (WINAPI *PtrToCreateFile)(LPCSTR lpszName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, DWORD dwAttrsAndFlags, HANDLE	hTemplate);
	PtrToCreateFile m_pCreateFile;
	FT_HANDLE CreateFile(LPCSTR lpszName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, DWORD dwAttrsAndFlags, HANDLE	hTemplate);

	typedef BOOL (WINAPI *PtrToCloseHandle)(FT_HANDLE ftHandle);
	PtrToCloseHandle m_pCloseHandle;
	BOOL CloseHandle(FT_HANDLE ftHandle);

	typedef BOOL (WINAPI *PtrToReadFile)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
	PtrToReadFile m_pReadFile;
	BOOL ReadFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

	typedef BOOL (WINAPI *PtrToWriteFile)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten, LPOVERLAPPED lpOverlapped);
	PtrToWriteFile m_pWriteFile;
	BOOL WriteFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten, LPOVERLAPPED lpOverlapped);

	typedef BOOL (WINAPI *PtrToGetOverlappedResult)(FT_HANDLE ftHandle, LPOVERLAPPED lpOverlapped, LPDWORD lpdwBytesTransferred, BOOL bWait);
	PtrToGetOverlappedResult m_pGetOverlappedResult;
	BOOL GetOverlappedResult(FT_HANDLE ftHandle, LPOVERLAPPED lpOverlapped,LPDWORD lpdwBytesTransferred, BOOL bWait);

	typedef BOOL (WINAPI *PtrToGetCommState)(FT_HANDLE ftHandle, LPFTDCB lpftDcb);
	PtrToGetCommState m_pGetCommState;
	BOOL GetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb);

	typedef BOOL (WINAPI *PtrToGetCommTimeouts)(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts);
	PtrToGetCommTimeouts m_pGetCommTimeouts;
	BOOL GetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts);

	typedef BOOL (WINAPI *PtrToPurgeComm)(FT_HANDLE ftHandle, DWORD dwMask);
	PtrToPurgeComm m_pPurgeComm;
	BOOL PurgeComm(FT_HANDLE ftHandle, DWORD dwMask);

	typedef BOOL (WINAPI *PtrToSetCommMask)(FT_HANDLE ftHandle, ULONG ulEventMask);
	PtrToSetCommMask m_pSetCommMask;
	BOOL SetCommMask(FT_HANDLE ftHandle, ULONG ulEventMask);

	typedef BOOL (WINAPI *PtrToSetCommState)(FT_HANDLE ftHandle, LPFTDCB lpftDcb);
	PtrToSetCommState m_pSetCommState;
	BOOL SetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb);

	typedef BOOL (WINAPI *PtrToSetCommTimeouts)(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts);
	PtrToSetCommTimeouts m_pSetCommTimeouts;
	BOOL SetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts);

	typedef BOOL (WINAPI *PtrToSetupComm)(FT_HANDLE ftHandle, DWORD dwReadBufferSize, DWORD dwWriteBufferSize);
	PtrToSetupComm m_pSetupComm;
	BOOL SetupComm(FT_HANDLE ftHandle, DWORD dwReadBufferSize, DWORD dwWriteBufferSize);
	
	typedef BOOL (WINAPI *PtrToClearCommError)(FT_HANDLE ftHandle, LPDWORD lpdwErrors, LPFTCOMSTAT lpftComstat);
	PtrToClearCommError m_pClearCommError;
	BOOL ClearCommError(FT_HANDLE ftHandle, LPDWORD lpdwErrors, LPFTCOMSTAT lpftComstat);

	typedef BOOL (WINAPI *PtrToEscapeCommFunction)(FT_HANDLE ftHandle, DWORD dwFunc);
	PtrToEscapeCommFunction m_pEscapeCommFunction;
	BOOL EscapeCommFunction(FT_HANDLE, DWORD dwFunc);

	typedef BOOL (WINAPI *PtrToGetLastError)(FT_HANDLE ftHandle);
	PtrToGetLastError m_pGetLastError;
	BOOL GetLastError(FT_HANDLE ftHandle);

	CUSBInterface();
	~CUSBInterface();
};

// Functions Needed
/*
FT_W32_CreateFile
FT_W32_CloseHandle
FT_W32_SetupComm
FT_W32_GetCommState
FT_W32_SetCommState
FT_W32_GetCommTimeouts
FT_W32_SetCommTimeouts
FT_W32_SetCommMask
FT_W32_CloseHandle
FT_W32_WriteFile
FT_W32_GetOverlappedResult
FT_W32_ReadFile
FT_W32_ClearCommError
FT_W32_EscapeCommFunction
FT_W32_PurgeComm
*/

#endif // __USBINTERFACE_H__

