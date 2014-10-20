# override with `make BUILD=release`
# default to debug build
BUILD := debug

CC := g++

PROG := core

SRC_DIR := ./src
TEST_DIR := ./test
TEMP_DIR := ./include/core

CC_FLAGS.debug := -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -pedantic -fpic
CC_FLAGS.release := -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -fpic
CC_FLAGS := $(CC_FLAGS.$(BUILD))

INC	:= -I$(SRC_DIR)  
INC	+= -I./include/core
INC += -I$(TEST_DIR)
INC += -I../include/controller

LIBS :=
#LIBS := -lpthread
#LIBS +=	-lboost_thread-mt
#LIBS += -lboost_thread

# using make's computed variables to select object and bin folders
# depending on the build type
OBJ_DIR.debug := ./obj/debug
OBJ_DIR.release := ./obj/release
OBJ_DIR := $(OBJ_DIR.$(BUILD))

BIN_DIR.debug := ./bin/debug
BIN_DIR.release := ./bin/release
BIN_DIR := $(BIN_DIR.$(BUILD))

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
CPP_FILES += $(wildcard $(TEST_DIR)/*.cpp)
CPP_FILES += $(wildcard $(TEMP_DIR)/*.cpp)
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))
H_FILES	:= $(wildcard ./include/core/*.hpp)
H_FILES	+= $(wildcard ./include/core/*.h)
H_FILES += $(wildcard $(TEST_DIR)/*.h)
LD_FLAGS := -L/usr/lib


all: $(PROG)

$(PROG): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LD_FLAGS) $(LIBS) 	
	mv ./$(PROG) ./$(BIN_DIR)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h $(H_FILES)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(H_FILES)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<
	
$(OBJ_DIR)/%.o: $(TEMP_DIR)/%.cpp $(H_FILES)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<	

.PHONY: clean
clean :
	rm -rf ./obj/debug/*
	rm -rf ./obj/release/*
	rm -rf ./bin/debug/*
	rm -rf ./bin/release/*


.PHONY: print_vars

print_vars:
	@echo ""
	@echo 'BUILD     = '$(value BUILD)
	@echo 'INC       = '$(value INC)
	@echo 'CPP_FILES = '$(value CPP_FILES)
	@echo 'LIBS      = '$(value LIBS)
  	

