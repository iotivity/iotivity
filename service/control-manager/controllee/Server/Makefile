#########################################################################################
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	controllee

CONFIG			?= 	debug

BUILD_DIR		?= 	/home/oic/SHPNM/OIC/Code/controllee/build

#configure linker program
LINKER 			?=	$(CXX)
CXX_FLAGS = -std=c++0x -Wall -pthread -fpermissive

#configure Source Root Directory
SRC_DIR 		=	/home/oic/SHPNM/OIC/Code/controllee/Server/
OIC_ROOT 		= 	/home/oic/oic-resource/
SHP_ROOT		=	/home/oic/SHPNM/OIC/Code/framework/

#configure Source Build Directory
BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure CPP Files, include paths, cpp compiler option
CXXFLAGS += -DCMAPI_EXPORTS=1 -DSHPPAL_API_EXPORTS=1
CPP_INC_DIR = 	-I$(SRC_DIR)../include \
		-I$(SRC_DIR)../include/api \
		-I$(SRC_DIR)../include/core \
		-I$(SRC_DIR)../include/device \
		-I$(SRC_DIR)../include/Server \
		-I$(SRC_DIR)../include/Serialization \
		-I$(SRC_DIR)../include/utils \
		-I$(SRC_DIR)../include/xsd
OIC_INC_DIR =	-I$(OIC_ROOT)include \
		-I$(OIC_ROOT)csdk/stack/include \
		-I$(OIC_ROOT)oic-resource/csdk/occoap/include \
		-I$(OIC_ROOT)csdk/ocrandom/include \
		-I$(OIC_ROOT)csdk/ocsocket/include \
		-I$(OIC_ROOT)oc_logger/include

ALL_INC = $(CPP_INC_DIR)
ALL_INC += $(OIC_INC_DIR)

SRC_CPP_FILES = $(wildcard $(SRC_DIR)*.cpp)
DD_OBJ = $(DD_SRC:.c=.o)

OC_LIB = /home/oic/oic-resource/release/obj/liboc.a /home/oic/oic-resource/csdk/release/liboctbstack.a

MAIN = Server

.PHONY : all static_lib

all: $(MAIN) static_lib

$(MAIN): $(DD_OBJ)
	 $(CXX) $(CXX_FLAGS) $(ALL_INC) $(SRC_CPP_FILES) -c

static_lib:
	ar -cvq $(SRC_DIR)libServer.a $(SRC_DIR)*.o 

#.c.o : 
#	 $(CXX) $(CXX_FLAGS) $(ALL_INC) -c $< -o $@ 

clean:
	rm -rf *.o

# depend: $(DD_SRC)
#	makedepend $(ALL_INC) $^

# DO NOT DELETE THIS LINE
