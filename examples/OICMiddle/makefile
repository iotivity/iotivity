#//******************************************************************
#//
#// Copyright 2014 Intel Corporation.
#//
#//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#//
#// Licensed under the Apache License, Version 2.0 (the "License");
#// you may not use this file except in compliance with the License.
#// You may obtain a copy of the License at
#//
#//      http://www.apache.org/licenses/LICENSE-2.0
#//
#// Unless required by applicable law or agreed to in writing, software
#// distributed under the License is distributed on an "AS IS" BASIS,
#// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#// See the License for the specific language governing permissions and
#// limitations under the License.
#//
#//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

# override with `make BUILD=release`
# default to release build
BUILD	  := debug
PLATFORM  := linux
CXX	  := g++
#CXX	  := clang
OUT_DIR	  := $(BUILD)
OIC       := ../..
OIC_RES   := $(OIC)/resource
OIC_LIB   := $(OIC)/out/linux/x86_64/release
OBJS	  := OICMiddle.o \
				Client.o \
				Server.o \
				WrapResource.o \
				LineInput.o \
				RestInput.o
				
CXX_FLAGS.debug     := -O0 -g3 -std=c++0x -Wall -pthread

CXX_FLAGS.release   := -O3 -std=c++0x -Wall -pthread

CXX_INC	  := -I$(OIC_RES)/include/
CXX_INC   += -I$(OIC_RES)/oc_logger/include
CXX_INC	  += -I$(OIC_RES)/csdk/stack/include
CXX_INC	  += -I$(OIC_RES)/csdk/ocsocket/include
CXX_INC	  += -I$(OIC_RES)/csdk/ocrandom/include
CXX_INC	  += -I$(OIC_RES)/csdk/logger/include
CXX_INC	  += -I$(OIC_RES)/csdk/libcoap
CXX_INC   += -I$(OIC_RES)/../extlibs/cereal/include

CXX_LIBS  := $(OIC_LIB)/liboc.so
CXX_LIBS  += $(OIC_LIB)/liboctbstack.so
CXX_LIBS  += $(OIC_LIB)/liboc_logger.so
CXX_LIBS  += $(OIC_LIB)/liboc_logger_core.so
CXX_LIBS  += $(OIC_LIB)/libcoap.so

all: prep_dirs OICMiddle

prep_dirs:
	-mkdir -p $(OUT_DIR)
	
OICMiddle: $(OBJS)
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ $(OBJS) $(CXX_LIBS)

OICMiddle.o: OICMiddle.cpp OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) OICMiddle.cpp $(CXX_INC)

Client.o: Client.cpp Client.h OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) Client.cpp $(CXX_INC)

Server.o: Server.cpp Server.h OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) Server.cpp $(CXX_INC)

WrapResource.o: WrapResource.cpp WrapResource.h OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) WrapResource.cpp $(CXX_INC)
	
LineInput.o: LineInput.cpp LineInput.h OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) LineInput.cpp $(CXX_INC)
	
RestInput.o: RestInput.cpp RestInput.h OICMiddle.h
	$(CXX) -c $(CXX_FLAGS.$(BUILD)) RestInput.cpp $(CXX_INC)
	
clean:
	rm $(OBJS)
	rm -rf debug
	rm -rf release

