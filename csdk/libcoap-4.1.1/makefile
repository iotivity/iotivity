# override with `make BUILD=debug`
# override with `make PLATFORM=arduino`
# default to release build
# default to build for linux
BUILD	 := release
#other options are android, arduino
PLATFORM=linux
OUT_DIR	  := ./$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj

ROOT_DIR = ..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
STACK_DIR  = $(ROOT_DIR)/stack
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
    CFLAGS_PLATFORM = -std=gnu99 -DWITH_POSIX
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
-DTB_LOG
CC_FLAGS.release := -Os -Wall -ffunction-sections -fdata-sections -fno-exceptions

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

all: prep_dirs libcoap.a

prep_dirs:
	-mkdir $(OUT_DIR)
	-mkdir $(OBJ_DIR)

%.o: %.c
	$(CC) -c $(CC_FLAGS.$(BUILD)) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $(OBJ_DIR)/$@

%.o: %.cpp
	$(CCPLUS) -c $(CC_FLAGS.$(BUILD)) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $(OBJ_DIR)/$@

libcoap.a: $(OBJECTS) $(OBJECTSCPP)
	$(AR) rcs $(OUT_DIR)/$@ $(addprefix $(OBJ_DIR)/,$^)
	$(RANLIB) $(OUT_DIR)/$@

.PHONY: clean

clean:	legacy_clean
	-rm -rf release
	-rm -rf debug

legacy_clean:
	rm -f *.o libcoap.a

