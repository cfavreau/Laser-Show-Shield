
#ifndef __FTSERIAL_H__
#define __FTSERIAL_H__

//#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "FTD2XX.h"

// Helps keep track of which thread exited properly
#define SERIAL_THREAD_ID 1003

class CFTSerial
{
public:
	CFTSerial();
	virtual ~CFTSerial();

public:
	
	int Init(LPSTR port_name, unsigned int BaudRate, unsigned int ByteSize, unsigned int Parity, unsigned int StopBits, DWORD Events = 0, bool bUseThread = true);
	int Close();
	void Reset();
	bool Valid();
	int Write(char *data, unsigned int length);
	int Read(char *data, unsigned int length);
	BOOL Purge(bool rx, bool tx);

	BOOL ReadByte(char &data);
	BOOL WriteByte(char data);
	BOOL ReadByte(unsigned char &data);
	BOOL WriteByte(unsigned char data);
	void FlushTX();
	void FlushRX();
	void FlushALL();

private:
	// Callback variables
	bool bGo;						// Starts and stops the thread
	HANDLE hCFTSerial_Thread;			// Handle to the helper thread
	HANDLE hCFTSerial_Read;			// Handle to an overlapped read event;
	HANDLE hCFTSerial_Write;			// Handle to an overlapped write event;

	// Callback functionality
	virtual void OnRead();		// Overrideable callback function for reading
	virtual void OnWrite();	// Overrideable callback function for writing
	virtual void OnError();	// Overrideable callback function for handling errors
	void init_helper_thread();	// Initialize the helper thread
	void cleanup_helper_thread();	// Cleanup the helper thread
	static DWORD WINAPI CFTSerial_Thread(LPVOID pParam);	// Helper thread that does the waiting and message posting
	DWORD ProcessEventsUSB();

	// Serial Port Variables
	CString			m_szPortName;
	bool			m_bIsValid;
	BOOL			m_bPortReady;
	HANDLE			m_hCom;
	unsigned int	m_iComPort;
	DCB				m_dcb;
	COMMTIMEOUTS	m_CommTimeouts;
	DWORD			m_dwEvents;
	BOOL			bWriteRC;
	BOOL			bReadRC;
	unsigned long int	iBytesWritten;
	unsigned long int	iBytesRead;

	OVERLAPPED rOverlapped;
	OVERLAPPED wOverlapped;
};

#endif // __FTSERIAL_H__

