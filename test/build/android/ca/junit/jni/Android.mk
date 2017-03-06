APP_PATH := $(call my-dir)
CURRENT_PATH = $(IOTIVITY_TEST_ROOT)/build/android/ca/junit
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := connectivity_abstraction
LOCAL_SRC_FILES := $(IOTIVITY_ROOT)/out/android/$(IOTIVITY_TARGET_ARCH)/$(RELEASE_DIR)/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := CAJni
LOCAL_SHARED_LIBRARIES := connectivity_abstraction
LOCAL_C_INCLUDES := $(CURRENT_PATH)
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/api 
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/inc 
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/common/inc 
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/c_common/
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/c_common/oic_malloc/include
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/c_common/oic_string/include
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/c_common/ocrandom/include
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/logger/include 
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/lib/libcoap-4.1.1
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/lib/libcoap-4.1.1/include
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/src/bt_le_adapter/android/
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/util/inc/
LOCAL_C_INCLUDES += $(IOTIVITY_ROOT)/resource/csdk/connectivity/util/src/camanager/android/

LOCAL_SRC_FILES := $(CURRENT_PATH)/jni/CAJni.c

LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -std=c99 -D__CA__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
include $(BUILD_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
