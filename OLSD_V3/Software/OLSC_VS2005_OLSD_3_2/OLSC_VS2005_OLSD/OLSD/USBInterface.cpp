#include <windows.h>
#include "USBInterface.h"
#include "trace.h"			// Trace/Debugging Utility

CUSBInterface::CUSBInterface()
{
	m_hModule = NULL;
	m_pCreateFile = NULL;
	m_pCloseHandle = NULL;
	m_pReadFile = NULL;
	m_pWriteFile = NULL;
	m_pGetOverlappedResult = NULL;
	m_pGetCommState = NULL;
	m_pGetCommTimeouts = NULL;
	m_pPurgeComm = NULL;
	m_pSetCommMask = NULL;
	m_pSetCommState = NULL;
	m_pSetCommTimeouts = NULL;
	m_pSetupComm = NULL;
	m_pClearCommError = NULL;
	m_pEscapeCommFunction = NULL;
	m_pGetLastError = NULL;

	LoadDLL();
}

CUSBInterface::~CUSBInterface()
{
	if (m_hModule) FreeLibrary(m_hModule);

	m_hModule = NULL;
	m_pCreateFile = NULL;
	m_pCloseHandle = NULL;
	m_pReadFile = NULL;
	m_pWriteFile = NULL;
	m_pGetOverlappedResult = NULL;
	m_pGetCommState = NULL;
	m_pGetCommTimeouts = NULL;
	m_pPurgeComm = NULL;
	m_pSetCommMask = NULL;
	m_pSetCommState = NULL;
	m_pSetCommTimeouts = NULL;
	m_pSetupComm = NULL;
	m_pClearCommError = NULL;
	m_pEscapeCommFunction = NULL;
	m_pGetLastError = NULL;
}

void CUSBInterface::LoadDLL()
{
	m_hModule = LoadLibrary("Ftd2xx.dll");	
	
	if(m_hModule == NULL)
		return;

	m_pCreateFile = (PtrToCreateFile)GetProcAddress(m_hModule, "FT_W32_CreateFile");
	if (m_pCreateFile == NULL)
	{
		Trace("Error: Can't Find FT_W32_CreateFile");
		return;
	}

	m_pCloseHandle = (PtrToCloseHandle)GetProcAddress(m_hModule, "FT_W32_CloseHandle");
	if (m_pCloseHandle == NULL)
	{
		Trace("Error: Can't Find FT_W32_CloseHandle");
		return;
	}

	m_pReadFile = (PtrToReadFile)GetProcAddress(m_hModule, "FT_W32_ReadFile");
	if (m_pReadFile == NULL)
	{
		Trace("Error: Can't Find FT_W32_ReadFile");
		return;
	}

	m_pWriteFile = (PtrToWriteFile)GetProcAddress(m_hModule, "FT_W32_WriteFile");
	if (m_pWriteFile == NULL)
	{
		Trace("Error: Can't Find FT_W32_WriteFile");
		return;
	}

	m_pGetOverlappedResult = (PtrToGetOverlappedResult)GetProcAddress(m_hModule, "FT_W32_GetOverlappedResult");
	if (m_pGetOverlappedResult == NULL)
	{
		Trace("Error: Can't Find FT_W32_GetOverlappedResult");
		return;
	}

	m_pGetCommState = (PtrToGetCommState)GetProcAddress(m_hModule, "FT_W32_GetCommState");
	if (m_pGetCommState == NULL)
	{
		Trace("Error: Can't Find FT_W32_GetCommState");
		return;
	}

	m_pGetCommTimeouts = (PtrToGetCommTimeouts)GetProcAddress(m_hModule, "FT_W32_GetCommTimeouts");
	if (m_pGetCommTimeouts == NULL)
	{
		Trace("Error: Can't Find FT_W32_GetCommTimeouts");
		return;
	}

	m_pPurgeComm = (PtrToPurgeComm)GetProcAddress(m_hModule, "FT_W32_PurgeComm");
	if (m_pPurgeComm == NULL)
	{
		Trace("Error: Can't Find FT_W32_PurgeComm");
		return;
	}

	m_pSetCommMask = (PtrToSetCommMask)GetProcAddress(m_hModule, "FT_W32_SetCommMask");
	if (m_pSetCommMask == NULL)
	{
		Trace("Error: Can't Find FT_W32_SetCommMask");
		return;
	}

	m_pSetCommState = (PtrToSetCommState)GetProcAddress(m_hModule, "FT_W32_SetCommState");
	if (m_pSetCommState == NULL)
	{
		Trace("Error: Can't Find FT_W32_SetComMState");
		return;
	}

	m_pSetCommTimeouts = (PtrToSetCommTimeouts)GetProcAddress(m_hModule, "FT_W32_SetCommTimeouts");
	if (m_pSetCommTimeouts == NULL)
	{
		Trace("Error: Can't Find FT_W32_SetCommTimeouts");
		return;
	}

	m_pSetupComm = (PtrToSetupComm)GetProcAddress(m_hModule, "FT_W32_SetupComm");
	if (m_pSetupComm == NULL)
	{
		Trace("Error: Can't Find FT_W32_SetupComm");
		return;
	}

	m_pClearCommError = (PtrToClearCommError)GetProcAddress(m_hModule, "FT_W32_ClearCommError");
	if (m_pClearCommError == NULL)
	{
		Trace("Error: Can't Find FT_W32_ClearCommError");
		return;
	}

	m_pEscapeCommFunction = (PtrToEscapeCommFunction)GetProcAddress(m_hModule, "FT_W32_EscapeCommFunction");
	if (m_pEscapeCommFunction == NULL)
	{
		Trace("Error: Can't Find FT_W32_EscapeCommFunction");
		return;
	}

	m_pGetLastError = (PtrToGetLastError)GetProcAddress(m_hModule, "FT_W32_GetLastError");
	if (m_pGetLastError == NULL)
	{
		Trace("Error: Can't Find FT_W32_GetLastError");
		return;
	}
}

FT_HANDLE CUSBInterface::CreateFile(LPCSTR lpszName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, DWORD dwAttrsAndFlags, HANDLE	hTemplate)
{
	if (!m_pCreateFile) return (FT_HANDLE)FT_INVALID_HANDLE;

	return (*m_pCreateFile)(lpszName, dwAccess, dwShareMode, lpSecurityAttributes, dwCreate, dwAttrsAndFlags, hTemplate);
}

BOOL CUSBInterface::CloseHandle(FT_HANDLE ftHandle)
{
	if (!m_pCloseHandle) return FALSE;

	return (*m_pCloseHandle)(ftHandle);
}

BOOL CUSBInterface::ReadFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
	if (!m_pReadFile) return FALSE;

	return (*m_pReadFile)(ftHandle, lpBuffer, nBufferSize, lpBytesReturned, lpOverlapped);
}

BOOL CUSBInterface::WriteFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten, LPOVERLAPPED lpOverlapped)
{
	if (!m_pWriteFile) return FALSE;

	return (*m_pWriteFile)(ftHandle, lpBuffer, nBufferSize, lpBytesWritten, lpOverlapped);
}

BOOL CUSBInterface::GetOverlappedResult(FT_HANDLE ftHandle, LPOVERLAPPED lpOverlapped,LPDWORD lpdwBytesTransferred, BOOL bWait)
{
	if (!m_pGetOverlappedResult) return FALSE;

	return (*m_pGetOverlappedResult)(ftHandle, lpOverlapped, lpdwBytesTransferred, bWait);
}

BOOL CUSBInterface::GetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb)
{
	if (!m_pGetCommState) return FALSE;

	return (*m_pGetCommState)(ftHandle, lpftDcb);
}

BOOL CUSBInterface::GetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts)
{
	if (!m_pGetCommTimeouts) return FALSE;

	return (*m_pGetCommTimeouts)(ftHandle, pTimeouts);
}

BOOL CUSBInterface::PurgeComm(FT_HANDLE ftHandle, DWORD dwMask)
{
	if (!m_pPurgeComm) return FALSE;

	return (*m_pPurgeComm)(ftHandle, dwMask);
}

BOOL CUSBInterface::SetCommMask(FT_HANDLE ftHandle, ULONG ulEventMask)
{
	if (!m_pSetCommMask) return FALSE;

	return (*m_pSetCommMask)(ftHandle, ulEventMask);
}

BOOL CUSBInterface::SetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb)
{
	if (!m_pSetCommState) return FALSE;

	return (*m_pSetCommState)(ftHandle, lpftDcb);
}

BOOL CUSBInterface::SetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pTimeouts)
{
	if (!m_pSetCommTimeouts) return FALSE;

	return (*m_pSetCommTimeouts)(ftHandle, pTimeouts);
}

BOOL CUSBInterface::SetupComm(FT_HANDLE ftHandle, DWORD dwReadBufferSize, DWORD dwWriteBufferSize)
{
	if (!m_pSetupComm) return FALSE;

	return (*m_pSetupComm)(ftHandle, dwReadBufferSize, dwWriteBufferSize);
}

BOOL CUSBInterface::ClearCommError(FT_HANDLE ftHandle, LPDWORD lpdwErrors, LPFTCOMSTAT lpftComstat)
{
	if (!m_pClearCommError) return FALSE;

	return (*m_pClearCommError)(ftHandle, lpdwErrors, lpftComstat);
}

BOOL CUSBInterface::EscapeCommFunction(FT_HANDLE ftHandle, DWORD dwFunc)
{
	if (!m_pEscapeCommFunction) return FALSE;

	return (*m_pEscapeCommFunction)(ftHandle, dwFunc);
}

BOOL CUSBInterface::GetLastError(FT_HANDLE ftHandle)
{
	if (!m_pGetLastError) return FALSE;

	return (*m_pGetLastError)(ftHandle);
}

BOOL CUSBInterface::IsDLLFound()
{
   return (BOOL)(m_hModule != NULL);
}
