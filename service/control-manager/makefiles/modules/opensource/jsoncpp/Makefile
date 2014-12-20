#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	jsoncpp

CONFIG				?= 	release

BUILD_DIR			?= ../../../../build

#configure linker program
LINKER 				?=	$(CXX)

#configure Source Root Directory
SRC_DIR =	../../../../opensource/jsoncpp/src

#configure Source Build Directory

override BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
override OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure object directories (sub directories to store obj files of included source files)
override OBJ_DIRS = 

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS +=
override CPP_INC_DIR = -I../../../../opensource/jsoncpp/include
override SRC_CPP_FILES = json_reader.cpp json_value.cpp json_writer.cpp

#configure C Files, include paths, c compiler option
override CFLAGS +=
override C_INC_DIR = 
override SRC_C_FILES = 

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
override LDFLAGS +=
override LDLIBS +=

#configure target names
STATIC_LIB_TARGET	=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}
SHARED_IMPORT_LIB_TARGET = $(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}.a

include ../../../BuildTargets.mk