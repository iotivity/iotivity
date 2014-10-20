# override with `make BUILD=debug`
# default to release build
BUILD	  := release
CXX	      := g++
#CXX	  := clang
OUT_DIR	  := $(PWD)/$(BUILD)
OBJ_DIR	  := $(OUT_DIR)/obj

CXX_FLAGS.debug     := -g3 -std=c++0x -Wall -pthread -O0 
CXX_FLAGS.release   := -std=c++0x -Wall -pthread -O3 

CXX_INC	  := -I./include/
CXX_INC	  += -I./csdk/stack/include
CXX_INC	  += -I./csdk/ocsocket/include
CXX_INC	  += -I./csdk/ocrandom/include
CXX_INC	  += -I./csdk/logger/include
CXX_INC	  += -I./csdk/libcoap

# Force metatargets to build:
.PHONY: prep_dirs c_sdk liboc.a examples 

all:	.PHONY

prep_dirs:
	-mkdir -p $(OUT_DIR)
	-mkdir -p $(OBJ_DIR)

c_sdk: 
	cd csdk && $(MAKE) "BUILD=$(BUILD)"

examples: 
	cd examples && $(MAKE) "BUILD=$(BUILD)"

liboc.a: OCPlatform.o OCResource.o OCReflect.o OCUtilities.o InProcServerWrapper.o InProcClientWrapper.o
	ar -cvq $(OBJ_DIR)/liboc.a $(OBJ_DIR)/OCPlatform.o $(OBJ_DIR)/OCResource.o $(OBJ_DIR)/OCReflect.o $(OBJ_DIR)/OCUtilities.o $(OBJ_DIR)/InProcServerWrapper.o $(OBJ_DIR)/InProcClientWrapper.o

OCReflect.o: src/OCReflect.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCReflect.cpp $(CXX_INC)

OCPlatform.o: src/OCPlatform.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCPlatform.cpp $(CXX_INC)

OCResource.o: src/OCResource.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCResource.cpp $(CXX_INC)

OCUtilities.o: src/OCUtilities.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/OCUtilities.cpp $(CXX_INC)

InProcServerWrapper.o: src/InProcServerWrapper.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/InProcServerWrapper.cpp $(CXX_INC)

InProcClientWrapper.o: src/InProcClientWrapper.cpp
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OBJ_DIR)/$@ -c src/InProcClientWrapper.cpp $(CXX_INC)

clean: clean_legacy
	-rm -rf release
	-rm -rf debug
	cd csdk && $(MAKE) clean
	cd csdk && $(MAKE) deepclean
	cd examples && $(MAKE) clean
clean_legacy:
	-rm -f -v $(OBJ_DIR)/liboc.a $(OBJ_DIR)/*.o 
