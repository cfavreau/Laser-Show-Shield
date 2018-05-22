#include "stdafx.h"
#include "FTSerial.h"
#include "trace.h"			// DEBUG TRACE -> Generates Trace.TXT file

// This number should be bigger than 1000 points per second per write
// Since we are using a maximum of 1365 points per write we should use a timeout bigger than 1 second
#define SERIAL_WRITE_TIMEOUT 1000

CFTSerial::CFTSerial()
{
	// Call the reset function
	Reset();
}

void CFTSerial::Reset()
{
	// Initialize a bunch o variables so we don't have problems
	bGo = false;
	hCFTSerial_Thread = INVALID_HANDLE_VALUE;
	
	m_bIsValid = false;
	m_bPortReady = FALSE;;
	m_hCom = INVALID_HANDLE_VALUE;
	m_iComPort = 0;
	memset(&m_dcb,0,sizeof(DCB));
	memset(&m_CommTimeouts,0,sizeof(COMMTIMEOUTS));
	m_dwEvents = 0;
	bWriteRC = FALSE;
	bReadRC = FALSE;
	iBytesWritten = 0;
	iBytesRead = 0;

	memset(&rOverlapped,0,sizeof(OVERLAPPED));
	memset(&wOverlapped,0,sizeof(OVERLAPPED));
}

int CFTSerial::Init(LPSTR port_name, unsigned int BaudRate, unsigned int ByteSize, unsigned int Parity, unsigned int StopBits, DWORD Events, bool bUseThread)
{
	// Check to see if the port is already opened
	if (m_bIsValid)
		Close();

	// Open the USB Serial Port using the Description!
	// Non-Blocking IO (kind of)
	//m_hCom = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FT_OPEN_BY_DESCRIPTION, NULL);
	// Blocking IO
	//m_hCom = FT_W32_CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FT_OPEN_BY_DESCRIPTION, NULL);
	if (FT_Open(0, &m_hCom) != FT_OK)
	{
		Trace("CFTSerial::Init() - FTOpen() failed!\n");
		return FALSE;
	}

	// Check to see if there wasn't a problem opening the Com Port
	if (m_hCom == INVALID_HANDLE_VALUE)
   {
      // DEBUG
      Trace("CFTSerial::Init() - CreateFile returned INVALID_HANDLE_VALUE.\n");
	  return FALSE;
   }

	// Set the baud rate
	if (FT_SetBaudRate(m_hCom, BaudRate) != FT_OK)
	{
		return FALSE;
	}

	// Setup flow control
	FT_SetFlowControl(m_hCom, FT_FLOW_RTS_CTS, 0, 0);

	// Setup the timeouts
	FT_SetTimeouts(m_hCom, 1000, 10000);

	FT_SetLatencyTimer(m_hCom, 2);


	
	// Flush the Comm Port Buffers in order to get the Comm State
	int result = 0;
	//sFlushALL();

	/*
	// Setup the Com Port buffer size
	m_bPortReady = FT_W32_SetupComm(m_hCom, 2048, 2048);
	//m_bPortReady = m_usb.SetupComm(m_hCom, 2048, 2048);
	//m_bPortReady = m_usb.SetupComm(m_hCom, 4096, 4096);

	if (!m_bPortReady)
	{
      Trace("CFTSerial::Init() - SetupComm failed.\n");
	  Close();
	  return FALSE;
	}
	*/
	
	// Flush the Comm Port Buffers in order to get the Comm State
	FlushALL();

	/*
	// Setup The Com Port parameters to what we want to be initialized to
	m_bPortReady = FT_W32_GetCommState(m_hCom, (LPFTDCB)&m_dcb);

	if (!m_bPortReady)
	{
      // DEBUG
      Trace("CFTSerial::Init() - GetCommState failed.\n");
	  Close();
	  return FALSE;
	}

	m_dcb.BaudRate = BaudRate;
	m_dcb.ByteSize = ByteSize;
	m_dcb.Parity = Parity;
	m_dcb.StopBits = StopBits;
	m_dcb.fAbortOnError = true;
	// Turn ON CTS Flow Control so we don't overflow our buffers
	m_dcb.fOutxCtsFlow = true;

	// Flush the Comm Port Buffers in order to set a new Comm State
	FlushALL();

	// Set the New Com Parameters (State)
	m_bPortReady = FT_W32_SetCommState(m_hCom, (LPFTDCB)&m_dcb);

	if (!m_bPortReady)
	{
      Trace("CFTSerial::Init() - SetCommState failed.\n");
	  Close();
	  return FALSE;
	}

	
	// Setup The Timeouts
	m_bPortReady = FT_W32_GetCommTimeouts(m_hCom, (LPFTTIMEOUTS)&m_CommTimeouts);

	// Set the read timeout to 1000 milliseconds
	// This should give enough time for whatever microcontroller or computer to respond to a request
	m_CommTimeouts.ReadIntervalTimeout = 0;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	
	// Have to set a really big timeout for the FTD2XX drivers for the ILDA_PLAYER APPLICATION
	// This is important to set this to be something large compared to the length in time
	// of the slowest point output rate and the maximum points to be sent to the device at 1 time
	m_CommTimeouts.WriteTotalTimeoutConstant = 10000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 10;

	m_bPortReady = FT_W32_SetCommTimeouts(m_hCom, (LPFTTIMEOUTS)&m_CommTimeouts);

	// Set the Serial Port Events We Will Wait For
	m_dwEvents = Events;

	result = FT_W32_SetCommMask(m_hCom, Events);
		
	if (!result)
	{
		Close();
		return FALSE;
	}
	
	// Setup our latency for the USB Ports
	// Set the latency timer to 2 milliseconds
	//if (FT_SetLatencyTimer(m_hCom, 2) != FT_OK) TRACE0("Failed to set latency timer!\n");
	// Set the USB transfer size
	//if (FT_SetUSBParameters(m_hCom, 4096, 65535) != FT_OK) TRACE0("Failed to set USB transfer size!\n");

	// Setup RTS/CTS Flow Control - necessary for the OLSD to work properly
	if (FT_SetFlowControl(m_hCom, FT_FLOW_RTS_CTS, 0, 0) != FT_OK) TRACE("Failed to setup RTS/CTS flow control\n");

	// Setup our event handles
	hCFTSerial_Read = ::CreateEvent(0,TRUE,FALSE,0);
	hCFTSerial_Write = ::CreateEvent(0,TRUE,FALSE,0);

	// Initialize the overlapped structures
	memset(&rOverlapped,0,sizeof(OVERLAPPED));
	memset(&wOverlapped,0,sizeof(OVERLAPPED));
	rOverlapped.hEvent = hCFTSerial_Read;
	wOverlapped.hEvent = hCFTSerial_Write;
*/

	// Set the valid flag
	m_bIsValid = true;

	// Initialize our helper
	bGo = false;

	// Record wether or not we are using a thread
	init_helper_thread();

	return TRUE;
}

int CFTSerial::Close()
{
	// Reset the valid flag
	m_bIsValid = false;

	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		// Flush our buffers
		FlushALL();
	}
	
	// Cleanup our helper
	cleanup_helper_thread();

	// Close our serial port
	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		int result = 0;
		//result = FT_W32_CloseHandle(m_hCom);
		FT_Close(m_hCom);
		if (!result)
		{
			m_bIsValid = false;
			return FALSE;
		}
	}

	// Reset all of our variables
	Reset();

	return TRUE;
}

CFTSerial::~CFTSerial()
{
	Close();
}


bool CFTSerial::Valid()
{
	return m_bIsValid;
}

void CFTSerial::FlushTX()
{
	Purge(false, true);
}

void CFTSerial::FlushRX()
{
	Purge(true, false);
}

void CFTSerial::FlushALL()
{
	Purge(true, true);
}

BOOL CFTSerial::ReadByte(char &data)
{
	return (Read(&data, 1) == 1);
}

BOOL CFTSerial::WriteByte(char data)
{
	int result = Write(&data, 1);

	return (result == 1);
}

BOOL CFTSerial::ReadByte(unsigned char &data)
{
	return (Read((char *)&data, 1) == 1);
}

BOOL CFTSerial::WriteByte(unsigned char data)
{
	int result = Write((char *)&data, 1);

	return (result == 1);
}

int CFTSerial::Write(char *data, unsigned int length)
{
	iBytesWritten = 0;

	FT_Write(m_hCom, data, length, &iBytesWritten);

	return iBytesWritten;

	/*
	iBytesWritten = 0;

	bWriteRC = FT_W32_WriteFile(m_hCom, data, length, &iBytesWritten, &wOverlapped);

	if (bWriteRC == 0) //Zero means error.
   {
      DWORD dwError = FT_W32_GetLastError(m_hCom);
      if (dwError == ERROR_IO_PENDING)
	   {
		   // Wait for the write to complete
		WaitForSingleObject(hCFTSerial_Write, SERIAL_WRITE_TIMEOUT);

		if (FT_W32_GetOverlappedResult(m_hCom, &wOverlapped, &iBytesWritten, TRUE))
			return iBytesWritten;
		else
			return -1;
	   }
   }

	return iBytesWritten;
*/
}

int CFTSerial::Read(char *data, unsigned int length)
{
	iBytesRead = 0;

	FT_Read(m_hCom, data, length, &iBytesRead);

	return iBytesRead;

	/*
	bReadRC = FT_W32_ReadFile(m_hCom, data, length, &iBytesRead, &rOverlapped);
	
	if ((!bReadRC) && (GetLastError() == ERROR_IO_PENDING))
	{
		// Wait for the read to complete
		WaitForSingleObject(hCFTSerial_Read, 1000);

		if (FT_W32_GetOverlappedResult(m_hCom, &rOverlapped, &iBytesRead, TRUE))
			return iBytesRead;
		else
			return -1;
	}

	return iBytesRead;
	*/
}

// ********************************************************
// ********************************************************
// **  This next section makes possible callback functions
// ********************************************************
// ********************************************************


// ********************************************************
// ** Overrideable Read Callback Function
// ** - this function is called when the class receives
// **   notification of data being available for reading.
// ********************************************************

void CFTSerial::OnRead()
{
}

// ********************************************************
// ** Overrideable Write Callback Function
// ** - this function is called when the class receives
// **   notification of the serial ports readiness for
// **   writing.
// ********************************************************
void CFTSerial::OnWrite()
{
}

// ********************************************************
// ** Overrideable Error Callback Function
// ** - this function is called when the class receives
// **	notification of a serial port error
// ********************************************************
void CFTSerial::OnError()
{
}

// ********************************************************
// ** Initialize the Helper Thread
// ********************************************************

void CFTSerial::init_helper_thread()
{
	// See if thread is already running
	if (!bGo)
	{
		// Set the Go Flag
		bGo = true;
		
		// Turn on the high resolution settings
		timeBeginPeriod(1);

		// Start the thread
		DWORD dwThreadID = 0;
		hCFTSerial_Thread = CreateThread(0,0,CFTSerial_Thread,LPVOID(this),0,&dwThreadID);
		// Set the thread priority a bit higher
		SetThreadPriority(hCFTSerial_Thread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
}

// ********************************************************
// ** Cleanup the helper thread
// ********************************************************
void CFTSerial::cleanup_helper_thread()
{
	// Check to see if the thread is running
	if (bGo)
	{
		// Make sure that go is false
		bGo = false;

		// Wait until the thread is stopped with a 1 second timeout
		WaitForSingleObject(hCFTSerial_Thread, 1000);

		// Close the handle to the thread
		CloseHandle(hCFTSerial_Thread);

		// Make it NULL
		hCFTSerial_Thread = 0;

		// Turn off the high resolution setting
		timeEndPeriod(1);
	}
}

// ********************************************************
// ** The helper thread
// ** - this thread waits on reads and writes inorder
// **   to post a message to notify the program of
// **   availability of these two conditions
// ********************************************************
DWORD CFTSerial::CFTSerial_Thread(LPVOID pParam)
{
	CFTSerial  *pThis = (CFTSerial *)pParam;
	DWORD result = 0;

	// Call the actual object so that we may have access to it
	result = pThis->ProcessEventsUSB();

	return result;
}

// ********************************************************
// ** The class object half of the helper thread
// ** - uses this function because the thread itself
// **   has no direct access to the class
// ********************************************************

DWORD CFTSerial::ProcessEventsUSB()
{
	FTCOMSTAT status;
	DWORD	errors;

	DWORD rxBytes = 0;
	DWORD txBytes = 0;
	DWORD dwStatus = 0;

	// Clear the Communications Status Structure
	memset(&status,0,sizeof(COMSTAT));
	// Clear the Errors Variable
	errors = 0;

	while (bGo)
	{	
		if (FT_GetStatus(m_hCom, &rxBytes, &txBytes, &dwStatus) == FT_OK)
		//if (FT_W32_ClearCommError(m_hCom, &errors, &status) != 0)
		{
			//FT_GetStatus(m_hCom, &rxBytes, &txBytes, &dwStatus);

			// DEBUG
			TRACE("FT_GetStatus: %d, %d, %d\n", rxBytes, txBytes, dwStatus);

			/*
			if ((m_dwEvents & EV_ERR) && (errors > 0))
			{
				OnError();
			}

			if ((m_dwEvents & EV_RXCHAR) && (status.cbInQue > 0))
			{
				OnRead();
			}
			*/

			if (txBytes < 128)
			//if ((m_dwEvents & EV_TXEMPTY) && (status.cbOutQue < 256))
			{
				OnWrite();
			}
		}

		Sleep(1);
	}

	return SERIAL_THREAD_ID;
}

// *************************************************************************************************************
// Helper Functions
// *************************************************************************************************************

BOOL CFTSerial::Purge(bool rx, bool tx)
{
	if (m_hCom == INVALID_HANDLE_VALUE)
		return FALSE;

	//FT_W32_PurgeComm(m_hCom, (tx ? (PURGE_TXCLEAR | PURGE_TXABORT) : 0) | (rx ? (PURGE_RXCLEAR | PURGE_RXABORT) : 0));
	FT_Purge(m_hCom, (rx ? FT_PURGE_RX : 0) | (tx ? FT_PURGE_TX : 0));
	return TRUE;
}
