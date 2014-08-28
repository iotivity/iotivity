PLATFORM := arduinomega

ROOT_DIR = ../../..
include $(ROOT_DIR)/local.properties
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
BIN_DIR = $(OCSOCK_DIR)/bin/arduino/
OBJ_DIR = $(OCSOCK_DIR)/obj/arduino/

#Source directories
SDIR_ARD_CORE = $(ARDUINO_DIR)/hardware/arduino/cores/arduino
SDIR_ARD_SPI = $(ARDUINO_DIR)/libraries/SPI
SDIR_ARD_ETH = $(ARDUINO_DIR)/libraries/Ethernet
SDIR_ARD_ETH_UTIL = $(ARDUINO_DIR)/libraries/Ethernet/utility

VPATH = $(SDIR_ARD_CORE):$(SDIR_ARD_SPI):$(SDIR_ARD_ETH):$(SDIR_ARD_ETH_UTIL):$(OCSOCK_DIR)/src:$(LOGGER_DIR)/src

#include directories
IDIR_ARD_CORE = $(ARDUINO_DIR)/hardware/arduino/cores/arduino
IDIR_ARD_VARIANT = $(ARDUINO_DIR)/hardware/arduino/variants/mega
IDIR_ARD_SPI = $(ARDUINO_DIR)/libraries/SPI
IDIR_ARD_ETH = $(ARDUINO_DIR)/libraries/Ethernet
IDIR_ARD_ETH_UTIL = $(ARDUINO_DIR)/libraries/Ethernet/utility

INC_DIRS = -I$(IDIR_ARD_CORE) -I$(IDIR_ARD_VARIANT) -I$(IDIR_ARD_SPI) -I$(IDIR_ARD_ETH) -I$(IDIR_ARD_ETH_UTIL) -I$(OCSOCK_DIR)/include -I$(LOGGER_DIR)/include -I$(ROOT_DIR)/stack/include

#Compiler/Linker flags
COPTIONS_ARDUINO = -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=156 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR -MMD
CFLAGS += -c -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions -felide-constructors -std=c++0x -DATMEGA2560 -DTB_LOG

#Compilers
CCPLUS=avr-g++
CC=avr-gcc
AR=avr-ar
RANLIB=avr-ranlib

TESTAPP = ocsocket_test

CORE_COBJ = WInterrupts.o wiring.o wiring_digital.o
CORE_CPPOBJ = main.o Stream.o WMath.o WString.o HardwareSerial.o Print.o SPI.o IPAddress.o
ETH_CPPOBJ = Dhcp.o Dns.o Ethernet.o EthernetUdp.o 
ETH_UTIL_CPPOBJ = socket.o w5100.o 
OCSOCKET_CPPOBJ = logger.o ocsocket_arduino.o

all: core.a $(TESTAPP).o $(TESTAPP).elf $(TESTAPP).hex
	mkdir -p $(BIN_DIR)
	cp $(TESTAPP).hex $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	cp *.o $(OBJ_DIR)
	cp *.elf $(OBJ_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(COPTIONS_ARDUINO) $(INC_DIRS) $< -o $@

%.o: %.cpp
	$(CCPLUS) $(CFLAGS) $(COPTIONS_ARDUINO) $(INC_DIRS) $< -o $@

#logger needs to be compiled using C++ compiler
logger.o: logger.c
	$(CCPLUS) $(CFLAGS) $(COPTIONS_ARDUINO) $(INC_DIRS) $< -o $@

core.a: $(CORE_COBJ) $(CORE_CPPOBJ) $(ETH_CPPOBJ) $(ETH_UTIL_CPPOBJ)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(TESTAPP).elf: $(TESTAPP).o core.a $(OCSOCKET_CPPOBJ)
	$(CC) -Os -Wl,--gc-sections,--relax $(COPTIONS_ARDUINO) $^ -lm -o $@

$(TESTAPP).hex: $(TESTAPP).elf
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $(TESTAPP).eep 
	avr-objcopy -O ihex -R .eeprom $< $@ 

install: all
	avrdude -C$(ARDUINO_DIR)/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cstk500v2 -P/dev/ttyACM0 -b115200 -D -Uflash:w:$(TESTAPP).hex:i 

.PHONY: clean

clean:
	rm -f *.o *.d *.elf *.eep *.a *.hex *-
	rm -fr $(BIN_DIR)
	rm -fr $(OBJ_DIR)

