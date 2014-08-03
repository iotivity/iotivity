# override with `make BUILD=release`
# default to release build
BUILD	 := release
#other options are android, arduino
PLATFORM=linux

ROOT_DIR = ..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
INC_DIRS = -I$(OCSOCK_DIR)/include/ -I$(LOGGER_DIR)/include -I$(RANDOM_DIR)/include

#Arduino specific configurations
INCD_ARD_CORE = -I$(ARDUINO_DIR)/hardware/arduino/cores/arduino
INCD_ARD_VARIANT = -I$(ARDUINO_DIR)/hardware/arduino/variants/mega
INCD_ARD_SPI = -I$(ARDUINO_DIR)/libraries/SPI
INCD_ARD_ETH = -I$(ARDUINO_DIR)/libraries/Ethernet
INCD_ARD_ETH_UTIL = -I$(ARDUINO_DIR)/libraries/Ethernet/utility
INCD_ARD_TIME = -I$(ARDUINO_DIR)/libraries/Time
SDIR_ARD_TIME = $(ARDUINO_DIR)/libraries/Time

# Note for Arduino: The CC flag is set to the C++ compiler since Arduino build 
# includes Time.h header file which has C++ style definitions.
ifeq ($(PLATFORM),android)
    CCPLUS=arm-linux-androideabi-g++
    CC=arm-linux-androideabi-gcc
    AR=arm-linux-androideabi-ar
    RANLIB=arm-linux-androideabi-ranlib
    CFLAGS_PLATFORM =  -DWITH_POSIX -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
    LDFLAGS_PLATFORM = -march=armv7-a -Wl,--fix-cortex-a8 -llog
else ifeq ($(PLATFORM),linux)
    CCPLUS=g++
    CC=gcc
    AR=ar
    RANLIB=ranlib
    CFLAGS_PLATFORM = -DWITH_POSIX
else ifeq ($(PLATFORM),arduino)
    include local.properties
    CCPLUS=avr-g++
    CC=avr-g++
    AR=avr-ar
    RANLIB=avr-ranlib
    CFLAGS_PLATFORM = -DATMEGA2560 -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=156 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR -DWITH_ARDUINO -MMD -DNDEBUG
    INC_DIR_PLATFORM = $(INCD_ARD_CORE) $(INCD_ARD_VARIANT) $(INCD_ARD_SPI) $(INCD_ARD_ETH) $(INCD_ARD_ETH_UTIL) $(INCD_ARD_TIME)
else
   $(error Wrong value for PLATFORM !!)
endif

CC_FLAGS.debug := -O0 -g3 -Wall -ffunction-sections -fdata-sections -fno-exceptions -pedantic \
-std=gnu99 -DTB_LOG
CC_FLAGS.release := -Os -Wall -ffunction-sections -fdata-sections -fno-exceptions -std=gnu99 \

CFLAGS		+= $(CC_FLAGS.$(BUILD))


SOURCES:= pdu.c net.c debug.c encode.c uri.c coap_list.c resource.c hashkey.c \
          str.c option.c async.c subscribe.c block.c logger.c ocrandom.c 
VPATH := $(OCSOCK_DIR)/src:$(LOGGER_DIR)/src:$(RANDOM_DIR)/src
ifeq ($(PLATFORM),arduino)
SOURCES += ocsocket_arduino.c
SOURCESCPP:= Time.cpp
OBJECTSCPP:= $(patsubst %.cpp, %.o, $(SOURCESCPP))
VPATH += $(SDIR_ARD_TIME)
else
SOURCES += ocsocket.c
endif

OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@

%.o: %.cpp
	$(CCPLUS) -c $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@

libcoap.a: $(OBJECTS) $(OBJECTSCPP)
	$(AR) rcs $@ $^
	$(RANLIB) $@

.PHONY: clean

clean:
	rm -f *.o libcoap.a

