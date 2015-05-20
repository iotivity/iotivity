LOCAL_PATH := $(call my-dir)

ifeq ($(strip $(ANDROID_NDK)),)
$(error ANDROID_NDK is not set!)
endif

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../../dep/android/$(TARGET_ARCH_ABI)/usr/lib
LOCAL_MODULE := libandroid-boost_system
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../../out/android/$(TARGET_ARCH_ABI)/release
LOCAL_MODULE := libandroid-thingsmanager
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libTGMSDKLibrary.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(TARGET_ARCH_ABI)/release
BASE_LIB_PATH := ../../../../android/android_api/base/libs/$(TARGET_ARCH_ABI)
OIC_RESOURCE_PATH := ../../../../resource
OIC_SERVICE_PATH := ../../../../service
OIC_OUT_PATH := ../../../../out
LOCAL_MODULE    := things-manager-jni

LOCAL_C_INCLUDES := $(OIC_RESOURCE_PATH)/include \
                    $(OIC_RESOURCE_PATH)/csdk/stack/include \
                    $(OIC_RESOURCE_PATH)/csdk/ocsocket/include \
                    $(OIC_RESOURCE_PATH)/oc_logger/include \
                    $(OIC_RESOURCE_PATH)/android/include \
                    $(OIC_RESOURCE_PATH)/dependencies/cereal/include \
                    $(OIC_RESOURCE_PATH)/../extlibs/boost/boost_1_58_0 \
                    $(OIC_RESOURCE_PATH)/../extlibs/timer \
                    $(OIC_SERVICE_PATH)/things-manager/sdk/inc \
                    $(OIC_SERVICE_PATH)/things-manager/sdk/src \
                    $(OIC_SERVICE_PATH)/../android/android_api/base/jni \
                    $(OIC_SERVICE_PATH)/../build_common/android/compatibility \
                    $(LOCAL_PATH)/jniutil/inc \
                    $(LOCAL_PATH)/tm/inc \
                    $(LOCAL_PATH)/tm/src \
                    $(LOCAL_PATH)/tm/src/base \
                    $(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(TOOLCHAIN_VERSION)/include \
                    $(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(TOOLCHAIN_VERSION)/libs/$(TARGET_ARCH_ABI)/include \

LOCAL_SRC_FILES += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/jniutil/src/*.cpp))
LOCAL_SRC_FILES += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/tm/src/*.cpp))

LOCAL_CPPFLAGS := -std=c++0x -frtti -fexceptions

LOCAL_LDLIBS := -llog -L$(BASE_LIB_PATH) -locstack-jni -L$(OIC_LIB_PATH) -loc -loctbstack -lcoap -loc_logger
LOCAL_LDLIBS += -L$(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(TOOLCHAIN_VERSION)/libs/$(TARGET_ARCH_ABI) -lgnustl_shared
LOCAL_SHARED_LIBRARIES := android-thingsmanager
LOCAL_STATIC_LIBRARIES := android-boost_system

include $(BUILD_SHARED_LIBRARY)
