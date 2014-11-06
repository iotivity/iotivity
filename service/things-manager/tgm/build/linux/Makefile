

#OIC_ROOT=../../../resource/
IOT_BASE=../../../../../resource
BOOST_ROOT=../../../../../../
BOOST_DIR = $(BOOST_ROOT)boost_1_51_0
#BOOST=${BOOST_DIR}
RST_NAME=.


# Insert your project name.
TARGET=libTGMClient.a
#EXCLUDE_LIST=SSMCore_JNI.cpp
EXCLUDE_LIST=tgmserver.cpp

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl

CXX_INC=-I../../ -I../../inc/ -I${IOT_BASE}/include/ -I${IOT_BASE}/csdk/stack/include -I${IOT_BASE}/csdk/ocsocket/include -I${IOT_BASE}/csdk/ocrandom/include -I${IOT_BASE}/csdk/logger/include -I${BOOST_DIR} 
CXX_LIB=-L""

CXX_SRCPATH=${wildcard ../../src/*.cpp}
CXX_SRCLIST=${notdir ${CXX_SRCPATH}}
CXX_USESRCS=${filter-out ${EXCLUDE_LIST}, ${CXX_SRCLIST}}
CXX_OBJLIST=${CXX_USESRCS:.cpp=.o}


JSON = ../../../lib/inc
JSONCPP = ../../../lib/jsoncpp/inc
JSONLIB = ../../../lib/jsoncpp

TGM = ../../

#JSON_INC = -I$(TGM)common/serialize
#JSON_SRC = $(TGM)common/serialize/json/jsoncpp.cpp

TGM_INC = -I../../../lib/inc -D_DEBUG -I../../../lib/jsoncpp/inc

TGM_SRC = 	$(TGM)src/Group.cpp $(TGM)src/GroupManager.cpp 

SQLITE = ../../../lib/sqlite
SQLITEINC = -I../../../lib/sqlite
SQLITELIB = $(SQLITE)/sqlite3.o -lpthread -ldl 

# Linker FLAGS define.
# LIBS=-lajdaemon -lalljoyn -lssl -lcrypto -ldl -llog -lz -lm -lc -lstdc++  -lgcc -lgnustl_static
# LD_FLAGS=-std=c++11 -Wall -shared -lpthread -ldl
GAR=ar
#LD_LIB=${IOT_BASE}/OCLib.a ${IOT_BASE}/csdk/liboctbstack.a 


# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job tgmserver 

pre_job:
	@echo " " 
	@echo "Build Begin." 
	@echo " "

#tgmserver: ../../src/tgmserver.cpp
#	$(CXX) $(CXX_FLAGS) -o tgmserver $(IOT_BASE)/release/obj/OCLib.a  $(IOT_BASE)/csdk/release/liboctbstack.a  $(JSONLIB)/libjsoncpp.a $(CXX_SRCPATH) $(CXX_INC) -I$(JSON) -I$(JSONCPP) 

tgmserver: ../../src/tgmserver.cpp
	$(CXX) $(CXX_FLAGS) -o tgmserver ../../src/tgmserver.cpp  $(IOT_BASE)/release/obj/liboc.a  $(IOT_BASE)/csdk/release/liboctbstack.a  $(JSONLIB)/libjsoncpp.a  $(TGM_SRC) $(TGM_INC) $(CXX_INC) $(SQLITEINC) $(SQLITELIB)
	
${TARGET}: ${CXX_OBJLIST}
	#$(CXX) $(LD_FLAGS) -o ./${RST_NAME}/$@ $^ ${LD_LIB}		// shared object.
	@for sublib in ${LD_LIB} ; do \
	echo "${GAR} -x $${sublib}" ; \
	cd ./${RST_NAME} ; \
	${GAR} -x ../$${sublib} ; \
	cd ../ ; \
	done
	${GAR} -r ./${RST_NAME}/$@  ./${RST_NAME}/*.o
	@echo " "



		
%.o : ../../src/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} ${TGM_INC}  -o ./${RST_NAME}/$@
	@echo " " 
	
post_job:
	@echo " " 
	@echo "Build Successful."
	@echo " " 


clean:
	rm -f -v *.o ${TARGET}
	rm -f tgmserver
	
