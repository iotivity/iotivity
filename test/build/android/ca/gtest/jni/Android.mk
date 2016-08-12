APP_PATH := $(call my-dir)
PROJECT_ROOT_PATH	= $(IOTIVITY_PATH)/resource/csdk/connectivity
SIMULATOR_INCLUDE_PATH = $(SECTEST_PATH)/src/testapp/ca/c_cpp/include
SIMULATOR_SRC_PATH = $(SECTEST_PATH)/src/testapp/ca/c_cpp/src
ITC_PATH = $(SECTEST_PATH)/src/tc/ca/gtest/src/stc
UTC_PATH = $(SECTEST_PATH)/src/tc/ca/gtest/src/btc
HELPER_SRC_PATH = $(SECTEST_PATH)/src/tc/ca/gtest/src/helper
HELPER_HEADER_PATH = $(SECTEST_PATH)/src/tc/ca/gtest/include
COMMON_UTIL_SRC_PATH = $(SECTEST_PATH)/src/common/commonutil/c_cpp
COMMON_UTIL_HEADER_PATH = $(SECTEST_PATH)/include
CURRENT_PATH = $(SECTEST_PATH)/build/android/ca/gtest/jni
EXTERNAL_PATH = $(CURRENT_PATH)/external
GTEST_PATH = $(IOTIVITY_PATH)/extlibs/gtest/gtest-1.7.0
BOOST_PATH = $(IOTIVITY_PATH)/extlibs/boost/boost_1_58_0

include $(CLEAR_VARS)
LOCAL_MODULE := libgtest
LOCAL_CPP_EXTENSION := .cc
LOCAL_C_INCLUDES := $(GTEST_PATH)/include $(GTEST_PATH)
LOCAL_SRC_FILES := $(GTEST_PATH)/src/gtest-all.cc
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boostsystemModule
LOCAL_C_INCLUDES := $(BOOST_PATH)
LOCAL_SRC_FILES := $(BOOST_PATH)/libs/system/src/error_code.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boostfilesystemModule
LOCAL_C_INCLUDES := $(BOOST_PATH)
LOCAL_SRC_FILES := $(BOOST_PATH)/libs/filesystem/src/codecvt_error_category.cpp \
				$(BOOST_PATH)/libs/filesystem/src/operations.cpp \
				$(BOOST_PATH)/libs/filesystem/src/path.cpp \
				$(BOOST_PATH)/libs/filesystem/src/path_traits.cpp \
				$(BOOST_PATH)/libs/filesystem/src/portability.cpp \
				$(BOOST_PATH)/libs/filesystem/src/unique_path.cpp \
				$(BOOST_PATH)/libs/filesystem/src/utf8_codecvt_facet.cpp \
				$(BOOST_PATH)/libs/filesystem/src/windows_file_codecvt.cpp \
				$(BOOST_PATH)/libs/filesystem/src/windows_file_codecvt.hpp
include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := CAInterface
LOCAL_STATIC_LIBRARIES := libgtest boostsystemModule boostfilesystemModule CA

LOCAL_C_INCLUDES = $(PROJECT_ROOT_PATH)/api 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/external/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include 
LOCAL_C_INCLUDES += $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) $(PROJECT_ROOT_PATH)/../stack/include 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common 
LOCAL_C_INCLUDES += $(GTEST_PATH)/include $(GTEST_PATH)
LOCAL_C_INCLUDES += $(BOOST_PATH)

LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAHelper.cpp $(COMMON_UTIL_SRC_PATH)/CommonUtil.cpp

LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -D__WITH_DTLS__ -std=gnu++11 -fpermissive -D__CA__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
include $(BUILD_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#Build CACommon CACoap CA
include $(CLEAR_VARS)
#PROJECT_ROOT_PATH	= ../../../IotivityOrgSource/last/iotivity/resource/csdk/connectivity
#PROJECT_ROOT_PATH			= ../../..
#CA_LIB_PATH 				= $(PROJECT_ROOT_PATH)/build/android
include $(EXTERNAL_PATH)/Android.mk
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_test
LOCAL_SHARED_LIBRARIES := CAInterface
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__WITH_DTLS__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions

LOCAL_C_INCLUDES = $(EXTERNAL_PATH) $(PROJECT_ROOT_PATH)/api 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/inc $(PROJECT_ROOT_PATH)/external/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include
LOCAL_C_INCLUDES += $(GTEST_PATH)/include $(GTEST_PATH)
LOCAL_C_INCLUDES += $(BOOST_PATH)
LOCAL_C_INCLUDES += $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../stack/include 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common    

LOCAL_SRC_FILES := $(COMMON_UTIL_SRC_PATH)/GtestMain.cpp \
					 $(ITC_PATH)/CACommonTest.cpp \
					 $(ITC_PATH)/CANetworkCommonTest.cpp  \
					 $(ITC_PATH)/CAClientTest.cpp \
					 $(ITC_PATH)/CAMulticastTest.cpp \
					 $(UTC_PATH)/CANetworkTest.cpp \
					 $(UTC_PATH)/CATest.cpp

LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_simulator
LOCAL_STATIC_LIBRARIES := CA
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__WITH_DTLS__ -D__TC_PLATFORM_ANDROID_NATIVE__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions 
LOCAL_C_INCLUDES = $(PROJECT_ROOT_PATH)/api $(PROJECT_ROOT_PATH)/inc $(PROJECT_ROOT_PATH)/external/inc $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include $(SIMULATOR_INCLUDE_PATH) 
LOCAL_SRC_FILES := $(SIMULATOR_SRC_PATH)/ca_simulator.c
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_sample_client
LOCAL_STATIC_LIBRARIES := CA
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__WITH_DTLS__ -D__TC_PLATFORM_ANDROID_NATIVE__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions 
LOCAL_C_INCLUDES = $(PROJECT_ROOT_PATH)/api $(PROJECT_ROOT_PATH)/inc $(PROJECT_ROOT_PATH)/external/inc $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include 
LOCAL_SRC_FILES := $(SIMULATOR_SRC_PATH)/sample_client.c
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_sample_server
LOCAL_STATIC_LIBRARIES := CA
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__WITH_DTLS__ -D__TC_PLATFORM_ANDROID_NATIVE__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions 
LOCAL_C_INCLUDES = $(PROJECT_ROOT_PATH)/api $(PROJECT_ROOT_PATH)/inc $(PROJECT_ROOT_PATH)/external/inc $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include 
LOCAL_SRC_FILES := $(SIMULATOR_SRC_PATH)/sample_server.c
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
