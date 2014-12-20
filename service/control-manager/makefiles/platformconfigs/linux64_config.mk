#configure target names
export STATIC_LIB_PREFIX		=	lib
export SHARED_LIB_PREFIX		=	lib
export STATIC_LIB_EXTENSION		=	a
export SHARED_LIB_EXTENSION		=	so

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
#unset BUILD_IMPORT_LIBRARIES

ifeq ($(CONFIG),debug)
export BOOST_SYSTEM_LIB = boost_system-mt-d
export BOOST_THREAD_LIB = boost_thread-mt-d
export BOOST_REGEX_LIB  = boost_regex-mt-d
export OC_LIB = oc
export OCTBSTACK_LIB = octbstack
export OCLOGGER_LIB = oc_logger
else
export BOOST_SYSTEM_LIB = boost_system-mt
export BOOST_THREAD_LIB = boost_thread-mt
export BOOST_REGEX_LIB  = boost_regex-mt
export OC_LIB = oc
export OCTBSTACK_LIB = octbstack
export OCLOGGER_LIB = oc_logger
endif

export OPENSSL_CRYPTO	= crypto
export OPENSSL_SSL		= ssl

export JDK_INCLUDES		= -I${JDK_HOME}/include/linux \
						  -I${JDK_HOME}/include\


OSLDFLAGS=$(LDFLAGS)
NOSLDFLAGS=$(LDFLAGS) -Wl,--no-undefined
RTLIB=-lrt
PTLIB=-lpthread
						  
