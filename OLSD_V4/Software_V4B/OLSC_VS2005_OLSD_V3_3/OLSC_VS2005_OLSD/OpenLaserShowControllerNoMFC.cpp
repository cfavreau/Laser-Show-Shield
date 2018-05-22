// OpenLaserShowController.cpp : Defines the initialization routines for the DLL.
//

#include <stdio.h>

// Defining OLSC_EXPORTS insures that the OLSC functions will be exported
// Please see OpenLaserShowControllerVX.X.X.h for more information
#define OLSC_EXPORTS

#include "OpenLaserShowControllerV1.0.0.h"
#include "OLSD.h"                       // Frame Streamer Express V2 Code
#include "OLSD\trace.h"                 // Debugging Log File

// Laser Output Dialog
BOOL g_bInitialized = FALSE;
OLSD *g_pDAC = NULL;

#define OPEN_LASER_SHOW_DEVICE_API_VERSION	1

#define OPEN_LASER_SHOW_DEVICE_NAME "OLSD"

// OLSC_GetAPIVersion
// Inputs: None
// Outputs: Returns a single integer that represents the version number
// Description: Returns the version of the OpenDAC API
//				so we don't make any critical errors when using this DLL
//
OLSC_API int WINAPI OLSC_GetAPIVersion(void)
{
	return OPEN_LASER_SHOW_DEVICE_API_VERSION;
}

// OLSC_GetAPIVersion
// Inputs: Pointer to a string at least 64 characters in length
// Outputs: Returns success or failure
// Description: Returns the string name of the OpenDC Interface
//
OLSC_API int WINAPI OLSC_GetInterfaceName(char *pString)
{
	if (!pString) return 0;
	
	sprintf(pString, OPEN_LASER_SHOW_DEVICE_NAME);

	return 1;
}

// OLSC_Initialize
// Inputs: None
// Outputs: Returns the number of available devices.
// Description: Initializes the hardware
OLSC_API int WINAPI OLSC_Initialize(void)
{
    // Disable the trace file for now
    //EnableTrace(true);

    Trace("OLSC_Initialize() - enter\n");

    // Check to see if our OLSD class array has been initialized
    if (!g_bInitialized)
    {
        Trace("OLSC_Initialize - Creating new instance of the OLSD object.\n");
        g_pDAC = new OLSD();
        g_bInitialized = TRUE;
    }
    else
    {
        Trace("OLSC_Initialize - already initialized!\n");
    }

    if (g_pDAC)
    {
        Trace("OLSC_Initialize - Calling the OLSD Object Initialize Function.\n");
        int result = g_pDAC->Initialize();
        Trace("OLSC_Initialize - OLSD->Initialize() returned %d.\n", result);
        return result;
    }
    
    Trace("OLSC_Initialize - exit - failure!\n");

    return 0;
}

// OLSC_Shutdown
// Inputs: None
// Outputs: Returns success or failure
// Description: Shuts down all devices
//
OLSC_API int WINAPI OLSC_Shutdown(void)
{
    Trace("OLSC_Shutdown() - enter.\n");

    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC)
    {
        Trace("OLSC_Shutdown() - not initialized!\n");
        return TRUE;
    }

    Trace("OLSC_Shutdown() - calling OLSD->Shutdown().\n");

    // Shutdown the DAC code
    int result = g_pDAC->Shutdown();

    Trace("OLSC_Shutdown() - OLSD->Shutdown() returned %d.\n", result);

    Trace("OLSC_Shutdown() - cleaning up allocated object memory.\n");

    // Cleanup the allocated memory
    delete g_pDAC;
    g_pDAC = NULL;
    g_bInitialized = FALSE;

    Trace("OLSC_Shutdown() - returning shutdown result.\n");
    
    // Return the shutdown result
    return result;
}

// OLSC_GetDeviceCount
// Inputs: None
// Outputs: Returns the number of available devices
// Description: Has to be called AFTER Intialize has been called.  Thsi function
// is to be used to query the number of available devices without calling
// intialize again and again
OLSC_API int WINAPI OLSC_GetDeviceCount(void)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return 0;
    
    return g_pDAC->GetDeviceCount();
}

// OLSC_GetCapabilities
// Inputs:  device Number, device capabilties structure to be filled in
// Outputs: Returns success or failure
// Description: Gets the capabilties of each device attached
//				Caps:
//				Device Name
//				Device Version
//				Min Speed/Max Speed
//				Min Frame Size/Max Frame Size
//				DMX In/Out?
//				TTL In/Out?
//				X/Y Resolution (8 to 16 bits)
//				Color Resolution (1 to 16 bits)
//				Uses callbacks?
//				Some reserved space for future use
OLSC_API int WINAPI OLSC_GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->GetDeviceCapabilities(device_number, device_capabilities);
}

// OLSC_GetLastErrorNumber
// Inputs: device number, error string pointer, and string length max.
// Outputs: error number, and actual error string
// Description: Get the string and/or number associated with the last error
//
OLSC_API int WINAPI OLSC_GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->GetLastErrorNumber(device_number, number, string_pointer, string_length);
}

// OLSC_Play
// Inputs: device number
// Outputs: Returns success or failure
// Description: Starts the output on a particular device or all devices
//
OLSC_API int WINAPI OLSC_Play(int device_number)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    Trace("OLSC_Play()\n");

    return g_pDAC->Play(device_number);
}

// OLSC_Pause
// Inputs: device number
// Outputs: Returns success or failure
// Description: Stops the output on a particular device or all devices
//
OLSC_API int WINAPI OLSC_Pause(int device_number)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->Pause(device_number);
}

// OLSC_Shutter
// Inputs: device number, shutter state (LASER_SHOW_DEVICE_SHUTTER_STATE)
// Outputs: Returns success or failure
// Description: Turns the laser show device's shutter On/Off
//
OLSC_API int WINAPI OLSC_Shutter(int device_number, int state)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->Shutter(device_number, state);
}

// OLSC_WriteFrameEx
// Inputs: device number, display speed (pps), point count, point to array of LASER_SHOW_DEVICE_POINTs 
// Outputs: Returns success or failure
// Description:
// Writes a frame to a particular device or all devices
// Point Array:
// X 16 bit unsigned
// Y 16 bit unsigned
// R 16 bit unsigned
// G 16 bit unsigned
// B 16 bit unsigned
// I 16 bit unsigned
//
// ** Any frame that is written will be displayed until another frame is written to take its place or the Output is Paused
//
OLSC_API int WINAPI OLSC_WriteFrameEx(int device_number, int display_speed, int point_count, struct LASER_SHOW_DEVICE_POINT *points)
{
    struct LASER_SHOW_DEVICE_FRAME frame;
    frame.display_speed = display_speed;
    frame.point_count = point_count;
    frame.points = points;
    
    Trace("OLSC_WriteFrameEx()\n");

    return OLSC_WriteFrame(device_number, frame);
}

// OLSC_WriteFrame
// Inputs: device number, frame structure (LASER_SHOW_DEIVCE_FRAME)
// Outputs: Returns success or failure
// Description:
// Writes a frame to a particular device or all devices
// Pass in a frame:
// Point Count
// Display Speed PPS
// Point Array:
// X 16 bit unsigned
// Y 16 bit unsigned
// R 16 bit unsigned
// G 16 bit unsigned
// B 16 bit unsigned
// I 16 bit unsigned
//
// ** Any frame that is written will be displayed until another frame is written to take its place or the Output is Paused
//

OLSC_API int WINAPI OLSC_WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame)
{
    // TEMPORARY!!! - until people get the idea that we are supposed to hit play first!
    OLSC_Play(device_number);

    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    // Check to see if the point count is OK
    if ((frame.point_count < 1) || (frame.point_count > MAX_FRAME_SIZE)) return FALSE;

    // Check to see if the point array pointer is good
    if (!frame.points) return FALSE;

    int result = g_pDAC->WriteFrame(device_number, frame);

    //Trace("OLSC_WriteFrame() - device_number(%d) display_speed(%d) point_count(%d) points(0x%08X) return(%d)\n", device_number, frame.display_speed, frame.point_count, (unsigned int)frame.points, result);

    return result;
}

// OLSC_GetStatus()
// Inputs: device number, status DWORD
// Outputs:  Returns success or failure
// Description:
// Gets the status of a particular device
// Can be used for polling or in conjunction with Windows Message Callback
// Status Structure or Bit Mask:
// Bit 0 = Buffer Full
// Bit 1 = Buffer Empty
// Bit 2 = DMX Out Complete
// Bit 3 = DMX In Ready
// Bit 4 = TTL Out Complete
// Bit 5 = TLL In Ready
// Bit 6 = Resreved
// Bit 7 = Reserved
// Bit 8 = Reserved
// Bit 9-30 = Reserved
// Bit 31 = An Error Occured
OLSC_API int WINAPI OLSC_GetStatus(int device_number, DWORD &status)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->GetStatus(device_number, status);
}

// OLSC_SetCallback()
// Inputs: device number, parent window handle (for SendMessage), message DWORD
// Outputs:  Returns success or failure
// Description:
// Sets the call back window parent handle and windows message to send to run the callback
// and set the call back notification mask bits per the GetStatus mask.  The Callback message
// will return the Device Number in the wParam and the Status mask in the lParam.
// We all work in windows right?
// This interface is optional in the DLL and in the application.  The DLL writers should take this into account.
// So if you use the callback ... be able to function without it... it is used for efficiency.
OLSC_API int WINAPI OLSC_SetCallback(int device_number, HWND parent_window_handle, DWORD message)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

    return g_pDAC->SetCallback(device_number, parent_window_handle, message);
}

// OLSC_WriteDMX()
// Inputs: device number, start address, data pointer, data length
// Outputs:  Returns success or failure
// Description:
// Writes DMX data to a particular device
OLSC_API int WINAPI OLSC_WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

	return g_pDAC->WriteDMX(device_number, start_address, data_pointer, length);
}

// OLSC_ReadDMX()
// Inputs: device number, start address, data pointer, data length
// Outputs:  Returns success or failure
// Description:
// Reads DMX data from a particular device
OLSC_API int WINAPI OLSC_ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

	return g_pDAC->ReadDMX(device_number, start_address, data_pointer, length);
}

// OLSC_WriteTTL()
// Inputs: device number, data DWORD (up to 32 bits of TTL outputs)
// Outputs:  Returns success or failure
// Description:
// Writes TTL data from a particular device
OLSC_API int WINAPI OLSC_WriteTTL(int device_number, DWORD data)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

	return g_pDAC->WriteTTL(device_number, data);
}

// OLSC_ReadTTL()
// Inputs: device number, data DWORD (up to 32 bits of TTL inputs)
// Outputs:  Returns success or failure
// Description:
// Reads TTL data from a particular device
OLSC_API int WINAPI OLSC_ReadTTL(int device_number, DWORD &data)
{
    // Check to see if we have been initialized
    if (!g_bInitialized || !g_pDAC) return FALSE;

	return g_pDAC->ReadTTL(device_number, data);
}

