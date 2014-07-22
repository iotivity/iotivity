#         LOGGER           #
LOCAL_PATH := $(call my-dir)

ifeq ($(CCFL_CORE), )
CCFL_CORE	:= $(HOME)/peg_ccfl-core
endif

ifeq ($(LOGGER_OBJ), )
LOGGER_OBJ	:= $(CCFL_CORE)/logger
endif

# ==============================================================
# OCLogger - Android
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

LOCAL_MODULE    := OCLogger

LOCAL_SRC_FILES := $(LOGGER_OBJ)/src/logger.c

LOCAL_C_INCLUDES := $(LOGGER_OBJ)/include

LOCAL_STATIC_LIBRARIES += tbstack

include $(BUILD_STATIC_LIBRARY)