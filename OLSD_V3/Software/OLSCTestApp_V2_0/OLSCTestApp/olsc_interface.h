/*

Open Laser Show Controller Interface Object

Copyright (c) 2009 Dennis Kromhout, Chris Favreau, Andreas Unger

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/


#include "OpenLaserShowControllerV1.0.0.h"

#ifndef __OLSC_INTERFACE_H__
#define __OLSC_INTERFACE_H__

#include <windows.h>
#include <stdio.h>

class COpenLaserShowControllerInterface
{
private:

	HMODULE m_hModule;
	bool m_bLoaded;

public:

	COpenLaserShowControllerInterface();
	~COpenLaserShowControllerInterface();

	bool LoadInterface(char *pFileName, bool verbose);
	void UnloadInterface(void);

	bool IsInterfaceLoaded();

	// Start OLSC Library Functions
	
	// GetAPIVersion
	typedef int (CONVENTION *PtrToGetAPIVersion)(void);
	PtrToGetAPIVersion m_pGetAPIVersion;
	int GetAPIVersion(void);

	// GetInterfaceName
	typedef int (CONVENTION *PtrToGetInterfaceName)(char *pString);
	PtrToGetInterfaceName m_pGetInterfaceName;
	int GetInterfaceName(char *pString);

	// Initialize
	typedef int (CONVENTION *PtrToInitialize)(void);
	PtrToInitialize m_pInitialize;
	int Initialize(void);

	// Shutdown
	typedef int (CONVENTION *PtrToShutdown)(void);
	PtrToShutdown m_pShutdown;
	int Shutdown(void);

	// GetDeviceCount
	typedef int (CONVENTION *PtrToGetDeviceCount)(void);
	PtrToGetDeviceCount m_pGetDeviceCount;
	int GetDeviceCount(void);

	// GetDeviceCapabilities
	typedef int (CONVENTION *PtrToGetDeviceCapabilities)(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities);
	PtrToGetDeviceCapabilities m_pGetDeviceCapabilities;
	int GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities);

	// GetLastErrorNumber
	typedef int (CONVENTION *PtrToGetLastErrorNumber)(int device_number, int &number, char *string_pointer, int string_length);
	PtrToGetLastErrorNumber m_pGetLastErrorNumber;
	int GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length);

	// Play
	typedef int (CONVENTION *PtrToPlay)(int device_number);
	PtrToPlay m_pPlay;
	int Play(int device_number);

	// Pause
	typedef int (CONVENTION *PtrToPause)(int device_number);
	PtrToPause m_pPause;
	int Pause(int device_number);

	// Shutter
	typedef int (CONVENTION *PtrToShutter)(int device_number, int state);
	PtrToShutter m_pShutter;
	int Shutter(int device_number, int state);

	// WriteFrame
	typedef int (CONVENTION *PtrToWriteFrame)(int device_number, struct LASER_SHOW_DEVICE_FRAME frame);
	PtrToWriteFrame m_pWriteFrame;
	int WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame);

	// GetStatus
	typedef int (CONVENTION *PtrToGetStatus)(int device_number, DWORD &status);
	PtrToGetStatus m_pGetStatus;
	int GetStatus(int device_number, DWORD &status);

	// SetCallback
	typedef int (CONVENTION *PtrToSetCallback)(int device_number, HWND parent_window_handle, DWORD message);
	PtrToSetCallback m_pSetCallback;
	int SetCallback(int device_number, HWND parent_window_handle, DWORD message);

	// WriteDMX
	typedef int (CONVENTION *PtrToWriteDMX)(int device_number, int start_address, unsigned char *data_pointer, int length);
	PtrToWriteDMX m_pWriteDMX;
	int WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length);

	// ReadDMX
	typedef int (CONVENTION *PtrToReadDMX)(int device_number, int start_address, unsigned char *data_pointer, int length);
	PtrToReadDMX m_pReadDMX;
	int ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length);

	// WriteTTL
	typedef int (CONVENTION *PtrToWriteTTL)(int device_number, DWORD data);
	PtrToWriteTTL m_pWriteTTL;
	int WriteTTL(int device_number, DWORD data);

	// ReadTTL
	typedef int (CONVENTION *PtrToReadTTL)(int device_number, DWORD &data);
	PtrToReadTTL m_pReadTTL;
	int ReadTTL(int device_number, DWORD &data);
};

#endif
