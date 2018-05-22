Please edit the PROGRAM.BAT file to use the COM# that your Arduino is on.

To find out please go to the Ports in the Device Manager.

avrdude.exe -V -F -C avrdude.conf -p m328p -c stk500v1 -P COM6 -b 57600 -U flash:w:olsd.hex:a

The above line is set for COM6.   Your COM# may vary.  Replace the "6" with your own number.
Then run the PROGRAM.BAT file by double clicking on it.   This will program the Arduino with 
the new firmware.

