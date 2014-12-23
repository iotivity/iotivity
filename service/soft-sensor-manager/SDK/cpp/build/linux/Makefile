-include ../../../../build/linux/root_path.inc
-include ../../../../build/linux/environment.mk

BOOST=${BOOST_BASE}
SRC_PATH=../../src
INC_PATH=../../include
RST_NAME=release

GAR=ar

# Insert your project name.
TARGET=${SSM_LIB}

CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread -DLINUX
CXX_INC=-I../../ -I${INC_PATH}/ -I${FD_SSMCORE}/include -I${FD_SSMCORE}/src -I${BOOST}
CXX_LIB= 

SRCLIST=${wildcard ${SRC_PATH}/*.cpp}
OBJPATH=${SRCLIST:.cpp=.o}
OBJLIST=${notdir ${OBJPATH}}

# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job

pre_job:
	@echo " " 
	@echo "--------- SDK Build Begin. ----------" 
	@mkdir -p ${RST_NAME}
	@echo " "

${TARGET}: ${OBJLIST}
	${GAR} -r ./${RST_NAME}/$@  ./*.o
	@cp -Rdp ${INC_PATH}/*.h ./${RST_NAME}/
	@echo " " 

%.o : ${SRC_PATH}/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o $@
	@echo " " 

post_job:
	@echo " " 
	@cp -Rdp ./${RST_NAME}/* ${OUTPUTS_DIR}/
	@echo "--------- SDK Build Successful. ---------"
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 
	
	
clean:
	rm -f -v *.o ${TARGET}
	rm -rf ./${RST_NAME}
	
