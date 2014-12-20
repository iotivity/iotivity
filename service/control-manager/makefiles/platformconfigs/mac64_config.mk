#configure target names
export STATIC_LIB_PREFIX		=	lib
export SHARED_LIB_PREFIX		=	lib
export STATIC_LIB_EXTENSION		=	a
export SHARED_LIB_EXTENSION		=	dylib

#configure linker program
LINKER = $(CXX)

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -m64 -fPIC

#configure C Files, include paths, c compiler option
override CFLAGS += -m64 -fPIC

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
override LDFLAGS += -m64
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

export OPENSSL_CRYPTO	= crypto
export OPENSSL_SSL		= ssl

export JDK_INCLUDES		= -I${JDK_HOME}/Headers

OSLDFLAGS=$(LDFLAGS)
NOSLDFLAGS=$(LDFLAGS)
PTLIB=-lpthread
						  
