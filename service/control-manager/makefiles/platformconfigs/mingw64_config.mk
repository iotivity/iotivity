#configure target names
export STATIC_LIB_PREFIX		=	lib
export SHARED_LIB_PREFIX		=	
export STATIC_LIB_EXTENSION		=	a
export SHARED_LIB_EXTENSION		=	dll

#configure linker program
LINKER = $(CXX)

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -m64

#configure C Files, include paths, c compiler option
override CFLAGS += -m64

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
override LDFLAGS += -m64 -static-libgcc -static-libstdc++
override LDLIBS +=

#build import libraries
export BUILD_IMPORT_LIBRARIES = true

ifeq ($(CONFIG),debug)
export BOOST_SYSTEM_LIB = boost_system-mt-d
export BOOST_THREAD_LIB = boost_thread-mt-d
export BOOST_REGEX_LIB  = boost_regex-mt-d
else
export BOOST_SYSTEM_LIB = boost_system-mt
export BOOST_THREAD_LIB = boost_thread-mt
export BOOST_REGEX_LIB  = boost_regex-mt
endif

export JDK_INCLUDES		= -I"${JDK_HOME}/include/win32" \
						  -I"${JDK_HOME}/include"\

export OPENSSL_CRYPTO	= eay32
export OPENSSL_SSL		= ssleay32

OSLDFLAGS=$(LDFLAGS)
NOSLDFLAGS=$(LDFLAGS) -Wl,--no-undefined
