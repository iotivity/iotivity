LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := coap
LOCAL_SRC_FILES := ../libs/libcoap.so
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
LOCAL_MODULE := SSMCore
LOCAL_SRC_FILES := ../libs/libSSMCore.so
include $(PREBUILT_SHARED_LIBRARY)