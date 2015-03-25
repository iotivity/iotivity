-include ../../../../build/arduino/root_path.inc
-include ../../../../build/arduino/environment.mk

ARDUINO_DIR = /usr/share/arduino
SRC_PATH=../src

# override with `make PLATFORM=arduinomega ARDUINOWIFI=1` to enable Arduino WiFi shield
ARDUINOWIFI := 0

APP_NAME := trackee

BUILD := release
#BUILD := debug
PLATFORM := arduinomega
ARDUINO_PORT := /dev/ttyACM0

SRCLIST=${wildcard ${SRC_PATH}/*.cpp}
OBJPATH=${SRCLIST:.cpp=.o}
OBJLIST=${notdir ${OBJPATH}}

TB_DIR = ${IOT_BASE}/csdk
OIC_BUILD_DIR = ${IOT_CSDK_MAKE_PATH}
LOGGER_DIR = $(TB_DIR)/logger
OC_LOG_DIR = $(TB_DIR)/../oc_logger
TBSTACK_DIR = $(TB_DIR)/stack
TBSOCKET_DIR = $(TB_DIR)/ocsocket

include $(TB_DIR)/local.properties
include $(TB_DIR)/$(PLATFORM).properties

VPATH := $(SDIR_ARD_PLATFORM)

#include directories
OCSOCK_DIR = $(TB_DIR)/ocsocket
LOGGER_DIR = $(TB_DIR)/logger
STACK_DIR  = $(TB_DIR)/stack
INC_DIRS = -I$(OCSOCK_DIR)/include/ -I$(OC_LOG_DIR)/include -I$(LOGGER_DIR)/include -I$(STACK_DIR)/include -I../include

CC_FLAGS.debug		:= -O1 -g3 -Wall -c -fmessage-length=0 -pedantic -fpic
CC_FLAGS.release	:= -Os -Wall -c -fmessage-length=0 -fpic

CFLAGS := $(CC_FLAGS.$(BUILD)) -DTB_LOG

ifeq ($(ARDUINOWIFI),1)
	CFLAGS += -DARDUINOWIFI
	ARDUINO_SHIELD_TYPE := "/wifi_shield"
	TRANSPORT_OBJS = $(WIFI_COBJ)
else
	ARDUINO_SHIELD_TYPE := "/ethernet_shield"
	TRANSPORT_OBJS = $(ETH_CPPOBJ) $(ETH_UTIL_CPPOBJ)
endif

.PHONY: all install resource clean

all: prep_dirs core.a $(APP_NAME).elf $(APP_NAME).hex

OBJ_DIR := ./bin

resource: preclean
	@cd $(OIC_BUILD_DIR) && make PLATFORM=${PLATFORM} ARDUINOWIFI=${ARDUINOWIFI}

preclean: 
	@cd $(OIC_BUILD_DIR) && make deepclean 

core.a: $(PLATFORM_OBJS)
	@echo ">>>>>>>>>>> core.a build <<<<<<<<<<<<<<"
	@cd $(OBJ_DIR) && $(AR) -x $(TB_DIR)/$(PLATFORM)$(ARDUINO_SHIELD_TYPE)/$(BUILD)/liboctbstack.a
	$(AR) rcs $@ $^ $(OBJ_DIR)/*.o
	$(RANLIB) $@

prep_dirs:
	-mkdir -p $(OBJ_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@
%.o: %.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@
%.o: ${SRC_PATH}/%.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@

$(APP_NAME).elf: $(OBJLIST) core.a
ifeq ($(PLATFORM),arduinomega)
	$(CC) -Os -Wl,--gc-sections,--relax $(CFLAGS_PLATFORM) $^ -lm -o $@
else ifeq ($(PLATFORM),arduinodue)
#	$(CXX) -Os -Wl,--gc-sections -mcpu=cortex-m3 -T/$(SDIR_ARD_CORE_3)/linker_scripts/gcc/flash.ld -Wl,-Map,$(APP_NAME).map -o $(OBJ_DIR)/$@ -lm -lgcc -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols -Wl,--start-group $(foreach obj, $(APP_NAME).o $(SYSCALLS_SAM3_OBJ) $(SPI_OBJ) $(TRANSPORT_OBJS) $(VARIANT_OBJ) core.a, $(OBJ_DIR)/$(obj)) $(SDIR_ARD_CORE_3)/libsam_sam3x8e_gcc_rel.a -Wl,--end-group
	$(error Not yet Supported !! )
else
	$(error Wrong value for PLATFORM !!)
endif

$(APP_NAME).hex: $(APP_NAME).elf
ifeq ($(PLATFORM),arduinomega)
	$(AVR_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $(APP_NAME).eep
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@
else ifeq ($(PLATFORM),arduinodue)
	$(ARDUINO_TOOLS_DIR)/arm-none-eabi-objcopy -O binary $< $@
else
	$(error Wrong value for PLATFORM !!)
endif

install: 
ifeq ($(PLATFORM),arduinomega)
	$(AVR_PROGRAMMER) -C$(ARDUINO_DIR)/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cstk500v2 -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(APP_NAME).hex:i
else ifeq ($(PLATFORM),arduinodue)
	stty -F /dev/$(ARDUINO_PORT) speed 1200 cs8 -cstopb -parenb
	$(ARDUINO_DIR)/hardware/tools/bossac -i -d --port=$(ARDUINO_PORT) -U false -e -w -v -b $(APP_NAME).hex -R
else
	$(error Wrong value for PLATFORM !!)
endif



clean:
	@rm -f *.o *.d *.elf *.eep *.a *.hex *.bin *.map *-
	@rm -rf $(OBJ_DIR)

