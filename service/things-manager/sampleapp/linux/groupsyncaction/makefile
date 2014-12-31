
TGMROOT=../../../
IOT_BASE=${TGMROOT}../../resource
RST_NAME=.
TARGET1=group
TARGET2=phone
TARGET3=musicplayer
TARGET4=speaker
BUILD:=release

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

CXX_INC	:= -I../../ -I../../inc/ -I../../../sdk/inc/ -I../../../sdk/src/
CXX_INC	+= -I${IOT_BASE}/include/
CXX_INC += -I${IOT_BASE}/oc_logger/include
CXX_INC	+= -I${IOT_BASE}/csdk/stack/include
CXX_INC	+= -I${IOT_BASE}/csdk/ocsocket/include
CXX_INC	+= -I${IOT_BASE}/csdk/ocrandom/include
CXX_INC	+= -I${IOT_BASE}/csdk/logger/include
CXX_INC	+= -I${IOT_BASE}/dependencies/cereal/include

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

LD_LIB := $(IOT_BASE)/$(BUILD)/obj/liboc.a
LD_LIB += $(IOT_BASE)/csdk/linux/$(BUILD)/liboctbstack.a
LD_LIB += $(LIB_OC_LOGGER)


# Force metatargets to build:
.PHONY: all clean

all: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}

${TARGET1}: ./group.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET1} ./group.cpp   $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

${TARGET2}: ./phone.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET2} ./phone.cpp   $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

${TARGET3}: ./musicplayer.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET3} ./musicplayer.cpp   $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

${TARGET4}: ./speaker.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET4} ./speaker.cpp   $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

clean:
	rm -f -v *.o ${TARGET1}
	rm -f -v *.o ${TARGET2}
	rm -f -v *.o ${TARGET3}
	rm -f -v *.o ${TARGET4}
