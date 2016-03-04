LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../../../out/android/$(TARGET_ARCH_ABI)/debug
LOCAL_MODULE := android-oc
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../../../out/android/$(TARGET_ARCH_ABI)/debug
LOCAL_MODULE := android-easysetup
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libESMediatorRich.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := easysetup-jni

#Add Pre processor definitions
DEFINE_FLAG =  -DWITH_POSIX -D__ANDROID__

#Add Debug flags here
DEBUG_FLAG      = -DTB_LOG

BUILD_FLAG = $(DEFINE_FLAG) $(DEBUG_FLAG)

LOCAL_CPPFLAGS = $(BUILD_FLAG)
LOCAL_CPPFLAGS += -std=c++0x -frtti -fexceptions

$(info CPPFLAGSUPDATED=$(LOCAL_CPPFLAGS))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../inc \
					$(LOCAL_PATH)/../../src \
					$(LOCAL_PATH)/../../../../inc \
					$(LOCAL_PATH)/../../../../../../resource/csdk/logger/include \
				    $(LOCAL_PATH)/../../../../../../resource/csdk/connectivity/common/inc \
					$(LOCAL_PATH)/../../../../../../resource/c_common/oic_string/include \
					$(LOCAL_PATH)/../../../../../../resource/include \
					$(LOCAL_PATH)/../../../../../../resource/c_common \
					$(LOCAL_PATH)/../../../../../../resource/oc_logger/include \
					$(LOCAL_PATH)/../../../../../../resource/csdk/ocmalloc/include \
					$(LOCAL_PATH)/../../../../../../resource/csdk/connectivity/api \
					$(LOCAL_PATH)/../../../../../../resource/csdk/stack/include \
					$(LOCAL_PATH)/../../../../../../resource/csdk/logger/include \
					$(LOCAL_PATH)/../../../../../../resource/csdk/security/include \
					$(LOCAL_PATH)/../../../../../../extlibs/cjson \
					$(LOCAL_PATH)/../../../../../../extlibs/boost/boost_1_58_0 \
                    $(LOCAL_PATH)/../../../../../../extlibs/timer \
					$(LOCAL_PATH)/../../../../../../android/android_api/base/jni \

LOCAL_SRC_FILES += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/*.cpp))
LOCAL_SRC_FILES += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/*.h))

LOCAL_LDLIBS := -llog
#LOCAL_SHARED_LIBRARIES += android-connectivity_abstraction
LOCAL_SHARED_LIBRARIES += android-oc
LOCAL_SHARED_LIBRARIES += android-easysetup

include $(BUILD_SHARED_LIBRARY)
