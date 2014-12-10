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
PLATFORM  := linux
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(BUILD)

DEPEND_DIR:= ../dependencies
CEREAL_DIR:= $(DEPEND_DIR)/cereal

CXX_INC	  := -I../include/
CXX_INC   += -I../oc_logger/include
CXX_INC	  += -I../csdk/stack/include
CXX_INC	  += -I../csdk/ocsocket/include
CXX_INC	  += -I../csdk/ocrandom/include
CXX_INC	  += -I../csdk/logger/include
CXX_INC	  += -I../csdk/libcoap
CXX_INC   += -I$(CEREAL_DIR)/include

LIB_OC_LOGGER := ../oc_logger/lib/oc_logger.a

CXX_LIBS  := ../$(BUILD)/obj/liboc.a ../csdk/$(PLATFORM)/$(BUILD)/liboctbstack.a $(LIB_OC_LOGGER)

ifeq ($(PLATFORM), "")
	PLATFORM := "linux"
endif

ifeq ($(PLATFORM),android)
	CXX=arm-linux-androideabi-g++

	CXX_FLAGS.debug     := -O0 -g3 -std=c++11 -Wall --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm -L$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/  -Wno-unused
	CXX_FLAGS.release   := -O3 -std=c++11 -Wall --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm -L$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/ -Wno-unused
	CXX_LIBS+=-lgnustl_static -llog 

	NDK_INC0=$(NDK_DIR)/platforms/android-19/arch-arm/usr/include
	NDK_INC1=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/include
	NDK_INC2=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include

	CXX_INC+=-I$(NDK_INC0)
	CXX_INC+=-I$(NDK_INC1)
	CXX_INC+=-I$(NDK_INC2)
	CXX_INC+=-I../../../oic-utilities/android/boost/include/
else ifeq ($(PLATFORM),linux)
	CXX:=g++
	CXX_FLAGS.debug     := -O0 -g3 -std=c++0x -Wall -pthread
	CXX_FLAGS.release   := -O3 -std=c++0x -Wall -pthread
else
	$(error Wrong value for PLATFORM !!)
endif


# Force metatargets to build:
all.PHONY: prep_dirs oc_cpp_sdk simpleserver simpleserverHQ simpleclient simpleclientHQ simpleclientserver roomserver roomclient presenceserver presenceclient garageserver garageclient fridgeserver fridgeclient ocicuc_target threadingsample

apps.PHONY: prep_dirs oc_cpp_sdk simpleserver simpleserverHQ simpleclient simpleclientHQ simpleclientserver roomserver roomclient presenceserver presenceclient garageserver garageclient fridgeserver fridgeclient threadingsample

buildScript_all.PHONY: prep_dirs simpleserver simpleserverHQ simpleclient simpleclientHQ simpleclientserver roomserver roomclient presenceserver presenceclient garageserver garageclient fridgeserver fridgeclient threadingsample

all: all.PHONY

apps: apps.PHONY

buildScript_all: buildScript_all.PHONY

prep_dirs:
	-mkdir -p $(OUT_DIR)

oc_cpp_sdk:
	cd ../ && $(MAKE) cpp_sdk "BUILD=$(BUILD)"

simpleserver: simpleserver.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleserver.cpp $(CXX_INC) $(CXX_LIBS)

simpleserverHQ: simpleserverHQ.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleserverHQ.cpp $(CXX_INC) $(CXX_LIBS)

simpleclientHQ: simpleclientHQ.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ simpleclientHQ.cpp $(CXX_INC) $(CXX_LIBS)

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

threadingsample: threadingsample.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ threadingsample.cpp $(CXX_INC) $(CXX_LIBS)

ocicuc_target:
	cd ocicuc && $(MAKE) apps

clean:
	rm -rf debug
	rm -rf release
	cd ../ && $(MAKE) clean_cpp_sdk
	cd ocicuc && $(MAKE) clean
	cd ocicuc && $(MAKE) clean_apps

clean_apps:
	rm -rf debug
	rm -rf release
	cd ocicuc && $(MAKE) clean
	cd ocicuc && $(MAKE) clean_apps
