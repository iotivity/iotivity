CXX = g++

CXX_FLAGS = -std=c++0x -Wall -pthread

TOP_DIR = ../../../../../..
LIB_DIR = ../../../../../../resource

SRC_DIR = ../../src

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
CXX_INC += -I../../../../lib/cpluff/libcpluff
CXX_INC	  += -I../csdk/libcoap
CXX_INC   += -I$(CEREAL_DIR)/include

LIB_OC_LOGGER := $(LIB_DIR)/oc_logger/lib/oc_logger.a
CXX_LIBS  := ../../../../lib/cpluff/libcpluff/.libs/libcpluff.a

.PHONY: lib release_build ./release/lightserver_mqtt_plugin.so 

all: .PHONY

lib:
	cd ../../lib && $(MAKE)

release_build:
	-mkdir release
	-mkdir release/obj
	cp plugin.xml release

./release/lightserver_mqtt_plugin.so: ./release/obj/lightserver_mqtt_plugin.o ./release/obj/lightserver.o
	$(CXX) -shared -o ./release/lightserver_mqtt_plugin.so ./release/obj/lightserver_mqtt_plugin.o ./release/obj/lightserver.o $(CXX_LIBS)  -L../../lib  -L$(TOP_DIR)/out/linux/x86/release -lmosquitto -lssl -lrt -loc -loctbstack -loc_logger -lcoap

./release/obj/lightserver_mqtt_plugin.o: $(SRC_DIR)/lightserver_mqtt_plugin.cpp
	$(CXX) $(CXX_INC) -fPIC -o ./release/obj/lightserver_mqtt_plugin.o -c $(SRC_DIR)/lightserver_mqtt_plugin.cpp 

./release/obj/lightserver.o: $(SRC_DIR)/lightserver.cpp
	$(CXX) $(CXX_FLAGS) -fPIC -o ./release/obj/lightserver.o -c $(SRC_DIR)/lightserver.cpp $(CXX_INC)
	
clean:
	cd ../../lib && $(MAKE) clean
	rm -rf ./release/obj
	cd ./release && rm -f *.so
