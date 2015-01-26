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
CXX       := g++
#CXX	  := clang

ifeq ($(ROOT_DIR), )
	ROOT_DIR := $(PWD)
endif
ifeq ($(PLATFORM), )
	PLATFORM := "linux"
endif

DEPEND_DIR:= $(ROOT_DIR)/dependencies
CEREAL_DIR:= $(DEPEND_DIR)/cereal

OUT_DIR	  := $(ROOT_DIR)/$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj
DEST_LIB_DIR?=${DESTDIR}/opt/iotivity

CXX_FLAGS.debug     := -g3 -std=c++0x -Wall -pthread -O0
CXX_FLAGS.release   := -std=c++0x -Wall -pthread -O3

CXX_INC	  := -I./include/
CXX_INC	  += -I./oc_logger/include

CXX_INC	  += -I./csdk/stack/include
CXX_INC	  += -I./csdk/ocsocket/include
CXX_INC	  += -I./csdk/ocrandom/include
CXX_INC	  += -I./csdk/logger/include
CXX_INC	  += -I./csdk/libcoap
CXX_INC   += -I$(CEREAL_DIR)/include
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

liboc.a: OCPlatform_impl.o OCPlatform.o OCResource.o OCException.o OCUtilities.o InProcServerWrapper.o InProcClientWrapper.o OCRepresentation.o
	ar -cvq $(OBJ_DIR)/liboc.a $(OBJ_DIR)/OCPlatform_impl.o $(OBJ_DIR)/OCPlatform.o $(OBJ_DIR)/OCResource.o $(OBJ_DIR)/OCException.o $(OBJ_DIR)/OCUtilities.o $(OBJ_DIR)/InProcServerWrapper.o $(OBJ_DIR)/InProcClientWrapper.o $(OBJ_DIR)/OCRepresentation.o

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

install: examples/$(BUILD)
	for file in $</* ; do \
	install -d $(DEST_LIB_DIR)/examples ; \
	install $${file} $(DEST_LIB_DIR)/examples/ ; \
	done
