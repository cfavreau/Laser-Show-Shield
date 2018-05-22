#include <windows.h>
#include "OpenLaserShowControllerV1.0.0.h" // Important data structures for the OLSC interface
#include "OLSD\laser_output.h"             // Frame Streamer Express Control Class

#define MAX_OLSD_DEVICES    8

class OLSD
{
private:
    
    // Array of initialized and allocated OLSD devices
    CLaserOutput *m_pLasers[MAX_OLSD_DEVICES];

    // Count of successfully initialized OLSD devices
    int m_iDeviceCount;

public:
    
    // Constructor
    OLSD();
    // Destructor
    virtual ~OLSD();

    // OLSC Interface Functions
    int Initialize(void);
    int Shutdown(void);
    int GetDeviceCount(void);
    int GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities);
    int GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length);
    int Play(int device_number);
    int Pause(int device_number);
    int Shutter(int device_number, int state);
    int WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame);
    int GetStatus(int device_number, DWORD &status);
    int SetCallback(int device_number, HWND parent_window_handle, DWORD message);
    int WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length);
    int ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length);
    int WriteTTL(int device_number, DWORD data);
    int ReadTTL(int device_number, DWORD &data);
    
    // Put Helper Functions Here
    int ValidDevice(int device_number);
};

