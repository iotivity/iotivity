APP_STL := gnustl_static

APP_PLATFORM = android-19
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
APP_CFLAGS = -Wno-error=format-security