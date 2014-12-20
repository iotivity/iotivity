
PROJECT_NAME		=	controlee-server

CONFIG			?= 	release

ROOT			?=	../../

BOOST_PATH		?=	/usr/lib

#configure Source Build Directory
BUILD_DIR		?= 	${CONFIG}

OBJ_DIRS		?= 	$(BUILD_DIR)

#configure linker program
LINKER 			?=	$(CXX)

#configure Source Root Directory
SRC_DIR 		=	src

#configure OC Root Directory
OUTPUT_OC_DIR		= oic-resource/resource

#configure CPP Files, include paths, cpp compiler option
CXXFLAGS 		= 	-g -c -fmessage-length=0 -std=c++0x -DUSE_SQLITE3_SUBS_MANAGER -DENABLE_LOG -fpermissive
CPP_INC_DIR 		= 	-I./${SRC_DIR}

ifndef TARGET_OS
$(info TARGET_OS is set so "linux32")
export TARGET_OS=linux32
endif

SRC_CPP_FILES 		= 	main.cpp\
				ProgramUtils.cpp				

#configure C Files, include paths, c compiler option
CFLAGS +=
C_INC_DIR =
SRC_C_FILES =

CPP_INC_DIR +=		-I../../controllee/include/api \
                        -I../../controllee/include/core \
                        -I../../controllee/include/utils \
                        -I../../controllee/include/xsd \
                        -I../../controllee/include/Server \
                        -I../../controllee/include/device \
                        -I../../controllee/include/Notification \
                        -I../../controllee/include/Serialization \
			-I../../controllee/include/SubscriptionStore \
                        -I../../controllee/include \
                        -I../../controllee/framework/gen/src/ShpGen \
                        -I../../../../../${OUTPUT_OC_DIR}/include \
                        -I../../../../../${OUTPUT_OC_DIR}/csdk/stack/include \
                        -I../../../../../${OUTPUT_OC_DIR}/csdk/occoap/include \
                        -I../../../../../${OUTPUT_OC_DIR}/csdk/ocrandom/include \
                        -I../../../../../${OUTPUT_OC_DIR}/csdk/ocsocket/include \
			-I../../opensource/boost/include \
                        -I../../opensource/sqlite3/include \
			-I../../../../../${OUTPUT_OC_DIR}/dependencies/cereal/include \
                        -I../../../../../${OUTPUT_OC_DIR}/oc_logger/include

ifndef MKDIR_COMMAND
export MKDIR_COMMAND=mkdir -p
endif

#configure archiver flags
ARFLAGS = rcs

#configure linker flags
LDFLAGS += -L$(ROOT)/${CONFIG}/${TARGET_OS}/opensource/boost/static/ -L$(ROOT)/${CONFIG}/${TARGET_OS}/opensource/sqlite3/static -L$(ROOT)/${CONFIG}/${TARGET_OS}/opensource/jsoncpp/static
LDFLAGS += -L$(ROOT)/${CONFIG}/${TARGET_OS}/controllee/static/
LDFLAGS += -L../../../../../${OUTPUT_OC_DIR}/${CONFIG}/obj -L../../../../../${OUTPUT_OC_DIR}/csdk/linux/${CONFIG} -L../../../../../${OUTPUT_OC_DIR}/oc_logger/lib
LDFLAGS += -L${BOOST_PATH}

ifeq ($(TARGET_OS),linux64)
	LDFLAGS += -m64
else
	LDFLAGS += -m32
endif

LDLIBS += -lcontrollee 
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
