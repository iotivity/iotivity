APP_PATH := $(call my-dir)
IOTIVITY_RELEASE_PATH = $(IOTIVITY_PATH)/out/android/armeabi/$(RELEASE_DIR)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := OC
LOCAL_SRC_FILES := $(IOTIVITY_RELEASE_PATH)/liboc.so
include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := OC_LOGGER
LOCAL_SRC_FILES := $(IOTIVITY_RELEASE_PATH)/liboc_logger.so
include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := OC_PM_API
LOCAL_SRC_FILES := $(IOTIVITY_RELEASE_PATH)/libocpmapi.so
include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := OC_TB_STACK
LOCAL_SRC_FILES := $(IOTIVITY_RELEASE_PATH)/liboctbstack.so
include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := CA
LOCAL_SRC_FILES := $(IOTIVITY_RELEASE_PATH)/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
include $(CLEAR_VARS)
LOCAL_MODULE := ConformanceSimulator
LOCAL_SHARED_LIBRARIES := OC OC_LOGGER OC_PM_API OC_TB_STACK CA
LOCAL_CFLAGS := -std=gnu++11 -fPIE -fpermissive -D__WITH_DTLS__ -D__ANDROID__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
LOCAL_C_INCLUDES = $(HEADER_PATH)
LOCAL_C_INCLUDES += $(IOTIVITY_PATH)/resource/include
LOCAL_C_INCLUDES += $(IOTIVITY_PATH)/resource/csdk/stack/include
LOCAL_C_INCLUDES += $(IOTIVITY_PATH)/resource/c_common
LOCAL_C_INCLUDES += $(IOTIVITY_PATH)/resource/oc_logger/include
LOCAL_C_INCLUDES += $(IOTIVITY_PATH)/extlibs/boost/boost_1_58_0
LOCAL_C_INCLUDES += $(TEST_PROJECT_ROOT)/include
LOCAL_SRC_FILES := $(TEST_PROJECT_ROOT)/src/testapp/IUTSimulatorApp/c_cpp/IUTSimulator.cpp \
					 $(TEST_PROJECT_ROOT)/src/common/commonutil/c_cpp/ResourceHelper.cpp  \
					 $(TEST_PROJECT_ROOT)/src/common/commonutil/c_cpp/ResourceServer.cpp \
					 $(TEST_PROJECT_ROOT)/src/common/commonutil/c_cpp/SampleResource.cpp
LOCAL_LDFLAGS := -fPIE -pie
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

