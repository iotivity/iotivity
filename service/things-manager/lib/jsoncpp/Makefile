
#IOT_BASE=../../../oic-resource
#BOOST_ROOT=../../../
#BOOST_DIR = $(BOOST_ROOT)boost_1_51_0
#BOOST=${BOOST_DIR}
RST_NAME=.

# Insert your project name.
TARGET=libjsoncpp.a
#EXCLUDE_LIST=SSMCore_JNI.cpp
EXCLUDE_LIST=

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl


CXX_INC=-I./ -I./inc/
CXX_LIB=-L""

CXX_SRCPATH=${wildcard **/*.cpp}
CXX_SRCLIST=${notdir ${CXX_SRCPATH}}
CXX_USESRCS=${filter-out ${EXCLUDE_LIST}, ${CXX_SRCLIST}}
CXX_OBJLIST=${CXX_USESRCS:.cpp=.o}


# Linker FLAGS define.
# LIBS=-lajdaemon -lalljoyn -lssl -lcrypto -ldl -llog -lz -lm -lc -lstdc++  -lgcc -lgnustl_static
# LD_FLAGS=-std=c++11 -Wall -shared -lpthread -ldl
GAR=ar
#LD_LIB=${IOT_BASE}/OCLib.a ${IOT_BASE}/csdk/liboctbstack.a 


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

		
%.o : **/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o ./${RST_NAME}/$@
	@echo " " 
	
post_job:
	@echo " " 
	@echo "Build Successful."
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 


clean:
	rm -f -v *.o ${TARGET}

	
