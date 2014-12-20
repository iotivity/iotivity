#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################
.PHONY = all\
	controlmanager cmjni controllee samples\
	clean clean_linux clean_all clean_all_linux clean_target clean_target_linux\
clean_vars help

PROJECT_NAME		=	controlmanager

CONFIG				?= 	release

$(info Make Commmand goals are $(MAKECMDGOALS))

ifndef TARGET_OS
ifeq ($(MAKECMDGOALS), clean_vars)
else
ifeq ($(MAKECMDGOALS), help)
else
$(info TARGET_OS variable not set using linux32)
export TARGET_OS=linux32
endif
endif
endif

$(info Target os is set to $(TARGET_OS))

ifdef CROSS_HOST
export CROSS_HOST:=${CROSS_HOST}/
$(info Cross Compiling on ${CROSS_HOST})
endif

ifndef MKDIR_COMMAND
export MKDIR_COMMAND=mkdir -p
endif

ifndef COPY_COMMAND
export COPY_COMMAND=cp -u -f
endif

ifndef COPY_RECURSIVE_COMMAND
export COPY_RECURSIVE_COMMAND=cp -u -f -r
endif

ifneq ($(CONFIG),debug)
	export CONFIG=release
endif

$(info building $(CONFIG) binaries for $(TARGET_OS))

BUILD_ROOT_DIR		= build
BUILD_TARGETOS_DIR	= build/${CONFIG}/${TARGET_OS}
BUILD_CONFIG_DIR	= build/${CONFIG}
BUILD_OPENSOURCE_DIR	= build/${CONFIG}/${TARGET_OS}/opensource
BUILD_CM_DIR		= ${BUILD_TARGETOS_DIR}
BUILD_CD_DIR		= ${BUILD_CM_DIR}

#configure Output Directory
OUTPUT_ROOT_DIR		= .
OUTPUT_TARGETOS_DIR	= ./${TARGET_OS}
OUTPUT_CONFIG_DIR	= ./${TARGET_OS}/${CONFIG}

INSTALL_ROOT_DIR	= install_dir
INSTALL_TARGETOS_DIR	= install_dir/${TARGET_OS}
INSTALL_CONFIG_DIR	= install_dir/${TARGET_OS}/${CONFIG}

export OUTPUT_OPENSOURCE_DIR	= ${CONFIG}/${TARGET_OS}/opensource
export OUTPUT_CM_DIR		= ${CONFIG}/${TARGET_OS}
export OUTPUT_CD_DIR		= ${OUTPUT_CM_DIR}
export OUTPUT_OC_DIR		= oic-resource/resource
export OUTPUT_OC_UTILITY_DIR    = oic-utilities
export BOOST_PATH			= /usr/lib

####  Enable/Disable Supported Logging Type during Compile Time  ####
####  SILENT(-3) < ERROR(-2) < WARNING(-1) < INFO(0) < DEBUG(1)
####
####  To disable logging completely, Set CM_LOGGING_FLAGS as following ::
####		CM_LOGGING_FLAGS= -DCM_LOGGING_SILENT=1  	(OR)
####		CM_LOGGING_FLAGS=  				(OR)
####		CM_LOGGING_FLAGS= -DCM_LOGGING_ERROR=0 -DCM_LOGGING_WARNING=0 -DCM_LOGGING_INFO=0 -DCM_LOGGING_DEBUG=0

#CM_LOGGING_FLAGS	= -DCM_LOGGING_ERROR=1 -DCM_LOGGING_WARNING=1 -DCM_LOGGING_INFO=1 -DCM_LOGGING_DEBUG=1

#override CXXFLAGS += $(CM_LOGGING_FLAGS)
#override CFLAGS += $(CM_LOGGING_FLAGS)
override CXXFLAGS += -DENABLE_LOG

ifeq ($(CONFIG),debug)
	override CXXFLAGS += -g -DENABLE_LOG
	override CFLAGS +=	-g
endif

ifneq ($(TARGET_OS),mingw32)
ifneq ($(TARGET_OS),mingw64)
ifneq ($(TARGET_OS),mac64)
ifeq ($(CONFIG),release)
	override LDFLAGS += -s
	override CXXFLAGS += -Os
	override CFLAGS += -Os
endif
endif
endif
endif
ifeq ($(TARGET_OS),android)
	override CXXFLAGS += -std=c++11
endif
	
include makefiles/platformconfigs/$(TARGET_OS)_config.mk

ifeq ($(TARGET_OS),android)
all: cmjni
else
all: samples
endif

sqlite3: build_output_dirs
	$(info ******************************************************************************************************)
	$(info *			Building sqlite3 -> $(CONFIG) binaries for $(TARGET_OS)									*)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/opensource/sqlite3 BUILD_DIR:=../../../../$(BUILD_OPENSOURCE_DIR) OUTPUT_DIR:=../../../../$(OUTPUT_OPENSOURCE_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(OSLDFLAGS)" LDLIBS:="$(LDLIBS)"
	
jsoncpp: build_output_dirs
	$(info ******************************************************************************************************)
	$(info *			Building jsoncpp -> $(CONFIG) binaries for $(TARGET_OS)									*)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/opensource/jsoncpp BUILD_DIR:=../../../../$(BUILD_OPENSOURCE_DIR) OUTPUT_DIR:=../../../../$(OUTPUT_OPENSOURCE_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(OSLDFLAGS)" LDLIBS:="$(LDLIBS)"	
	
boost: build_output_dirs
	$(info ******************************************************************************************************)
	$(info *			Building boost -> $(CONFIG) binaries for $(TARGET_OS)									*)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/opensource/boost BUILD_DIR:=../../../../$(BUILD_OPENSOURCE_DIR) OUTPUT_DIR:=../../../../$(OUTPUT_OPENSOURCE_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(OSLDFLAGS)" LDLIBS:="$(LDLIBS)"

controllee: build_output_dirs sqlite3 jsoncpp
	$(info ******************************************************************************************************)
	$(info *                        Building Controllee -> $(CONFIG) binaries for $(TARGET_OS)                                                         *)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/controllee BUILD_DIR:=../../../$(BUILD_CD_DIR) OUTPUT_DIR:=../../../$(OUTPUT_CD_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(NOSLDFLAGS)" LDLIBS:="$(LDLIBS)" RTLIB:="${RTLIB}" PTLIB:="${PTLIB}"
	
cmjni:	build_output_dirs controlmanager
	$(info ******************************************************************************************************)
	$(info *			Building cmjni -> $(CONFIG) binaries for $(TARGET_OS)								*)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/cmjni BUILD_DIR:=../../../$(BUILD_CM_DIR) OUTPUT_DIR:=../../../$(OUTPUT_CM_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(NOSLDFLAGS)$(JNI_LDFLAGS)" LDLIBS:="$(LDLIBS)"

samples: controllee controlmanager
	$(info ******************************************************************************************************)
	$(info *                        Building Controllee -> $(CONFIG) binaries for $(TARGET_OS)                                                         *)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./samples/controller-client
	$(MAKE) -C ./samples/controlee-server

controlmanager: build_output_dirs sqlite3 jsoncpp
	$(info ******************************************************************************************************)
	$(info *			Building Control-Manager -> $(CONFIG) binaries for $(TARGET_OS)								*)
	$(info ******************************************************************************************************)
	$(MAKE) -C ./makefiles/modules/controlmanager BUILD_DIR:=../../../$(BUILD_CM_DIR) OUTPUT_DIR:=../../../$(OUTPUT_CM_DIR) CXXFLAGS:="$(CXXFLAGS)" CFLAGS:="$(CFLAGS)" LDFLAGS:="$(NOSLDFLAGS)" LDLIBS:="$(LDLIBS)" RTLIB:="${RTLIB}" PTLIB:="${PTLIB}"

install: $(INSTALL_ROOT_DIR) $(INSTALL_TARGETOS_DIR) $(INSTALL_CONFIG_DIR)
	$(info ******************************************************************************************************)
	$(info *			installing -> $(CONFIG) binaries for $(TARGET_OS)								*)
	$(info ******************************************************************************************************)
	-rm -r $(INSTALL_CONFIG_DIR)/*
	${MKDIR_COMMAND} $(INSTALL_CONFIG_DIR)/include
	${COPY_RECURSIVE_COMMAND} controlmanager/api/include/* $(INSTALL_CONFIG_DIR)/include
	${MKDIR_COMMAND} $(INSTALL_CONFIG_DIR)/libs
	${MKDIR_COMMAND} $(INSTALL_CONFIG_DIR)/libs/static
ifeq ($(TARGET_OS),android)
	${COPY_COMMAND} $../../../(OUTPUT_OC_UTILITY_DIR)/android/boost/libs/armeabi/libboost_system-gcc-mt-1_49.a $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $../../../(OUTPUT_OC_UTILITY_DIR)/android/boost/libs/armeabi/libboost_thread-gcc-mt-1_49.a $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $../../../(OUTPUT_OC_UTILITY_DIR)/android/boost/libs/armeabi/libboost_regex-gcc-mt-1_49.a $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $../../../(OUTPUT_OC_DIR)/csdk/android/release/liboctbstack.a $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $../../../(OUTPUT_OC_DIR)/release/obj/liboc.a $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $../../../(OUTPUT_OC_DIR)/oc_logger/lib/*oc_logger.a $(INSTALL_CONFIG_DIR)/libs/static
else 
	${COPY_RECURSIVE_COMMAND} $(OUTPUT_OPENSOURCE_DIR)/boost/static/* $(INSTALL_CONFIG_DIR)/libs/static
endif
	
	${COPY_COMMAND} $(OUTPUT_OPENSOURCE_DIR)/jsoncpp/static/* $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $(OUTPUT_OPENSOURCE_DIR)/sqlite3/static/* $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $(OUTPUT_CM_DIR)/controlmanager/static/* $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $(OUTPUT_CD_DIR)/controllee/static/* $(INSTALL_CONFIG_DIR)/libs/static
	
	${MKDIR_COMMAND} $(INSTALL_CONFIG_DIR)/libs/shared
	
ifeq ($(TARGET_OS),android)	
	${COPY_COMMAND} $(OUTPUT_SHP_DIR)/controlmanager/shared/* $(INSTALL_CONFIG_DIR)/libs/shared
	${COPY_COMMAND} $(OUTPUT_SHP_DIR)/cmjni/static/* $(INSTALL_CONFIG_DIR)/libs/static
	${COPY_COMMAND} $(OUTPUT_SHP_DIR)/cmjni/shared/* $(INSTALL_CONFIG_DIR)/libs/shared
else 
	${COPY_COMMAND} $(OUTPUT_OPENSOURCE_DIR)/openssl/shared/* $(INSTALL_CONFIG_DIR)/libs/shared
	${COPY_COMMAND} $(OUTPUT_OPENSOURCE_DIR)/sqlite3/shared/* $(INSTALL_CONFIG_DIR)/libs/shared
endif
	
build_output_dirs : $(BUILD_ROOT_DIR) $(OUTPUT_ROOT_DIR) $(BUILD_TARGETOS_DIR) $(OUTPUT_TARGETOS_DIR) $(BUILD_CONFIG_DIR) $(OUTPUT_CONFIG_DIR) $(BUILD_SHP_DIR) $(BUILD_OPENSOURCE_DIR) $(OUTPUT_SHP_DIR) $(OUTPUT_OPENSOURCE_DIR)
	
clean_all: clean_all_linux clean_all_windows 

clean_all_linux:
	-rm -rf build output
	
clean: clean_linux

clean_linux:
	-rm -rf ${BUILD_ROOT_DIR} ${CONFIG} ${OUTPUT_TARGETOS_DIR}
	$(MAKE) -C ./samples/controller-client clean
	$(MAKE) -C ./samples/controlee-server clean


clean_target: clean_target_linux

clean_target_linux:
	-rm -rf ${BUILD_TARGETOS_DIR} ${OUTPUT_TARGETOS_DIR}

#clear_vars won't clear values defined in parent bash	
clear_vars:
	-unset TARGET_OS
	-unset CONFIG	
	-unset BUILD_IMPORT_LIBRARIES
	
help:
	$(info This make file builds all controlmanager libraries (shared, static))
	$(info Before starting build, user must define following environment variables)
	$(info )
	$(info 	TARGET_OS -> set this value to the name of the target operating system ex: linux32, linux64, mingw32, ming64)
	$(info )
	$(info 	CONFIG -> configuration to use. default value is "release". supported values are "debug", "release". if defined with any value other than supported values, it will be reset to "release")
	$(info )
	$(info Following are the target description)
	$(info )
	$(info all -> builds all targets)
	$(info )
	$(info clean -> cleans output and build directories of current target os and current configuration )
	$(info )
	$(info clean_target -> cleans output and build directories of all configurations of current target os )
	$(info )
	$(info clean_all -> cleans output and build directories of all configurations and all target os )
	#$(info )
	#$(info clear_vars -> clears all user exported variables which are needed by this makefile )
	#$(info )
	 	 	

$(INSTALL_ROOT_DIR):
	${MKDIR_COMMAND} $@
	
$(INSTALL_TARGETOS_DIR):
	${MKDIR_COMMAND} $@
	
$(INSTALL_CONFIG_DIR):
	${MKDIR_COMMAND} $@
			
$(BUILD_ROOT_DIR):
	${MKDIR_COMMAND} $@
	
$(OUTPUT_ROOT_DIR):
	${MKDIR_COMMAND} $@

$(BUILD_TARGETOS_DIR):
	${MKDIR_COMMAND} $@
	
$(OUTPUT_TARGETOS_DIR):
	${MKDIR_COMMAND} $@	

$(BUILD_CONFIG_DIR):
	${MKDIR_COMMAND} $@
	
$(OUTPUT_CONFIG_DIR):
	${MKDIR_COMMAND} $@

$(BUILD_OPENSOURCE_DIR):
	${MKDIR_COMMAND} $@	

$(OUTPUT_CM_DIR):
	${MKDIR_COMMAND} $@
	
$(OUTPUT_OPENSOURCE_DIR):
	${MKDIR_COMMAND} $@	
