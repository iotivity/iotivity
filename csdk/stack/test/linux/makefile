VERSION:=4.1.1

# override with `make BUILD=release`
# default to release build
BUILD	 := release

#other options are android, arduino
PLATFORM=linux

ROOT_DIR = ../../..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
OCSTACK_DIR = $(ROOT_DIR)/stack
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom

OBJ_DIR = ./

INC_DIRS := -I$(OCSTACK_DIR)/include
INC_DIRS += -I$(OCSOCK_DIR)/include
INC_DIRS += -I$(LOGGER_DIR)/include
INC_DIRS += -I$(RANDOM_DIR)/include

ifeq ($(PLATFORM),android)
	CCPLUS=arm-linux-androideabi-g++
	CC=arm-linux-androideabi-gcc
	AR=arm-linux-androideabi-ar
	RANLIB=arm-linux-androideabi-ranlib
	CFLAGS_ANDROID =  -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
	LDFLAGS_ANDROID = -march=armv7-a -Wl,--fix-cortex-a8 -llog
else ifeq ($(PLATFORM),linux)
	CCPLUS=g++
	CC=gcc
	AR=ar
	RANLIB=ranlib
else
   $(error Wrong value for PLATFORM !!)
endif

DEP_LIBS = $(ROOT_DIR)/$(BUILD)/liboctbstack.a

CFLAGS += -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
	-Wextra -Wno-variadic-macros -pedantic -std=gnu99 -DWITH_POSIX -DTB_LOG

PROGRAMS = ocserver occlient

SOURCES:= occlient.c ocserver.c

OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))

all: objdirs $(PROGRAMS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ANDROID) $(INC_DIRS) $< -o $(BUILD)/$@

objdirs: $(ROOT_DIR)
	mkdir -p $(BUILD)
	
##Added -lm as needed by json calling floor()
occlient: occlient.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $(BUILD)/$^ -o $(BUILD)/$@ -lm

ocserver: ocserver.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $(BUILD)/$^ -o $(BUILD)/$@ -lm

install: all
	@echo "Installing for PLATFORM $(PLATFORM)"
ifeq ($(PLATFORM),android)
	adb push coap-client /data/local/tmp/
	adb push coap-server /data/local/tmp/
endif

.PHONY: clean

clean:
	rm -rf debug
	rm -rf release
