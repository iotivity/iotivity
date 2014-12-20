# Target specific configuration

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += 

#comment below line to disable remote access
override ENABLE_REMOTE_ACCESS = 1

#configure C Files, include paths, c compiler option
override CFLAGS += 

#configure archiver flags
#override ARFLAGS = 

#configure linker flags
override LDLIBS 	+= -llog