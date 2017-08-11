Open Laser Show DAC
Arduino Laser Shield
By Christopher Favreau
cfavreau@fab-favreau.com
Last revised on August 17, 2011
Current Hardware Version: 3B
Project Web Sites:
http://www.fab-favreau.com/index.php/Main/OpenLaserShowDAC
https://github.com/cfavreau/Laser-Show-Shield

All included materials are copyright Christopher Favreau 2010 to 2017

This work is licensed under the Creative Commons Attribution 3.0 Unported License.

You are free to share and remix this work as long as you attribute the me the author
somewhere on/in your related work.

To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/

Description
--------------------------------------------------------------------------------

The Open Laser Show DAC is an Open Source Hardware/Software project.

It implements a Laser Show DAC (Digital to Analog Converter) that uses the ILDA standard laser projector
connector. The one exception that it uses singled ended color signals.

Specifications->
Scan Rates: 1 kpps to 30 kpps
XY Resolution:  12 Bits
Color Resolution:  3 bits (for a total of 512 colors)
Colors: Red, Green, Blue outputs
TTL Shutter Output
Built in test patterns accessible by button.

This board is built as an Arduino Shield.   It requires a Arduino 2009 (Duemilanove) or compatible Arduino
micro-controller board.   The important thing that is needed for compatibility is the FTDI USB to Serial
converter chip.  The newer UNO boards are not supported yet.   In order for the software/hardware to work
properly a jumper wire needs to be added between the CTS pin on the Arduino/FTDI chip and digital pin 8.  
This is used to control the stream of serial data being sent to the Arduino.  Some boards make this easy and
include a pad for the CTS pin (Arduino and Seeduino brands).   The Freeduino will work however it does not
break out a pad for the CTS pin (this makes it a bit more difficult to modify).

------------------------------------------------------------------------------------------------------------

Included in this project:

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

The source code for the DLL test application is included as well.


OLSD Firmware
--------------------------------------------------------------------------------
This is the Arduino firmware.  It was tested using Arduino Duemilanove
with ATMega328 (http://www.arduino.cc/en/Main/ArduinoBoardDuemilanove)

It will fit in a smaller amount of flash however you may need to remove some
of the built in test frames.

This firmware supports point output rates up to 30kpps.  It also has
built in test patterns.  These are activated by pressed the button near the ILDA
connector.


OLSD Laser Shield Hardware
--------------------------------------------------------------------------------
Schematic, Board Layout, Gerbers, and Bill Of Materials (with Digi-Key stock numbers).
The schematic and board were done in the free version of Eagle CAD.  A PDF version
of the schematic has been included.  All parts except for PCB and Arduino can be
purchased from Digi-Key using the included Digi-Key part numbers.

