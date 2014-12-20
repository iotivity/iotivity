#configure target names
export STATIC_LIB_PREFIX		=	lib
export SHARED_LIB_PREFIX		=	lib
export STATIC_LIB_EXTENSION		=	a
export SHARED_LIB_EXTENSION		=	so

#configure linker program
LINKER = $(CXX)

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -fpic

#configure C Files, include paths, c compiler option
override CFLAGS +=  -fpic

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
#override LDFLAGS +=
override LDLIBS +=
 

#build import libraries
#unset BUILD_IMPORT_LIBRARIES

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

OSLDFLAGS=$(LDFLAGS)
NOSLDFLAGS=$(LDFLAGS) -Wl,--no-undefined
RTLIB=-lrt
PTLIB=-lpthread
						  
