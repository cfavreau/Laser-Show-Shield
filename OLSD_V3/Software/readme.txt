Open Laser Show DAC
Arduino Laser Shield
By Christopher Favreau
cfavreau@fab-favreau.com
Last revised on October 12, 2011

All included materials are copyright Christopher Favreau 2010, 2011

Open Laser Show DAC by Christopher Favreau is licensed under a 
Creative Commons Attribution 3.0 Unported License.

You are free to share and remix this work as long as you attribute the author and distribute
the resulting work only under the same or similar license to this one.

To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/

Project History:
--------------------------------------------------------------------------------

October 12, 2011

Software
- Added the MLD driver and source code.
- Added a FIFO buffer to all of the CLaserOutput code (except in the Test Application ...
still need to add it there).
- Modified the Streaming protocol to combine the blue color bits with the command byte
in order to save a byte for every command issued.

Hardware
- No changes in the hardware.

August 8, 2011
- Initial public release of all code and hardware.

Included in this project:

Compiled Versions
--------------------------------------------------------------------------------
Compiled versions of the DLL, MLD, Firmware HEX file (along with AVRDUDE and a program.bat file),
and exe files are included in the Compiled folder.

OLSDTest
--------------------------------------------------------------------------------
This program is written in Microsoft Visual C++ using MFC (Visual Studio 2005).
Use this program to test the OLSD hardware.  This software includes several built
in test patterns inlcuding the ILDA12k, ILDA30k, Quadrature, Laser Media, and a
few other test frames that I devised.


OLSD DLL
--------------------------------------------------------------------------------
This is a DLL library that uses the Open Laser Show Controller interface.   It can be used
by other applications to interface with the OLSD hardware in a standard way.   This DLL project
is written in Microsoft Visual C++ (using Visual Studio 2005).

The source code for the DLL test application is included as well (OLSCTestApp)


OLSD MLD
--------------------------------------------------------------------------------
This is a MLD (Mamba Laser Driver ... really a DLL) that uses the Mamba Laser Driver
interface for use with Mamba Black software.  This project is written in Visual C++
(using Visual Studio 2005).

OLSD Firmware
--------------------------------------------------------------------------------
This is the Arduino firmware.  It was tested using Arduino Duemilanove
with ATMega328 (http://www.arduino.cc/en/Main/ArduinoBoardDuemilanove)

It will fit in a smaller amount of flash however you may need to remove some
of the built in test frames.

This firmware supports point output rates up to 30kpps.  It also has
built in test patterns.  These are activated by pressed the button near the ILDA
connector.

