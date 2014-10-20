# override with `make BUILD=debug`
# override with `make PLATFORM=arduinomega` or `make PLATFORM=arduinodue`
# default to release build
# default to build for linux
BUILD	 := release
PLATFORM := linux

# override with `make PLATFORM=arduinomega ARDUINOWIFI=1` to enable Arduino WiFi shield
ARDUINOWIFI := 0

ifeq ($(ROOT_DIR), )
ROOT_DIR	= $(PWD)
endif

ifeq ($(OBJ_DIR), )
OBJ_DIR	= obj
endif


ifeq ($(PLATFORM),linux)
    CCPLUS=g++
    CC=gcc
    AR=ar
    RANLIB=ranlib
    CFLAGS_PLATFORM = -DWITH_POSIX -std=c99
else ifeq ($(PLATFORM),arduinomega)
    include local.properties
    include $(PLATFORM).properties
	CC=$(ARDUINO_TOOLS_DIR)/avr-g++
else ifeq ($(PLATFORM),arduinodue)
    include local.properties
    include $(PLATFORM).properties
	CC=$(ARDUINO_TOOLS_DIR)/arm-none-eabi-g++
else
   $(error Wrong value for PLATFORM !!)
endif

OCLOGGER_DIR	= $(ROOT_DIR)/logger
OCRANDOM_DIR	= $(ROOT_DIR)/ocrandom
OCSOCKET_DIR	= $(ROOT_DIR)/ocsocket
LCOAP_DIR	= $(ROOT_DIR)/libcoap-4.1.1
OCCOAP_DIR	= $(ROOT_DIR)/occoap
OCTBSTACK_DIR	= $(ROOT_DIR)/stack
EXTLIBS_DIR	= $(ROOT_DIR)/../../extlibs
CJSON_DIR	= $(EXTLIBS_DIR)/cjson

OCCOAP_SRC	= $(OCCOAP_DIR)/src
OCTBSTACK_SRC	= $(OCTBSTACK_DIR)/src
CJSON_SRC	= $(CJSON_DIR)

OCLOGGER_INC	= $(OCLOGGER_DIR)/include
OCRANDOM_INC	= $(OCRANDOM_DIR)/include
OCSOCKET_INC	= $(OCSOCKET_DIR)/include
LCOAP_INC	= $(LCOAP_DIR)
OCCOAP_INC	= $(OCCOAP_DIR)/include
OCTBSTACK_INC	= $(OCTBSTACK_DIR)/include
CJSON_INC	= $(CJSON_DIR)

INC_DIRS	:= -I$(OCLOGGER_INC)
INC_DIRS	+= -I$(OCRANDOM_INC)
INC_DIRS	+= -I$(OCSOCKET_INC)
INC_DIRS	+= -I$(LCOAP_INC)
INC_DIRS	+= -I$(OCCOAP_INC)
INC_DIRS	+= -I$(OCTBSTACK_INC)
INC_DIRS	+= -I$(OCTBSTACK_INC)/internal
INC_DIRS	+= -I$(CJSON_INC)

CC_FLAGS.debug		:= -O0 -g3 -Wall -c -fmessage-length=0 -pedantic -fpic -DTB_LOG
CC_FLAGS.release	:= -Os -Wall -c -fmessage-length=0 -fpic

CFLAGS		+= $(CC_FLAGS.$(BUILD)) $(INC_DIRS) $(CFLAGS_PLATFORM) $(INC_DIR_PLATFORM)
LDLIBS		+= -lcoap

CJSON_SOURCES		:= $(CJSON_SRC)/cJSON.c

OCCOAP_SOURCES		:= $(OCCOAP_SRC)/occoap.c
OCCOAP_SOURCES		+= $(OCCOAP_SRC)/occoaphelper.c

OCTBSTACK_SOURCES	:= $(OCTBSTACK_SRC)/ocstack.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/occlientcb.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocresource.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocobserve.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/occollection.c

SOURCES			:= $(CJSON_SOURCES)
SOURCES			+= $(OCCOAP_SOURCES)
SOURCES			+= $(OCTBSTACK_SOURCES)

all:	make_lcoap objdirs obj_build liboctbstack.a #print_vars

make_lcoap: 
	$(MAKE) -C $(LCOAP_DIR) "BUILD=$(BUILD)" "PLATFORM=$(PLATFORM)" "ARDUINOWIFI=$(ARDUINOWIFI)"

objdirs: $(ROOT_DIR)
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/$(OBJ_DIR)

obj_build:
	@echo "Building $@"
	# Output all *.o files to $(OBJ_DIR)/$(BUILD)
	$(foreach source,$(SOURCES), $(CC) $(LDLIBS) $(CFLAGS) $(source) -o $(patsubst %.c, %.o, $(patsubst %, $(BUILD)/$(OBJ_DIR)/%, $(notdir $(source))));)

liboctbstack.a:
	@echo "Building $@" 
	# Unpackage libcoap.a to $(OBJ_DIR)/$(BUILD). The output objects from OCStack and OCCoap are already at this location
	@cd $(BUILD)/$(OBJ_DIR) && $(AR) -x $(LCOAP_DIR)/$(BUILD)/libcoap.a
	# Repackage all the objects at this location into a single archive. This is OCStack, OCCoap, and LibCoap (LibCoap contains OCRandom, OCLogger, and OCSocket.).
	$(AR) -r $(BUILD)/$@ $(BUILD)/$(OBJ_DIR)/*.o

.PHONY: clean print_vars

clean: legacy_clean
	-rm -rf release
	-rm -rf debug

deepclean: legacy_deepclean
	-rm -rf release
	-rm -rf debug

legacy_clean:	$(ROOT_DIR)
	@echo "Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/$(BUILD)/liboctbstack.a
	rm -rf $(OBJ_DIR)

legacy_deepclean: $(ROOT_DIR)
	@echo "Deep-Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/liboctbstack.a
	rm -rf $(OBJ_DIR)
	$(MAKE) clean -C $(LCOAP_DIR)

print_vars:
	@echo ""
	@echo 'INCLUDES ARE: '$(value INC_DIRS)
	@echo ""
	@echo 'SOURCE FILES ARE: '$(value SOURCES)
	@echo ""
	@echo 'OBJ_DIR/BUILD is: '$(OBJ_DIR)'/'$(BUILD)
	@echo ""
	@echo 'ROOT_DIR is: '$(ROOT_DIR)
