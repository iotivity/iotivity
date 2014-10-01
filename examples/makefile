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
# override with `make BUILD=release`
# default to release build
BUILD	  := release
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(PWD)/$(BUILD)

CXX_FLAGS.debug     := -O0 -g3 -std=c++0x -Wall -pthread

CXX_FLAGS.release   := -O3 -std=c++0x -Wall -pthread

CXX_INC	  := -I../include/
CXX_INC   += -I../oc_logger/include

CXX_INC	  += -I../csdk/stack/include
CXX_INC	  += -I../csdk/ocsocket/include
CXX_INC	  += -I../csdk/ocrandom/include
CXX_INC	  += -I../csdk/logger/include
CXX_INC	  += -I../csdk/libcoap

LIB_OC_LOGGER := ../oc_logger/lib/oc_logger.a

CXX_LIBS  := ../$(BUILD)/obj/liboc.a ../csdk/$(BUILD)/liboctbstack.a $(LIB_OC_LOGGER)

APPS  += simpleserver
APPS  += simpleclient
APPS  += simpleclientserver
APPS  += roomserver
APPS  += roomclient
APPS  += presenceserver
APPS  += presenceclient
APPS  += fridgeserver
APPS  += fridgeclient
APPS  += garageserver
APPS  += garageclient
APPS  += ocicuc

# Force metatargets to build:
.PHONY:	prep_dirs oc_cpp_sdk $(APPS)

all: .PHONY

apps: prep_dirs $(APPS)

prep_dirs:
	-mkdir -p $(OUT_DIR)

oc_cpp_sdk:
	cd ../ && $(MAKE) cpp_sdk "BUILD=$(BUILD)"

simpleserver: simpleserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleserver.cpp $(CXX_INC) $(CXX_LIBS)

simpleclient: simpleclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleclient.cpp $(CXX_INC) $(CXX_LIBS)

fridgeserver: fridgeserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ fridgeserver.cpp $(CXX_INC) $(CXX_LIBS)

fridgeclient: fridgeclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ fridgeclient.cpp $(CXX_INC) $(CXX_LIBS)

presenceserver: presenceserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ presenceserver.cpp $(CXX_INC) $(CXX_LIBS)

presenceclient: presenceclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ presenceclient.cpp $(CXX_INC) $(CXX_LIBS)

simpleclientserver: simpleclientserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleclientserver.cpp $(CXX_INC) $(CXX_LIBS)

roomserver: roomserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ roomserver.cpp $(CXX_INC) $(CXX_LIBS)

roomclient: roomclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ roomclient.cpp $(CXX_INC) $(CXX_LIBS)

garageserver: garageserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ garageserver.cpp $(CXX_INC) $(CXX_LIBS)

garageclient: garageclient.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ garageclient.cpp $(CXX_INC) $(CXX_LIBS)

ocicuc:
	cd ocicuc && $(MAKE) apps

clean:
	rm -rf $(OUT_DIR)
	cd ../ && $(MAKE) clean_cpp_sdk
	cd ocicuc && $(MAKE) clean_apps

clean_apps:
	rm -rf $(OUT_DIR)
	cd ocicuc && $(MAKE) clean_apps
