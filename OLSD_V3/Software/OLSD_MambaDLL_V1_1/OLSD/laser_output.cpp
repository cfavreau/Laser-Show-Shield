#include "laser_output.h"
#include "trace.h"				// Debugging Trace File

CLaserOutput::CLaserOutput()
{
	 // Clear out the temp frame
   memset(&m_points, 0, sizeof(laser_point) * MAX_FRAME_SIZE);
   m_total_points = 0;

   // Clear the total points written
   m_total_points_written = 0;

   // Reset the blank and center flag
   m_bBlankAndCenter = false;
   
   // Set the old and new scan speeds to be the same
   m_old_scan_speed = LASEROUTPUT_DEFAULT_SCAN_SPEED;
   m_new_scan_speed = LASEROUTPUT_DEFAULT_SCAN_SPEED;

   // Set the Go Flag
   m_bGo = true;

	// Initialize the POINT FIFO
	int i;
	for (i = 0; i < POINT_FIFO_SIZE; i++)
	{
		memset(&m_fifo[i], 0, sizeof(POINT_FIFO));
	}
	m_fifo_end = 0;
	m_fifo_front = 0;

    // Initialize the parent window handle and user message
    m_hParentWnd = NULL;
    m_dwCallbackMessage = WM_USER + 999 + 1;

}

CLaserOutput::~CLaserOutput()
{
}

BOOL CLaserOutput::Init()
{
	// Clear the total points written
	m_total_points_written = 0;

	// Attempt to open the communications port
	if (!CSerial::Init("FT232R USB UART", 2000000, 8 , NOPARITY, ONESTOPBIT, /*EV_RXCHAR |*/ EV_TXEMPTY, true))
	{
		// Return failed
        Trace("CLaserOutput::Init() - CSerial::Init() call failed!\n");
		return FALSE;
	}
    //Trace("Succesfully initialized.\n");

	// Pause a 100 ms to let the device connect
	Sleep(100);

	// Reset the Light Stream device
	// And verify that it is a Light Stream device

	// TODO - The reset procedure sometimes works... sometimes doesn't
	// so don't fail here.
		
	if (!Reset())
	{
		// Close the serial port
	  	//CSerial::Close();
	  
	    Trace("CLaserOutput::Init() - Reset() call failed.\n");
	    
		// TODO - The reset procedure sometimes works... sometimes doesn't
		// so don't fail here.
		//return FALSE;
	}

	// Set the Go flag
	m_bGo = true;

	// Return successful
	return TRUE;
}

void CLaserOutput::SetCallback(HWND hWnd, DWORD dwMessage)
{
    // Set the callback message first so that the callback does not
    // happen without a good message
    // If the window is set first then there is a chance that the
    // The callback will happen without a good message
    m_dwCallbackMessage = dwMessage;

    // Set out parent window's handle
    m_hParentWnd = hWnd;
}

BOOL CLaserOutput::Shutdown()
{
	// Check to see if we are still open
	if (!CSerial::Valid()) return TRUE;

    // Stop the laser output
    Stop();
    
    // Blank and Center
    WriteBlankAndCenter();
    
    // Sleep for about 1/10 th second
    Sleep(100);
    
    // Close the serial port
	CSerial::Close();

    // Return successful
	return TRUE;
}

BOOL CLaserOutput::Reset()
{
    // Flush the buffers
    Purge(true, true);

	// Write the reset packet
	WriteResetPacket();
    
	// Write the reset packet
	WriteResetPacket();

    // Might as well blank and center the beam
    WriteBlankAndCenter();
	
   // And we will also write a scan speed packet too
   // Now set the speed to the default (12 kpps)
   SetScanSpeed(LASEROUTPUT_DEFAULT_SCAN_SPEED);
   WriteScanSpeed(LASEROUTPUT_DEFAULT_SCAN_SPEED);

	// Wait a couple of milliseconds for the reply to get back to us
	Sleep(10);
	
	//sprintf("
	
	// Initialize the POINT FIFO                                               
	int i;
	for (i = 0; i < POINT_FIFO_SIZE; i++)
	{
		m_fifo[i].count = 0;
	}
	m_fifo_end = 0;
	m_fifo_front = 0;

	// Clear the total points in our frame
	m_total_points = 0;

	// Clear the total points written
	m_total_points_written = 0;

	// Return sucessful
	return TRUE;
}

void CLaserOutput::SetPoint(laser_point &lp, unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned short i = 0;

	// Reduce the x and y to a 12 bit number
	x = x >> 4;
	y = y >> 4;
	
	lp.command = 0x20;

	// Now we do 4 bit colors encoded into colors and dummy
	lp.rg = (r & 0xF0) | (g >> 4);
	lp.x = x >> 4;
	lp.y = y >> 4;
	lp.xy = (x & 0x0F) | ((y & 0x0F) << 4);
	lp.command |= ((b & 0xF0) >> 4);
}

void CLaserOutput::Start()
{
	// Set the GO flag
	m_bGo = true;
}

void CLaserOutput::Stop()
{
	// Reset the GO flag
	m_bGo = false;

	// Blank and Center the laser
	BlankAndCenter();
}

void CLaserOutput::BlankAndCenter()
{
	// Set the blank and center flag
	m_bBlankAndCenter = true;
}

void CLaserOutput::WriteBlankAndCenter()
{
   // Make a temporary laser point that blanks the laser(s) I.E. turns the RGB off
   // And sets the points at center (65535,65535)
   laser_point temp;

   SetPoint(temp, (65536 / 2), (65536 / 2), 0, 0, 0);
   
   // Write the packet to the stream
   Write((char *)&temp, sizeof(laser_point));
}

void CLaserOutput::WriteResetPacket()
{
   // Attempt to reset the FrameStream Express Device by sending a couple of reset packets
   laser_point temp;		// Temporary packet for sending commands

   temp.command = 0xFF;
   temp.x = 0xFF;
   temp.y = 0xFF;
   temp.xy = 0xFF;
   temp.rg = 0xFF;
   
   int wrote = Write((char *)&temp, sizeof(laser_point));

   Sleep(10);
}

void CLaserOutput::SetScanSpeed(WORD speed)
{
	// Set the new scan speed
	m_new_scan_speed = speed;
}

void CLaserOutput::WriteScanSpeed(int speed)
{
	Trace("CLaserOutput::WriteScanSpeed(%d pps)\n", speed);

	// Set point output rate at 12kpps
	laser_point temp;
	temp.command = 0x10;
	temp.x = speed >> 8;
	temp.y = speed & 0x00FF;
	temp.xy = 0;
	temp.rg = 0;
	if (sizeof(laser_point) != Write((char *)&temp, sizeof(laser_point)))
	{
		Trace("CLaserOutput::WriteScanSpeed() FAILED!!!!\n");
		// Try this again later
		m_old_scan_speed = 0;
		return;
	}

	// Set the old scan speed
	m_old_scan_speed = speed;
}

#define MAX_POINTS (4095 / sizeof(laser_point))

void CLaserOutput::WriteStream()
{
	// Check to see if we should blank and center the beam
	if (m_bBlankAndCenter)
	{
		WriteBlankAndCenter();
		m_bBlankAndCenter = false;
	}
	
	// Check to see if we have a new scan speed to set
	if (m_new_scan_speed != m_old_scan_speed)
	{
		Trace("CLaserOutput::WriteStream() -> Writing new_scan_speed(%d)\n", m_new_scan_speed);
		// If the scan speed is positive then it just means
		// a new scan speed.
		// Send the new scan speed to the FrameStream device
		WriteScanSpeed(m_new_scan_speed);
	}

	// Check to see if we should update our output rate

	// Check the GO flag to see if we should enable the output
	if (!m_bGo) return;

	// Figure out how many points to write
	int points = m_total_points - m_total_points_written;

	// Restrict the number of points written ... to less than 1365 which is less than 4kB worth of data.   4096/3 bytes per packet = 1365.
	if (points > MAX_POINTS) points = MAX_POINTS;

	// Write the a chunk of points
	int wrote = Write((char *)&m_points[m_total_points_written], sizeof(laser_point) * points);

	// Check to see if the actual number of points written is the same as the number of points we wanted to write
	if (points != (int)(wrote / sizeof(laser_point)))
	{
		// Inform the debugger
		//TRACE("WriteStream: Failed to write %d (%d points) bytes.  Actually wrote %d bytes.\n", points * sizeof(laser_point), points, wrote);
		// DEBUG
		//Trace("WriteStream: Failed to write %d (%d points) bytes.  Actually wrote %d bytes.\n", points * sizeof(laser_point), points, wrote);

		// In this case we should check for some ? returned from the device
		//char sTemp[32];
		//int length = Read(sTemp, 32);
		//if (strchr(sTemp, '?') != NULL)
		//{
		//	// We are out of sync with the host ... reset the device
		//	WriteResetPacket();
		//}
	}
	else
	{
		// DEBUG
		//Trace("Wrote %d bytes.\n", wrote);
	}

	// Increment the number of points written
	m_total_points_written += points;
}

void CLaserOutput::PlayFrame(struct LASER_SHOW_DEVICE_FRAME *pFrame, bool invertX, bool invertY, bool swapXY)
{
	LASER_SHOW_DEVICE_POINT point;

	// Make sure the frame pointer is good
	if (!pFrame) return;

	// Get the number of points in this frame
	int points = pFrame->point_count;

	// TODO - do we really need to stop the output?

	// Temporarily stop the output
	//bool go = m_bGo;
	//m_bGo = false;

   // Calculate our actual raw point rate
   double actual_rate = (2000000.0 / 10.0) / (double)sizeof(struct laser_point);

   // Calculate the packets per point number based on the desired point output rate
   // and the actual rate of the point output
   //double packets_per_point = actual_rate / (double)pFrame->display_speed;
   //double points_per_packet = 1.0 / packets_per_point;
   //6points_per_packet = 1.0;//0.5;

   // Make sure we don't exceed our total number of points in our temporary frame
   if (points > MAX_FRAME_SIZE) points = MAX_FRAME_SIZE;

	// Increment the front index
	if ((m_fifo[m_fifo_front + 1].count < 1) && (m_fifo_front != m_fifo_end)) m_fifo_front++;

    // Get a pointer to the front of the FIFO
	POINT_FIFO *pFIFO = &m_fifo[m_fifo_front];

    // Store the scan speed
    pFIFO->display_speed = pFrame->display_speed;

	// Copy the frame that is being passed in to our Streaming format
	int count = 0;
	double point_count = 0.0;
	for (int index = 0; index < points; index++)
	{
		// Get a point at the index
		point = pFrame->points[index];

		if (invertX)
		  point.x = 65535 - point.x;
		
		// For some reason we need to invert the Y axis by default
		if (!invertY)
		  point.y = 65535 - point.y;
		
		if (swapXY)
		{
		  register unsigned int temp = point.x;
		  point.x = point.y;
		  point.y = temp;
		}

		SetPoint(pFIFO->points[count], point.x, point.y, (unsigned char)point.r, (unsigned char)point.g, (unsigned char)point.b);
		count++;

		/*`
	  int before_point_count = (int)point_count;
	  while (before_point_count == (int)point_count)
	  {
		  SetPoint(pFIFO->points[count], point.x, point.y, (unsigned char)point.r, (unsigned char)point.g, (unsigned char)point.b);
		  count++;
		point_count += points_per_packet;
		if (count >= MAX_FRAME_SIZE) count = MAX_FRAME_SIZE - 1;
	  }
	  */
	  
	  // TODO - do all the invert stuff later - add this to the SetPoint() function
	  // TODO - based on the points per second output rate... resample
	  // the incoming points since we have a fixed point output rate.

/*
		// Get a point at the index
        point = pFrame->points[index];

        // Transfer the data to our stream
        pFIFO->points[index].control = 0x20;
      

      // Check to see if we need to invert or swap
      if (invertX) point.x = 65535 - point.x;
      if (invertY) point.y = 65535 - point.y;
      if (swapXY)
      {
         unsigned short int temp = point.x;
         point.x = point.y;
         point.y = temp;
      }

      // Set the points in the stream
      pFIFO->points[index].x = point.x;
      pFIFO->points[index].y = point.y;
	  
	  // Set the RGB to their respective values
      if (point.i > 0)
      {
        pFIFO->points[index].r = (unsigned char)point.r;
        pFIFO->points[index].g = (unsigned char)point.g;
        pFIFO->points[index].b = (unsigned char)point.b;
      }
      else
      {
        pFIFO->points[index].r = 0;
        pFIFO->points[index].g = 0;
        pFIFO->points[index].b = 0;
      }
*/
	}
	
	// DEBUG
	//Trace("PlayFrame: Playing %d points.\n", points);

	// It is important that we fill in this last so that the OnWrite callback will
	// display the data when it is called.
	// Set the number of points in our stream
	// We need to put exactly how many points we have in our stream...
	// This will probably be a lot more than the frame has since we are resampling
	// to get our desired point output rate.
 	pFIFO->count = count;

	// Start the output back up if we have to
	//m_bGo = go;

	// Do not write to the serial port right here.  It has the potential for slowing down the main application thread
	// and making the UI unresponsive.  The OnWrite will actually initiate the stream
}

void CLaserOutput::OnWrite()
{
	// We have some points in the frame so lets see how many points we have written to the stream
	if (m_total_points_written < m_total_points)
	{
		// We have some points left so write them to the stream
		WriteStream();
	}
	else
	{
		// We are done sending this frame
		// Send the parent a message that says we need a new frame
		if (m_hParentWnd)
		{
			// Check to see if we have a frame to send or if it is just
			// was a command we were sending.
			if (m_total_points > 0)
			{
				// DEBUG
				// Calculate and write the PPS
				//DWORD dwTime = (timeGetTime() - stamp);
				//double dTime = (double)dwTime / 1000.0;
				//dTime = (double)temp_frame.nPoints / dTime;
				//TRACE1("PPS %0.2f\n", dTime);
				// Write the time difference between last time and this time
				//TRACE1("TX Buffer Empty in %d ms\n", dwTime);
				//stamp = timeGetTime();
				// Send a message to the parent stating we should write again
				// Use PostMessage so this happens asynchronously!
                //PostMessage(m_hParentWnd, m_dwCallbackMessage, 0, 0);
			}
		}

		// Get the next frame from the FIFO
		if ((m_fifo[(m_fifo_end + 1) % POINT_FIFO_SIZE].count > 1) && (m_fifo_end != m_fifo_front)) m_fifo_end++;

		POINT_FIFO *pPoints = &m_fifo[m_fifo_end % POINT_FIFO_SIZE];
        
        // Now write the frame
    	if (pPoints->count > 0)
		{
			// Set the new scan rate
			SetScanSpeed(pPoints->display_speed);

			// Copy the frame from the FIFO into the internal frame
			memcpy(m_points, pPoints->points, MAX_FRAME_SIZE * sizeof(laser_point));
			
			// Clear the total points written
			m_total_points_written = 0;

			// Set the number of points in our internal frame
			m_total_points = pPoints->count;

			// Zero out the FIFO's count to signal that this element is empty
			pPoints->count = 0;
		}
		else
		{
			// Keep on displaying this frame
			// Reset the total points written counter
			m_total_points_written = 0;

			// Call the WriteStream function
			WriteStream();
		}

		// Check to see if there are any points in the frame
		if (m_total_points < 1) return;
	}
}

void CLaserOutput::OnRead()
{
	// TODO - THIS CAUSES THE PROGRAM TO CRASH WHEN CLOSING THE APPLICATION IF THERE IS ANYTHING BEING READ!!!!
	// DEBUG
	//char sTemp[64];
	//memset(sTemp, 0, 64);
	//Read(sTemp, 60);
	//TRACE1("%s\n", sTemp);
}
