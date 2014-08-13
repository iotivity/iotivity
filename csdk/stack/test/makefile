# override with `make BUILD=release`
# default to debug build
BUILD	 := debug
PLATFORM := linux
CC	 := gcc

# You must create the file "local.properties" on your local machine which contains any local paths, etc
# local.properties should NOT be committed to repo
include ./local.properties
# GTEST_DIR contains the path to Google Test libs and must be defined in local.properties
#  Example:
#  GTEST_DIR := /home/johndoe/utils/gtest-1.7.0

# NOTE:  to run app, make sure that LD_LIBRARY_PATH env variable
#        contains $(GTEST_DIR)/lib/.libs

ifeq ($(ROOT_DIR), )
ROOT_DIR	= $(PWD)/../..
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
INC_DIRS	+= -I$(OCTBSTACK_INC)/internal
INC_DIRS  += -I$(GTEST_DIR)/include

CC_FLAGS.debug      := -g -O0 -g3 -Wall -ffunction-sections -fdata-sections -fno-exceptions \
                        -std=c++0x -pedantic $(INC_DIRS) -L$(ROOT_DIR) -DTB_LOG
CC_FLAGS.release    := -g -Os -Wall -fdata-sections -Wl,--gc-sections -Wl,-s -fno-exceptions \
                        -std=c++0x $(INC_DIRS) -L$(ROOT_DIR) -DTB_LOG
    					
LDLIBS		+= -loctbstack -lpthread -lgtest -lgtest_main
CPPFLAGS	+= $(CC_FLAGS.$(BUILD)) $(LDLIBS) -L$(GTEST_DIR)/lib/.libs

PROGRAMS	= stacktests

all:	$(PROGRAMS) 

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)
