CXX = g++

CXX_FLAGS = -std=c++0x -Wall -pthread

LIB_DIR = ../../../../../resource

CXX_INC := -I$(LIB_DIR)/include/ 
CXX_INC += -I$(LIB_DIR)/oc_logger/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/stack/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocsocket/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/ocrandom/include/ 
CXX_INC += -I$(LIB_DIR)/csdk/logger/include/
CXX_INC += -I$(BOOST_DIR) 
CXX_INC += -I../../lib
CXX_INC += -I../../../lib/cpluff/libcpluff
CXX_INC += -I../../../plugin-manager/src

LIB_OC_LOGGER := $(LIB_DIR)/oc_logger/lib/oc_logger.a
CXX_LIBS  := $(LIB_DIR)/release/obj/liboc.a $(LIB_DIR)/csdk/linux/release/liboctbstack.a $(LIB_OC_LOGGER) ../../../build/linux/release/libppm.a ../../../lib/cpluff/libcpluff/.libs/libcpluff.a
LINK_LIB = -lboost_system -ldl -lexpat -lboost_thread 

.PHONY: fanclient

all: .PHONY

fanclient: fanclient.o
	$(CXX) $(CXX_FLAGS) -o fanclient fanclient.o $(CXX_LIBS) $(LINK_LIB)

fanclient.o: fanclient.cpp
	$(CXX) $(CXX_FLAGS) -c fanclient.cpp $(CXX_INC)

	
clean:
	rm -f *.o
	rm -f fanclient 
