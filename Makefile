
CXX=g++
#CXX=clang
CXX_FLAGS=-std=c++11 -Wall -pthread -ggdb
CXX_INC=-I./include/ -I./csdk/stack/include -I./csdk/ocsocket/include -I./csdk/ocrandom/include -I./csdk/logger/include

# Force metatargets to build:
.PHONY: c_sdk simpleserver simpleclient examples

all: .PHONY

c_sdk:
	cd csdk && $(MAKE)

examples:
	cd examples && $(MAKE)

simpleserver: OCLib.a simpleserver.o
	$(CXX) $(CXX_FLAGS) -o simpleserver simpleserver.o OCLib.a csdk/liboctbstack.a 

simpleclient: OCLib.a simpleclient.o
	$(CXX) $(CXX_FLAGS) -o simpleclient simpleclient.o OCLib.a csdk/liboctbstack.a 

OCLib.a: OCPlatform.o OCResource.o OCReflect.o InProcServerWrapper.o InProcClientWrapper.o  
	ar -cvq OCLib.a OCPlatform.o OCResource.o OCReflect.o InProcServerWrapper.o InProcClientWrapper.o 

OCReflect.o: OCLib/OCReflect.cpp
	$(CXX) $(CXX_FLAGS) -c OCLib/OCReflect.cpp $(CXX_INC)

OCPlatform.o: OCLib/OCPlatform.cpp
	$(CXX) $(CXX_FLAGS) -c OCLib/OCPlatform.cpp $(CXX_INC)
 
OCResource.o: OCLib/OCResource.cpp
	$(CXX) $(CXX_FLAGS) -c OCLib/OCResource.cpp $(CXX_INC)
	
InProcServerWrapper.o: OCLib/InProcServerWrapper.cpp
	$(CXX) $(CXX_FLAGS) -c OCLib/InProcServerWrapper.cpp $(CXX_INC)

InProcClientWrapper.o: OCLib/InProcClientWrapper.cpp
	$(CXX) $(CXX_FLAGS) -c OCLib/InProcClientWrapper.cpp $(CXX_INC)

simpleserver.o : examples/simpleserver.cpp
	$(CXX) $(CXX_FLAGS) -c examples/simpleserver.cpp $(CXX_INC)

simpleclient.o : examples/simpleclient.cpp
	$(CXX) $(CXX_FLAGS) -c examples/simpleclient.cpp $(CXX_INC)

clean: 
	rm -f -v OCLib.a *.o simpleserver simpleclient
	cd examples && $(MAKE) clean

	cd csdk && $(MAKE) clean
	cd csdk && $(MAKE) deepclean
