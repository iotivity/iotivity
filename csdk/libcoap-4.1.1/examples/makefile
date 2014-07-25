VERSION:=4.1.1

#other options are android, arduino
PLATFORM=linux

ROOT_DIR = ../..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
OBJ_DIR = ./
INC_DIRS = -I.. -I$(OCSOCK_DIR)/include/ -I$(LOGGER_DIR)/include -I$(RANDOM_DIR)/include

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

DEP_LIBS = ../libcoap.a

CFLAGS += -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
    -Wextra -Wno-variadic-macros -pedantic -std=gnu99 -DWITH_POSIX

PROGRAMS = coap-server coap-client

SOURCES:= client.c server.c

OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))

all: $(PROGRAMS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ANDROID) $(INC_DIRS) $< -o $@

coap-client: client.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $^ -o $@

coap-server: server.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $^ -o $@

install: all
	@echo "Installing for PLATFORM $(PLATFORM)"
ifeq ($(PLATFORM),android)
	adb push coap-client /data/local/tmp/
	adb push coap-server /data/local/tmp/
endif

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)

