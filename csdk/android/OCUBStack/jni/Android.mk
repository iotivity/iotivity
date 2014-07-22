#          UBSTACK         #
LOCAL_PATH := $(call my-dir)

ifeq ($(CCFL_CORE), )
CCFL_CORE	:= $(HOME)/peg_ccfl-core
endif

ifeq ($(UB_OBJ), )
UB_OBJ	:= $(CCFL_CORE)/ubstack
endif

ifeq ($(TB_OBJ), )
TB_OBJ	:= $(CCFL_CORE)/stack
endif

ifeq ($(ANDROID_OBJ), )
ANDROID_OBJ	:= $(CCFL_CORE)/android
endif

ifeq ($(LOGGER_OBJ), )
LOGGER_OBJ	:= $(CCFL_CORE)/logger
endif

ifeq ($(BOOST_OBJ), )
BOOST_OBJ	:= $(HOME)/android-dev/boost/android
endif

#
# TBStack
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE	:= tbstack
LOCAL_SRC_FILES	:= $(ANDROID_OBJ)/OCTBStack/obj/local/$(TARGET_ARCH_ABI)/libTBStack.a
include $(PREBUILT_STATIC_LIBRARY)

#
# OCCoap
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE	:= occoap
LOCAL_SRC_FILES	:= $(ANDROID_OBJ)/OCCoap/obj/local/$(TARGET_ARCH_ABI)/libOCCoap.a
include $(PREBUILT_STATIC_LIBRARY)

#
# OCLogger
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE	:= oclogger
LOCAL_SRC_FILES	:= $(ANDROID_OBJ)/OCLogger/obj/local/$(TARGET_ARCH_ABI)/libOCLogger.a
include $(PREBUILT_STATIC_LIBRARY)

#
# Boost
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE    := boost_thread
LOCAL_SRC_FILES	:= $(BOOST_OBJ)/$(TARGET_ARCH_ABI)/lib/libboost_thread-gcc-mt-1_49.a
include $(PREBUILT_STATIC_LIBRARY)

# ==============================================================
# UBStack - Android
# ==============================================================
include $(CLEAR_VARS)

TARGET_PLATFORM	:= android-16

LOCAL_CPPFLAGS	+= -D_GLIBCXX__PTHREADS
LOCAL_CPPFLAGS	+= -D_THREAD_SAFE
LOCAL_CPPFLAGS	+= -D_REENTRANT
LOCAL_CPPFLAGS	+= -std=gnu++11
LOCAL_CPPFLAGS	+= -s
LOCAL_CPPFLAGS	+= -DHAVE_SOCKET_ERRNO

ifneq ($(TARGET_ARCH), $(X86_ARCH))
	LOCAL_CPPFLAGS		+= -D__arm__
endif

X86_ARCH := x86

LOCAL_MODULE    := UBStack

LOCAL_SRC_FILES := $(UB_OBJ)/src/ocmodelimpl.cpp

LOCAL_C_INCLUDES := $(UB_OBJ)/src
LOCAL_C_INCLUDES += $(UB_OBJ)/include
LOCAL_C_INCLUDES += $(TB_OBJ)/include
LOCAL_C_INCLUDES += $(TB_OBJ)/internal 
LOCAL_C_INCLUDES += $(LOGGER_OBJ)/include

LOCAL_C_INCLUDES += $(BOOST_OBJ)/include

LOCAL_STATIC_LIBRARIES := tbstack
LOCAL_STATIC_LIBRARIES += oclogger
LOCAL_STATIC_LIBRARIES += boost_thread
LOCAL_STATIC_LIBRARIES += occoap

include $(BUILD_SHARED_LIBRARY)