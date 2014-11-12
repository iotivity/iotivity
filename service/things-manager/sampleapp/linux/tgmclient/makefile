
TGMROOT=../../../
IOT_BASE=${TGMROOT}../../resource
RST_NAME=.
TARGET=tgmclient


# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

CXX_INC	:= -I../../ -I../../inc/ 
CXX_INC	+= -I${IOT_BASE}/include/ 
CXX_INC += -I${IOT_BASE}/oc_logger/include
CXX_INC	+= -I${IOT_BASE}/csdk/stack/include 
CXX_INC	+= -I${IOT_BASE}/csdk/ocsocket/include 
CXX_INC	+= -I${IOT_BASE}/csdk/ocrandom/include 
CXX_INC	+= -I${IOT_BASE}/csdk/logger/include 

CXX_LIB=-L""

CXX_SRCPATH=${wildcard ../../src/*.cpp}
CXX_SRCLIST=${notdir ${CXX_SRCPATH}}
CXX_USESRCS=${filter-out ${EXCLUDE_LIST}, ${CXX_SRCLIST}}
CXX_OBJLIST=${CXX_USESRCS:.cpp=.o}


TGM = ../../
SDK = ${TGMROOT}sdk
TGM_INC = -I${TGMROOT}sdk/inc
SDK_LIB = ${TGMROOT}sdk/build/linux/libTGMSDKLibrary.a

LIB_OC_LOGGER := $(IOT_BASE)/oc_logger/lib/oc_logger.a

LD_LIB := $(IOT_BASE)/release/obj/liboc.a  
LD_LIB += $(IOT_BASE)/csdk/linux/release/liboctbstack.a 
LD_LIB += $(LIB_OC_LOGGER)


# Force metatargets to build:
.PHONY: all clean

all: ${TARGET} 

${TARGET}: ./tgmsdkclient.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET} ./tgmsdkclient.cpp   $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)
	
clean:
	rm -f -v *.o ${TARGET}
	
