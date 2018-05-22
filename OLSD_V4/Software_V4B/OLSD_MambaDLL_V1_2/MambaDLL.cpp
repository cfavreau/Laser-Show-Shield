// MambaDLL2.cpp : Defines the initialization routines for the DLL.
//
#include "mamba_structs.h"				// Mamba Structures
#include ".\OLSD\trace.h"				// Debugging log file
#include "LaserOutputMamba.h"			// OLSD Laser Output Class

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// OLSD Laser Output Class Object
CLaserOutputMamba laser_output;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// DEBUG
	//Trace("DLLMain\n");

	return 1;   // ok
}

extern "C" WORD __stdcall MLRegisterDriver(char* b)
{
	// DEBUG
	//Trace("MLRegisterDriver\n");

	// Copy the Mamba Library Device string
	strncpy(b, "MLDevice", 8);

	return 0;
}

extern "C" bool __stdcall MLDevicePresent()
{
	// DEBUG
	//Trace("MLDevicePresent\n");

	// Check for the OLSD device

	// Attempt to initialize the OLSD device
	bool success = (laser_output.Init() == TRUE);

	// Shutdown the device
	laser_output.Shutdown();

	// Return success
	return success;
}

extern "C" bool __stdcall MLGetName(char* n)
{
	// DEBUG
	//Trace("MLGetName\n");
	
	// This is limited to 20 characters
	// So use a string copy that we can constrain to
	// no more than 20 characters

	// Identify ourselves as a frame streamer express
	strncpy(n, "OLSD", 4);

	return true;
}

extern "C" WORD __stdcall MLKind()
{
	// DEBUG
	//Trace("MLKind\n");
	
	// Return 1 because we are a frame oriented device
	return 1;
}

extern "C" bool __stdcall MLInitDevice(WORD h)
{
	// DEBUG
	//Trace("MLInitDevice\n");

	// Initialize the frame streamer device
	return (laser_output.Init() == TRUE);
}

extern "C" bool __stdcall MLInitShow(WORD h, WORD c)
{
	// DEBUG
	//Trace("MLInitShow\n");
	
	// Start
	laser_output.Start();

	return true;
}

extern "C" bool __stdcall MLDraw(WORD h, WORD c, char* d, int n)
{
	// DEBUG
	//Trace("MLDraw: %d points.\n", n);

	// Make sure the d pointer is good and that there are actually points
	if (!d) return false;
	if (n < 1) return false;

	// Call the Laser Output function to display the frame
	laser_output.PlayFrame((struct MAMBA_POINT *)d, n);

	// Return successful
	return true;
}

extern "C" bool __stdcall MLHaltShow(WORD h, WORD c)
{
	// DEBUG
	//Trace("MLHaltShow\n");
	
	// Stop the laser output
	laser_output.Stop();

	// Call the reset function in the laser output in order to stop the output
	laser_output.Reset();

	// Blank and center the laser output
	laser_output.BlankAndCenter();

	// Return successful
	return true;
}

extern "C" bool __stdcall MLHalt()
{
	// DEBUG
	//Trace("MLHalt\n");

	// Shutdown the laser output function
	return (laser_output.Shutdown() == TRUE);
}

extern "C" bool __stdcall MLParams(WORD h, WORD c, char* p)
{
	// DEBUG
	//Trace("MLParams\n");
	
	// Make sure the p pointer is good
	if (!p) return false;

	// Cast the p Pointer into a PARAMS structure
	struct MAMBA_PARAMS *pParams = (struct MAMBA_PARAMS *)p;

	// Check to make sure the size is either 6 or 7
	// Any other values will make this call invalid!
	if ((pParams->size != 6) && (pParams->size != 7)) return false;

	// DEBUG
	// Print out the params structure
	//Trace("Params: Size(%d) PointRate(%d) InvertBlanking(%s)\n", pParams->size, pParams->point_rate, (pParams->invert_blanking ? "INVERT BLANKING" : "NORMAL BLANKING"));

	// Make sure the point output rate is not bigger than 65535
	if (pParams->point_rate > 65535) pParams->point_rate = 65535;

	// Set the point output rate for the laser output device
	laser_output.SetScanSpeed((WORD)pParams->point_rate);

	// TODO
	// Set the RGB/Blank inversion here

   return true;
}

extern "C" WORD __stdcall MLGetError(WORD h)
{
	// DEBUG
	//Trace("MLGetError\n");

	// TODO - need to figure out what numbers to use
	// and how to use this function
	return 0;
}

extern "C" WORD __stdcall MLNumDevices(WORD h)
{
	// DEBUG
	//Trace("MLNumDevices\n");

	// TODO - need to figure out what numbers to use
	// and how to use this function
	return 1;
}

