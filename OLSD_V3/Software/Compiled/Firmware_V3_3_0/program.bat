@echo *** REMEMBER TO CHANGE THE COM PORT TO YOUR COM PORT NUMBER OF YOUR ARDUINO!!!!
avrdude.exe -V -F -C avrdude.conf -p m328p -c stk500v1 -P COM6 -b 57600 -U flash:w:olsd.hex:a
@echo *** SHOULD BE DONE ... ENJOY!
