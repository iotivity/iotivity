# override with `make BUILD=release`
# default to release build
BUILD	:= release
CXX	:= g++
#CXX=clang -lstdc++ -g -O0
OUT_DIR := $(PWD)/$(BUILD)
OBJ_DIR	:= $(OUT_DIR)/obj

CXX_FLAGS.debug     := -g3 -std=c++11 -Wall -pthread

CXX_FLAGS.release   := -std=c++11 -Wall -pthread

CXX_INC	:= -I../include/
CXX_INC += -I./client/
CXX_INC += -I../csdk/stack/include
CXX_INC += -I../csdk/ocsocket/include
CXX_INC += -I../csdk/ocrandom/include
CXX_INC += -I../csdk/logger/include

LIB_SRC0 := ../OCLib/OCReflect.cpp
LIB_SRC0 += ../OCLib/OCProperties.cpp
LIB_SRC0 += ../OCLib/OCObserver.cpp
LIB_SRC0 += ../OCLib/OCResource.cpp
LIB_SRC0 += ../OCLib/OCServer.cpp

LIB_SRC1 := ../OCLib/OCReflect.cpp
LIB_SRC1 += ../OCLib/OCObserver.cpp
LIB_SRC1 += ./client/MyObserverHandler.cpp
LIB_SRC1 += ../OCLib/OCResource.cpp
LIB_SRC1 += ../OCLib/OCSecurityModel.cpp
LIB_SRC1 += ../OCLib/InProcClientWrapper.cpp
LIB_SRC1 += ../OCLib/OCPlatform.cpp
LIB_SRC1 += ../OCLib/InProcServerWrapper.cpp
LIB_SRC1 += ../csdk/liboctbstack.a
LIB_SRC1 += ../OCLib/OCServer.cpp

CXX_LIBS := -Lboost_regex

.PHONY: prep_dirs ocreflect test_OCReflect test_OCClient test_properties

all: .PHONY

prep_dirs:
	-mkdir $(OUT_DIR)
	-mkdir $(OBJ_DIR)

ocreflect: test_properties test_OCClient

test_properties:
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ reflect/test_properties.cpp $(LIB_SRC0) $(CXX_INC) $(CXX_LIBS)

test_OCReflect:
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ reflect/test_OCReflect.cpp ../OCLib/OCReflect.cpp ../OCLib/OCProperties.cpp ../OCLib/OCServer.cpp  $(CXX_INC) $(CXX_LIBS)

test_OCClient:
	$(CXX) $(CXX_FLAGS.$(BUILD)) -o $(OUT_DIR)/$@ reflect/test_OCClient.cpp $(LIB_SRC0) $(CXX_INC) $(CXX_LIBS)

clean:	clean_legacy
	-rm -rf release/*
	-rm -rf release/obj/*.o
	-rm -rf release/obj/*.a
	-rmdir release
	-rm -rf debug/*
	-rm -rf debug/obj/*.o
	-rm -rf debug/obj/*.a
	-rmdir debug

clean_legacy:
	-rm -f -v test_properties test_OCReflect test_OCClient
