TGMROOT=../../../
IOT_BASE=${TGMROOT}../../resource
RST_NAME=.
TARGET1=con-server
TARGET2=con-client
TARGET3=bootstrapserver

OBJS = ConfigurationCollection.o DiagnosticsCollection.o FactorySetCollection.o con-server.o
SRCS = $(OBJS:.o=.c)

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

CXX_INC	:= -I../../ -I../../inc/ -I../../../sdk/inc/ -I../../../sdk/src/
CXX_INC	+= -I${IOT_BASE}/include/
CXX_INC 	+= -I${IOT_BASE}/oc_logger/include
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

LD_LIB := $(IOT_BASE)/release/obj/liboc.a
LD_LIB += $(IOT_BASE)/csdk/linux/release/liboctbstack.a
LD_LIB += $(LIB_OC_LOGGER)


# Force metatargets to build:
.PHONY: all clean

all: ${TARGET1} ${TARGET2} ${TARGET3}

.cpp.o:
	$(CXX) $(CXX_FLAGS) -c -o $@ $<  $(CXX_INC)  $(TGM_INC)

$(TARGET1): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ ${OBJS} $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

${TARGET2}: con-client.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET2} $<  $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)

${TARGET3}: bootstrapserver.cpp
	$(CXX) $(CXX_FLAGS) -o ${TARGET3} $<  $(CXX_INC)  ${LD_LIB} $(TGM_INC) $(SDK_LIB)


dep:
	gccmakedep $(SRCS)

clean:
	rm -f -v *.o con-client
	rm -f -v *.o con-server
	rm -f -v *.o bootstrapserver

#dep:
#	gccmakedep $(SRCS)
