include ../../../config.mk

CXX = g++

CXX_FLAGS = -std=c++0x -Wall

CPLUFF_DIR = ../../../lib/cpluff/libcpluff

LIB_DIR = ../../../../../oic-resource

SRC = ../../src/

CXX_INC = -I. -I$(CPLUFF_DIR)
CXX_INC += -I$(BOOST_DIR)
CXX_INC += -I$(LIB_DIR)/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/stack/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocsocket/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocrandom/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/logger/include/ 

LINK_LIB = -ldl -lexpat -lboost_thread -lboost_system

PHONY : ppm 
#PluginLifecycleManager.o

all: .PHONY

ppm: PluginManager.o Plugin.o PluginManagerService.o
	$(CXX) $(CXX_INC) -o $@ $^ $(CPLUFF_DIR)/.libs/libcpluff.a $(LIB_DIR)/build/linux/release/obj/liboc.a $(LIB_DIR)/csdk/build/linux/release/liboctbstack.a $(LINK_LIB)

PluginManagerService.o : $(SRC)PluginManagerService.cpp
	$(CXX) $(CXX_INC) -c -fpermissive $(CXX_FLAGS) $(SRC)PluginManagerService.cpp

PluginManager.o : $(SRC)PluginManager.cpp
	$(CXX) $(CXX_INC) -c $(CXX_FLAGS) -Wsign-compare $(SRC)PluginManager.cpp

Plugin.o : $(SRC)Plugin.cpp
	$(CXX) $(CXX_INC) -c $(SRC)Plugin.cpp

#PluginLifecycleManager.o : PluginLifecycleManager.cpp
#	$(CXX) $(CXX_INC) -c PluginLifecycleManager.cpp

clean:
	rm -rf *.o
	rm -f ppm 
