#         OCCOAP          #
LOCAL_PATH := $(call my-dir)

ifeq ($(CCFL_CORE), )
CCFL_CORE	:= $(HOME)/peg_ccfl-core
endif

ifeq ($(LOGGER_OBJ), )
LOGGER_OBJ	:= $(CCFL_CORE)/logger
endif

ifeq ($(COAP_OBJ), )
COAP_OBJ	:= $(CCFL_CORE)/libcoap-4.1.1
endif

ifeq ($(OCCOAP_OBJ), )
OCCOAP_OBJ	:= $(CCFL_CORE)/occoap
endif

ifeq ($(TB_OBJ), )
TB_OBJ	:= $(CCFL_CORE)/stack
endif

ifeq ($(SOCKET_OBJ), )
SOCKET_OBJ	:= $(CCFL_CORE)/ocsocket
endif

ifeq ($(ANDROID_OBJ), )
ANDROID_OBJ	:= $(CCFL_CORE)/android
endif

#
# OCSocket
#

#
# OCLogger
#
include $(CLEAR_VARS)
TARGET_PLATFORM := android-16
LOCAL_MODULE	:= oclogger
LOCAL_SRC_FILES	:= $(ANDROID_OBJ)/OCLogger/obj/local/$(TARGET_ARCH_ABI)/libOCLogger.a
include $(PREBUILT_STATIC_LIBRARY)

# ==============================================================
# OCCoap - Android
# ==============================================================
include $(CLEAR_VARS)

TARGET_PLATFORM	:= android-16

LOCAL_CPPFLAGS	+= -D_GLIBCXX__PTHREADS
LOCAL_CPPFLAGS	+= -D_THREAD_SAFE
LOCAL_CPPFLAGS	+= -D_REENTRANT
LOCAL_CPPFLAGS	+= -std=gnu++11
LOCAL_CPPFLAGS	+= -s
LOCAL_CPPFLAGS	+= -DHAVE_SOCKET_ERRNO

LOCAL_CFLAGS 	+= -DWITH_POSIX

ifneq ($(TARGET_ARCH), $(X86_ARCH))
	LOCAL_CPPFLAGS		+= -D__arm__
endif

X86_ARCH := x86

LOCAL_MODULE    := OCCoap

LOCAL_SRC_FILES := $(COAP_OBJ)/async.c
LOCAL_SRC_FILES += $(COAP_OBJ)/block.c
LOCAL_SRC_FILES += $(COAP_OBJ)/coap_list.c
LOCAL_SRC_FILES += $(COAP_OBJ)/debug.c
LOCAL_SRC_FILES += $(COAP_OBJ)/encode.c
LOCAL_SRC_FILES += $(COAP_OBJ)/hashkey.c
LOCAL_SRC_FILES += $(COAP_OBJ)/net.c
LOCAL_SRC_FILES += $(COAP_OBJ)/option.c
LOCAL_SRC_FILES += $(COAP_OBJ)/pdu.c
LOCAL_SRC_FILES += $(COAP_OBJ)/resource.c
LOCAL_SRC_FILES += $(COAP_OBJ)/str.c
LOCAL_SRC_FILES += $(COAP_OBJ)/subscribe.c
LOCAL_SRC_FILES += $(COAP_OBJ)/uri.c

LOCAL_C_INCLUDES := $(COAP_OBJ)

#==
#==  OC STUFF -> Consumes LibCoap stuff
LOCAL_SRC_FILES += $(OCCOAP_OBJ)/src/occoap.c

LOCAL_C_INCLUDES := $(OCCOAP_OBJ)/include
LOCAL_C_INCLUDES += $(LOGGER_OBJ)/include
LOCAL_C_INCLUDES += $(TB_OBJ)/include/
LOCAL_C_INCLUDES += $(TB_OBJ)/include/internal
LOCAL_C_INCLUDES += $(SOCKET_OBJ)/include

LOCAL_STATIC_LIBRARIES := oclogger
#==
#==
include $(BUILD_STATIC_LIBRARY)