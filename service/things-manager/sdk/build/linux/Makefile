

#OIC_ROOT=../../../resource/
IOT_BASE=../../../../../resource
#BOOST=${BOOST_DIR}
RST_NAME=.


# Insert your project name.
TARGET=libTGMSDKLibrary.a
EXCLUDE_LIST=

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl


CXX_INC	:= -I../../ -I../../inc/ -I../../src/
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






# Linker FLAGS define.
# LIBS=-lajdaemon -lalljoyn -lssl -lcrypto -ldl -llog -lz -lm -lc -lstdc++  -lgcc -lgnustl_static
# LD_FLAGS=-std=c++11 -Wall -shared -lpthread -ldl
GAR=ar
#LD_LIB=${IOT_BASE}/OCLib.a ${IOT_BASE}/csdk/liboctbstack.a $(JSONLIB)/libjsoncpp.a


# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job  

pre_job:
	@echo " " 
	@echo "Build Begin." 
	@echo " "
	
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
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o ./${RST_NAME}/$@
	@echo " " 
	
post_job:
	@echo " " 
	@echo "Build Successful."
	@echo " " 


clean:
	rm -f -v *.o ${TARGET}
	
