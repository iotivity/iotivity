# Target specific configuration

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -D_WIN32_WINNT=0x0501

#configure C Files, include paths, c compiler option
override CFLAGS += -D_WIN32_WINNT=0x0501

#configure archiver flags
#override ARFLAGS = rcs

#configure linker flags
override LDLIBS 	+= -lws2_32