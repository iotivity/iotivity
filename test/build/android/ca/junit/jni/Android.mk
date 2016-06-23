APP_PATH := $(call my-dir)
PROJECT_ROOT_PATH	= $(OICTEST_PATH)/resource/csdk/connectivity
CURRENT_PATH = $(SECTEST_PATH)/build/android/ca/junit
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include $(OICTEST_REPO)/IotivityOrgSource/$(IOTIVITY_BRANCH)/iotivity/resource/csdk/connectivity/build/android/jni/Android.mk
include $(SECTEST_PATH)/build/android/ca/gtest/jni/external/Android.mk

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := RMInterface
LOCAL_STATIC_LIBRARIES := CA
LOCAL_C_INCLUDES := $(PROJECT_ROOT_PATH)/api 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/common/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/external/inc 
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_malloc/include
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/oic_string/include
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../c_common/ocrandom/include
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/../../csdk/logger/include 
LOCAL_C_INCLUDES += $(CURRENT_PATH)
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/src/bt_le_adapter/android/
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/util/inc/
LOCAL_C_INCLUDES += $(PROJECT_ROOT_PATH)/util/src/camanager/android/

LOCAL_SRC_FILES := $(CURRENT_PATH)/jni/ResourceModel.c \
				$(PROJECT_ROOT_PATH)/util/src/cautilinterface.c \
                $(PROJECT_ROOT_PATH)/util/src/camanager/android/caleautoconnector.c \
                $(PROJECT_ROOT_PATH)/util/src/camanager/android/caleconnectionmanager.c \
                $(PROJECT_ROOT_PATH)/util/src/camanager/android/camanagerdevice.c \
                $(PROJECT_ROOT_PATH)/util/src/camanager/android/camanagerleutil.c

LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -D__WITH_DTLS__ -std=c99 -D__CA__
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions
include $(BUILD_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
