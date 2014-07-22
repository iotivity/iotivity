# override with `make BUILD=release`
# default to debug build
BUILD	 := debug
PLATFORM := linux
CC	 := gcc

ifeq ($(ROOT_DIR), )
ROOT_DIR	= $(PWD)
endif

ifeq ($(OBJ_DIR), )
OBJ_DIR	= $(ROOT_DIR)/obj
endif

OCLOGGER_DIR	= $(ROOT_DIR)/logger
OCRANDOM_DIR	= $(ROOT_DIR)/ocrandom
OCSOCKET_DIR	= $(ROOT_DIR)/ocsocket
LCOAP_DIR	= $(ROOT_DIR)/libcoap-4.1.1
OCCOAP_DIR	= $(ROOT_DIR)/occoap
OCTBSTACK_DIR	= $(ROOT_DIR)/stack
UTILS_DIR	= $(ROOT_DIR)/../../../ccf-utilities/tb
CJSON_DIR	= $(UTILS_DIR)/cJSON

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

CC_FLAGS.debug		:= -O0 -g3 -Wall -c -fmessage-length=0 -pedantic -std=c99 -fpic -DTB_LOG -L./libcoap-4.1.1/
CC_FLAGS.release	:= -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -fpic -L./libcoap-4.1.1/

CFLAGS		+= $(CC_FLAGS.$(BUILD)) -DWITH_POSIX $(INC_DIRS)
LDLIBS		+= -lcoap

CJSON_SOURCES		:= $(CJSON_SRC)/cJSON.c

OCCOAP_SOURCES		:= $(OCCOAP_SRC)/occoap.c

OCTBSTACK_SOURCES	:= $(OCTBSTACK_SRC)/ocstack.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/occlientcb.c
OCTBSTACK_SOURCES	+= $(OCTBSTACK_SRC)/ocserverrequest.c

SOURCES			:= $(CJSON_SOURCES)
SOURCES			+= $(OCCOAP_SOURCES)
SOURCES			+= $(OCTBSTACK_SOURCES)

all:	make_lcoap objdirs obj_build liboctbstack.a #print_vars

make_lcoap: 
	$(MAKE) -C $(LCOAP_DIR)

objdirs: $(ROOT_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/$(BUILD)

obj_build:
	@echo "Building $@"
	$(foreach source,$(SOURCES), $(CC) $(LDLIBS) $(CFLAGS) $(source) -o $(patsubst %.c, %.o, $(patsubst %, $(OBJ_DIR)/$(BUILD)/%, $(notdir $(source))));) # Output all *.o files to $(OBJ_DIR)/$(BUILD)

liboctbstack.a:
	@echo "Building $@" 
	@cd $(OBJ_DIR)/$(BUILD) && ar -x $(LCOAP_DIR)/libcoap.a # Unpackage libcoap.a to $(OBJ_DIR)/$(BUILD). The output objects from OCStack and OCCoap are already at this location
	ar -r $@ $(OBJ_DIR)/$(BUILD)/*.o # Repackage all the objects at this location into a single archive. This is OCStack, OCCoap, and LibCoap (LibCoap contains OCRandom, OCLogger, and OCSocket.).

.PHONY: clean print_vars

clean:	$(ROOT_DIR)
	@echo "Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/liboctbstack.a

deepclean: $(ROOT_DIR)
	@echo "Deep-Cleaning all."
	rm -f $(OBJ_DIR)/$(BUILD)/*.o
	rm -f $(ROOT_DIR)/liboctbstack.a
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
