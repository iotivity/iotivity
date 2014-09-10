# //******************************************************************
# //
# // Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# override with `make BUILD=debug`
# override with `make PLATFORM=arduinomega` or `make PLATFORM=arduinodue`
# default to release build
# default to build for linux
BUILD	 := release
#other options are android, arduino
PLATFORM=linux
# override with `make PLATFORM=arduinomega ARDUINOWIFI=1` to enable Arduino WiFi shield
ARDUINOWIFI := 0

OUT_DIR	  := ./$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj

ROOT_DIR = ..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
STACK_DIR  = $(ROOT_DIR)/stack
INC_DIRS = -I$(OCSOCK_DIR)/include/ -I$(LOGGER_DIR)/include -I$(RANDOM_DIR)/include

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
else ifeq ($(PLATFORM),arduinomega)
    include $(ROOT_DIR)/local.properties
    include $(ROOT_DIR)/$(PLATFORM).properties
	CC=$(ARDUINO_TOOLS_DIR)/avr-g++
else ifeq ($(PLATFORM),arduinodue)
    include $(ROOT_DIR)/local.properties
    include $(ROOT_DIR)/$(PLATFORM).properties
	CC=$(ARDUINO_TOOLS_DIR)/arm-none-eabi-g++
else
   $(error Wrong value for PLATFORM !!)
endif

CC_FLAGS.debug := -O0 -g3 -Wall -ffunction-sections -fdata-sections -fno-exceptions -pedantic \
-DTB_LOG
CC_FLAGS.release := -Os -Wall -ffunction-sections -fdata-sections -fno-exceptions

SOURCES:= pdu.c net.c debug.c encode.c uri.c coap_list.c resource.c hashkey.c \
          str.c option.c async.c subscribe.c block.c logger.c ocrandom.c 
VPATH := $(OCSOCK_DIR)/src:$(LOGGER_DIR)/src:$(RANDOM_DIR)/src
ifeq (arduino, $(findstring arduino,$(PLATFORM)))
	ifeq ($(ARDUINOWIFI),1)
		SOURCES += ocsocket_arduino_wifi.c
	else
		SOURCES += ocsocket_arduino.c
	endif
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

