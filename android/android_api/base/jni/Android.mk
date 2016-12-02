LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI := $(APP_ABI)
SECURED := $(SECURE)
WITH_CLOUD := $(WITH_CLOUD)
WITH_TCP := $(WITH_TCP)
WITH_MQ_PUB := $(WITH_MQ_PUB)
WITH_MQ_SUB := $(WITH_MQ_SUB)
WITH_MQ_BROKER := $(WITH_MQ_BROKER)
RD_MODE := $(RD_MODE)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-oc_logger
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboc_logger.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-octbstack
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboctbstack.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-oc
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/liboc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-ca
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libconnectivity_abstraction.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-rd
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libresource_directory.so
include $(PREBUILT_SHARED_LIBRARY)

ifeq ($(SECURED), 1)
include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-ocprovision
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libocprovision.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_LIB_PATH := ../../../../out/android/$(APP_ABI)/$(APP_OPTIM)
LOCAL_MODULE := libandroid-ocpmapi
LOCAL_SRC_FILES := $(OIC_LIB_PATH)/libocpmapi.so
include $(PREBUILT_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)
OIC_SRC_PATH := ../../../resource
LOCAL_MODULE := libca-interface
LOCAL_SRC_FILES := JniCaInterface.c
LOCAL_STATIC_LIBRARIES := libandroid-ca
LOCAL_LDLIBS := -llog
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/connectivity/api
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/c_common
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
OIC_SRC_PATH := ../../../resource
OIC_OUT_PATH := ../../../out
LOCAL_MODULE    := ocstack-jni

ifeq ($(WITH_CLOUD), 1)
    LOCAL_CPPFLAGS += -DWITH_CLOUD
endif

ifeq ($(WITH_TCP), 1)
    LOCAL_CPPFLAGS += -D__WITH_TLS__
endif

MQ_FLAG = 0
ifeq ($(WITH_MQ_PUB), 1)
LOCAL_CFLAGS += -DWITH_MQ -DMQ_PUBLISHER
MQ_FLAG = 1
endif
ifeq ($(WITH_MQ_SUB), 1)
LOCAL_CFLAGS += -DWITH_MQ -DMQ_SUBSCRIBER
MQ_FLAG = 1
endif
ifeq ($(WITH_MQ_BROKER), 1)
LOCAL_CFLAGS += -DWITH_MQ -DMQ_BROKER
MQ_FLAG = 1
endif

LOCAL_SRC_FILES :=  JniOcStack.cpp \
                    JniUtils.cpp \
                    JniEntityHandler.cpp \
                    JniOnResourceFoundListener.cpp \
                    JniOnDeviceInfoListener.cpp \
                    JniOnPlatformInfoListener.cpp \
                    JniOnPresenceListener.cpp \
                    JniOnGetListener.cpp \
                    JniOnPutListener.cpp \
                    JniOnPostListener.cpp \
                    JniOnDeleteListener.cpp \
                    JniOnObserveListener.cpp \
                    JniOcRepresentation.cpp \
                    JniOcResourceHandle.cpp \
                    JniOcPresenceHandle.cpp \
                    JniOcRequestHandle.cpp \
                    JniOcResourceRequest.cpp \
                    JniOcResourceResponse.cpp \
                    JniOcPlatform.cpp \
                    JniOcResource.cpp \
                    JniOcResourceIdentifier.cpp \
                    JniOcSecurity.cpp \
                    JniOnDPDevicesFoundListener.cpp \
                    JniOnDirectPairingListener.cpp \
                    JniOcDirectPairDevice.cpp \
                    JniOnPublishResourceListener.cpp \
                    JniOnDeleteResourceListener.cpp \
                    JniOcRDClient.cpp

ifeq ($(MQ_FLAG), 1)
    LOCAL_SRC_FILES +=  JniOnMQTopicFoundListener.cpp \
                        JniOnMQSubscribeListener.cpp
endif

ifeq ($(SECURED), 1)
    LOCAL_SRC_FILES +=  JniOcSecureResource.cpp \
                        JniOcProvisioning.cpp \
                        JniSecureUtils.cpp \
                        JniProvisionResultListner.cpp \
                        JniPinCheckListener.cpp \
                        JniDisplayPinListener.cpp
endif

ifeq ($(WITH_CLOUD), 1)
    LOCAL_SRC_FILES +=  JniOcAccountManager.cpp
endif

ifeq ($(WITH_TCP), 1)
ifeq ($(SECURED), 1)
    LOCAL_SRC_FILES +=  JniOcCloudProvisioning.cpp \
                        JniOcCloudResultListener.cpp \
                        JniGetAclIdByDeviceListener.cpp
endif
endif

LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES := android-oc
LOCAL_STATIC_LIBRARIES += android-octbstack
LOCAL_STATIC_LIBRARIES += android-coap
LOCAL_STATIC_LIBRARIES += android-oc_logger
LOCAL_STATIC_LIBRARIES += android-ca
LOCAL_STATIC_LIBRARIES += android_cpp11_compat
ifeq ($(SECURED), 1)
LOCAL_STATIC_LIBRARIES += android-ocprovision
LOCAL_STATIC_LIBRARIES += android-ocpmapi
endif
LOCAL_SHARED_LIBRARIES += android-rd

ifeq "$(RD_MODE)" "CLIENT"
LOCAL_CPPFLAGS += -DRD_CLIENT
endif
LOCAL_CPPFLAGS += -std=c++0x
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_C_INCLUDES := $(OIC_SRC_PATH)/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/c_common
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/c_common/oic_string/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/c_common/oic_malloc/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/stack/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/ocsocket/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/resource-directory/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/oc_logger/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/../extlibs/boost/boost_1_58_0
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/../extlibs/cjson
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/../extlibs/tinycbor/tinycbor/src
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/../build_common/android/compatibility
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/provisioning/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/provisioning/include/oxm/
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/provisioning/include/internal
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/provisioning/include/cloud/
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/include
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/security/include/internal
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/connectivity/api
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/connectivity/inc/pkix
LOCAL_C_INCLUDES += $(OIC_SRC_PATH)/csdk/connectivity/lib/libcoap-4.1.1/include
include $(BUILD_SHARED_LIBRARY)
