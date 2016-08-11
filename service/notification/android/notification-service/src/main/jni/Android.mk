LOCAL_PATH := $(call my-dir)

ROOT_PATH := ../../../../../../..

include $(CLEAR_VARS)
OIC_LIB_PATH := $(ROOT_PATH)/out/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)
LOCAL_MODULE := notification_consumer
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libnotification_consumer.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := $(ROOT_PATH)/out/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)
LOCAL_MODULE := notification_provider
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libnotification_provider.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := notification_consumer_jni
LOCAL_CPPFLAGS := -std=c++0x -frtti -fexceptions
LOCAL_LDLIBS := -llog

LOCAL_STATIC_LIBRARIES := ca_interface
LOCAL_STATIC_LIBRARIES += ca
LOCAL_STATIC_LIBRARIES += oc_logger_core
LOCAL_STATIC_LIBRARIES += oc_logger
LOCAL_STATIC_LIBRARIES += octbstack
LOCAL_STATIC_LIBRARIES += oc
LOCAL_STATIC_LIBRARIES += ocstack-jni
LOCAL_STATIC_LIBRARIES += notification_consumer

OIC_SRC_DIR := ../../../../../..

LOCAL_C_INCLUDES := $(OIC_SRC_DIR)/resource/csdk/stack/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/csdk/logger/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/c_common
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/oc_logger/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/src/common
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/src/consumer
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/android/notification-service/src/main/jni/common

LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/extlibs/

LOCAL_SRC_FILES := consumer/notificationConsumer.c
LOCAL_SRC_FILES += common/jniNSCommon.c

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := notification_provider_jni
LOCAL_CPPFLAGS := -std=c++0x -frtti -fexceptions
LOCAL_LDLIBS := -llog

LOCAL_STATIC_LIBRARIES := ca_interface
LOCAL_STATIC_LIBRARIES += ca
LOCAL_STATIC_LIBRARIES += oc_logger_core
LOCAL_STATIC_LIBRARIES += oc_logger
LOCAL_STATIC_LIBRARIES += octbstack
LOCAL_STATIC_LIBRARIES += oc
LOCAL_STATIC_LIBRARIES += ocstack-jni
LOCAL_STATIC_LIBRARIES += notification_provider

OIC_SRC_DIR := ../../../../../..

LOCAL_C_INCLUDES := $(OIC_SRC_DIR)/resource/csdk/stack/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/csdk/logger/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/c_common
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/resource/oc_logger/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/include
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/src/common
LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/service/notification/src/provider

LOCAL_C_INCLUDES += $(OIC_SRC_DIR)/extlibs/

LOCAL_SRC_FILES := notificationProvider.c

include $(BUILD_SHARED_LIBRARY)