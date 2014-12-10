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

BIN_DIR=../bin
LIB_DIR=../lib

INC=-I../include/

LIB=$(LIB_DIR)/oc_logger.a

BUILD := release

PLATFORM := linux

ifeq ($(PLATFORM),android)
	CXX=arm-linux-androideabi-g++
	CXX_FLAGS.debug=-Wall -std=c++11 -O0 -g -Wno-unused
	CXX_FLAGS.release=-Wall -std=c++11 -O2 -Wno-unused
	NDK_INC0=$(NDK_DIR)/platforms/android-19/arch-arm/usr/include
	NDK_INC1=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/include
	NDK_INC2=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include
	INC+=-I$(NDK_INC0)
	INC+=-I$(NDK_INC1)
	INC+=-I$(NDK_INC2)
	INC+=-I../../../../oic-utilities/android/boost/include/
else ifeq ($(PLATFORM),linux)
	CXX=g++
	CXX_FLAGS.debug=-Wall -std=c++0x -O0 -g
	CXX_FLAGS.release=-Wall -std=c++0x -O2
else
	$(error Wrong value for PLATFORM !!)
endif

CXX_X=$(CXX) $(CXX_FLAGS.$(BUILD)) $(INC)

all: dirs targets

dirs:
	-mkdir -p $(BIN_DIR)/targets

targets:
	$(CXX_X) -c -o $(BIN_DIR)/targets/oc_ostream_logger.o oc_ostream_logger.cpp

clean:
	rm -f ./test_logging ./*.o
