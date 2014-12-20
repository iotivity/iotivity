#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	controller-client

CONFIG			?= 	release

#configure Source Build Directory
BUILD_DIR		?= 	${CONFIG}

OBJ_DIRS		?= 	$(BUILD_DIR)

BOOST_PATH		?=	/usr/lib 

#configure linker program
LINKER 			?=	$(CXX)

#configure Source Root Directory
SRC_DIR 		=	src

#configure CPP Files, include paths, cpp compiler option
CXXFLAGS 		= 	-c -fmessage-length=0 -std=c++0x

CPP_INC_DIR 		= 	-I./${SRC_DIR}

ifndef TARGET_OS
$(info TARGET_OS is set so "linux32")
export TARGET_OS=linux32
endif

SRC_CPP_FILES 		= 	main.cpp\
				ProgramUtils.cpp\
				testConfiguration.cpp\
				testDoors.cpp\
				testEnergyConsumptionResource.cpp\
				testHumidityResource.cpp\
				testNetworksResource.cpp\
				testSubscriptionListener.cpp\
				testInformationController.cpp\
				testLightResource.cpp\
				testMode.cpp\
				testOperationResource.cpp\
				testTemperaturesResource.cpp\
				testUsage.cpp\
				testAccessPointsResource.cpp\
				testWifiResource.cpp\
				ControlManagerListener.cpp
				

#configure C Files, include paths, c compiler option
CFLAGS +=
C_INC_DIR =
SRC_C_FILES =

CPP_INC_DIR +=		-I../../controlmanager/api/include

ifndef MKDIR_COMMAND
export MKDIR_COMMAND=mkdir -p
endif

#configure archiver flags
ARFLAGS = rcs

#configure linker flags
LDFLAGS += -L${BOOST_PATH} -L../../${CONFIG}/${TARGET_OS}/opensource/sqlite3/static -L../../${CONFIG}/${TARGET_OS}/opensource/jsoncpp/static
LDFLAGS += -L../../${CONFIG}/${TARGET_OS}/controlmanager/static/
LDFLAGS += -L../../../../../oic-resource/resource/${CONFIG}/obj -L../../../../../oic-resource/resource/csdk/linux/${CONFIG} -L../../../../../oic-resource/resource/oc_logger/lib

ifeq ($(TARGET_OS),linux64)
	LDFLAGS += -m64
else
	LDFLAGS += -m32
endif

LDLIBS += -lcontrolmanager
LDLIBS += -lboost_system -lboost_regex -lboost_thread -lsqlite3 -ldl -lrt -lpthread -ljsoncpp
LDLIBS += -loc -loctbstack ../../../../../oic-resource/resource/oc_logger/lib/oc_logger.a

BUILD_CPP_OBJECTS=$(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(SRC_CPP_FILES)) )
BUILD_C_OBJECTS=$(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(SRC_C_FILES)) )
BUILD_OBJ_DIRS=$(addprefix $(BUILD_DIR), $(OBJ_DIRS))

ifeq ($(CONFIG),debug)
	CXXFLAGS += -g
	CFLAGS += -g
endif

ifeq ($(CONFIG),release)
	LDFLAGS += -s
	CXXFLAGS += -Os
	CFLAGS += -Os
endif

include ../../makefiles/platformconfigs/$(TARGET_OS)_config.mk

all : $(BUILD_DIR) ${PROJECT_NAME} 

build_output_dirs: $(BUILD_DIR) $(BUILD_OBJ_DIRS)

$(BUILD_DIR):
	${MKDIR_COMMAND} $@

$(PROJECT_NAME): $(BUILD_CPP_OBJECTS)
	$(info Creating executable : $@)
	$(CXX) $(LDFLAGS) $(BUILD_CPP_OBJECTS) $(LDLIBS) -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(info compiling : ${PROJECT_NAME} <= $(notdir $+))
	$(CXX) -c -o $@ $+ $(CXXFLAGS) $(CPP_INC_DIR)

$(BUILD_DIR)/%: 
	${MKDIR_COMMAND} $@

clean: 
	-rm -rf $(BUILD_DIR)
