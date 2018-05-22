/*

Open Laser Show Controller Interface Object

Copyright (c) 2009 Dennis Kromhout, Chris Favreau, Andreas Unger

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#define _WIN32_WINNT 0x0400
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "olsc_interface.h"
#include "ilda.h"

int main(int argc, char *argv[])
{
    // Create an instance of the OSLC interface
    COpenLaserShowControllerInterface test;
    
    // Create a string with the interfaces' file name in it
    char filename[MAX_PATH];
	if (argc < 2)
		sprintf(filename, "OLSC.dll");
	else
		sprintf(filename, argv[1]);

    // Load the interface with the DLL name we just specified
    if (!test.LoadInterface(filename, true))
	{
		printf("Failed to Load the Interface.\n");
		// Quit while we are ahead
		return -1;
	}
	
	// Success!!!
	printf("Successfully Loaded the Interface.\n");
		
	// Query the name of the interface
	char sInterfaceName[MAX_PATH];
	test.GetInterfaceName(sInterfaceName);
	printf("Using Interface %s.\n", sInterfaceName);

	// Initialze the OLSC interface
    if (test.Initialize())
        printf("Successfully Initialized the OLSC Interface.\n");
    else
        printf("Failed to Initialize the OLSC Interface.\n");
    
    // Create an OLSC frame
	LASER_SHOW_DEVICE_FRAME frame;
	// With the number of points equalling the number of test points in our test frame
	frame.points = new LASER_SHOW_DEVICE_POINT[test_frame_size];

	// Copy the ILDA test frame into an OLSC frame
	int i;
	for (i = 0; i < test_frame_size; i++)
	{
		// TODO - for some reason we have to invert the X axis (is this from NLS?)
		// TODO - check the validity of the axis values for X and Y
		frame.points[i].x = 32768 + (test_frame[i][0]);
		frame.points[i].y = 32768 + (test_frame[i][1]);
		//printf (" Pixels : %d %d\n",  frame.points[i].x, frame.points[i].y);
		frame.points[i].r = test_frame[i][2];
		frame.points[i].g = test_frame[i][3];
		frame.points[i].b = test_frame[i][4];
		frame.points[i].i = 255;
	}

	frame.point_count = test_frame_size;
	frame.display_speed = 16000;
    
	// Write the 1st frame
	if (test.WriteFrame(0, frame))
		printf("Displaying ILDA test frame on device 0.\n");
	else
		printf("Failed to display ILDA test frame on device 0.\n");

    // Tell the user that they can press any key to exit this demo
    printf("Please press any key to quit!\n");
    
    // Loop until the user presses a key (in our console window ... not the vector window!)
    while (!_kbhit())
    {
    	//DWORD status = 0;
		//test.GetStatus(0, status);
        // Give up some time to the rest of the computer
        Sleep(50);
    }
    
    // Tell the user we are quitting
    printf("Thanks for using OLSC!\n");
    
    // Shutdown the OLSC interface
    test.Shutdown();
    
    // Return successful
    return EXIT_SUCCESS;
}
