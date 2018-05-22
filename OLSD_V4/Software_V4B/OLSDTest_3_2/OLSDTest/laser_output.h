#ifndef _CLASEROUTPUT_
#define _CLASEROUTPUT_

#include "FTSerial.h"
#include "mmsystem.h"

#define WM_LASEROUTPUT_NEXTFRAME	WM_USER + 8765
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


// Streaming Packet Format
#pragma pack(push)
#pragma pack(1)

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

//#define MAX_FRAME_SIZE 4096
#define MAX_FRAME_SIZE 8092

#pragma pack(pop)

class CLaserOutput : public CFTSerial
{
private:
   
	// Name of the laser device
	char m_szName[256];

protected:
	// Currently displayed frame
	//laser_point m_points[MAX_FRAME_SIZE];
	BYTE *m_point_stream;
	int m_total_bytes;
	int m_total_points;
	int m_total_bytes_written;
	
	bool m_bGo;

	bool m_bBlankAndCenter;

	int m_old_scan_speed;
	int m_new_scan_speed;

	void WriteScanSpeed(int speed);
	void WriteUserOutputMask(DWORD dwMask);
	void WriteStream();
	void WriteResetPacket();
	void WriteBlankAndCenter();

	virtual void OnWrite();
	virtual void OnRead();

	int SetPoint(laser_point &lp, unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b, bool first_point = true);
	int SetSpeed(laser_point &lp, int speed);
	void ClearLastPoint();

public:
	CLaserOutput();
	~CLaserOutput();

	BOOL Init();
    char *GetName();
	BOOL Shutdown();

	BOOL Reset();

	BOOL IsInitialized();

	void SetScanSpeed(WORD speed);
	void Start();
	void Stop();
	void Shutter(bool on);
	void SetUserOutput(DWORD mask);
	void BlankAndCenter();
	void PlayFrame(struct LASER_SHOW_DEVICE_FRAME *pFrame, bool invertX = false, bool invertY = false, bool swapXY = false);
};

#endif // _CLASEROUPUT_