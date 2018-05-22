GCCFLAGS=-g -Os -Wall -mmcu=atmega328p -DF_CPU=16000000
LINKFLAGS=-Wl,-u,vfprintf -lprintf_flt -Wl,-u,vfscanf -lscanf_flt -lm
AVRDUDEFLAGS=-V -F -C avrdude.conf -p m328p -c stk500v1 -P COM6 -b 57600
LINKOBJECTS=

all:    program

rebuild:	clean olsd.hex

olsd.hex:  OLSD.c spi.c timer.c usart.c switch.c
	avr-gcc ${GCCFLAGS} ${LINKFLAGS} -o OLSD.elf OLSD.c spi.c timer.c usart.c switch.c ${LINKOBJECTS}
	avr-objcopy -j .text -O ihex OLSD.elf OLSD.hex
	avr-size -C OLSD.elf

program:   olsd.hex
	avrdude ${AVRDUDEFLAGS} -U flash:w:olsd.hex:a

clean:
	rm -f *.o *.hex *.elf