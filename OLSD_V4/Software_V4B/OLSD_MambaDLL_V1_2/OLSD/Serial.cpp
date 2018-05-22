#include "Serial.h"
#include "trace.h"			// DEBUG TRACE -> Generates Trace.TXT file

// This number should be bigger than 1000 points per second per write
// Since we are using a maximum of 1365 points per write we should use a timeout bigger than 1 second
#define SERIAL_WRITE_TIMEOUT 1000

CSerial::CSerial()
{
	// Call the reset function
	Reset();
}

void CSerial::Reset()
{
	// Initialize a bunch o variables so we don't have problems
	bGo = false;
	hCSerial_Thread = INVALID_HANDLE_VALUE;
	hCSerial_Read = INVALID_HANDLE_VALUE;
	hCSerial_Write = INVALID_HANDLE_VALUE;
	
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

	// Reset the use thread flag
	m_bUseThread = true;
}

int CSerial::Init(LPSTR port_name, unsigned int BaudRate, unsigned int ByteSize, unsigned int Parity, unsigned int StopBits, DWORD Events, bool bUseThread)
{
	// Check to see if the port is already opened
	if (m_bIsValid)
		Close();

	// Open the USB Serial Port using the Description!
	// Non-Blocking IO (kind of)
	//m_hCom = m_usb.CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FT_OPEN_BY_DESCRIPTION, NULL);
	// Blocking IO
	m_hCom = FT_W32_CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FT_OPEN_BY_DESCRIPTION, NULL);

	// Check to see if there wasn't a problem opening the Com Port
	if (m_hCom == INVALID_HANDLE_VALUE)
   {
      // DEBUG
      Trace("CSerial::Init() - CreateFile returned INVALID_HANDLE_VALUE.\n");
	  return FALSE;
   }

	// Flush the Comm Port Buffers in order to get the Comm State
	int result = 0;
	FlushALL();

	// Setup the Com Port buffer size
	m_bPortReady = m_usb.SetupComm(m_hCom, 2048, 2048);
	//m_bPortReady = m_usb.SetupComm(m_hCom, 4096, 4096);

	if (!m_bPortReady)
	{
      Trace("CSerial::Init() - SetupComm failed.\n");
	  Close();
	  return FALSE;
	}
	
	// Flush the Comm Port Buffers in order to get the Comm State
	FlushALL();

	// Setup The Com Port parameters to what we want to be initialized to
	m_bPortReady = m_usb.GetCommState(m_hCom, (LPFTDCB)&m_dcb);

	if (!m_bPortReady)
	{
      // DEBUG
      Trace("CSerial::Init() - GetCommState failed.\n");
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
	m_bPortReady = m_usb.SetCommState(m_hCom, (LPFTDCB)&m_dcb);

	if (!m_bPortReady)
	{
      Trace("CSerial::Init() - SetCommState failed.\n");
	  Close();
	  return FALSE;
	}

	
	// Setup The Timeouts
	m_bPortReady = m_usb.GetCommTimeouts(m_hCom, (LPFTTIMEOUTS)&m_CommTimeouts);

	// Set the read timeout to 1000 milliseconds
	// This should give enough time for whatever microcontroller or computer to respond to a request
	m_CommTimeouts.ReadIntervalTimeout = 0;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	
	// Have to set a really big timeout for the FTD2XX drivers for the ILDA_PLAYER APPLICATION
	// This is important to set this to be something large compared to the length in time
	// of the slowest point output rate and the maximum points to be sent to the laser device at 1 time
	m_CommTimeouts.WriteTotalTimeoutConstant = 10000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 10;

	m_bPortReady = m_usb.SetCommTimeouts(m_hCom, (LPFTTIMEOUTS)&m_CommTimeouts);

	// Set the Serial Port Events We Will Wait For
	m_dwEvents = Events;

	result = m_usb.SetCommMask(m_hCom, Events);
		
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
	if (FT_SetFlowControl(m_hCom, FT_FLOW_RTS_CTS, 0, 0) != FT_OK) Trace("Failed to setup RTS/CTS flow control\n");

	// Setup our event handles
	hCSerial_Read = ::CreateEvent(0,TRUE,FALSE,0);
	hCSerial_Write = ::CreateEvent(0,TRUE,FALSE,0);

	// Initialize the overlapped structures
	memset(&rOverlapped,0,sizeof(OVERLAPPED));
	memset(&wOverlapped,0,sizeof(OVERLAPPED));
	rOverlapped.hEvent = hCSerial_Read;
	wOverlapped.hEvent = hCSerial_Write;

	// Set the valid flag
	m_bIsValid = true;

	// Initialize our helper
	bGo = false;

	// Record wether or not we are using a thread
	m_bUseThread = bUseThread;
	if (m_bUseThread) init_helper_thread();

	return TRUE;
}

int CSerial::Close()
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
		result = m_usb.CloseHandle(m_hCom);
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

CSerial::~CSerial()
{
	Close();
}


bool CSerial::Valid()
{
	return m_bIsValid;
}

void CSerial::FlushTX()
{
	Purge(false, true);
}

void CSerial::FlushRX()
{
	Purge(true, false);
}

void CSerial::FlushALL()
{
	Purge(true, true);
}

BOOL CSerial::ReadByte(char &data)
{
	return (Read(&data, 1) == 1);
}

BOOL CSerial::WriteByte(char data)
{
	int result = Write(&data, 1);

	return (result == 1);
}

BOOL CSerial::ReadByte(unsigned char &data)
{
	return (Read((char *)&data, 1) == 1);
}

BOOL CSerial::WriteByte(unsigned char data)
{
	int result = Write((char *)&data, 1);

	return (result == 1);
}

int CSerial::Write(char *data, unsigned int length)
{
	iBytesWritten = 0;

	bWriteRC = m_usb.WriteFile(m_hCom, data, length, &iBytesWritten, &wOverlapped);

	if (bWriteRC == 0) //Zero means error.
   {
      DWORD dwError = m_usb.GetLastError(m_hCom);
      if (dwError == ERROR_IO_PENDING)
	   {
		   // Wait for the write to complete
		WaitForSingleObject(hCSerial_Write, SERIAL_WRITE_TIMEOUT);

		if (m_usb.GetOverlappedResult(m_hCom, &wOverlapped, &iBytesWritten, TRUE))
			return iBytesWritten;
		else
			return -1;
	   }
   }

	return iBytesWritten;
}

int CSerial::Read(char *data, unsigned int length)
{
	iBytesRead = 0;

	bReadRC = m_usb.ReadFile(m_hCom, data, length, &iBytesRead, &rOverlapped);
	
	if ((!bReadRC) && (GetLastError() == ERROR_IO_PENDING))
	{
		// Wait for the read to complete
		WaitForSingleObject(hCSerial_Read, 1000);

		if (m_usb.GetOverlappedResult(m_hCom, &rOverlapped, &iBytesRead, TRUE))
			return iBytesRead;
		else
			return -1;
	}

	return iBytesRead;
}

BOOL CSerial::ToggleDTR(BOOL on)
{
	// Set the function according to what the caller wants
	DWORD dwFunction = (on ? SETDTR : CLRDTR);
	
	// Do it!
	return m_usb.EscapeCommFunction(m_hCom, dwFunction);
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

void CSerial::OnRead()
{
}

// ********************************************************
// ** Overrideable Write Callback Function
// ** - this function is called when the class receives
// **   notification of the serial ports readiness for
// **   writing.
// ********************************************************
void CSerial::OnWrite()
{
}

// ********************************************************
// ** Overrideable Error Callback Function
// ** - this function is called when the class receives
// **	notification of a serial port error
// ********************************************************
void CSerial::OnError()
{
}

// ********************************************************
// ** Initialize the Helper Thread
// ********************************************************

void CSerial::init_helper_thread()
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
		hCSerial_Thread = CreateThread(0,0,CSerial_Thread,LPVOID(this),0,&dwThreadID);
		// Set the thread priority a bit higher
		SetThreadPriority(hCSerial_Thread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
}

// ********************************************************
// ** Cleanup the helper thread
// ********************************************************
void CSerial::cleanup_helper_thread()
{
	// Check to see if the thread is running
	if (bGo)
	{
		// Make sure that go is false
		bGo = false;

		// Wait until the thread is stopped with a 1 second timeout
		WaitForSingleObject(hCSerial_Thread, 1000);

		// Close the handle to the thread
		CloseHandle(hCSerial_Thread);

		// Make it NULL
		hCSerial_Thread = 0;

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
DWORD CSerial::CSerial_Thread(LPVOID pParam)
{
	CSerial  *pThis = (CSerial *)pParam;
	DWORD result = 0;

	// Call the actual object so that we may have access to it
	result = pThis->ProcessEventsUSB();

	return result;
}
// ********************************************************
// ** Public function to process events just in case
// ** the programmer does not wish to use our thread
// ** to do this.
// ********************************************************
BOOL CSerial::ProcessEvents()
{
	// Check to see if our thread is operational
	if (m_bUseThread) return FALSE;

	// Regular Windows Serial Port Stuff
	COMSTAT status;
	DWORD	errors;

	// Clear the Communications Status Structure
	memset(&status, 0, sizeof(COMSTAT));
	// Clear the Errors Variable
	errors = 0;

	if (m_usb.ClearCommError(m_hCom, &errors, (FTCOMSTAT *)&status) != 0)
	{
		if ((m_dwEvents & EV_ERR) && (errors > 0)) OnError();

		if ((m_dwEvents & EV_RXCHAR) && (status.cbInQue > 0)) OnRead();

		if ((m_dwEvents & EV_TXEMPTY) && (status.cbOutQue < 1)) OnWrite();
	}

	return TRUE;
}

// ********************************************************
// ** The class object half of the helper thread
// ** - uses this function because the thread itself
// **   has no direct access to the class
// ********************************************************

DWORD CSerial::ProcessEventsUSB()
{
	FTCOMSTAT status;
	DWORD	errors;

	// Clear the Communications Status Structure
	memset(&status,0,sizeof(COMSTAT));
	// Clear the Errors Variable
	errors = 0;

	while (bGo)
	{	
		if (m_usb.ClearCommError(m_hCom, &errors, &status) != 0)
		{
			if ((m_dwEvents & EV_ERR) && (errors > 0))
			{
				OnError();
			}

			if ((m_dwEvents & EV_RXCHAR) && (status.cbInQue > 0))
			{
				OnRead();
			}

			if ((m_dwEvents & EV_TXEMPTY) && (status.cbOutQue < 256))
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

BOOL CSerial::Purge(bool rx, bool tx)
{
	if (m_hCom == INVALID_HANDLE_VALUE)
		return FALSE;

	m_usb.PurgeComm(m_hCom, (tx ? (PURGE_TXCLEAR | PURGE_TXABORT) : 0) | (rx ? (PURGE_RXCLEAR | PURGE_RXABORT) : 0));

	return TRUE;
}

BOOL CSerial::IsDLLFound()
{
   return m_usb.IsDLLFound();
}
