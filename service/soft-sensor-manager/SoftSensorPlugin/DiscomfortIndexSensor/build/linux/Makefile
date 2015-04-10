-include ../../../../build/linux/root_path.inc
-include ../../../../build/linux/environment.mk

SRC_PATH=../../src
INC_PATH=../../include
RST_NAME=release

# Insert your project name.
TARGET=DiscomfortIndexSensor

CXX=g++
CXX_FLAGS=-std=c++0x -Wall -DLINUX
CXX_INC=-I${INC_PATH} -I${OUTPUTS_DIR}

SRCLIST=${wildcard ${SRC_PATH}/*.cpp}
OBJPATH=${SRCLIST:.cpp=.o}
OBJLIST=${notdir ${OBJPATH}}

# Force metatargets to build:
.PHONY: all clean

all: pre_job ${TARGET} post_job

pre_job:
	@echo " " 
	@echo "---------- ${TARGET} Build Begin. ------------" 
	@mkdir -p ${RST_NAME}
	@echo " "

${TARGET}: ${OBJLIST}
	$(CXX) -shared -o ./${RST_NAME}/lib$@.so $^
	@echo " "

%.o : ${SRC_PATH}/%.cpp 
	${CXX} ${CXX_FLAGS} -c $< ${CXX_INC} -o $@
	@echo " "

post_job:
	@echo " " 
	cp -Rdp ./${RST_NAME}/lib${TARGET}.so ${OUTPUTS_DIR}/
	cp -Rdp ../../../${SSXML_NAME} ${OUTPUTS_DIR}/
	@echo "-------------- ${TARGET} Build Successful. -------------"
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 
	
	
clean:
	rm -f -v *.o ${TARGET}
	rm -rf ./${RST_NAME}
	
