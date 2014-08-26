# override with `make BUILD=release`
# default to release build
BUILD	  := release
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(PWD)/$(BUILD)

CXX_FLAGS.debug     := -O0 -g3 -std=c++0x -Wall -pthread

CXX_FLAGS.release   := -O3 -std=c++0x -Wall -pthread 

# Please note that many of these settings are unused at the moment on account of the rush to get this out the door-- look carefully
# at the actual commands to see which are relevant (this makefile needs to be rewritten):

OC_CSDK=../../ccf-resource/csdk
OC_STACK=$(OC_CSDK)/stack
OC_OCCOAP=$(OC_CSDK)/occoap
OC_SOCKET=$(OC_CSDK)/ocsocket
OC_RANDOM=$(OC_CSDK)/ocrandom
OC_LOGGER=$(OC_CSDK)/logger
OC_LIB=$(OC_CSDK)/liboctbstack.a

OCLIB=../..
OCLIB_LIB=$(OCLIB)/$(BUILD)/obj/OCLib.a #$(OCLIB)/release/obj/OCLib.a

BOOST_BASE=/usr/local/boost
BOOST_INC=$(BOOST_BASE)/include
BOOST_LIB=$(BOOST_BASE)/lib

CXX_CC=g++ -std=c++0x -O3
#CXX_CC=g++ -std=c++0x -O0 -ggdb # debug
#CXX_CC=clang -std=c++0x -lstdc++

CXX_FLAGS=-Wall 

# There's probably nicer Makefile magic for this, but hopefully it will suffice:
CXX_INC=-I$(OCLIB)/include -I$(OC_STACK)/include -I$(OC_SOCKET)/include -I$(OC_RANDOM)/include -I$(OC_LOGGER)/include -I$(OC_OCCOAP)/include -I$(BOOST_INC)

CXX_LIBS=-L$(BOOST_LIB) $(OC_LIB) $(OCLIB_LIB) -lpthread -lboost_program_options

CXX=$(CXX_CC) $(CXX_FLAGS) $(CXX_INC) $(CXX_LIBS)

.PHONY: init driver resources client server

all: init driver resources client server 
	
init:
	rm -f *.o ./client ./server

driver:
	$(CXX_CC) -Wall -pthread  -c driver.cpp utility.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ 

resources:
	$(CXX_CC) -Wall -pthread -c light_resource.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ 

client:
	$(CXX_CC) -pthread  -c client.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ -I/usr/local/include $(CXX_INC) $(CXX_FLAGS)

	$(CXX_CC) -std=c++0x -Wall -L/usr/local/boost/lib/ -I/usr/local/boost/include -pthread  -o client driver.o utility.o client.o ../../$(BUILD)/obj/OCLib.a ../../csdk/$(BUILD)/liboctbstack.a -lboost_program_options $(CXX_INC) $(CXX_FLAGS) 

server: resources
	$(CXX_CC) -pthread  -c server.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ -I/usr/local/include $(CXX_INC) 

	$(CXX_CC) -std=c++0x -Wall -L/usr/local/boost/lib/ -I/usr/local/boost/include -pthread  -o server driver.o utility.o server.o light_resource.o ../../$(BUILD)/obj/OCLib.a ../../csdk/$(BUILD)/liboctbstack.a -lboost_program_options $(CXX_INC) 

clean:
	rm -f server.o client.o driver.o utility.o light_resource.o server client
