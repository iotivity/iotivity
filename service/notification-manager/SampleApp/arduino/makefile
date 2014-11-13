CC=avr-gcc
CCPLUS=avr-g++
AR=avr-ar
RANLIB=avr-ranlib

APP_NAME := thserver
#BUILD := release
BUILD := debug
ARDUINO_PORT := /dev/ttyACM0

OBJ_DIR := ./bin

TB_DIR = ../../../../oic-resource_a/csdk

include $(TB_DIR)/local.properties
CC=avr-gcc
LOGGER_DIR = $(TB_DIR)/logger
TBSTACK_DIR = $(TB_DIR)/stack
TBSOCKET_DIR = $(TB_DIR)/ocsocket

#Source directories
SDIR_ARD_CORE = $(ARDUINO_DIR)/hardware/arduino/cores/arduino
SDIR_ARD_SPI = $(ARDUINO_DIR)/libraries/SPI
SDIR_ARD_ETH = $(ARDUINO_DIR)/libraries/Ethernet
SDIR_ARD_ETH_UTIL = $(ARDUINO_DIR)/libraries/Ethernet/utility
SDIR_ARD_TIME = $(ARDUINO_DIR)/libraries/Time

VPATH := $(SDIR_ARD_CORE):$(SDIR_ARD_SPI):$(SDIR_ARD_ETH):$(SDIR_ARD_ETH_UTIL):$(OCSOCK_DIR)/src

#include directories
INCD_ARD_CORE = -I$(ARDUINO_DIR)/hardware/arduino/cores/arduino
INCD_ARD_VARIANT = -I$(ARDUINO_DIR)/hardware/arduino/variants/mega
INCD_ARD_SPI = -I$(ARDUINO_DIR)/libraries/SPI
INCD_ARD_ETH = -I$(ARDUINO_DIR)/libraries/Ethernet
INCD_ARD_ETH_UTIL = -I$(ARDUINO_DIR)/libraries/Ethernet/utility
INCD_ARD_TIME = -I$(ARDUINO_DIR)/libraries/Time
INCD_TBLOGGER = -I$(LOGGER_DIR)/include
INCD_TBSTACK = -I$(TBSTACK_DIR)/include
INCD_TBSOCKET = -I$(TBSOCKET_DIR)/include

CPPFLAGSZ := -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=156 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR -MMD

CPPFLAGS := -Wall -Os -c
CPPFLAGS +=  $(CPPFLAGSZ) $(INCD_ARD_CORE) $(INCD_ARD_VARIANT) $(INCD_ARD_SPI) $(INCD_ARD_ETH) $(INCD_ARD_ETH_UTIL) $(INCD_ARD_TIME) $(INCD_TBLOGGER) $(INCD_TBSTACK) $(INCD_TBSOCKET)
CPPFLAGS += -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions -felide-constructors -std=c++0x -DATMEGA2560 -DTB_LOG

SERVER_CPP_SRC := $(APP_NAME).cpp

CORE_COBJ = WInterrupts.o wiring.o wiring_digital.o
CORE_CPPOBJ = main.o Stream.o WMath.o WString.o HardwareSerial.o Print.o SPI.o IPAddress.o
ETH_CPPOBJ = Dhcp.o Dns.o Ethernet.o EthernetUdp.o
ETH_UTIL_CPPOBJ = socket.o w5100.o
OCDEPENDENT_CPPOBJ = wiring_analog.o

SERVER_OBJ := $(SERVER_CPP_SRC:.cpp=.o)

#all: prep_dirs core.a $(APP_NAME).o $(APP_NAME).elf $(APP_NAME).hex
all: preclean prebuild precopy prep_dirs core.a $(APP_NAME).o $(APP_NAME).elf $(APP_NAME).hex

precopy:
	@cd $(TB_DIR) && mkdir debug && cp release/liboctbstack.a debug

preclean: 
	@cd $(TB_DIR) && make deepclean 
	
prebuild:
	@cd $(TB_DIR) && make PLATFORM=arduino

core.a: $(CORE_COBJ) $(CORE_CPPOBJ) $(ETH_CPPOBJ) $(ETH_UTIL_CPPOBJ)
	@cd $(OBJ_DIR) && $(AR) -x ../$(TB_DIR)/$(BUILD)/liboctbstack.a 
	$(AR) rcs $@ $^ $(OBJ_DIR)/*.o
	$(RANLIB) $@

prep_dirs:
	-mkdir -p $(OBJ_DIR)

%.o: %.cpp
	$(CCPLUS) $(CPPFLAGS) $< -o $@

$(APP_NAME).elf: $(APP_NAME).o core.a $(OCDEPENDENT_CPPOBJ)
	$(CCPLUS) -Os -Wl,--gc-sections,--relax $(CPPFLAGSZ) -nodefaultlibs $^ -lm -lgcc -lc -lgcc -o $@

$(APP_NAME).hex: $(APP_NAME).elf
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $(APP_NAME).eep
	avr-objcopy -O ihex -R .eeprom $< $@

install: all
	avrdude -C$(ARDUINO_DIR)/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cstk500v2 -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(APP_NAME).hex:i

.PHONY: clean

clean:
	@rm -f *.o *.d *.elf *.eep *.a *.hex *-
	@rm -rf $(OBJ_DIR)
