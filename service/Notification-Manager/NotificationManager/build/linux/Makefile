VPATH = ../../src:../../../SampleApp/linux/sampleConsumer:../../../SampleApp/linux/sampleProvider

OCPATH = ../../../../../resource
OCINCLUDE = $(OCPATH)/include
STACKINCLUDE = $(OCPATH)/csdk/stack/include
SOCKETINCLUDE = $(OCPATH)/csdk/ocsocket/include
NOTIFICATIONINCLUDE = ../../include
BOOSTPATH = ../../../../../boost_1_51_0

CXX=g++
CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -std=c++0x -I$(NOTIFICATIONINCLUDE) -I$(OCINCLUDE) -I$(STACKINCLUDE) -I$(SOCKETINCLUDE) -I$(BOOSTPATH)

OBJS =	 ResourceManager.o RegistrationManager.o VirtualRepresentation.o NotificationManager.o LinuxMain.o

LIBS = $(OCPATH)/build/linux/release/obj/liboc.a $(OCPATH)/csdk/build/linux/release/liboctbstack.a

all:	NotificationManager SampleProvider SampleConsumer

NotificationManager:	$(OBJS)
	$(CXX) -o NotificationManager $(OBJS)  $(LIBS) -pthread 

SampleProvider:	SampleProvider.o
	$(CXX) -o SampleProvider SampleProvider.o  $(LIBS) -pthread

SampleConsumer:	SampleConsumer.o
	$(CXX) -o SampleConsumer SampleConsumer.o  $(LIBS) -pthread

clean:
	rm -f $(OBJS) SampleConsumer.o SampleProvider.o NotificationManager SampleProvider SampleConsumer
