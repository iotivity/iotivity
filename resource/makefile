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
# override with `make BUILD=debug`
# default to release build
BUILD     := release

ifeq ($(ROOT_DIR), )
	ROOT_DIR := $(PWD)
endif

PLATFORM := linux

DEPEND_DIR:= $(ROOT_DIR)/dependencies
CEREAL_DIR:= $(DEPEND_DIR)/cereal

OUT_DIR	  := $(ROOT_DIR)/$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj


CXX_INC	  := -I./include/
CXX_INC	  += -I./oc_logger/include

CXX_INC	  += -I./csdk/stack/include
CXX_INC	  += -I./csdk/ocsocket/include
CXX_INC	  += -I./csdk/ocrandom/include
CXX_INC	  += -I./csdk/logger/include
CXX_INC	  += -I./csdk/libcoap
CXX_INC   += -I$(CEREAL_DIR)/include

ifeq ($(PLATFORM),android)
	AR:=arm-linux-androideabi-ar
	CXX:=arm-linux-androideabi-g++
	CXX_FLAGS.debug     := -g3 -std=c++11 -Wall -O0 -DTB_LOG -Wno-unused
	CXX_FLAGS.release   := -std=c++11 -Wall -O3 -Wno-unused
	NDK_INC0=$(NDK_DIR)/platforms/android-19/arch-arm/usr/include
	NDK_INC1=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/include
	NDK_INC2=$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include

	CXX_INC+=-I$(NDK_INC0)
	CXX_INC+=-I$(NDK_INC1)
	CXX_INC+=-I$(NDK_INC2)
	CXX_INC+=-I../../oic-utilities/android/boost/include/
else ifeq ($(PLATFORM),linux)
	AR 		  := ar
	CXX       := g++
	CXX_FLAGS.debug     := -g3 -std=c++0x -Wall -pthread -O0
	CXX_FLAGS.release   := -std=c++0x -Wall -pthread -O3
	#CXX	  := clang
else
   $(error Wrong value for PLATFORM !!)
endif

# Force metatargets to build:
all.PHONY: applyDepPatches prep_dirs c_sdk oc_logger_target liboc.a examples

buildScript_all.PHONY: applyDepPatches prep_dirs oc_logger_target liboc.a

all:	all.PHONY

buildScript_all:  buildScript_all.PHONY

prep_dirs: deps
	-mkdir -p $(OUT_DIR)
	-mkdir -p $(OBJ_DIR)

# used to fetch all dependencies
deps:
	-mkdir -p $(DEPEND_DIR)
#cereal fetch
	if [ ! -d "$(CEREAL_DIR)" ]; then\
		cd $(DEPEND_DIR) && git clone https://github.com/USCiLab/cereal.git;\
		cd $(CEREAL_DIR) && git checkout 7121e91e6ab8c3e6a6516d9d9c3e6804e6f65245;\
	fi

applyDepPatches: deps
#reset git to the 'base version', so we can apply our patch without issues
	cd $(CEREAL_DIR) && git reset --hard 7121e91e6ab8c3e6a6516d9d9c3e6804e6f65245;
	cd $(CEREAL_DIR) && git apply $(ROOT_DIR)/patches/cereal_gcc46.patch

c_sdk:
	cd csdk && $(MAKE) "BUILD=$(BUILD)" "PLATFORM=$(PLATFORM)"

oc_logger_target:
	cd oc_logger && $(MAKE) "BUILD=$(BUILD)"

cpp_sdk: prep_dirs c_sdk liboc.a

examples: liboc.a
	cd examples && $(MAKE) apps "BUILD=$(BUILD)"

ifeq ($(PLATFORM),android)
liboc.a: OCAndroid.o OCPlatform_impl.o OCPlatform.o OCResource.o OCException.o OCUtilities.o InProcServerWrapper.o InProcClientWrapper.o OCRepresentation.o
	$(AR) -cvq $(OBJ_DIR)/liboc.a  $(OBJ_DIR)/OCAndroid.o $(OBJ_DIR)/OCPlatform_impl.o $(OBJ_DIR)/OCPlatform.o $(OBJ_DIR)/OCResource.o $(OBJ_DIR)/OCException.o $(OBJ_DIR)/OCUtilities.o $(OBJ_DIR)/InProcServerWrapper.o $(OBJ_DIR)/InProcClientWrapper.o $(OBJ_DIR)/OCRepresentation.o

OCAndroid.o: src/OCAndroid.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCAndroid.cpp $(CXX_INC)
else
liboc.a: OCPlatform_impl.o OCPlatform.o OCResource.o OCException.o OCUtilities.o InProcServerWrapper.o InProcClientWrapper.o OCRepresentation.o
	$(AR) -cvq $(OBJ_DIR)/liboc.a  $(OBJ_DIR)/OCPlatform_impl.o $(OBJ_DIR)/OCPlatform.o $(OBJ_DIR)/OCResource.o $(OBJ_DIR)/OCException.o $(OBJ_DIR)/OCUtilities.o $(OBJ_DIR)/InProcServerWrapper.o $(OBJ_DIR)/InProcClientWrapper.o $(OBJ_DIR)/OCRepresentation.o
endif

OCPlatform_impl.o: src/OCPlatform_impl.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCPlatform_impl.cpp $(CXX_INC)

OCPlatform.o: src/OCPlatform.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCPlatform.cpp $(CXX_INC)

OCRepresentation.o: src/OCRepresentation.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCRepresentation.cpp $(CXX_INC)

OCResource.o: src/OCResource.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCResource.cpp $(CXX_INC)

OCException.o: src/OCException.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCException.cpp $(CXX_INC)

OCUtilities.o: src/OCUtilities.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCUtilities.cpp $(CXX_INC)

InProcServerWrapper.o: src/InProcServerWrapper.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/InProcServerWrapper.cpp $(CXX_INC)

InProcClientWrapper.o: src/InProcClientWrapper.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/InProcClientWrapper.cpp $(CXX_INC)

clean: clean_legacy
	-rm -rf release
	-rm -rf debug
	cd oc_logger && $(MAKE) clean

clean_cpp_sdk: clean_legacy
	-rm -rf release
	-rm -rf debug

clean_legacy:
	-rm -f -v $(OBJ_DIR)/liboc.a $(OBJ_DIR)/*.o
	cd csdk && $(MAKE) clean
	cd csdk && $(MAKE) deepclean
