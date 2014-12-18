-include ../../../../build/linux/root_path.inc
-include ../../../../build/linux/environment.mk

BOOST=${BOOST_BASE}
SRC_PATH=../src
INC_PATH=../include
RST_NAME=release

# Insert your project name.
TARGET=SSMTesterApp

CXX=g++
CXX_FLAGS=-std=c++0x -Wall -DLINUX
CXX_INC=-I${SRC_PATH}/ -I${INC_PATH}/ -I${OUTPUTS_DIR} -I${BOOST}


CXX_LIB+=${OUTPUTS_DIR}/${SSM_LIB} 
CXX_LIB+=${OUTPUTS_DIR}/${SSMCORE_LIB} 
CXX_LIB+=${IOT_RELEASE}/${IOT_LIB}
CXX_LIB+=${IOT_CSDK_RELEASE}/${IOT_CSDK_LIB} 
CXX_LIB+=${IOT_LOG_DIR}/lib/${IOT_LOG_LIB}
CXX_LIB+=-lpthread -ldl

SRCLIST=${wildcard ${SRC_PATH}/*.cpp}
OBJPATH=${SRCLIST:.cpp=.o}
OBJLIST=${notdir ${OBJPATH}}

# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job

pre_job:
	@echo " " 
	@echo "--------- ${TARGET} Build Begin. -----------" 
	@mkdir -p ${RST_NAME}
	@echo " "

${TARGET}: ${OBJLIST}
	$(CXX) $(CXX_FLAGS) -o ./${RST_NAME}/$@ $^ ${CXX_LIB}
	@echo " "

%.o : ${SRC_PATH}/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o $@
	@echo " "

post_job:
	@echo " " 
	cp -Rdp ./${RST_NAME}/${TARGET} ${OUTPUTS_DIR}/
	@echo "----------- ${TARGET} Build Successful. ------------"
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 
	
	
clean:
	rm -f -v *.o ${TARGET}
	rm -rf ./${RST_NAME}
	
