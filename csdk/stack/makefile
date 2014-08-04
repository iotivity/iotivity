# override with `make BUILD=release`
# default to release build
BUILD := release

# You must create the file "local.properties" on your local machine which contains any local paths, etc
# local.properties should NOT be committed to repo
include ./local.properties
# GTEST_DIR contains the path to Google Test libs and must be defined in local.properties
#  Example:
#  GTEST_DIR := /home/johndoe/utils/gtest-1.7.0

# NOTE:  to run app, make sure that LD_LIBRARY_PATH env variable
#        contains $(GTEST_DIR)/lib/.libs
                                
CC := gcc
CXX	:= g++

PROG := stacktest

SRC_DIR := ./src
TEST_DIR:= ./test
INC_DIR := ./include
LOG_DIR := ../logger
SOCKET_DIR := ../ocsocket
COAP_DIR := ../occoap
RANDOM_DIR := ../ocrandom
LIBCOAP_DIR := ../libcoap-4.1.1
UTIL_DIR := ../../../../ccf-utilities/tb
JSON_DIR := $(UTIL_DIR)/cJSON

CC_FLAGS.debug := -O0 -g3 -Wall -c -fmessage-length=0 -pedantic -std=c99 -fpic -Wno-write-strings -ffunction-sections -fdata-sections -fno-exceptions -DWITH_POSIX -D__USE_MISC -DTB_LOG
CC_FLAGS.release := -Os -Wall -c -fmessage-length=0 -std=c99 -fpic -DWITH_POSIX -D__USE_MISC
CC_FLAGS := $(CC_FLAGS.$(BUILD))

CXX_FLAGS.debug := -O0 -g3 -Wall -c -fmessage-length=0 -pedantic -std=c++0x -fpic -DTB_LOG
CXX_FLAGS.release := -Os -Wall -c -fmessage-length=0 -std=c++0x -fpic -DWITH_POSIX 
CXX_FLAGS := $(CXX_FLAGS.$(BUILD))

INC	:= -I$(SRC_DIR)  
INC += -I$(SOCKET_DIR)/include
INC += -I$(TEST_DIR)
INC += -I$(INC_DIR)
INC += -I$(INC_DIR)/internal
INC += -I$(LOG_DIR)/include
INC += -I$(COAP_DIR)/include
INC += -I$(RANDOM_DIR)/include
INC += -I$(JSON_DIR)
INC += -I$(LIBCOAP_DIR)
INC += -I$(GTEST_DIR)/include


# using make's computed variables to select object and bin folders
# depending on the build type
OBJ_DIR.debug := ./obj/debug
OBJ_DIR.release := ./obj/release
OBJ_DIR := $(OBJ_DIR.$(BUILD))

BIN_DIR.debug := ./bin/debug
BIN_DIR.release := ./bin/release
BIN_DIR := $(BIN_DIR.$(BUILD))

C_FILES := $(wildcard $(SRC_DIR)/*.c)
C_FILES += $(wildcard $(COAP_DIR)/src/*.c)
C_FILES += $(wildcard $(JSON_DIR)/*.c)

CPP_FILES := $(wildcard $(TEST_DIR)/*.cpp)

OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o) $(C_FILES:.c=.o)))

H_FILES := $(wildcard $(TEST_DIR)/*.h)
H_FILES += $(wildcard $(SOCKET_DIR)/include/*.h)
H_FILES += $(wildcard $(INC_DIR)/*.h)
H_FILES += $(wildcard $(INC_DIR)/internal/*.h)
H_FILES += $(wildcard $(LOG_DIR)/include/*.h)
H_FILES += $(wildcard $(COAP_DIR)/include/*.h)

#H_FILES += $(wildcard $(LIBCOAP_DIR)/*.h)
LD_FLAGS := -L/usr/lib
LD_FLAGS += -L$(GTEST_DIR)/lib/.libs


LIBS := -lgtest
LIBS += -lgtest_main
LIBS += -lpthread

DEP_LIBS := $(LIBCOAP_DIR)/libcoap.a

all: $(PROG)
	mkdir -p ./$(OBJ_DIR)
	mkdir -p ./$(BIN_DIR)
	
$(PROG): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(LIBS) $(DEP_LIBS)
	mkdir -p ./$(BIN_DIR)
	mv ./$(PROG) ./$(BIN_DIR)

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h $(H_FILES)
#	$(CXX) $(CXX_FLAGS) $(INC) -c -o $@ $<

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h $(H_FILES)
#	$(CXX) $(CXX_FLAGS) $(INC) -c -o $@ $<

# ocstack
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h $(H_FILES)
	echo "** Building ocstack" 
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

# occlientcb
$(OBJ_DIR)/occlientcb.o: $(SRC_DIR)/occlientcb.c $(H_FILES)
	echo "** Building occlientcb" 
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	
		
# logger		
$(OBJ_DIR)/%.o: $(LOG_DIR)/src/%.c $(LOG_DIR)/include/%.h $(H_FILES)
	echo "** Building logger"
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<
	
# occoap	
$(OBJ_DIR)/%.o: $(COAP_DIR)/src/%.c $(COAP_DIR)/include/%.h $(H_FILES) 
	echo "** Building occoap"
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

# ocsocket	
$(OBJ_DIR)/%.o: $(SOCKET_DIR)/src/%.c $(SOCKET_DIR)/include/%.h $(H_FILES) 
	echo "** Building ocsocket"
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

# util/json
$(OBJ_DIR)/%.o: $(JSON_DIR)/%.c $(JSON_DIR)/%.h $(H_FILES) 
	echo "** Building util/json"
	mkdir -p ./$(OBJ_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

# libcoap
#$(OBJ_DIR)/%.o: $(LIBCOAP_DIR)/%.c $(LIBCOAP_DIR)/%.h $(H_FILES) 
#	echo "** Building libcoap"
#	mkdir -p ./$(OBJ_DIR)
#	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(H_FILES)
	mkdir -p ./$(OBJ_DIR)
	$(CXX) $(CXX_FLAGS) $(INC) -c -o $@ $<	
	
.PHONY: clean
clean :
	rm -rf ./obj/debug/*
	rm -rf ./obj/release/*
	rm -rf ./lib/*
	rm -rf ./bin/debug/*
	rm -rf ./bin/release/*


.PHONY: print_vars

print_vars:
	@echo ""
	@echo 'BUILD     = '$(value BUILD)
	@echo 'INC       = '$(value INC)
	@echo 'CPP_FILES = '$(value CPP_FILES)
	@echo 'C_FILES   = '$(value C_FILES)
	@echo 'LIBS      = '$(value LIBS)
	@echo 'OBJ_FILES = '$(value OBJ_FILES)
	@echo 'SRC_DIR   = '$(value SRC_DIR)
	@echo 'H_FILES   = '$(value H_FILES)
  	

