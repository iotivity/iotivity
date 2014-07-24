CXX=g++ -g -O0
#CXX=clang -lstdc++ -g -O0

CXX_FLAGS=-std=c++11 -Wall -pthread

CXX_INC=-I../include/ -I./client/ -I../csdk/stack/include -I../csdk/ocsocket/include -I../csdk/ocrandom/include -I../csdk/logger/include

LIB_SRC0=../OCLib/OCReflect.cpp ../OCLib/OCProperties.cpp ../OCLib/OCObserver.cpp ../OCLib/OCResource.cpp ./client/MyObserverHandler.cpp  ../OCLib/OCServer.cpp

LIB_SRC1=../OCLib/OCReflect.cpp ../OCLib/OCObserver.cpp ./client/MyObserverHandler.cpp ../OCLib/OCResource.cpp ../OCLib/OCSecurityModel.cpp ../OCLib/InProcClientWrapper.cpp ../OCLib/OCPlatform.cpp ../OCLib/InProcServerWrapper.cpp ../csdk/liboctbstack.a ../OCLib/OCServer.cpp

CXX_LIBS=-lboost_regex

.PHONY: ocreflect resources test_OCReflect test_OCClient test_properties

all: .PHONY

ocreflect: test_properties test_OCClient

test_properties:

	$(CXX) $(CXX_FLAGS) -o test_properties reflect/test_properties.cpp $(LIB_SRC0) $(CXX_INC) $(CXX_LIBS)

test_OCReflect:
	$(CXX) $(CXX_FLAGS) -o test_OCReflect reflect/test_OCReflect.cpp ../OCLib/OCReflect.cpp ../OCLib/OCProperties.cpp ../OCLib/OCServer.cpp  $(CXX_INC) $(CXX_LIBS)

test_OCClient:
	$(CXX) $(CXX_FLAGS) -o test_OCClient reflect/test_OCClient.cpp $(LIB_SRC0) $(CXX_INC) $(CXX_LIBS)

resources: single_resource multiple_resources

single_resource:
	$(CXX) $(CXX_FLAGS) -o single_resource ./server/single_resource.cpp $(LIB_SRC1) $(CXX_INC) $(CXX_LIBS)

multiple_resources:
	#$(CXX) $(CXX_FLAGS) -o multiple_resources ./server/multiple_resources.cpp $(LIB_SRC1) $(CXX_INC) $(CXX_LIBS)

clean:
	rm -f -v test_properties test_OCReflect test_OCClient 

