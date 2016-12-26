APP_PATH := $(call my-dir)
PROJECT_LIB_PATH    = ../../../../../../../extlibs/android/ca/$(DEVICE_ARCH)

include $(CLEAR_VARS)
LOCAL_PATH = $(PROJECT_LIB_PATH)
LOCAL_SRC_FILES = libCAJni.so
LOCAL_MODULE = CAJni
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH = $(PROJECT_LIB_PATH)
LOCAL_SRC_FILES = libconnectivity_abstraction.so
LOCAL_MODULE = connectivity_abstraction
include $(PREBUILT_SHARED_LIBRARY)
