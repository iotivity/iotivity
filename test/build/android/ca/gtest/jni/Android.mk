APP_PATH := $(call my-dir)
PROJECT_ROOT_PATH = $(IOTIVITY_PATH)/resource/csdk/connectivity
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

IOTIVITY_INCLUDE := $(IOTIVITY_PATH)/resource/csdk/connectivity/api 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/connectivity/inc 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/connectivity/external/inc 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/c_common 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/stack/include 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/logger/include
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/c_common/oic_malloc/include 
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/c_common/oic_string/include
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/connectivity/lib/libcoap-4.1.1/include
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/resource/csdk/connectivity/common/inc
IOTIVITY_INCLUDE += $(IOTIVITY_PATH)/extlibs/mbedtls/mbedtls/include/mbedtls

COMMON_TC_INCLUDE := $(BOOST_PATH) $(GTEST_PATH) $(GTEST_PATH)/include 
COMMON_TC_INCLUDE += $(IOTIVITY_INCLUDE) 
COMMON_TC_INCLUDE += $(COMMON_UTIL_HEADER_PATH) $(EXTERNAL_PATH) $(HELPER_HEADER_PATH)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := connectivity_abstraction
LOCAL_SRC_FILES := $(IOTIVITY_PATH)/out/android/armeabi/${RELEASE_DIR}/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := helper_interface
LOCAL_STATIC_LIBRARIES := libgtest boostsystemModule boostfilesystemModule
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_C_INCLUDES := $(COMMON_TC_INCLUDE)
LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAHelper.cpp $(COMMON_UTIL_SRC_PATH)/CommonUtil.cpp
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -std=gnu++11 -fpermissive -D__CA__ -D__WITH_DTLS__ -DTCP_ADAPTER -D__ANDROID_NATIVE__ 
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := tcp_helper_interface
LOCAL_STATIC_LIBRARIES := libgtest boostsystemModule boostfilesystemModule
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_C_INCLUDES := $(COMMON_TC_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAHelper.cpp $(COMMON_UTIL_SRC_PATH)/CommonUtil.cpp
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -std=gnu++11 -fpermissive -D__CA__ -DTCP_ADAPTER -D__ANDROID_NATIVE__ -D__WITH_DTLS__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_test
LOCAL_STATIC_LIBRARIES := helper_interface
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__ANDROID__ -D__ANDROID_NATIVE__ -D__ALL_TRANSPORT__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
LOCAL_C_INCLUDES := $(COMMON_TC_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAAndroidGtestRunner.cpp \
                     $(ITC_PATH)/CACommonTest.cpp \
                     $(UTC_PATH)/CATest.cpp
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_ip_test
LOCAL_STATIC_LIBRARIES := helper_interface
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__ANDROID__ -D__ANDROID_NATIVE__ -D__ALL_TRANSPORT__ -D__IP__ -D__WITH_DTLS__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
LOCAL_C_INCLUDES := $(COMMON_TC_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAAndroidGtestRunner.cpp \
                     $(ITC_PATH)/CANetworkCommonTest.cpp  \
                     $(ITC_PATH)/CAClientTest.cpp \
                     $(ITC_PATH)/CAMulticastTest.cpp \
                     $(UTC_PATH)/CANetworkTest.cpp
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_tcp_test
LOCAL_STATIC_LIBRARIES := helper_interface
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -DTCP_ADAPTER -D__ANDROID__ -D__ANDROID_NATIVE__ -D__ALL_TRANSPORT__ -D__TCP__ -D__WITH_DTLS__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
LOCAL_C_INCLUDES := $(COMMON_TC_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_UTIL_HEADER_PATH) $(EXTERNAL_PATH) $(HELPER_HEADER_PATH) 
LOCAL_SRC_FILES := $(HELPER_SRC_PATH)/CAAndroidGtestRunner.cpp \
                     $(ITC_PATH)/CANetworkCommonTest.cpp  \
                     $(ITC_PATH)/CAClientTest.cpp \
                     $(UTC_PATH)/CANetworkTest.cpp

LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := iotivity_ca_simulator
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
#LOCAL_STATIC_LIBRARIES := CA
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__CA__ -D__WITH_DTLS__ -D__ANDROID_NATIVE__ -DTCP_ADAPTER
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions 
LOCAL_C_INCLUDES = $(IOTIVITY_INCLUDE)
LOCAL_C_INCLUDES += $(SIMULATOR_INCLUDE_PATH) 
LOCAL_SRC_FILES := $(SIMULATOR_SRC_PATH)/ca_simulator.c
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
