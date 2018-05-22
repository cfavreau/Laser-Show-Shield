#include "olsc_interface.h"

COpenLaserShowControllerInterface::COpenLaserShowControllerInterface()
{
	m_hModule = NULL;
	m_bLoaded = false;
	m_pGetAPIVersion = NULL;
	m_pInitialize = NULL;
	m_pShutdown = NULL;
	m_pGetDeviceCount = NULL;
	m_pGetDeviceCapabilities = NULL;
	m_pGetLastErrorNumber = NULL;
	m_pPlay = NULL;
	m_pPause = NULL;
	m_pShutter = NULL;
	m_pWriteFrame = NULL;
	m_pGetStatus = NULL;
	m_pSetCallback = NULL;
	m_pWriteDMX = NULL;
	m_pReadDMX = NULL;
	m_pWriteTTL = NULL;
	m_pReadTTL = NULL;
}

COpenLaserShowControllerInterface::~COpenLaserShowControllerInterface()
{
	UnloadInterface();
}

void COpenLaserShowControllerInterface::UnloadInterface(void)
{
	if (m_hModule) FreeLibrary(m_hModule);
	m_hModule = NULL;
	m_bLoaded = false;
	m_pGetAPIVersion = NULL;
	m_pGetInterfaceName = NULL;
	m_pInitialize = NULL;
	m_pShutdown = NULL;
	m_pGetDeviceCount = NULL;
	m_pGetDeviceCapabilities = NULL;
	m_pGetLastErrorNumber = NULL;
	m_pPlay = NULL;
	m_pPause = NULL;
	m_pShutter = NULL;
	m_pWriteFrame = NULL;
	m_pGetStatus = NULL;
	m_pSetCallback = NULL;
	m_pWriteDMX = NULL;
	m_pReadDMX = NULL;
	m_pWriteTTL = NULL;
	m_pReadTTL = NULL;
}

bool COpenLaserShowControllerInterface::LoadInterface(char *pFileName, bool verbose)
{
	// Make sure the pointer to the file name is good
	if (!pFileName) return false;

	// Load the DLL library
	m_hModule = LoadLibrary(pFileName);	
	
	// Check the handle
	if(m_hModule == NULL)
		return false;

	// TODO - make all fo the AfxMessageBox calls to some sort of logging class

	m_pGetAPIVersion = (PtrToGetAPIVersion)GetProcAddress(m_hModule, "OLSC_GetAPIVersion");
	if (m_pGetAPIVersion == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetAPIVersion");
		return false;
	}

	m_pGetInterfaceName = (PtrToGetInterfaceName)GetProcAddress(m_hModule, "OLSC_GetInterfaceName");
	if (m_pGetInterfaceName == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetInterfaceName");
		return false;
	}

	m_pInitialize = (PtrToInitialize)GetProcAddress(m_hModule, "OLSC_Initialize");
	if (m_pInitialize == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_Initialize");
		return false;
	}

	m_pShutdown = (PtrToShutdown)GetProcAddress(m_hModule, "OLSC_Shutdown");
	if (m_pShutdown == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_Shutdown");
		return false;
	}

	m_pGetDeviceCount = (PtrToGetDeviceCount)GetProcAddress(m_hModule, "OLSC_GetDeviceCount");
	if (m_pGetDeviceCount == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetDeviceCount");
		return false;
	}

	m_pGetDeviceCapabilities = (PtrToGetDeviceCapabilities)GetProcAddress(m_hModule, "OLSC_GetDeviceCapabilities");
	if (m_pGetDeviceCapabilities == NULL)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetDeviceCapabilities");
		return false;
	}

	m_pGetLastErrorNumber = (PtrToGetLastErrorNumber)GetProcAddress(m_hModule, "OLSC_GetLastErrorNumber");
	if (!m_pGetLastErrorNumber)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetLastErrorNumber");
		return false;
	}
	
	m_pPlay = (PtrToPlay)GetProcAddress(m_hModule, "OLSC_Play");
	if (!m_pPlay)
	{
		if (verbose) printf("Error: Can't Find OLSC_Play");
		return false;
	}

	m_pPause = (PtrToPause)GetProcAddress(m_hModule, "OLSC_Pause");
	if (!m_pPause)
	{
		if (verbose) printf("Error: Can't Find OLSC_Pause");
		return false;
	}

	m_pShutter = (PtrToShutter)GetProcAddress(m_hModule, "OLSC_Shutter");
	if (!m_pShutter)
	{
		if (verbose) printf("Error: Can't Find OLSC_Shutter");
		return false;
	}

	m_pWriteFrame = (PtrToWriteFrame)GetProcAddress(m_hModule, "OLSC_WriteFrame");
	if (!m_pWriteFrame)
	{
		if (verbose) printf("Error: Can't Find OSLC_WriteFrame");
		return false;
	}

	m_pGetStatus = (PtrToGetStatus)GetProcAddress(m_hModule, "OLSC_GetStatus");
	if (!m_pGetStatus)
	{
		if (verbose) printf("Error: Can't Find OLSC_GetStatus");
		return false;
	}

	m_pSetCallback = (PtrToSetCallback)GetProcAddress(m_hModule, "OLSC_SetCallback");
	if (!m_pSetCallback)
	{
		if (verbose) printf("Error: Can't Find OLSC_SetCallback");
		return false;
	}

	m_pWriteDMX = (PtrToWriteDMX)GetProcAddress(m_hModule, "OLSC_WriteDMX");
	if (!m_pWriteDMX)
	{
		if (verbose) printf("Error: Can't Find OLSC_WriteDMX");
		return false;
	}

	m_pReadDMX = (PtrToReadDMX)GetProcAddress(m_hModule, "OLSC_ReadDMX");
	if (!m_pReadDMX)
	{
		if (verbose) printf("Error: Can't Find OLSC_ReadDMX");
		return false;
	}

	m_pWriteTTL = (PtrToWriteTTL)GetProcAddress(m_hModule, "OLSC_WriteTTL");
	if (!m_pWriteTTL)
	{
		if (verbose) printf("Error: Can't Find OLSC_WriteTTL");
		return false;
	}

	m_pReadTTL = (PtrToReadTTL)GetProcAddress(m_hModule, "OLSC_ReadTTL");
	if (!m_pReadTTL)
	{
		if (verbose) printf("Error: Can't Find OLSC_ReadTTL");
		return false;
	}

	// Set the loaded flag
	m_bLoaded = true;

	return true;
}

bool COpenLaserShowControllerInterface::IsInterfaceLoaded(void)
{
	if (!m_hModule) return false;

	return m_bLoaded;
}

int COpenLaserShowControllerInterface::GetAPIVersion(void)
{
	if (!m_hModule) return 0;

	if (!m_pGetAPIVersion) return 0;

	return (*m_pGetAPIVersion)();
}

int COpenLaserShowControllerInterface::GetInterfaceName(char *pString)
{
	if (!m_hModule) return 0;

	if (!m_pGetInterfaceName) return 0;

	return (*m_pGetInterfaceName)(pString);
}

int COpenLaserShowControllerInterface::Initialize(void)
{
	if (!m_hModule) return 0;

	if (!m_pInitialize) return 0;

	return (*m_pInitialize)();
}

int COpenLaserShowControllerInterface::Shutdown(void)
{
	if (!m_hModule) return 0;

	if (!m_pShutdown) return 0;

	return (*m_pShutdown)();
}

int COpenLaserShowControllerInterface::GetDeviceCount(void)
{
	if (!m_hModule) return 0;

	if (!m_pGetDeviceCount) return 0;

	return (*m_pGetDeviceCount)();
}

int COpenLaserShowControllerInterface::GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities)
{
	if (!m_hModule) return 0;

	if (!m_pGetDeviceCapabilities) return 0;

	return (*m_pGetDeviceCapabilities)(device_number, device_capabilities);
}

int COpenLaserShowControllerInterface::GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length)
{
	if (!m_hModule) return 0;

	if (!m_pGetLastErrorNumber) return 0;

	return (*m_pGetLastErrorNumber)(device_number, number, string_pointer, string_length);
}

int COpenLaserShowControllerInterface::Play(int device_number)
{
	if (!m_hModule) return 0;

	if (!m_pPlay) return 0;

	return (*m_pPlay)(device_number);
}

int COpenLaserShowControllerInterface::Pause(int device_number)
{
	if (!m_hModule) return 0;

	if (!m_pPause) return 0;

	return (*m_pPause)(device_number);
}

int COpenLaserShowControllerInterface::Shutter(int device_number, int state)
{
	if (!m_hModule) return 0;

	if (!m_pShutter) return 0;

	return (*m_pShutter)(device_number, state);
}

int COpenLaserShowControllerInterface::WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame)
{
	if (!m_hModule) return 0;

	if (!m_pWriteFrame) return 0;

	return (*m_pWriteFrame)(device_number, frame);
}

int COpenLaserShowControllerInterface::GetStatus(int device_number, DWORD &status)
{
	if (!m_hModule) return 0;

	if (!m_pGetStatus) return 0;

	return (*m_pGetStatus)(device_number, status);
}

int COpenLaserShowControllerInterface::SetCallback(int device_number, HWND parent_window_handle, DWORD message)
{
	if (!m_hModule) return 0;

	if (!m_pSetCallback) return 0;
	
	return (*m_pSetCallback)(device_number, parent_window_handle, message);
}

int COpenLaserShowControllerInterface::WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
	if (!m_hModule) return 0;

	if (!m_pWriteDMX) return 0;

	return (*m_pWriteDMX)(device_number, start_address, data_pointer, length);
}

int COpenLaserShowControllerInterface::ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
	if (!m_hModule) return 0;

	if (!m_pReadDMX) return 0;

	return (*m_pReadDMX)(device_number, start_address, data_pointer, length);
}

int COpenLaserShowControllerInterface::WriteTTL(int device_number, DWORD data)
{
	if (!m_hModule) return 0;

	if (!m_pWriteTTL) return 0;

	return (*m_pWriteTTL)(device_number, data);
}

int COpenLaserShowControllerInterface::ReadTTL(int device_number, DWORD &data)
{
	if (!m_hModule) return 0;

	if (!m_pReadTTL) return 0;

	return (*m_pReadTTL)(device_number, data);
}

