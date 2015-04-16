APP_PATH := $(call my-dir)
#specify project root path here wrt make file directory
PROJECT_ROOT_PATH	= ../../..

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := CAInterface
LOCAL_STATIC_LIBRARIES = CA
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH := $(APP_PATH)
LOCAL_MODULE := RMInterface
LOCAL_SRC_FILES := ResourceModel.c
LOCAL_STATIC_LIBRARIES := CA
LOCAL_LDLIBS := -llog
LOCAL_C_INCLUDES := ../../../api
include $(BUILD_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build CACommon CACoap CA
include $(CLEAR_VARS)
PROJECT_ROOT_PATH			= ../../..
CA_LIB_PATH 				= $(PROJECT_ROOT_PATH)/build/android
include $(CA_LIB_PATH)/jni/Android.mk
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

