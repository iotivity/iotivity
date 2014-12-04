#
# override with `make BUILD=debug`
# override with `make PLATFORM=arduinomega` or `make PLATFORM=arduinodue`
# default to release build
# default to build for linux
BUILD	 := release
PLATFORM := linux

ifeq ($(PLATFORM),linux)
$(info *******************linux Build !!******************)

CC=gcc

ROOT_DIR 	   = inc/
INC_DIRS	  := -I$(ROOT_DIR)/

#CFLAGS=$(INC_DIRS) -std=c99 -DWITH_POSIX
#CFLAGS=-std=c99
#CFLAGS=$(INC_DIRS) -std=c99 -DWITH_POSIX -g
CFLAGS=$(INC_DIRS) -DWITH_POSIX -D__TIZEN__
#COMPILEFLAG = `pkg-config --cflags --libs com.samsung.oicca com.samsung.oicca-devel`
COMPILEFLAG = `pkg-config --cflags --libs com.samsung.oicca capi-network-wifi gobject-2.0 glib-2.0 dlog capi-network-bluetooth`

OBJECTS	:= interfacesample.o casample.o

all: $(OBJECTS)
	$(CC) interfacesample.o -o interface_sample /usr/lib/liboicinterface.a -lm -lpthread -lrt -ldl -lstdc++ -lgobject-2.0 -lgio-2.0 -lglib-2.0 -lcapi-network-wifi -ldlog -lcapi-network-bluetooth
	$(CC) casample.o -o ca_sample /usr/lib/liboicinterface.a -lm -lpthread -lrt -ldl -lstdc++ -lgobject-2.0 -lgio-2.0 -lglib-2.0 -lcapi-network-wifi -ldlog -lcapi-network-bluetooth
interfacesample.o: interfacesample.c
	$(CC) $(CFLAGS) $(COMPILEFLAG) -c interfacesample.c -o interfacesample.o
casample.o: casample.c
	$(CC) $(CFLAGS) $(COMPILEFLAG) -c casample.c -o casample.o
pack:
#$(CC) $(CFLAGS) $(COMPILEFLAG) wifi_interface.c bt_interface.c tc_interface.c sample_main.c -o sample /usr/lib/liboctbstack.a -lm
	$(info **************************************TEST***************************)
install:
#cp -rf sample ./usr/apps/com.samsung.oicca/bin/sample

clean:
	rm -rf *.o interface_sample
	rm -rf *.o ca_sample
	
else ifeq ($(PLATFORM),arduinodue)
$(info *********Arduinodue Build!!**********)	

#Make Compilation Script for arduino


else
   $(info *********error**********)
   $(error Wrong value for PLATFORM !!)
endif
