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
BUILD:=release
PLATFORM:=linux

OCLIB=../..
OCLIB_LIB=../../$(BUILD)/obj/liboc.a 

CXX_FLAGS.debug     := -g3 -O0
CXX_FLAGS.release   := -O3
CXX_FLAGS:=-Wall -std=c++0x -ggdb $(CXX_FLAGS.$(BUILD)) -pthread

# There's probably nicer Makefile magic for this, but hopefully it will suffice:
CXX_INC=-I$(OCLIB)/include \
	-I../../include/ \
	-I../../oc_logger/include/ \
	-I../../csdk/stack/include \
	-I../../csdk/ocsocket/include \
	-I../../csdk/ocrandom/include \
	-I../../csdk/logger/include

BOOST_LIBS=-lboost_program_options
#BOOST_LIBS=/usr/local/boost/lib/libboost_program_options.a	# for boost_1_55 lib

LIB_OC_LOGGER:=../../oc_logger/lib/oc_logger.a

CXX_LIBS=$(OCLIB_LIB) ../../csdk/$(PLATFORM)/$(BUILD)/liboctbstack.a $(LIB_OC_LOGGER) $(BOOST_LIBS)

APPS += client 
APPS += server 
APPS += monoprocess 

.PHONY: client server

all: oc_cpp_sdk apps
	@echo Remember to \"export LD_LIBRARY_PATH=$(BOOST_LIB)\:\$$LD_LIBRARY_PATH\"

apps: $(APPS)

oc_cpp_sdk:
	cd ../../ && $(MAKE) cpp_sdk "BUILD=$(BUILD)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CXX_FLAGS) $(CXX_INC) -c -o $@ $<

client: client.o driver.o utility.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

server: server.o driver.o utility.o light_resource.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

monoprocess: monoprocess.o driver.o utility.o light_resource.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

clean:
	rm -f *.o $(APPS)
	cd ../../ && $(MAKE) clean_cpp_sdk	

clean_apps:
	rm -f *.o $(APPS)
	
