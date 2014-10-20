APP_STL := gnustl_static #stlport_static
APP_PLATFORM := android-16
APP_ABI := armeabi armeabi-v7a x86
APP_OPTIM := release
#APP_OPTIM := debug
APP_CPPFLAGS += -fno-strict-aliasing
APP_CPPFLAGS += -std=gnu++11
APP_CPPFLAGS += -D_GLIBCXX_USE_WCHAR_T
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
#NDK_TOOLCHAIN_VERSION=4.8