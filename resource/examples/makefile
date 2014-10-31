# override with `make BUILD=release`
# default to release build
BUILD	  := release
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(PWD)/$(BUILD)

CXX_FLAGS.debug     := -O0 -g3 -std=c++0x -Wall -pthread 

CXX_FLAGS.release   := -O3 -std=c++0x -Wall -pthread 

CXX_INC	  := -I../include/
CXX_INC	  += -I../csdk/stack/include
CXX_INC	  += -I../csdk/ocsocket/include
CXX_INC	  += -I../csdk/ocrandom/include
CXX_INC	  += -I../csdk/logger/include
CXX_INC	  += -I../csdk/libcoap

# Force metatargets to build:
all.PHONY: prep_dirs simpleserver simpleclient simpleclientserver roomserver roomclient presenceserver presenceclient ocicuc_target

apps.PHONY: prep_dirs simpleserver simpleclient simpleclientserver roomserver roomclient presenceserver presenceclient

all: all.PHONY

apps: apps.PHONY

prep_dirs:
	-mkdir -p $(OUT_DIR)

simpleserver: simpleserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleserver.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

simpleclient: simpleclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleclient.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

presenceserver: presenceserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ presenceserver.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

presenceclient: presenceclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ presenceclient.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

simpleclientserver: simpleclientserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleclientserver.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

roomserver: roomserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ roomserver.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

roomclient: roomclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ roomclient.cpp $(CXX_INC) ../build/linux/$(BUILD)/obj/liboc.a ../csdk/build/linux/$(BUILD)/liboctbstack.a

ocicuc_target:
	cd ocicuc && $(MAKE)

clean: 
	rm -rf $(OUT_DIR)/*
	cd ocicuc && $(MAKE) clean

