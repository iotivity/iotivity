# Target specific configuration

#configure CPP Files, include paths, cpp compiler option
#comment below line to disable remote access
#override ENABLE_REMOTE_ACCESS = 1
#override LDFLAGS += -lpthread -lrt

override CXXFLAGS += -D__TIZEN__=1
override CFLAGS += -D__TIZEN__=1

override CPP_INC_DIR += -I"${TIZEN_SDK_LOCATION}/platforms/tizen2.2/rootstraps/tizen-device-2.2.native/usr/include/osp"
override LDFLAGS += -L"${TIZEN_SDK_LOCATION}/platforms/tizen2.2/rootstraps/tizen-device-2.2.native/usr/lib/osp"

override RTLIB+=-losp-appfw
