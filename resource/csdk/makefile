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
# default to ETHERNET for transport
BUILD	 := release
PLATFORM := linux
TRANSPORT := ETHERNET

# override with `make PLATFORM=arduinomega ARDUINOWIFI=1` to enable Arduino WiFi shield
ARDUINOWIFI := 0
CA_INT_DTLS := 0

ifeq ($(ROOT_DIR), )
ROOT_DIR	= ./
endif

PLATFORM_TYPE = linux

ifeq ($(PLATFORM),linux)
    CXX=g++
    CC=gcc
    AR=ar
    RANLIB=ranlib
    CFLAGS_PLATFORM = -DWITH_POSIX -std=gnu99
    PLATFORM_TYPE=linux
else ifeq ($(PLATFORM),arduinomega)
    include local.properties
    include $(PLATFORM).properties
    CC=$(ARDUINO_TOOLS_DIR)/avr-g++
    ifeq ($(wildcard $(ARDUINO_DIR)/libraries/Time/Time/),)
        $(error Arduino Time library needs to be moved from <ARDUINO_DIR>/libraries/Time \
        to <ARDUINO_DIR>/libraries/Time/Time. You may need to create \
        <ARDUINO_DIR>/libraries/Time/Time directory. Please refer to the wiki or readme \
        for more information)
    endif
    PLATFORM_TYPE=arduino
else ifeq ($(PLATFORM),arduinodue)
    include local.properties
    include $(PLATFORM).properties
    CC=$(ARDUINO_TOOLS_DIR)/arm-none-eabi-g++
    PLATFORM_TYPE=arduino
else
    $(error Wrong value for PLATFORM !!)
endif

ifeq ($(PLATFORM_TYPE),arduino)
    ifeq ($(ARDUINOWIFI),0)
    ARDUINO_SHIELD_TYPE := "/ethernet_shield"
    else ifeq ($(ARDUINOWIFI), 1)
    ARDUINO_SHIELD_TYPE := "/wifi_shield"
    endif
    PLATFORM_SPECIFIC_BACKOUT:= ../../../../
else
    PLATFORM_SPECIFIC_BACKOUT := ../../../
endif

ifeq ($(OUT_DIR), )
OUT_DIR	= $(PLATFORM)$(ARDUINO_SHIELD_TYPE)/$(BUILD)
endif

ifeq ($(OBJ_DIR), )
OBJ_DIR	= $(OUT_DIR)/obj
endif

OCLOGGER_DIR		= logger
OC_LOG_DIR      	= ../oc_logger
OCRANDOM_DIR		= ocrandom
OCTBSTACK_DIR		= stack
OCMALLOC_DIR 		= ocmalloc
OCSECURITY_DIR  = security
EXTLIBS_DIR		= ../../extlibs
CJSON_DIR		= $(EXTLIBS_DIR)/cjson
TINYDTLS_DIR	= $(EXTLIBS_DIR)/tinydtls
CONNECTIVITY_DIR	= connectivity

OCTBSTACK_SRC		= $(OCTBSTACK_DIR)/src
OCMALLOC_SRC 		= $(OCMALLOC_DIR)/src
OCSECURITY_SRC    = $(OCSECURITY_DIR)/src
CJSON_SRC		= $(CJSON_DIR)
CONNECTIVITY_SRC	= $(CONNECTIVITY_DIR)
OCLOGGER_SRC		= $(OCLOGGER_DIR)/src
OCRANDOM_SRC		= $(OCRANDOM_DIR)/src
OCLOGGER_INC		= $(OCLOGGER_DIR)/include
OC_LOG_INC		= $(OC_LOG_DIR)/include
OCRANDOM_INC		= $(OCRANDOM_DIR)/include
OCTBSTACK_INC		= $(OCTBSTACK_DIR)/include
OCMALLOC_INC		= $(OCMALLOC_DIR)/include
OCSECURITY_INC	= $(OCSECURITY_DIR)/include
CJSON_INC		= $(CJSON_DIR)
CONNECTIVITY_INC	= $(CONNECTIVITY_DIR)/inc
LIBCOAP_INC         = $(CONNECTIVITY_DIR)/lib/libcoap-4.1.1

INC_DIRS	:= -I$(OCLOGGER_INC)
INC_DIRS	+= -I$(OC_LOG_INC)
INC_DIRS	+= -I$(OCRANDOM_INC)
INC_DIRS	+= -I$(OCMALLOC_INC)
INC_DIRS	+= -I$(OCSECURITY_INC)
INC_DIRS	+= -I$(OCSECURITY_INC)/internal
INC_DIRS	+= -I$(OCTBSTACK_INC)
INC_DIRS	+= -I$(OCTBSTACK_INC)/internal
INC_DIRS	+= -I$(CJSON_INC)
INC_DIRS	+= -I$(CONNECTIVITY_INC)
INC_DIRS	+= -I$(CONNECTIVITY_DIR)/api
INC_DIRS    += -I$(LIBCOAP_INC)

# TODO-CA Remove -fstack-protector-all before merging to master
CC_FLAGS.debug		:= -O0 -g3 -Wall -fstack-protector-all -c -fmessage-length=0 -pedantic -fpic -DTB_LOG
# TODO-CA Remove -fstack-protector-all before merging to master
CC_FLAGS.release	:= -Os -Wall -fstack-protector-all -c -fmessage-length=0 -fpic

CFLAGS		+= $(CC_FLAGS.$(BUILD)) $(INC_DIRS) $(CFLAGS_PLATFORM) $(INC_DIR_PLATFORM)

CJSON_SOURCES		:= $(CJSON_SRC)/cJSON.c

OCTBSTACK_SOURCES	:= $(OCTBSTACK_SRC)/ocstack.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/occlientcb.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocresource.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocobserve.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocserverrequest.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/occollection.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/oicgroup.c
OCTBSTACK_SOURCES	+= $(OCSECURITY_SRC)/ocsecurity.c
OCTBSTACK_SOURCES	+= $(OCMALLOC_SRC)/ocmalloc.c
OCTBSTACK_SOURCES	+= $(OCLOGGER_SRC)/logger.c
OCTBSTACK_SOURCES	+= $(OCRANDOM_SRC)/ocrandom.c

SOURCES			:= $(CJSON_SOURCES)
SOURCES			+= $(OCTBSTACK_SOURCES)

all:	objdirs obj_build ca liboctbstack.a

buildScript_all: objdirs obj_build ca liboctbstack.a

objdirs:
	mkdir -p $(PLATFORM)
	mkdir -p $(PLATFORM)/$(ARDUINO_SHIELD_TYPE)
	mkdir -p $(OUT_DIR)
	mkdir -p $(OBJ_DIR)

obj_build:
	@echo "Building $@"
	# Output all *.o files to $(OUT_DIR)/$(BUILD)/$(OBJ_DIR)
	$(foreach source,$(SOURCES), $(CC) $(CFLAGS) $(source) -o $(patsubst %.c, %.o, $(patsubst %, $(OBJ_DIR)/%, $(notdir $(source))));)

ca:
	@echo "Building $@"
        ifeq ($(PLATFORM),linux)
	    @cd $(CONNECTIVITY_SRC)/lib/libcoap-4.1.1 && $(MAKE)
	    @cd $(CONNECTIVITY_SRC)/build/linux && $(MAKE) "BUILD=$(BUILD)" "PLATFORM=linux"
	    @cd $(CONNECTIVITY_SRC)/samples/linux && $(MAKE)
        else
	    @cd $(CONNECTIVITY_SRC)/build/ && $(MAKE) "PLATFORM=$(PLATFORM)" "ARDUINO_DIR=$(ARDUINO_DIR)" "TRANSPORT=$(TRANSPORT)"
        endif
liboctbstack.a:
	@echo "Building $@"
	$(AR) -r $(OUT_DIR)/$@ $(OBJ_DIR)/*.o

    ifeq ($(CA_INT_DTLS),0)
     ifeq ($(PLATFORM),linux)
      ifneq ($(wildcard $(TINYDTLS_DIR)/libtinydtls.a),)
			$(info "Building liboctbstack with DTLS support")
			mkdir -p $(OBJ_DIR)/tinydtls
			@cd $(OBJ_DIR)/tinydtls && $(AR) -x ../$(PLATFORM_SPECIFIC_BACKOUT)$(TINYDTLS_DIR)/libtinydtls.a
			$(AR) -q $(OUT_DIR)/$@ $(OBJ_DIR)/tinydtls/*.o
      endif
     endif
    endif

.PHONY: clean print_vars

clean: legacy_clean
	-rm -rf linux
	-rm -rf arduinomega
	-rm -rf arduinodue
	@cd $(CONNECTIVITY_SRC)/build/ && $(MAKE) clean "PLATFORM=arduinomega"
	@cd $(CONNECTIVITY_SRC)/samples/linux && $(MAKE) "clean"

deepclean: legacy_deepclean
	-rm -rf linux
	-rm -rf arduinomega
	-rm -rf arduinodue

legacy_clean:
	@echo "Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/$(BUILD)/liboctbstack.a
	cd $(CONNECTIVITY_SRC)/lib/libcoap-4.1.1 && $(MAKE) clean
	cd $(CONNECTIVITY_SRC)/build/linux && $(MAKE) clean
	cd $(CONNECTIVITY_SRC)/samples/linux && $(MAKE) clean
	rm -rf $(OBJ_DIR)
	rm -rf release
	rm -rf debug

legacy_deepclean:
	@echo "Deep-Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/liboctbstack.a
	cd $(CONNECTIVITY_SRC)/lib/libcoap-4.1.1 && $(MAKE) clean
	cd $(CONNECTIVITY_SRC)/build/linux && $(MAKE) clean
	cd $(CONNECTIVITY_SRC)/samples/linux && $(MAKE) clean
	rm -rf $(OBJ_DIR)
	rm -rf release
	rm -rf debug
