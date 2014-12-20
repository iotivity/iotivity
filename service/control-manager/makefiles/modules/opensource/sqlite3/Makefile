#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	sqlite3

CONFIG				?= 	release

BUILD_DIR			?= ../../../../build

#configure linker program
LINKER ?=	$(CXX)

#configure Source Root Directory
SRC_DIR =	../../../../opensource/sqlite3/src

override BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
override OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure object directories (sub directories to store obj files of included source files)
override OBJ_DIRS = 

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS +=
override CPP_INC_DIR =
override SRC_CPP_FILES = 

#configure C Files, include paths, c compiler option
override CFLAGS +=
override C_INC_DIR = -I../../../../opensource/sqlite3/include
override SRC_C_FILES = sqlite3.c

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
override LDFLAGS +=
override LDLIBS +=

#configure target names
STATIC_LIB_TARGET	=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}

include ../../../BuildTargets.mk
