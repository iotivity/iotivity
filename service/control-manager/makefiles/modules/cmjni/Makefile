#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	cmjni

CONFIG				?= 	release

BUILD_DIR			?= ../../../build

#configure linker program
LINKER 				?=	$(CXX)

#configure Source Root Directory
SRC_DIR =	../../../controlmanager/jni/native/src

#configure Source Build Directory

override BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
override OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += 
override CPP_INC_DIR = 	-I../../../opensource/jsoncpp/include \
						-I/usr/local/boost/include \
						-I../../../controlmanager/platform/include \
						-I../../../controlmanager/api/include \
						-I../../../controlmanager/api/include/Client \
						-I../../../controlmanager/api/include/Server \
						-I../../../controlmanager/api/include/xsd \
						-I../../../controlmanager/core/src \
						-I../../../controlmanager/api/src \
						-I../../../controlmanager/jni/native/include\
						-I../../../controlmanager/jni/native/src\
						-I../../../../../../${OUTPUT_OC_DIR}/include \
						-I../../../../../../${OUTPUT_OC_DIR}/csdk/stack/include \
						-I../../../../../../${OUTPUT_OC_DIR}/csdk/ocsocket/include \
						-I../../../../../../${OUTPUT_OC_DIR}/oc_logger/include \
						${JDK_INCLUDES}						
						
override SRC_CPP_FILES = 	JNIInvocation.cpp \
							JNIControlManager.cpp \
							JNIResourceHandlerFactory.cpp \
							JNIDeSerializer.cpp \
							JNISerializer.cpp \
							JNIGlobal.cpp \
							JNIDeviceFinder.cpp \
							JNIDevice.cpp \
							JNIResourceHandler.cpp \
							JNISession.cpp \
							JNIServerSession.cpp \
							JNIHeadersList.cpp \
							JNIResource.cpp \
							JNISubscriptionManager.cpp \
							JNINotificationResponse.cpp \
							JNIConfiguration.cpp \
							JNISerializable.cpp \
							JNIDeviceFactory.cpp \
							JNISerializableDataFactory.cpp \
							MyCMListener.cpp \
							CMResourceHandlerFactory.cpp \
							CMResourceHandler.cpp \
							CMJNISubscriptionProviderImpl.cpp \
							CMSerializableDataFactory.cpp \
							CMDeviceFactory.cpp \
							CMConfiguration.cpp \
							Serializable.cpp \
							MyNotificationListener.cpp \
							MyDeviceFinderListener.cpp \
							MyResourceResponseListener.cpp 
							
							


#configure C Files, include paths, c compiler option
override CFLAGS +=
override C_INC_DIR = 
override SRC_C_FILES = 

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
override LDFLAGS += -L../../../${CONFIG}/android/controlmanager/static/
override LDFLAGS += -L../../../../../../${OUTPUT_OC_UTILITY_DIR}/android/boost/libs/armeabi
override LDFLAGS += -L../../../${OUTPUT_OPENSOURCE_DIR}/sqlite3/static -L../../../${OUTPUT_OPENSOURCE_DIR}/jsoncpp/static
override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/release/obj -L../../../../../../${OUTPUT_OC_DIR}/oc_logger/lib
override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/csdk/android/release
#override LDFLAGS += -m32

override LDLIBS += -lcontrolmanager
override LDLIBS += -l${BOOST_SYSTEM_LIB} -l${BOOST_THREAD_LIB} -l${BOOST_REGEX_LIB} -lsqlite3 -ljsoncpp
override LDLIBS += -l${OC_LIB} -l${OCTBSTACK_LIB} -l${OCLOGGER_LIB}

-include platformconfigs/$(TARGET_OS)_config.mk


#configure target names
STATIC_LIB_TARGET	=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}
SHARED_IMPORT_LIB_TARGET = $(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}.a

include ../../BuildTargets.mk