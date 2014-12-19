
TGMROOT=../../../
IOT_BASE=${TGMROOT}../../resource
RST_NAME=.



# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

DEPEND_DIR:= ../../../../../resource/dependencies/
CEREAL_DIR:= $(DEPEND_DIR)/cereal

CXX_INC	:= -I../../ -I../../inc/
CXX_INC	+= -I${IOT_BASE}/include/
CXX_INC	+= -I${IOT_BASE}/oc_logger/include
CXX_INC	+= -I${IOT_BASE}/csdk/stack/include
CXX_INC	+= -I${IOT_BASE}/csdk/ocsocket/include
CXX_INC	+= -I${IOT_BASE}/csdk/ocrandom/include
CXX_INC	+= -I${IOT_BASE}/csdk/logger/include
CXX_INC	+= -I$(CEREAL_DIR)/include
CXX_INC	+= -I../../../sdk/inc
CXX_INC	+= -I../../../sdk/src


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

all: bookmark lightserver groupserver

#groupclient
bookmark: ./bookmark.cpp
	$(CXX) $(CXX_FLAGS) -o bookmark ./bookmark.cpp $(CXX_INC) ${LD_LIB} $(TGM_INC) $(SDK_LIB)

lightserver: ./lightserver.cpp
	$(CXX) $(CXX_FLAGS) -o lightserver ./lightserver.cpp $(CXX_INC) ${LD_LIB} $(TGM_INC) $(SDK_LIB)

groupserver: ./groupserver.cpp
	$(CXX) $(CXX_FLAGS) -o groupserver ./groupserver.cpp $(CXX_INC) ${LD_LIB} $(TGM_INC) $(SDK_LIB)

# groupclient: ./groupclient.cpp
# 	$(CXX) $(CXX_FLAGS) -o groupclient ./groupclient.cpp $(CXX_INC) ${LD_LIB} $(TGM_INC) $(SDK_LIB)

clean:
	rm -f -v *.o ${TARGET}
	rm -f groupserver
	rm -f groupclient
	rm -f lightserver
	rm -f bookmark
