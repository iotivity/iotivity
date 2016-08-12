APP_PATH := $(call my-dir)
#PROJECT_LIB_PATH	= /home/suchita/git/oictest_repo/IotivitySECTest/bin/android/ca/simulator
PROJECT_LIB_PATH	= ~/git/oictest_repo/IotivitySECTest_New/extlibs/android/ca/armeabi

include $(CLEAR_VARS)
LOCAL_PATH = $(PROJECT_LIB_PATH)
LOCAL_SRC_FILES = libRMInterface.so
LOCAL_MODULE = CASAMPLE
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH = $(PROJECT_LIB_PATH)
LOCAL_SRC_FILES = libTinyDtls.so
LOCAL_MODULE = TINYDTLS
include $(PREBUILT_SHARED_LIBRARY)
