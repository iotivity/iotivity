#other options are android, arduino
PLATFORM=linux

ROOT_DIR = ..
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
RANDOM_DIR = $(ROOT_DIR)/ocrandom
CJSON_DIR  = $(ROOT_DIR)/../../../ccf-utilities/tb/cJSON
INC_DIRS = -I$(OCSOCK_DIR)/include/ -I$(LOGGER_DIR)/include -I$(RANDOM_DIR)/include -I$(CJSON_DIR)

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

CFLAGS += -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
	-Wextra -Wno-variadic-macros -pedantic -std=gnu99 -DWITH_POSIX -DTB_LOG

SUBDIRS:= examples doc

SOURCES:= pdu.c net.c debug.c encode.c uri.c coap_list.c resource.c hashkey.c \
	  str.c option.c async.c subscribe.c block.c ocsocket.c logger.c \
	  ocrandom.c cJSON.c

OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))

VPATH = $(OCSOCK_DIR)/src:$(LOGGER_DIR)/src:$(RANDOM_DIR)/src:$(CJSON_DIR)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ANDROID) $(INC_DIRS) $< -o $@

all: libcoap.a
	$(MAKE) -C examples all

libcoap.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

install: all
	$(MAKE) -C examples install

doc:	
	$(MAKE) -C doc

.PHONY: clean

clean: $(SUBDIRS)
	rm -f *.o libcoap.a
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean ; \
	done


