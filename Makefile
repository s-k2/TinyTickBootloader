###############################################################################
# Makefile for the project TinyBootloader
###############################################################################

## General Flags
PROJECT = TinyBootloader
MCU = attiny461a
TARGET = bin/TinyBootloader.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT bin/$(*F).o -MF bin/dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,--section-start=.text=0xd00 -Wl,--section-start=.rjmp_loader_seg=0x0000  -Wl,--section-start=.leave_bootloader_seg=0x0cfe -Wl,-Map=bin/TinyBootloader.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = bin/main.o bin/protocol.o bin/writeASM.o bin/writeC.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) bin/TinyBootloader.hex bin/TinyBootloader.eep bin/TinyBootloader.lss

bin/main.o: src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

bin/protocol.o: src/protocol.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

bin/writeASM.o: src/writeASM.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c -o $@ $<

bin/writeC.o: src/writeC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

bin/%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

bin/%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

bin/%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) bin/TinyBootloader.elf bin/dep/* bin/TinyBootloader.hex bin/TinyBootloader.eep bin/TinyBootloader.lss bin/TinyBootloader.map


## Other dependencies
-include $(shell mkdir bin/dep 2>NUL) $(wildcard bin/dep/*)

