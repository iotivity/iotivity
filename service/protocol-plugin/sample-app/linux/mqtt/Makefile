CXX = g++

CXX_FLAGS = -std=c++0x -Wall -pthread

TOP_DIR = ../../../../..
LIB_DIR = ../../../../../resource

DEPEND_DIR:= $(LIB_DIR)/dependencies
CEREAL_DIR:= $(DEPEND_DIR)/cereal

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
CXX_INC   += -I$(CEREAL_DIR)/include
CXX_INC += -I../../../lib/rapidxml 

LIB_OC_LOGGER := $(LIB_DIR)/oc_logger/lib/oc_logger.a
CXX_LIBS  := ../../../build/linux/release/libppm.a ../../../lib/cpluff/libcpluff/.libs/libcpluff.a
LINK_LIB = -lboost_system -ldl -lexpat -lboost_thread -L$(TOP_DIR)/out/linux/x86/release -loc -loctbstack -loc_logger -lcoap

.PHONY: mqttclient

all: .PHONY

mqttclient: mqttclient.o
	$(CXX) $(CXX_FLAGS) -o mqttclient mqttclient.o $(CXX_LIBS) $(LINK_LIB)

mqttclient.o: mqttclient.cpp
	$(CXX) $(CXX_FLAGS) -c mqttclient.cpp $(CXX_INC)

	
clean:
	rm -f *.o
	rm -f *.so
	rm -f mqttclient
