
BUILD := release
PLATFORM := arduinomega
ARDUINO_PORT := /dev/ttyACM0

# override with `make PLATFORM=arduinomega ARDUINOWIFI=1` to enable Arduino WiFi shield
ARDUINOWIFI := 0

APP_NAME := ocserver

OBJ_DIR := ./bin

ROOT_DIR = ../../../../..
LOGGER_DIR = $(ROOT_DIR)/logger
TBSTACK_DIR = $(ROOT_DIR)/stack
TBSOCKET_DIR = $(ROOT_DIR)/ocsocket

include $(ROOT_DIR)/local.properties
include $(ROOT_DIR)/$(PLATFORM).properties

VPATH := $(SDIR_ARD_PLATFORM)

#include directories
OCSOCK_DIR = $(ROOT_DIR)/ocsocket
LOGGER_DIR = $(ROOT_DIR)/logger
STACK_DIR  = $(ROOT_DIR)/stack
INC_DIRS = -I$(OCSOCK_DIR)/include/ -I$(LOGGER_DIR)/include -I$(STACK_DIR)/include

CFLAGS	:= -Os -Wall -c -DTB_LOG

ifeq ($(ARDUINOWIFI),1)
	CFLAGS += -DARDUINOWIFI
endif

all: prep_dirs core.a $(APP_NAME).o $(APP_NAME).elf $(APP_NAME).hex

core.a: $(PLATFORM_OBJS)
	@cd $(OBJ_DIR) && $(AR) -x $(ROOT_DIR)/../$(BUILD)/liboctbstack.a
	$(AR) rcs $@ $^ $(OBJ_DIR)/*.o
	$(RANLIB) $@

prep_dirs:
	-mkdir $(OBJ_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@

%.o: %.cpp
	$(CCPLUS) $(CFLAGS) $(CFLAGS_PLATFORM) $(INC_DIRS) $(INC_DIR_PLATFORM) $< -o $@

$(APP_NAME).elf: $(APP_NAME).o core.a
	$(CC) -Os -Wl,--gc-sections,--relax $(CFLAGS_PLATFORM) $^ -lm -o $@

$(APP_NAME).hex: $(APP_NAME).elf
	$(AVR_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $(APP_NAME).eep
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@

install: all
	$(AVR_PROGRAMMER) -C$(ARDUINO_DIR)/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cstk500v2 -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(APP_NAME).hex:i

.PHONY: clean

clean:
	@rm -f *.o *.d *.elf *.eep *.a *.hex *.bin *.map *-
	@rm -rf $(OBJ_DIR)
