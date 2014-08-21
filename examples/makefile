# override with `make BUILD=release`
# default to release build
BUILD	  := release
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(PWD)/$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj
SAMPLES_OUT_DIR := $(OUT_DIR)/samples

CXX_FLAGS.debug     := -g3 -std=c++0x -Wall -pthread

CXX_FLAGS.release   := -std=c++0x -Wall -pthread

CXX_INC	  := -I../include/
CXX_INC	  += -I../csdk/stack/include
CXX_INC	  += -I../csdk/ocsocket/include
CXX_INC	  += -I../csdk/ocrandom/include
CXX_INC	  += -I../csdk/logger/include
CXX_INC	  += -I../csdk/libcoap

# Force metatargets to build:
.PHONY: prep_dirs simpleserver simpleclient simpleclientserver roomserver roomclient

all: .PHONY

prep_dirs:
	-mkdir $(OUT_DIR)
	-mkdir $(OBJ_DIR)
	-mkdir $(SAMPLES_OUT_DIR)

simpleserver: simpleserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(SAMPLES_OUT_DIR)/$@ simpleserver.cpp $(CXX_INC) ../$(BUILD)/obj/OCLib.a ../csdk/$(BUILD)/liboctbstack.a

simpleclient: simpleclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(SAMPLES_OUT_DIR)/$@ simpleclient.cpp $(CXX_INC) ../$(BUILD)/obj/OCLib.a ../csdk/$(BUILD)/liboctbstack.a

simpleclientserver: simpleclientserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(SAMPLES_OUT_DIR)/$@ simpleclientserver.cpp $(CXX_INC) ../$(BUILD)/obj/OCLib.a ../csdk/$(BUILD)/liboctbstack.a

roomserver: roomserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(SAMPLES_OUT_DIR)/$@ roomserver.cpp $(CXX_INC) ../$(BUILD)/obj/OCLib.a ../csdk/$(BUILD)/liboctbstack.a

roomclient: roomclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(SAMPLES_OUT_DIR)/$@ roomclient.cpp $(CXX_INC) ../$(BUILD)/obj/OCLib.a ../csdk/$(BUILD)/liboctbstack.a

clean: 
	rm -rf $(OUT_DIR)/*

