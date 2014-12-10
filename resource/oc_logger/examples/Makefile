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

BUILD := release

PLATFORM := linux

ifeq ($(PLATFORM),android)
	CC=arm-linux-androideabi-gcc
	CXX=arm-linux-androideabi-g++

	CC_FLAGS.debug=-Wall -std=c99 -Werror -O0 -g --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm
	CC_FLAGS.release=-Wall -std=c99 -Werror -O2 --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm  -L$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/

	CXX_FLAGS.debug=-Wall -std=c++11 -O0 -g --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm -Wno-unused
	CXX_FLAGS.release=-Wall -std=c++11 -O2 --sysroot=$(NDK_DIR)/platforms/android-19/arch-arm -L$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/ -Wno-unused
	CXX_STDLIB=-lstdc++ -lgnustl_static -lsupc++

	NDK_INC0=$(NDK_DIR)/platforms/android-19/arch-arm/usr/include
	NDK_INC1=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/include
	NDK_INC2=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include

	INC=-I../include/
	INC+=-I$(NDK_INC0)
	INC+=-I$(NDK_INC1)
	INC+=-I$(NDK_INC2)
	INC+=-I../../../../oic-utilities/android/boost/include/
else ifeq ($(PLATFORM),linux)
	CC_FLAGS.debug=-Wall -std=c99 -Werror -O0 -g
	CC_FLAGS.release=-Wall -std=c99 -Werror -O2

	CXX_FLAGS.debug=-Wall -std=c++0x -O0 -g
	CXX_FLAGS.release=-Wall -std=c++0x -O2

	CXX_STDLIB=-lstdc++

	CC:=gcc
	CXX:=g++
	INC=-I../include/
else
	$(error Wrong value for PLATFORM !!)
endif

CXX_X=$(CXX) $(CXX_FLAGS.$(BUILD)) $(INC)


BIN_DIR=../bin
LIB_DIR=../lib

CC_LIB=$(LIB_DIR)/oc_logger.a $(CXX_STDLIB)
CXX_LIB=$(LIB_DIR)/oc_logger.a $(CXX_STDLIB)

CCX=$(CC) $(CC_FLAGS.$(BUILD))

all: examples_c examples_cpp

examples_c:
	$(CCX) $(INC) -o $(BIN_DIR)/test_logging test_logging.c $(CC_LIB)

examples_cpp:
	$(CXX_X) -o $(BIN_DIR)/test_logging_cpp test_logging.cpp $(CXX_LIB)

clean:
	rm -f $(BIN_DIR)/test_logging $(BIN_DIR)/test_logging_cpp

