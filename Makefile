CC=avr-gcc
MCU=atmega168p
CFLAGS=-Wall -Os -mrelax
BUILDDIR=./bin
UPL=avrdude
UPLFLAGS=-p m168p -b 115200 -c arduino
SERIAL=/dev/ttyUSB0

.DEFAULT_GOAL := build

btle.s.o:
	avr-gcc -mmcu=$(MCU) -c -o $(BUILDDIR)/btle.s.o btle.S

btle.c.o:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/btle.c.o btle.c

spi.o:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/spi.o spi.c

uart.o:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/uart.o uart.c

main.o:
	$(CC) $(CFLAGS) -c -mmcu=$(MCU) -o $(BUILDDIR)/main.o main.c

build: btle.s.o btle.c.o spi.o uart.o main.o
	$(CC) $(CFLAGS) -mmcu=$(MCU) -o $(BUILDDIR)/out.bin $(BUILDDIR)/*.o
	avr-objcopy -j .data -j .text -O ihex $(BUILDDIR)/out.bin $(BUILDDIR)/program.hex
	avr-size $(BUILDDIR)/program.hex

asm: build
	avr-objdump -m avr5 -S -d $(BUILDDIR)/out.bin

clean:
	$(RM) $(BUILDDIR)/*

upload:
	$(UPL) -P $(SERIAL) $(UPLFLAGS) -U flash:w:$(BUILDDIR)/program.hex:i
