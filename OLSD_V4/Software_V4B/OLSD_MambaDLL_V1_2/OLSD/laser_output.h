#include "Serial.h"
#include "windows.h" 
#include "mmsystem.h"

#ifndef _CLASEROUTPUT_
#define _CLASEROUTPUT_

// Streaming Packet Format
#pragma pack(push,1)
#pragma pack(1)

#define LASEROUTPUT_DEFAULT_SCAN_SPEED	12000

// Frame & Point Structures
struct LASER_SHOW_DEVICE_POINT
{
	WORD x;
	WORD y;
	WORD r;
	WORD g;
	WORD b;
	WORD i;
};

struct LASER_SHOW_DEVICE_FRAME
{
	int display_speed;
	int point_count;
	struct LASER_SHOW_DEVICE_POINT *points;
};

// OLSD Laser Point
struct laser_point
{
	unsigned char command;
	unsigned char x;
	unsigned char y;
	unsigned char xy;
	unsigned char rg;
	//unsigned char bi;
};

#define MAX_FRAME_SIZE 8192
//s#define MAX_FRAME_SIZE 32768

struct POINT_FIFO
{
	laser_point points[MAX_FRAME_SIZE];
	int count;
    int display_speed;
};

#define POINT_FIFO_SIZE	16

#pragma pack(pop)

class CLaserOutput : public CSerial
{
protected:
	
	// TODO - Clean the implementation of the Frame FIFO up
	// Point Buffer
	POINT_FIFO m_fifo[POINT_FIFO_SIZE];
	int m_fifo_front;
	int m_fifo_end;

	// Currently displayed frame
	laser_point m_points[MAX_FRAME_SIZE];
	int m_total_points;
	int m_total_points_written;

	bool m_bBlankAndCenter;
	bool m_bGo;

	int m_old_scan_speed;
	int m_new_scan_speed;

    char m_sVersion[16];

    // Callback variables
    HWND m_hParentWnd;
    DWORD m_dwCallbackMessage;

	void SetPoint(laser_point &lp, unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b);
	void WriteStream();
	void WriteResetPacket();
	void WriteBlankAndCenter();
    void WriteUserOutputMask(DWORD dwMask);
	void WriteScanSpeed(int speed);

	virtual void OnWrite();
	virtual void OnRead();

public:
	CLaserOutput();
	~CLaserOutput();

	BOOL Init();
	BOOL Shutdown();

    char *GetVersion() {return m_sVersion;};
	
	BOOL Reset();

	void Start();
	void Stop();
	void BlankAndCenter();
	void SetScanSpeed(WORD speed);
	
    void SetCallback(HWND hWnd, DWORD dwMessage);
	
    // OLSC Based Play Frame
    void PlayFrame(struct LASER_SHOW_DEVICE_FRAME *pFrame, bool invertX = false, bool invertY = false, bool swapXY = false);
};

#endif // _CLASEROUPUT_
