#include ".\OLSD\laser_output.h"

#ifndef __LASEROUTPUTMAMBA_H_
#define __LASEROUTPUTMAMBA_H_

class CLaserOutputMamba : public CLaserOutput
{
private:
	
	// Temporary OLSC Frame
	LASER_SHOW_DEVICE_FRAME m_laser_frame;

	// Last scan speed that someone set
	int m_last_scan_speed;

public:
	CLaserOutputMamba()
	{		
		// Set the last scan speed to the default
		m_last_scan_speed = LASEROUTPUT_DEFAULT_SCAN_SPEED;

		// Allocate memory for our global laser frame
		m_laser_frame.points = new LASER_SHOW_DEVICE_POINT[MAX_FRAME_SIZE];
	}

	~CLaserOutputMamba()
	{
		// Free up our frame point allocation
		delete [] m_laser_frame.points;
		m_laser_frame.points = NULL;
	}

	void SetScanSpeed(WORD speed)
	{
		// Bounds check the speed
		if (speed < 1000) speed = 1000;
		if (speed > 30000) speed = 30000;
		
		// Store this as the last scan speed for the frame
		m_last_scan_speed = speed;

		// Call the laser output scan speed
		CLaserOutput::SetScanSpeed(speed);
	}

	void PlayFrame(struct MAMBA_POINT *pPoints, int count)
	{
		// Make sure the frame pointer is good
		if (!pPoints) return;

		// Make sure there is at least 1 point
		if (count < 1) return;

		// Make sure the OLSC point buffer has been allocated
		if (!m_laser_frame.points) return;

		// Copy the frame that is being passed in (Mamba Format)
		// to our internal OLSC frame format
		int frame_index = 0;
		for (int index = 0; index < count; index++)
		{
			// Get the repeat point count
			for (int repeat = 0; repeat <= pPoints[index].repeatpoint; repeat++)
			{
				// TODO - Why does it appear that we are given 12-bit unsigned data?
				m_laser_frame.points[frame_index].x = pPoints[index].x << 4;
				m_laser_frame.points[frame_index].y = pPoints[index].y << 4;

				// Encode the R,G,B (the blanking is inferred by all 3 being 0)
				// Check to see if we have been blanked
				if (pPoints[index].intensity > 0)
				{
					m_laser_frame.points[frame_index].r = pPoints[index].r;
					m_laser_frame.points[frame_index].g = pPoints[index].g;
					m_laser_frame.points[frame_index].b = pPoints[index].b;
				}
				else
				{
					m_laser_frame.points[frame_index].r = 0;
					m_laser_frame.points[frame_index].g = 0;
					m_laser_frame.points[frame_index].b = 0;
				}

				// Increment the stream point number
				frame_index++;

				// Bounds check the stream point count to make sure we don't exceed the maximum number of points in our stream
				if (frame_index >= MAX_FRAME_SIZE) frame_index = (MAX_FRAME_SIZE - 1);
			}
		}
		
		// Copy our output speed in to our frame
		// This would have been set using the laser output class object
		// functions so just take the new speed from there.
		m_laser_frame.display_speed = m_last_scan_speed;

		// Set the number of points in our frame
		m_laser_frame.point_count = frame_index;

		// Call the actual playframe in our base class
		CLaserOutput::PlayFrame(&m_laser_frame);
	}
};

#endif // __LASEROUTPUTMAMBA_H__
