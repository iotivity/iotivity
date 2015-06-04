LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libexpat
LOCAL_SRC_FILES	:= ../../../../../../dep/android/armeabi/usr/lib/libexpat.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libboost_thread
LOCAL_SRC_FILES	:= ../../../../../../dep/android/armeabi/usr/lib/libboost_thread.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libboost_system
LOCAL_SRC_FILES	:= ../../../../../../dep/android/armeabi/usr/lib/libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libcpluff
LOCAL_SRC_FILES	:= ../../../../../../out/android/armeabi/release/libcpluff.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libpmimpl
LOCAL_SRC_FILES	:= ../../../../../../out/android/armeabi/release/libpmimpl.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_DEFAULT_CPP_EXTENSION	:= cpp
LOCAL_MODULE				:= PluginManager
LOCAL_SRC_FILES				:= PluginManager.cpp
LOCAL_CPPFLAGS 				:= -std=c++0x -frtti -fexceptions
LOCAL_LDLIBS				:= -llog -ldl -lz

LOCAL_STATIC_LIBRARIES		:= libcpluff
LOCAL_STATIC_LIBRARIES		+= libpmimpl
LOCAL_STATIC_LIBRARIES		+= libboost_thread
LOCAL_STATIC_LIBRARIES		+= libboost_system
LOCAL_STATIC_LIBRARIES		+= libexpat

LOCAL_C_INCLUDES 			:= ../../../../../../extlibs/boost/boost_1_58_0
LOCAL_C_INCLUDES			+= ../../../../lib/cpluff/libcpluff
LOCAL_C_INCLUDES			+= ../../../src
LOCAL_C_INCLUDES			+= ../../../../../../extlibs/rapidxml

include $(BUILD_SHARED_LIBRARY)
