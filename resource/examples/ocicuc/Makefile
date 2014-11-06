# //******************************************************************
# //
# // Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Please note that many of these settings are unused at the moment on account of the rush to get this out the door-- look carefully
# at the actual commands to see which are relevant:

OC_CSDK=../../../ccf-resource/csdk
OC_STACK=$(OC_CSDK)/stack
OC_OCCOAP=$(OC_CSDK)/occoap
OC_SOCKET=$(OC_CSDK)/ocsocket
OC_RANDOM=$(OC_CSDK)/ocrandom
OC_LOGGER=$(OC_CSDK)/logger
OC_LIB=$(OC_CSDK)/build/linux/liboctbstack.a

OCLIB=../..
OCLIB_LIB=../../build/linux/$(BUILD)/obj/liboc.a #$(OCLIB)/release/obj/liboc.a

BOOST_BASE=/usr/local/boost
BOOST_INC=$(BOOST_BASE)/include
BOOST_LIB=$(BOOST_BASE)/lib

CXX_CC=g++ -std=c++0x -O0 -ggdb
#CXX_CC=clang -std=c++0x -lstdc++

CXX_FLAGS=-Wall 

# There's probably nicer Makefile magic for this, but hopefully it will suffice:
CXX_INC=-I$(OCLIB)/include -I$(OC_STACK)/include -I$(OC_SOCKET)/include -I$(OC_RANDOM)/include -I$(OC_LOGGER)/include -I$(OC_OCCOAP)/include -I$(BOOST_INC)

CXX_LIBS=-L$(BOOST_LIB) $(OC_LIB) $(OCLIB_LIB) -lpthread -lboost_program_options

CXX=$(CXX_CC) $(CXX_FLAGS) $(CXX_INC) $(CXX_LIBS)

.PHONY: init driver resources client server

all: init driver resources client server monoprocess
	@echo Remember to \"export LD_LIBRARY_PATH=$(BOOST_LIB)\:\$$LD_LIBRARY_PATH\"

init:
	rm -f *.o ./client ./server

driver:
	$(CXX_CC) -Wall -pthread  -c driver.cpp utility.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ 

resources:
	$(CXX_CC) -Wall -pthread -c light_resource.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ 

client:
	$(CXX_CC) -pthread  -c client.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ -I/usr/local/include $(CXX_INC) $(CXX_FLAGS)

	$(CXX_CC) -std=c++0x -Wall -L/usr/local/boost/lib/ -I/usr/local/boost/include -pthread  -o client driver.o utility.o client.o ../../build/linux/$(BUILD)/obj/liboc.a ../../csdk/build/linux/$(BUILD)/liboctbstack.a -lboost_program_options $(CXX_INC) $(CXX_FLAGS) 

server: resources
	$(CXX_CC) -pthread  -c server.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ -I/usr/local/include $(CXX_INC) 

	$(CXX_CC) -std=c++0x -Wall -L/usr/local/boost/lib/ -I/usr/local/boost/include -pthread  -o server driver.o utility.o server.o light_resource.o ../../build/linux/$(BUILD)/obj/liboc.a ../../csdk/build/linux/$(BUILD)/liboctbstack.a -lboost_program_options $(CXX_INC) 

monoprocess: driver resources
	$(CXX_CC) -pthread  -c monoprocess.cpp -I../../include/ -I../../csdk/stack/include -I../../csdk/ocsocket/include -I../../csdk/ocrandom/include -I../../csdk/logger/include -I/usr/local/include/boost/ -I/usr/local/include $(CXX_INC) 

	$(CXX_CC) -std=c++0x -Wall -L/usr/local/boost/lib/ -I/usr/local/boost/include -pthread  -o monoprocess driver.o utility.o monoprocess.o light_resource.o ../../build/linux/$(BUILD)/obj/liboc.a ../../csdk/build/linux/$(BUILD)/liboctbstack.a -lboost_program_options $(CXX_INC) 

clean:
	rm -f server.o client.o driver.o monoprocess.o server client monoprocess
