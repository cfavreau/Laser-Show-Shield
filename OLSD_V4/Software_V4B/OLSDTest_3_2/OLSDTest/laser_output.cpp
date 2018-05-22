#include "stdafx.h"
#include "laser_output.h"
#include "trace.h"			// DEBUG TRACE -> Generates "trace.txt" file

#define LASEROUTPUT_DEFAULT_SCAN_SPEED	12000

// DEBUG
int mid_point = 0;

CLaserOutput::CLaserOutput()
{
	// Clear out the name
	memset(m_szName, 0, 256);

	// Allocate some memory for our point stream
	m_point_stream = new BYTE[sizeof(laser_point) * MAX_FRAME_SIZE];
   // Clear out the temp frame
   memset(m_point_stream, 0, sizeof(laser_point) * MAX_FRAME_SIZE);
   m_total_bytes = 0;
   m_total_points = 0;

   // Clear the total bytes written
   m_total_bytes_written = 0;

   	// Set the old and new scan speeds to be the same
	m_old_scan_speed = LASEROUTPUT_DEFAULT_SCAN_SPEED;
	m_new_scan_speed = LASEROUTPUT_DEFAULT_SCAN_SPEED;

   // Set the Go Flag
   m_bGo = true;
 
   EnableTrace(false);
   Trace("CLaserOutput::CLaserOutput()\n");
}

CLaserOutput::~CLaserOutput()
{
	// Free up our byte stream memory
	if (m_point_stream)
	{
		delete [] m_point_stream;
		m_point_stream = NULL;
	}

   Trace("CLaserOutput::~CLaserOutput()\n");
}

BOOL CLaserOutput::Init()
{
   Trace("CLaserOutput::Init()\n");

   // Clear the byte counts
   m_total_bytes = 0;
   m_total_points = 0;
   m_total_bytes_written = 0;

   // Attempt to open the communications port
   if (!CFTSerial::Init("FT232R USB UART", 2000000, 8 , NOPARITY, ONESTOPBIT, /*EV_RXCHAR |*/ EV_TXEMPTY, true))
   {
     Trace("CLaserOutput::CLaserOutput() - Failed to initialize the device.\n");
      return FALSE;
   }

   // Set the name
   _snprintf_s(m_szName, 256, 256, "OLSD");

   // Pause a 100 ms to let the device connect
   Sleep(100);

   // Reset the device
   Reset();

   // Set the Go flag
   m_bGo = true;

   // DEBUG
   Trace("CLaserOutput::CLaserOutput() - Successfully initialized the device.\n");

   // Return successful
   return TRUE;
}

char *CLaserOutput::GetName()
{
	return m_szName;
}

BOOL CLaserOutput::Shutdown()
{
   Trace("CLaserOutput::Shutdown()\n");

   CFTSerial::Close();

   return TRUE;
}

BOOL CLaserOutput::IsInitialized()
{
	// Return our connected and initialized status
	// TODO

	// For now just indicate if the thread is moving
	return m_bGo;
}

BOOL CLaserOutput::Reset()
{
   Trace("CLaserOutput::Reset()\n");

   // Write the reset packet
   WriteResetPacket();

   // And we will also write a scan speed packet too
   // Now set the speed to the default (12 kpps)
   SetScanSpeed(LASEROUTPUT_DEFAULT_SCAN_SPEED);
   WriteScanSpeed(LASEROUTPUT_DEFAULT_SCAN_SPEED);

   // TODO - fill in the m_szName member variable

   // Return sucessful
   return TRUE;
}

void CLaserOutput::Start()
{
   Trace("CLaserOutput::Start()\n");
   
   // Set the GO flag
   m_bGo = true;
}

void CLaserOutput::Stop()
{
	Trace("CLaserOutput::Stop()\n");
	
	// Reset the GO flag
	m_bGo = false;

	// Blank and Center the laser
	// The hardware should blank for us when it does not
	// receive anything, however it is just good practice
	// to do it ourselves.
	BlankAndCenter();
}

void CLaserOutput::BlankAndCenter()
{
	Trace("CLaserOutput::BlankAndCenter()\n");

	// Set the blank and center flag
	m_bBlankAndCenter = true;
}

int CLaserOutput::SetSpeed(laser_point &lp, int speed)
{
	lp.command = 0x10;
	lp.x = speed >> 8;
	lp.y = speed & 0x00FF;
	lp.xy = 0;
	lp.rg = 0;
	//lp.bi = 0;

	return sizeof(laser_point);
}

int CLaserOutput::SetPoint(laser_point &lp, unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b, bool first_point /* = true */)
{
	unsigned char i = 0;

	// Reduce the x and y to a 12 bit number
	x = x >> 4;
	y = y >> 4;
	
	// Output point command
	lp.command = 0x20;

	// Encode the X and Y values
	lp.x = x >> 4;
	lp.y = y >> 4;
	lp.xy = (x & 0x0F) | ((y & 0x0F) << 4);

	// Code for mono-color
	/*
	// Set all the colors to the max of all
	int cmax = 0;
	if (r > cmax) cmax = r;
	if (g > cmax) cmax = g;
	if (b > cmax) cmax = b;
	if (i > cmax) cmax = i;
	g = b = i = r = (unsigned char)cmax;
	//*/

	// Encode the 4 bit color
	lp.rg = (r & 0xF0) | (g >> 4);
	//lp.bi = (b & 0xF0) | (i >> 4);
	lp.command |= (b >> 4);
	
	return sizeof(laser_point);
}

void CLaserOutput::ClearLastPoint()
{
	/*
	m_last_point.command = 0;
	m_last_point.x = 0;
	m_last_point.y = 0;
	m_last_point.xy = 0;
	m_last_point.rg = 0;
	//m_last_point.bi = 0;
	*/
}

void CLaserOutput::WriteBlankAndCenter()
{
   // Make a temporary laser point that blanks the laser(s) I.E. turns the RGB off
   // And sets the points at center (65535,65535)
   laser_point temp;

   // The command is output point
   // Since the lower nibble is 0 then all of the beams should be OFF
   // Clear the last point
   ClearLastPoint();
   int size = SetPoint(temp, (65536 / 2), (65536 / 2), 0, 0, 0);
   
   // Write the packet to the stream
   int wrote = Write((char *)&temp, size);

   Trace("CLaserOutput::WriteBlankAndCenter() - wrote(%d).\n", wrote);
}

void CLaserOutput::SetScanSpeed(WORD speed)
{
	// Set the new scan speed
	m_new_scan_speed = speed;
}

void CLaserOutput::WriteScanSpeed(int speed)
{
	TRACE("CLaserOutput::WriteScanSpeed(%d pps)\n", speed);

	// Set point output rate at 12kpps
	laser_point temp;
	temp.command = 0x10;
	temp.x = speed >> 8;
	temp.y = speed & 0x00FF;
	temp.xy = 0;
	temp.rg = 0;
	//temp.bi = 0;
	if (sizeof(laser_point) != Write((char *)&temp, sizeof(laser_point)))
	{
		TRACE("CLaserOutput::WriteScanSpeed() FAILED!!!!\n");
		// Try this again later
		m_old_scan_speed = 0;
		return;
	}

	// Set the old scan speed
	m_old_scan_speed = speed;
}

void CLaserOutput::WriteResetPacket()
{
	// Synchronize the stream first
	// Send 2 packets of 0xFF in all bytes
	// Then send a Reset command

	laser_point temp;		// Temporary packet for sending commands

	// Send a packet full of sync commands
	temp.command = 0xFF;
	temp.x = 0xFF;
	temp.y = 0xFF;
	// DEBUG - disabled for testing
	temp.xy = 0xFF;
	temp.rg = 0xFF;
	//temp.bi = 0xFF;

	// Send 2 SYNC packets
	Write((char *)&temp, sizeof(laser_point));
	Write((char *)&temp, sizeof(laser_point));

	// Send a RESET packet
	// TODO

	// Wait for it to reset
	Sleep(10);

	Trace("CLaserOutput::WriteResetPacket()\n");
}

// TODO - look in the FTDI docs and find the best size
#define MAX_BYTES 4095

void CLaserOutput::WriteStream()
{
	// Do this stuff ONLY on point boundaries
	if (m_total_bytes_written == 0)
	{
	   // Check to see if we should blank and center the beam
   if (m_bBlankAndCenter)
   {
      WriteBlankAndCenter();
      m_bBlankAndCenter = false;
   }

   // DEBUG
   ///*
   	// Check to see if we have a new scan speed to set
	if (m_new_scan_speed != m_old_scan_speed)
	{
		// Check to see if the scan speed is + or -
		if (m_new_scan_speed > 0)
		{
			// If the scan speed is positive then it just means
			// a new scan speed.
			// Send the new scan speed to the FrameStream device
			WriteScanSpeed(m_new_scan_speed);
		}
		else
		{
			// If the scan speed is negative then it is a frame rate instead
			// Set the new scan speed to the actual points in the frame times the frames per second
			WriteScanSpeed((m_total_points / sizeof(laser_point)) * abs(m_new_scan_speed));
		}
	}
	}
	//*/

   // Check the GO flag to see if we should enable the output
   if (!m_bGo) return;

   // Figure out how many bytes to write
   int bytes_to_write = m_total_bytes - m_total_bytes_written;

   // Restrict the number of bytes written ... to less than 4kB worth of data.
   //if (bytes_to_write > MAX_BYTES) bytes_to_write = MAX_BYTES;
   // DEBUG
   if (m_total_bytes_written < 1) bytes_to_write = mid_point;

   // Write the a chunk of points
   int wrote = Write((char *)&m_point_stream[m_total_bytes_written], bytes_to_write);

   // DEBUG
   TRACE("CLaserOutput::WriteStream() = wrote (%d) bytes.\n", wrote);

   // Check to see if the actual number of bytes written is the same as the number of bytes we wanted to write
   if (wrote != bytes_to_write)
   {
      // Inform the debugger
      TRACE("WriteStream: Failed to write %d bytes.  Actually wrote %d bytes.\n", bytes_to_write, wrote);
      Trace("CLaserOutput::WriteStream() - Failed to write %d bytes.  Actually wrote %d bytes.\n", bytes_to_write, wrote);
   }

   // Increment the number of bytes written
   m_total_bytes_written += wrote;

   // DEBUG
   Trace("CLaserOuput::WriteStream() - m_total_bytes_written(%d) bytes_to_write(%d)\n", m_total_bytes_written, bytes_to_write);
}

void CLaserOutput::PlayFrame(struct LASER_SHOW_DEVICE_FRAME *pFrame, bool invertX, bool invertY, bool swapXY)
{
   LASER_SHOW_DEVICE_POINT point;

   // Make sure the frame pointer is good
   if (!pFrame) return;

   // Get the number of points in this frame
   int points = pFrame->point_count;

   // Temporarily stop the output
   bool go = m_bGo;
   m_bGo = false;
   
   // Make sure we don't exceed our total number of points in our temporary frame
   if (points > MAX_FRAME_SIZE) points = MAX_FRAME_SIZE;

   // Set the number of bytes in our temporary frame to 0 so that we don't begin writeing the frame
   m_total_bytes = 0;
   m_total_points = 0;

   // Initialize our stream byte count
   int byte_count = 0;
   int size = 0;
   // Initialize our stream pointer
   BYTE *pStream = m_point_stream;
   
   // Set the display speed for this frame
   // TODO - why does this NOT work???
   //size = SetSpeed(*((laser_point *)pStream), pFrame->display_speed);
   //byte_count += size;
   //pStream += size;

   // Copy the frame that is being passed in to our Streaming format
   int count;
   for (count = 0; count < points; count++)
   {
      // Get a point at the index
      point = pFrame->points[count];

	  if (invertX)
		  point.x = 65535 - point.x;
	  // Always invert Y as it is inverted in Hardware or maybe
	  // the IDLA files have an inverted Y
	  if (!invertY)
		  point.y = 65535 - point.y;
	  if (swapXY)
	  {
		  register unsigned int temp = point.x;
		  point.x = point.y;
		  point.y = temp;
	  }

	  // Just copy over the point into our buffer
	  // We are writing a variable length packet byte stream
	  // The SetPoint() function will return the number of bytes we need
	  size = SetPoint(*((laser_point *)pStream), point.x, point.y, (unsigned char)point.r, (unsigned char)point.g, (unsigned char)point.b, (count == 0));
	  pStream += size;
	  byte_count += size;
	  if (byte_count < 4095) mid_point = byte_count;

	  // TODO - do all the invert stuff later - add this to the SetPoint() function
   }
   
   // Set our point count
   m_total_points = count;

   // Zero out the total bytes written
   // Zero this out before setting the number of bytes in the frame
   m_total_bytes_written = 0;
   
   // It is important that we fill in this last so that the OnWrite callback will
   // display the data when it is called.
   // Set the number of points in our stream
   m_total_bytes = byte_count;

   // Set the scan speed
   //SetScanSpeed(pFrame->display_speed);

   // Start the output back up if we have to
   m_bGo = go;

   // DEBUG
   TRACE("CLaserOutput::PlayFrame() - this frame has %d bytes.\n", m_total_bytes);
   Trace("CLaserOutput::PlayFrame() - this frame has %d bytes.\n", m_total_bytes);

   // Do not write to the serial port right here.  It has the potential for slowing down the main application thread
   // and making the UI unresponsive.  The OnWrite will actually initiate the stream
   //WriteStream();
}

// DEBUG
DWORD stamp = 0;

int count = 0;

void CLaserOutput::OnWrite()
{
   // Check to see if there are any bytes in the frame
   if (m_total_bytes < 1) return;

   // We have some bytes in the frame so lets see how many bytes we have written to the stream
   if (m_total_bytes_written < m_total_bytes)
   {
      Trace("CLaserOutput::OnWrite() - m_total_bytes_written < m_total_bytes.\n");

      // We have some points left so write them to the stream
      WriteStream();
   }
   else
   {
	   // DEBUG - output a frame count
	   //TRACE("CLaserOutput::OnWrite() - count(%d)\n", count++);
	   // DEBUG
        // Calculate and write the PPS
        DWORD dwTime = (timeGetTime() - stamp);
        double dTime = (double)dwTime / 1000.0;
        // DEBUG
		dTime = (double)m_total_points / dTime;
        TRACE("Points(%d) PPS %0.2f\n", m_total_points, dTime);
        // Write the time difference between last time and this time
        TRACE1("TX Buffer Empty in %d ms\n", dwTime);
        stamp = timeGetTime();

	   // Reset the number of bytes written
	   m_total_bytes_written = 0;
	   // This will in effect cause us to redisplay the frame since we do not have a new one
	   WriteStream();

	   // TODO - put the windows message back into this... it was useful
	   /*
      // We are sdone sending this frame
      // Send the parent a message that says we need a new frame
      if (m_pParentWnd)
      {
         // Check to see if we have a frame to send or if it is just
         // was a command we were sending.
         if (m_total_points > 0)
         {
            // DEBUG
            if (g_bDebugTrace)
               Trace("CLaserOutput::OnWrite() - Sending WM_LASEROUTPUT_NEXTFRAME message.\n");
            // DEBUG
            // Calculate and write the PPS
            DWORD dwTime = (timeGetTime() - stamp);
            double dTime = (double)dwTime / 1000.0;
            dTime = (double)m_total_points / dTime;
            TRACE("Points(%d) PPS %0.2f\n", m_total_points, dTime);
            // Write the time difference between last time and this time
            TRACE1("TX Buffer Empty in %d ms\n", dwTime);
            stamp = timeGetTime();
            // Send a message to the parent stating we should write again
            //m_pParentWnd->PostMessage(WM_USER_MESSAGE, UM_LASEROUTPUT_NEXTFRAME);
			m_pParentWnd->SendMessage(WM_USER_MESSAGE, UM_LASEROUTPUT_NEXTFRAME);
				// TODO
				// Call the callback

         }
      }
	  */
   }
}

void CLaserOutput::OnRead()
{
   // TODO - THIS CAUSES THE PROGRAM TO CRASH WHEN CLOSING THE APPLICATION IF THERE IS ANYTHING BEING READ!!!!
   // DEBUG
/*
   char sTemp[64];
   memset(sTemp, 0, 64);
   Read(sTemp, 60);
   TRACE1("%s\n", sTemp);
   //Trace("CLaserOutput::OnRead() " + csTemp + "\n");
*/
}
