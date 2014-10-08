
#IOT_BASE=../../../resource
#BOOST_ROOT=../../../
#BOOST_DIR = $(BOOST_ROOT)boost_1_51_0
#BOOST=${BOOST_DIR}
RST_NAME=.

# Insert your project name.
TARGET=libsqlite.a
#EXCLUDE_LIST=SSMCore_JNI.cpp
EXCLUDE_LIST=

# C++ type Compile Flag define.
CXX=gcc
CXX_FLAGS=-Wall -pthread -DLINUX -ldl


# Linker FLAGS define.
# LIBS=-lajdaemon -lalljoyn -lssl -lcrypto -ldl -llog -lz -lm -lc -lstdc++  -lgcc -lgnustl_static
# LD_FLAGS=-std=c++11 -Wall -shared -lpthread -ldl
GAR=ar
#LD_LIB=${IOT_BASE}/OCLib.a ${IOT_BASE}/csdk/liboctbstack.a 


# Force metatargets to build:
all: ${TARGET} 


${TARGET}: ./sqlite3.c
	${CXX} ${CXX_FLAGS} -c ./sqlite3.c
	ar -cvq ${TARGET} *.o


clean:
	rm -f -v *.o ${TARGET}

	
