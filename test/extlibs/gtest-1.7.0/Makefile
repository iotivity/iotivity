CC = g++
AR = ar
RM = /bin/rm -f

GTEST_DIR = .
DESTDIR ?= /usr

SRC = ${GTEST_DIR}/src
GTEST_CPP = ${SRC}/gtest-all.cc
TARGET = bin/linux/libgtest.a
TARGET_TIZEN = libgtest.a

CINCLUDE = -I${GTEST_DIR}/
CFLAGS = -isystem ${GTEST_DIR}/include -pthread

.PHONY: all clean 
all: gtest_linux

clean:
	$(RM) $(TARGET) *.o
	
gtest_linux:
	$(CC) $(CFLAGS) $(CINCLUDE) -c $(GTEST_CPP)
	$(AR) -rv $(TARGET) *.o
	$(RM) *.o

gtest_tizen:  
	$(CC) $(CFLAGS) $(CINCLUDE) -c $(GTEST_CPP)
	$(AR) -rv $(TARGET_TIZEN) *.o
	$(RM) *.o

install:
	mkdir -p ${DESTDIR}/bin
	cp ${TARGET_TIZEN} ${DESTDIR}/bin/	