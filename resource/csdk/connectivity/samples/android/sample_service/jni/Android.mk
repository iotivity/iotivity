APP_PATH := $(call my-dir)
#specify project root path here wrt make file directory
PROJECT_ROOT_PATH	= ../../..

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := RMInterface
LOCAL_SRC_FILES := ResourceModel.c
LOCAL_CFLAGS = -D__WITH_DTLS__
LOCAL_C_INCLUDES = $(PROJECT_ROOT_PATH)/api $(PROJECT_ROOT_PATH)/external/inc
LOCAL_STATIC_LIBRARIES := CA
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build CACommon CACoap CA
include $(CLEAR_VARS)
PROJECT_ROOT_PATH			= ../../..
CA_LIB_PATH 				= $(PROJECT_ROOT_PATH)/build/android
include $(CA_LIB_PATH)/jni/Android.mk
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

