
TGMROOT=../../../
IOT_BASE=${TGMROOT}../../oic-resource
BOOST_ROOT=${TGMROOT}../../../
BOOST_DIR = $(BOOST_ROOT)boost_1_51_0
RST_NAME=.
TARGET=bookmark


# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

CXX_INC=-I../../ -I../../inc/ -I${IOT_BASE}/include/ -I${IOT_BASE}/csdk/stack/include -I${IOT_BASE}/csdk/ocsocket/include -I${IOT_BASE}/csdk/ocrandom/include -I${IOT_BASE}/csdk/logger/include -I${BOOST_DIR} 
CXX_LIB=-L""

CXX_SRCPATH=${wildcard ../../src/*.cpp}
CXX_SRCLIST=${notdir ${CXX_SRCPATH}}
CXX_USESRCS=${filter-out ${EXCLUDE_LIST}, ${CXX_SRCLIST}}
CXX_OBJLIST=${CXX_USESRCS:.cpp=.o}

JSON = ${TGMROOT}lib/inc
JSONCPP = ${TGMROOT}lib/jsoncpp/inc
JSONLIB = ${TGMROOT}lib/jsoncpp

TGM = ../../
SDK = ${TGMROOT}sdk
TGM_INC = -I${TGMROOT}lib/inc -D_DEBUG -I${TGMROOT}lib/jsoncpp/inc -I${TGMROOT}sdk/inc -I${TGMROOT}tgm/inc

#LD_LIB = $(IOT_BASE)/release/obj/OCLib.a  $(IOT_BASE)/csdk/release/liboctbstack.a ${TGMROOT}SDK/build/linux/libTGMSDKLibrary.a ${TGMROOT}TGM/build/linux/libTGMClient.a ${TGMROOT}lib/jsoncpp/libjsoncpp.a
LD_LIB = $(IOT_BASE)/build/linux/release/obj/liboc.a  $(IOT_BASE)/csdk/build/linux/release/liboctbstack.a ${TGMROOT}sdk/build/linux/libTGMSDKLibrary.a ${TGMROOT}tgm/build/linux/libTGMClient.a ${TGMROOT}lib/jsoncpp/libjsoncpp.a

# Force metatargets to build:
.PHONY: all clean

all: ${TARGET} 

${TARGET}: ./bookmark.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET} ./bookmark.cpp $(SDK)/src/TGMClient.cpp ${LD_LIB} $(CXX_INC) $(TGM_INC) 
	
clean:
	rm -f -v *.o ${TARGET}
	
