CC=avr-gcc
MCU=atmega168p
CFLAGS=-Wall -Os -mrelax
BUILDDIR=./bin
UPL=avrdude
UPLFLAGS=-p m168p -b 115200 -c arduino
SERIAL=/dev/ttyUSB0

.DEFAULT_GOAL := build


# dependency modules

btle:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/btle.c.o btle.c
	avr-gcc -mmcu=$(MCU) -c -o $(BUILDDIR)/btle.s.o btle.S

spi:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/spi.o spi.c

uart:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/uart.o uart.c

time:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/time.o time.c

programs:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/tx_single.o samples/tx_single.c
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/rx_single.o samples/rx_single.c
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/rx_multi.o  samples/rx_multi.c


# targets

build: btle spi uart time programs
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/main.o main.c
	$(CC) $(CFLAGS) -mmcu=$(MCU) -o $(BUILDDIR)/out.bin $(BUILDDIR)/*.o
	avr-objcopy -j .data -j .text -O ihex $(BUILDDIR)/out.bin $(BUILDDIR)/program.hex
	avr-size $(BUILDDIR)/program.hex

asm: build
	avr-objdump -m avr5 -S -d $(BUILDDIR)/out.bin

clean:
	$(RM) $(BUILDDIR)/*

upload:
	$(UPL) -P $(SERIAL) $(UPLFLAGS) -U flash:w:$(BUILDDIR)/program.hex:i
