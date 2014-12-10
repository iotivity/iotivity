LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OIC_BASE_PATH := ../../../../resource
LOCAL_MODULE := libandroid-oc_logger
LOCAL_SRC_FILES := $(OIC_BASE_PATH)/oc_logger/lib/oc_logger.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_BASE_PATH := ../../../../resource
LOCAL_MODULE := libandroid-octbstack
LOCAL_SRC_FILES := $(OIC_BASE_PATH)/csdk/android/release/liboctbstack.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_BASE_PATH := ../../../../resource
LOCAL_MODULE := libandroid-oc
LOCAL_SRC_FILES := $(OIC_BASE_PATH)/release/obj/liboc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_BASE_PATH := ../../../resource
LOCAL_MODULE    := ocstack-jni
LOCAL_SRC_FILES :=  ocstack-jni.cpp \
                    ocheaderoption-jni.cpp \
                    ocrepresentation-jni.cpp \
                    ocresource-jni.cpp \
                    ocplatform-jni.cpp \
                    platformcfg-jni.cpp
LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES := android-oc
LOCAL_STATIC_LIBRARIES += android-octbstack
LOCAL_STATIC_LIBRARIES += android-oc_logger
LOCAL_CPPFLAGS += -std=c++0x -frtti -fexceptions
LOCAL_C_INCLUDES := $(OIC_BASE_PATH)/include
LOCAL_C_INCLUDES += $(OIC_BASE_PATH)/csdk/stack/include
LOCAL_C_INCLUDES += $(OIC_BASE_PATH)/csdk/ocsocket/include
LOCAL_C_INCLUDES += $(OIC_BASE_PATH)/oc_logger/include
LOCAL_C_INCLUDES += $(OIC_BASE_PATH)/../../oic-utilities/android/boost/include
LOCAL_C_INCLUDES += $(OIC_BASE_PATH)/dependencies/cereal/include
include $(BUILD_SHARED_LIBRARY)

