include ../../../config.mk

CXX = g++

CXX_FLAGS = -std=c++0x -Wall -pthread

LIB_DIR = ../../../../../resource

CXX_INC := -I$(LIB_DIR)/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/stack/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocsocket/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocrandom/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/logger/include/ 
CXX_INC += -I$(BOOST_DIR) 
CXX_INC += -I./lib


.PHONY: fanclient

all: .PHONY

fanclient: $(LIB_DIR)/build/linux/release/obj/liboc.a fanclient.o
	$(CXX) $(CXX_FLAGS) -o fanclient fanclient.o $(LIB_DIR)/build/linux/release/obj/liboc.a $(LIB_DIR)/csdk/build/linux/release/liboctbstack.a

fanclient.o: fanclient.cpp
	$(CXX) $(CXX_FLAGS) -c fanclient.cpp $(CXX_INC)
	
clean:
	rm -f *.o
	rm -f fanclient 
