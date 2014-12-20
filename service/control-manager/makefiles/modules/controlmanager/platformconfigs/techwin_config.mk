# Target specific configuration

#configure CPP Files, include paths, cpp compiler option
#comment below line to disable remote access
#override ENABLE_REMOTE_ACCESS = 1
override LDFLAGS +=${PTLIB} ${RTLIB}
