# override with `make BUILD=release`
# default to debug build
BUILD	 := release
PLATFORM := linux
CC	 := gcc

ifeq ($(ROOT_DIR), )
ROOT_DIR	= $(PWD)/../../..
endif

OUT_DIR	= $(PWD)

OCLOGGER_DIR	= $(ROOT_DIR)/logger
OCRANDOM_DIR	= $(ROOT_DIR)/ocrandom
OCSOCKET_DIR	= $(ROOT_DIR)/ocsocket
LCOAP_DIR		= $(ROOT_DIR)/libcoap-4.1.1
OCCOAP_DIR		= $(ROOT_DIR)/occoap
OCTBSTACK_DIR	= $(ROOT_DIR)/stack

OCLOGGER_INC	= $(OCLOGGER_DIR)/include
OCRANDOM_INC	= $(OCRANDOM_DIR)/include
OCSOCKET_INC	= $(OCSOCKET_DIR)/include
LCOAP_INC		= $(LCOAP_DIR)
OCCOAP_INC		= $(OCCOAP_DIR)/include
OCTBSTACK_INC	= $(OCTBSTACK_DIR)/include

INC_DIRS	:= -I$(OCLOGGER_INC)
INC_DIRS	+= -I$(OCRANDOM_INC)
INC_DIRS	+= -I$(OCSOCKET_INC)
INC_DIRS	+= -I$(LCOAP_INC)
INC_DIRS	+= -I$(OCCOAP_INC)
INC_DIRS	+= -I$(OCTBSTACK_INC)

CC_FLAGS.debug      := -O0 -g3 -Wall -ffunction-sections -fdata-sections -fno-exceptions \
                        -pedantic $(INC_DIRS) -L$(ROOT_DIR) -DTB_LOG
CC_FLAGS.release    := -Os -Wall -fdata-sections -Wl,--gc-sections -Wl,-s -fno-exceptions \
                        $(INC_DIRS) -L$(ROOT_DIR) -DTB_LOG
    					
LDLIBS		+= -loctbstack -lpthread
CPPFLAGS	+= $(CC_FLAGS.$(BUILD)) $(LDLIBS)

PROGRAMS	+= ocserver
PROGRAMS	+= occlient

all:	$(PROGRAMS) 

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)
