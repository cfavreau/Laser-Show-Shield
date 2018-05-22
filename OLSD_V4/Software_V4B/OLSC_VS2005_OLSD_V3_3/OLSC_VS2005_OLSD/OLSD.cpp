#include "OLSD.h"

OLSD::OLSD()
{
    // Allocate all of the laser output object pointers
    int i;
    for (i = 0; i < MAX_OLSD_DEVICES; i++)
        m_pLasers[i] = new CLaserOutput();

    // Reset the device count
    m_iDeviceCount = 0;
}

OLSD::~OLSD()
{
    // Free up the allocated laser output device objects
    int i;
    for (i = 0; i < MAX_OLSD_DEVICES; i++)
    {
		// We should have allocated all the laser interfaces
		// with an object however it is good practice to check than crashs
		// Maybe we ran out of memory or something weird...
		if (m_pLasers[i])
		{
			// Shutdown the interface first 
			m_pLasers[i]->Shutdown();
			// TODO - why does deleting the object cause
			// memory heap corruption???? For now just
			// comment it out.  It appears to work fine without it.
			// Now free up the object memory
			//delete m_pLasers[i];
			// Invalidate the pointer
			m_pLasers[i] = NULL;
		}
    }
    
    // Reset the device count
    m_iDeviceCount = 0;
}

int OLSD::Initialize(void)
{
    // Check to see if we have already been initialized
    if (m_iDeviceCount > 0)
    {
        // Shutdown First
        Shutdown();
        // Set the device count to 0
        m_iDeviceCount = 0;
    }
    
    // Initialize devices until one of the laser ouptut obejects fails
    int i = 0;
    for (i = 0; i < MAX_OLSD_DEVICES; i++)
    {
        if  (m_pLasers[i]->Init())
        {   
            // Blank and center the output
            m_pLasers[i]->BlankAndCenter();
            // Increment the device count
            m_iDeviceCount++;
        }
        else
            break;

        Sleep(100);
    }

    // Return the device count
    return m_iDeviceCount;
}

int OLSD::Shutdown(void)
{
    // Shutdown all of the valid laser output devices
    int i;
    for (i = 0; i < MAX_OLSD_DEVICES; i++)
    {
        // If the laser output object is valid
        if (m_pLasers[i]->Valid())
        {
            // Turn the shutter ON
            //m_pLasers[i]->Shutter(true);
            // Blank and center the beam
            m_pLasers[i]->BlankAndCenter();
            // Stop the output
            m_pLasers[i]->Stop();
            // Shutdown the object
            m_pLasers[i]->Shutdown();
        }
    }

    // Reset the device count
    m_iDeviceCount = 0;

    // Return successful
    return 1;
}

int OLSD::GetDeviceCount(void)
{
    // Return the device count
    return m_iDeviceCount;
}

int OLSD::GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // Fill out the device capabilities for this device
	device_capabilities.color_resolution = 8;
	device_capabilities.has_dmx_in = false;
	device_capabilities.has_dmx_out = false;
	device_capabilities.has_ttl_in = false;
	device_capabilities.has_ttl_out = true;
	device_capabilities.max_frame_size = MAX_FRAME_SIZE;
	device_capabilities.max_speed = 33333;                          
	device_capabilities.min_frame_size = 1;
	device_capabilities.min_speed =  1000;
	device_capabilities.uses_callbacks = true;
	device_capabilities.version_major = 1;
	device_capabilities.version_minor = 0;
	device_capabilities.xy_resolution = 12;
    // TODO - put the version number in this string from the CLaserOuptut Class
	_snprintf(device_capabilities.name, 1024, "OLSD %d", device_number);

    // Return successful
    return 1;
}

int OLSD::GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // For now just set the error number to 0
    number = 0;

    // and zero out the error string
    memset(string_pointer, 0, string_length);

    // Return successful
    return 1;
}

int OLSD::Play(int device_number)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // Call the particular devices start function
    m_pLasers[device_number]->Start();

    // Return successful
    return 1;
}

int OLSD::Pause(int device_number)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // Call the particular devices stop function
    m_pLasers[device_number]->Stop();

    // Return successful
    return 1;
}

int OLSD::Shutter(int device_number, int state)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // Turn the shutter ON/OFF for the correct device number
    //m_pLasers[device_number]->Shutter(state == LASER_SHOW_DEVICE_SHUTTER_STATE_ON);

    // Return successfuls
    return 1;
}

int OLSD::WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // Call the PlayFrame function for the correct device number
    // TODO - have a success or failure return from the playframe function
    m_pLasers[device_number]->PlayFrame(&frame, true, false, false);

    // Return successful
    return 1;
}

int OLSD::GetStatus(int device_number, DWORD &status)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

    // TODO - get the status from the laser output class
    // We will want to support the following status messages
    // -> OLSC_STATUS_NONE
    // -> OLSC_STATUS_BUFFER_FULL
    // -> OLSC_STATUS_BUFFER_EMPTY
    // -> OLSC_STATUS_TTL_OUT_COMPLETE
    // -> OLSC_STATUS_ERROR
    // These are bit flags so they can be combined together
    // *** Set these in the Laser Output class????

    // For now return NONE for the status
    return OLSC_STATUS_NONE;
}

int OLSD::SetCallback(int device_number, HWND parent_window_handle, DWORD message)
{
    // Make sure the device number that was passed in is good
    if (!ValidDevice(device_number)) return 0;

	// TODO
    // Set the parent window in the laser output object and callback messsage
    //m_pLasers[device_number]->SetCallback(parent_window_handle, message);

    // Return successful
    return 1;
}

int OLSD::WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
    // Not supported
    return 0;
}

int OLSD::ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
    // Not supported
    return 0;
}

int OLSD::WriteTTL(int device_number, DWORD data)
{
	// Not Supported
	return 0;
}

int OLSD::ReadTTL(int device_number, DWORD &data)
{
    // Not supported
    return 0;
}

int OLSD::ValidDevice(int device_number)
{
    // Check to see that the device number is within bounds
    if (device_number < 0) return 0;
    if (device_number >= MAX_OLSD_DEVICES) return 0;

    // Check to see if the device that the number refers to is OK
    if (!m_pLasers[device_number]) return 0;
    if (!m_pLasers[device_number]->Valid()) return 0;

    // Return success (this is a valid device)
    return 1;
}
