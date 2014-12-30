LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-oc_logger
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboc_logger.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-octbstack
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboctbstack.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-oc
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-coap
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libcoap.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-compatibility
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libcompatibility.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_SRC_PATH := ../../../resource
OIC_OUT_PATH := ../../../out
LOCAL_MODULE    := ocstack-jni
LOCAL_SRC_FILES := ocstack-jni.cpp \
                   ocheaderoption-jni.cpp \
                   ocrepresentation-jni.cpp \
                   ocresource-jni.cpp \
                   ocplatform-jni.cpp \
                   platformcfg-jni.cpp
LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES := android-oc
LOCAL_STATIC_LIBRARIES += android-octbstack
LOCAL_STATIC_LIBRARIES += android-coap
LOCAL_STATIC_LIBRARIES += android-oc_logger
LOCAL_STATIC_LIBRARIES += libandroid-compatibility
LOCAL_CPPFLAGS += -std=c++0x -frtti -fexceptions
LOCAL_C_INCLUDES := $(OIC_SRC_PATH)/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/stack/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/ocsocket/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/oc_logger/include
LOCAL_C_INCLUDES += $(OIC_OUT_PATH)/android/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/dependencies/cereal/include
include $(BUILD_SHARED_LIBRARY)

