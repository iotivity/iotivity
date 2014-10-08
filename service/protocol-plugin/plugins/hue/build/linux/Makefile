COMMON_DIR = ../..
include $(COMMON_DIR)/../../config.mk
BUILD = release

CXX = g++
CXX_FLAGS = -g3 -std=c++0x -Wall -pthread
LDFLAGS = -fPIC -shared
LIBS = -lpthread -lcurl
INC = .
INC+=-I../ -I$(COMMON_DIR)/lib/curl

SRC_DIR = $(COMMON_DIR)/src

SRC_CPP := $(SRC_DIR)/hue_light_sample.cpp 
SRC_CPP += $(SRC_DIR)/HuePlugin.cpp 
SRC_CPP += $(SRC_DIR)/simple_parse.cpp 
SRC_CPP += $(SRC_DIR)/OCResourceDesc.cpp 
SRC_CPP += $(SRC_DIR)/http_curl.cpp 
SRC_CPP += $(SRC_DIR)/HueLightHandler.cpp 
SRC_CPP += $(SRC_DIR)/HueBridge.cpp

OIC_DIR = $(COMMON_DIR)/../../../../oic-resource
OCLIB_DIR = $(OIC_DIR)/build/linux/$(BUILD)/obj
CSDK_DIR = $(OIC_DIR)/csdk/build/linux/$(BUILD)

CXX_INC := -I$(OIC_DIR)/include/ 
CXX_INC += -I$(OIC_DIR)/csdk/stack/include 
CXX_INC += -I$(OIC_DIR)/csdk/ocsocket/include 
CXX_INC += -I$(OIC_DIR)/csdk/ocrandom/include 
CXX_INC += -I$(OIC_DIR)/csdk/logger/include 
CXX_INC += -I$(BOOST_DIR) 
CXX_INC += -I$(OIC_DIR)/examples/lib 
CXX_INC += -I../../../../lib/cpluff/libcpluff

TARGET_DY_LIB=libplugin-hue-light.so


LDFLAGS=-fPIC -shared


.PHONY: hue-environment oic-light-client oic-light-server hue-light-plugin
all: .PHONY

hue-environment:
	-mkdir $(BUILD)
	cp plugin.xml $(BUILD)/

oic-light-client: $(SRC_DIR)/test/client_test.cpp $(OCLIB_DIR)/liboc.a
	$(CXX) $(CXX_FLAGS) -o $(BUILD)/oic-light-client $(SRC_DIR)/test/client_test.cpp $(OCLIB_DIR)/liboc.a $(CSDK_DIR)/liboctbstack.a $(CXX_INC)
	
oic-light-server: $(SRC_DIR)/test/server_test.cpp $(OCLIB_DIR)/liboc.a
	$(CXX) $(CXX_FLAGS) -o $(BUILD)/oic-light-server $(SRC_DIR)/test/server_test.cpp $(OCLIB_DIR)/liboc.a $(CSDK_DIR)/liboctbstack.a $(CXX_INC)

hue-light-plugin: $(SRC_CPP) $(OCLIB_DIR)/liboc.a
	$(CXX) $(CXX_FLAGS) -o $(BUILD)/$(TARGET_DY_LIB) $^ $(CSDK_DIR)/liboctbstack.a $(CXX_INC) -I $(INC) $(LDFLAGS) $(LIBS)

clean:
	rm -rf release debug


