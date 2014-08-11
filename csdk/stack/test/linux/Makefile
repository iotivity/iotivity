VERSION:=4.1.1

#other options are android, arduino
PLATFORM=linux

#peg_ccfl-core
ROOT_DIR = ../../..
LIBCOAP_DIR = $(ROOT_DIR)/libcoap-4.1.1
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
OCCOAP_DIR = $(ROOT_DIR)/occoap
OCSTACK_DIR = $(ROOT_DIR)/stack
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
JSON_DIR = $(ROOT_DIR)/../../../oic-utilities/tb/cJSON

OBJ_DIR = ./
INC_DIRS = -I$(LOGGER_DIR)/include/ -I$(OCCOAP_DIR)/include/ \
    -I$(OCSTACK_DIR)/include/ -I$(OCSTACK_DIR)/include/internal \
    -I$(LIBCOAP_DIR)/ -I$(OCSOCK_DIR)/include -I$(RANDOM_DIR)/include \
    -I$(JSON_DIR)

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

DEP_LIBS = $(LIBCOAP_DIR)/libcoap.a

CFLAGS += -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
    -Wextra -Wno-variadic-macros -pedantic -std=gnu99 -DWITH_POSIX -DTB_LOG

VPATH = $(OCCOAP_DIR)/src:$(OCSTACK_DIR)/src

PROGRAMS = ocserver occlient

SOURCES:= occlient.c ocserver.c ocstack.c occoap.c occlientcb.c ocserverrequest.c occoaphelper.c

OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))

all: $(PROGRAMS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ANDROID) $(INC_DIRS) $< -o $@

##Added -lm as needed by json calling floor()
occlient: occlient.o ocstack.o occoap.o occlientcb.o ocserverrequest.o occoaphelper.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $^ -o $@ -lm

ocserver: ocserver.o ocstack.o occoap.o occlientcb.o ocserverrequest.o occoaphelper.o $(DEP_LIBS)
	$(CC) -Os -Wl,--gc-sections $(LDFLAGS_ANDROID) $^ -o $@ -lm

install: all
	@echo "Installing for PLATFORM $(PLATFORM)"
ifeq ($(PLATFORM),android)
	adb push coap-client /data/local/tmp/
	adb push coap-server /data/local/tmp/
endif

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)

