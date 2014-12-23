-include ../../../build/linux/root_path.inc
-include ../../../build/linux/environment.mk

BOOST=${BOOST_BASE}
SRC_PATH=../../src
INC_PATH=../../include
RST_NAME=release

# Insert your project name.
TARGET=${SSMCORE_LIB}
EXCLUDE_LIST=SSMCore_JNI.cpp

# C++ type Compile Flag define.
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX -ldl
CXX_INC=-I${INC_PATH}/ -I${SRC_PATH}/ -I${IOT_BASE}/include/ -I${IOT_LOG_DIR}/include/ -I${IOT_BASE}/csdk/stack/include -I${IOT_BASE}/csdk/ocsocket/include -I${IOT_BASE}/csdk/ocrandom/include -I${IOT_BASE}/csdk/logger/include -I${BOOST}
CXX_LIB=-L""

CXX_SRCPATH=${wildcard ${SRC_PATH}/**/*.cpp}
CXX_SRCPATH+=${wildcard ${SRC_PATH}/*.cpp}
CXX_SRCLIST=${notdir ${CXX_SRCPATH}}
CXX_USESRCS=${filter-out ${EXCLUDE_LIST}, ${CXX_SRCLIST}}
CXX_OBJLIST=${CXX_USESRCS:.cpp=.o}

# C type Compile Flag define.
GCC=gcc
GCC_FLAGS=-Wall -pthread -DLINUX -ldl
GCC_INC=-I../../ -I${INC_PATH}/ -I${IOT_BASE}/include/ -I${IOT_BASE}/csdk/stack/include -I${IOT_BASE}/csdk/ocsocket/include -I${IOT_BASE}/csdk/ocrandom/include -I${IOT_BASE}/csdk/logger/include -I${BOOST} -I${SRC_PATH} -I${SRC_PATH}/Common -I${SRC_PATH}/QueryProcessor -I${SRC_PATH}/SensorProcessor -I${SRC_PATH}/SSMInterface

GCC_SRCPATH=${wildcard ${SRC_PATH}/**/*.c}
GCC_SRCLIST=${notdir ${GCC_SRCPATH}}
GCC_USESRCS=${filter-out ${EXCLUDE_LIST}, ${GCC_SRCLIST}}
GCC_OBJLIST=${GCC_USESRCS:.c=.o}

# Linker FLAGS define.
LD_FLAGS=-std=c++0x -Wall -DLINUX
GAR=ar
# LD_LIB=${IOT_RELEASE}/${IOT_LIB} ${IOT_CSDK_RELEASE}/${IOT_CSDK_LIB} ${IOT_LOG_DIR}/lib/${IOT_LOG_LIB} -lpthread -ldl 


# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job

pre_job:
	@echo " " 
	@echo "---------- SSMCore Build Begin. -------------" 
	@mkdir -p ${RST_NAME}
	@echo " "
	@echo "${MODEL_DIR}"
	
${TARGET}: ${GCC_OBJLIST} ${CXX_OBJLIST}
	# $(CXX) $(LD_FLAGS) -o ./${RST_NAME}/$@ $^ ${LD_LIB}		
	${GAR} -r ./${RST_NAME}/$@  ./*.o
	@echo " "

%.o : ${SRC_PATH}/**/%.c 
	${GCC} ${GCC_FLAGS} -c $< ${GCC_INC} -o $@
	@echo " " 
		
%.o : ${SRC_PATH}/**/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o $@
	@echo " " 
	
%.o : ${SRC_PATH}/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o $@
	@echo " "

post_job:
	@echo " " 
	cp -Rdp ${INC_PATH}/SSMInterface.h ${FD_SDK}/include/
	cp -Rdp ./${RST_NAME}/${TARGET} ${OUTPUTS_DIR}/
	cp -Rdp ${INC_PATH}/SSMInterface.h ${OUTPUTS_DIR}/
	cp -Rdp ${SRC_PATH}/SSMInterface/SSMModelDefinition.h ${OUTPUTS_DIR}/
	@echo "---------------- SSMCore Build Successful. ---------------"
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 


clean:
	rm -f -v *.o ${TARGET}
	rm -rf ./${RST_NAME}
	
