LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ca
LOCAL_SRC_FILES := ../libs/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := oc_logger_core
LOCAL_SRC_FILES := ../libs/liboc_logger_core.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := oc_logger
LOCAL_SRC_FILES := ../libs/liboc_logger.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := octbstack
LOCAL_SRC_FILES := ../libs/liboctbstack.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := oc
LOCAL_SRC_FILES := ../libs/liboc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libNotificationManager
LOCAL_SRC_FILES := ../libs/libNotificationManager.so
include $(PREBUILT_SHARED_LIBRARY)
