APP_PATH := $(call my-dir)

#specify project root path here wrt make file directory
PROJECT_ROOT_PATH	= ../../..
PROJECT_API_PATH	= $(PROJECT_ROOT_PATH)/api

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build sample_main
include $(CLEAR_VARS)
LOCAL_PATH = $(APP_PATH)
LOCAL_MODULE = sample
LOCAL_C_INCLUDES = $(PROJECT_API_PATH)
LOCAL_STATIC_LIBRARIES = CA
LOCAL_SRC_FILES = sample_main.c
include $(BUILD_EXECUTABLE)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build glib CACommon CACoap CA
include $(CLEAR_VARS)
PROJECT_LIB_PATH = $(PROJECT_ROOT_PATH)/build/android
include $(PROJECT_LIB_PATH)/jni/Android.mk
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

