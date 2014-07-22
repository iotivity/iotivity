#         TBSTACK          #
LOCAL_PATH := $(call my-dir)

ifeq ($(CCFL_CORE), )
CCFL_CORE	:= $(HOME)/peg_ccfl-core
endif

ifeq ($(OCCOAP_OBJ), )
OCCOAP_OBJ	:= $(CCFL_CORE)/occoap
endif

ifeq ($(TB_OBJ), )
TB_OBJ	:= $(CCFL_CORE)/stack
endif

ifeq ($(LOGGER_OBJ), )
LOGGER_OBJ	:= $(CCFL_CORE)/logger
endif

ifeq ($(ANDROID_OBJ), )
ANDROID_OBJ	:= $(CCFL_CORE)/android
endif

#
# OCCoap
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE	:= occoap
LOCAL_SRC_FILES	:= $(ANDROID_OBJ)/OCCoap/obj/local/$(TARGET_ARCH_ABI)/libOCCoap.a
include $(PREBUILT_STATIC_LIBRARY)

# ==============================================================
# OCLib - Android
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

LOCAL_MODULE    := TBStack

LOCAL_SRC_FILES := $(TB_OBJ)/src/ocstack.c

LOCAL_C_INCLUDES := $(TB_OBJ)/include/
LOCAL_C_INCLUDES += $(TB_OBJ)/include/internal
LOCAL_C_INCLUDES += $(OCCOAP_OBJ)/include
LOCAL_C_INCLUDES += $(LOGGER_OBJ)/include

LOCAL_STATIC_LIBRARIES += occoap

include $(BUILD_STATIC_LIBRARY)