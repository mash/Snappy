# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# PARTNO ....... The AVR device you compile for (for avrdude -p)f
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

CROSSPACK_BASE = /usr/local/CROSSPACK-AVR
CROSSPACK_BIN  = $(CROSSPACK_BASE)/bin
# t13: ATTINY13
PARTNO     = t13
MCU        = attiny13
# 9.6MHz divided by 8
CLOCK      = 1200000
PROGRAMMER = -c avrispmkII -P usb
OBJECTS    = main.o
FUSES      = -U hfuse:w:0xFF:m -U lfuse:w:0x6A:m

# ATMega8 fuse bits used above (fuse bits for other devices are different!):
# Example for 8 MHz internal oscillator
# Fuse high byte:
# 0xFF = 1 1 1 1   1 1 1 1 <-- RSTDISBLE (External Reset disable) (1: unprogrammed)
#        ^ ^ ^ ^   ^ ^ ^------ BODLEVEL0 (Brown-out Detector trigger level) (1: unprogrammed)
#        | | | |   | +-------- BODLEVEL1 (Brown-out Detector trigger level) (1: unprogrammed)
#        | | | |   +---------- DWEN (debugWire Enable) (1: unprogrammed)
#        | | | +-------------- SELFPRGEN (Self Programming Enable) (1: unprogrammed)
#        | | +---------------- - (1: unprogrammed)
#        | +------------------ - (1: unprogrammed)
#        +-------------------- - (1: unprogrammed)
# Fuse low byte:
# 0x6A = 0 1 1 0   1 0 1 0
#        ^ ^ ^ ^   \-/ \-/
#        | | | |    |   +------ CKSEL 1..0 (9.6M internal RC)
#        | | | |    +---------- SUT 1..0 (Select startup time)
#        | | | +--------------- CKDIV8 (Divide clock by 8)
#        | | +----------------- WDTON
#        | +------------------- EESAVE (Preserve EEPROM memory through chip erase) (1: unprogrammed = memory not preserved)
#        +--------------------- SPIEN (Enable Serial Programming and Data Downloading) (if 0: programmed = SPI prog. enabled)
#
# For computing fuse byte values for other devices and options see
# the fuse bit calculator at http://www.engbedded.com/fusecalc/


# Tune the lines below only if you know what you are doing:

AVRDUDE = $(CROSSPACK_BIN)/avrdude $(PROGRAMMER) -p $(PARTNO)
COMPILE = $(CROSSPACK_BIN)/avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(MCU)

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	$(CROSSPACK_BIN)/avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	$(CROSSPACK_BIN)/avr-size --format=avr --mcu=$(MCU) main.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	$(CROSSPACK_BIN)/avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
