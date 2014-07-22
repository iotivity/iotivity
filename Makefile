
CXX=g++
#CXX=clang
CXX_FLAGS=-std=c++11 -Wall -pthread -ggdb
CXX_INC=-I./include/ -I./csdk/stack/include -I./csdk/ocsocket/include -I./csdk/ocrandom/include -I./csdk/logger/include

# Force metatargets to build:
.PHONY: c_sdk single_resource client examples

all: .PHONY

c_sdk:
	cd csdk && $(MAKE)

examples:
	cd examples && $(MAKE)

single_resource: OCLib.a single_resource.o
	$(CXX) $(CXX_FLAGS) -o single_resource single_resource.o OCLib.a csdk/liboctbstack.a 

client: OCLib.a OCClient.o
	$(CXX) $(CXX_FLAGS) -o client OCClient.o OCLib.a csdk/liboctbstack.a 

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

single_resource.o : examples/server/single_resource.cpp
	$(CXX) $(CXX_FLAGS) -c examples/server/single_resource.cpp $(CXX_INC)

OCClient.o : examples/client/OCClient.cpp
	$(CXX) $(CXX_FLAGS) -c examples/client/OCClient.cpp $(CXX_INC)

clean: 
	rm -f -v OCLib.a *.o single_resource client
	cd examples && $(MAKE) clean

	cd csdk && $(MAKE) clean
	cd csdk && $(MAKE) deepclean


# Force metatargets to build:
.PHONY: c_sdk single_resource client examples

all: .PHONY

c_sdk:
	cd csdk && $(MAKE)

examples:
	cd examples && $(MAKE)

single_resource: OCLib.a single_resource.o
	$(CXX) $(CXX_FLAGS) -o single_resource single_resource.o OCLib.a csdk/liboctbstack.a 

client: OCLib.a OCClient.o
	$(CXX) $(CXX_FLAGS) -o client OCClient.o OCLib.a csdk/liboctbstack.a 

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

single_resource.o : examples/server/single_resource.cpp
	$(CXX) $(CXX_FLAGS) -c examples/server/single_resource.cpp $(CXX_INC)

OCClient.o : examples/client/OCClient.cpp
	$(CXX) $(CXX_FLAGS) -c examples/client/OCClient.cpp $(CXX_INC)

clean: 
	rm -f -v OCLib.a *.o single_resource client
	cd examples && $(MAKE) clean

	cd csdk && $(MAKE) clean
	cd csdk && $(MAKE) deepclean

