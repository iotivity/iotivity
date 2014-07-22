# override with `make BUILD=release`
# default to debug build
BUILD	 := debug
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

CC_FLAGS.debug 		:= -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
    					-Wextra -Wno-variadic-macros -pedantic $(INC_DIRS) -L$(ROOT_DIR) -DTB_LOG
CC_FLAGS.release	:= -Os -Wall -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions \
    					-Wextra -Wno-variadic-macros $(INC_DIRS) -L$(ROOT_DIR)
    					
LDLIBS		+= -loctbstack
CPPFLAGS	+= $(CC_FLAGS.$(BUILD)) $(LDLIBS)

SOURCES		+= logger.c
SOURCES		+= ./occlient.cpp
SOURCES		+= ./ocserver.cpp 

PROGRAMS	+= ocserver
PROGRAMS	+= occlient

VPATH += $(OCLOGGER_DIR)/src/

all:	$(PROGRAMS) 

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)
