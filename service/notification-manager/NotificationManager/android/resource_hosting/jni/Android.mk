LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ca
LOCAL_SRC_FILES := ../libs/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ca_i
LOCAL_SRC_FILES := ../libs/libca-interface.so
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
LOCAL_MODULE := ocstack-jni
LOCAL_SRC_FILES := ../libs/libocstack-jni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := NotificationManager
LOCAL_SRC_FILES := ../libs/libNotificationManager.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ResourceHosing_JNI
LOCAL_CPPFLAGS := -std=c++0x -frtti -fexceptions

LOCAL_STATIC_LIBRARIES = ca_i
LOCAL_STATIC_LIBRARIES += ca
LOCAL_STATIC_LIBRARIES += oc_logger_core
LOCAL_STATIC_LIBRARIES += oc_logger
LOCAL_STATIC_LIBRARIES += octbstack
LOCAL_STATIC_LIBRARIES += oc
LOCAL_STATIC_LIBRARIES += ocstack-jni
LOCAL_STATIC_LIBRARIES += NotificationManager

LOCAL_C_INCLUDES := ../../../../../../resource/csdk/stack/include/
LOCAL_C_INCLUDES += ../../../../../../resource/csdk/logger/include/
LOCAL_C_INCLUDES += ../../../../../../resource/include/
LOCAL_C_INCLUDES += ../../../../../../resource/oc_logger/include/
LOCAL_C_INCLUDES += ../../../../../../extlibs/boost/boost_1_58_0/
LOCAL_C_INCLUDES += ../../../../NotificationManager/include/


LOCAL_SRC_FILES := ResourceHosing_JNI.cpp
include $(BUILD_SHARED_LIBRARY)
