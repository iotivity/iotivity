
CXX=g++
#CXX=clang -lstdc++ -g

CXX_FLAGS=-std=c++11 -Wall -pthread

CXX_INC=-I../include/ -I./client/ -I../csdk/stack/include -I../csdk/ocsocket/include -I../csdk/ocrandom/include -I../csdk/logger/include

LIB_SRC0=../OCLib/OCReflect.cpp ../OCLib/OCProperties.cpp ../OCLib/OCObserver.cpp ../OCLib/OCResource.cpp 

LIB_SRC1=../OCLib/OCReflect.cpp ../OCLib/OCObserver.cpp ./client/MyObserverHandler.cpp ../OCLib/OCResource.cpp ../OCLib/OCSecurityModel.cpp ../OCLib/InProcClientWrapper.cpp ../OCLib/OCPlatform.cpp ../OCLib/InProcServerWrapper.cpp ../csdk/liboctbstack.a

all: ocreflect 

ocreflect: test_properties test_OCClient

test_properties:
	$(CXX) $(CXX_FLAGS) -o test_properties reflect/test_properties.cpp $(LIB_SRC0) $(CXX_INC)

test_OCReflect:
	$(CXX) $(CXX_FLAGS) -o test_OCReflect reflect/test_OCReflect.cpp ../OCLib/OCReflect.cpp ../OCLib/OCProperties.cpp  $(CXX_INC)

test_OCClient:
	$(CXX) $(CXX_FLAGS) -o test_OCClient reflect/test_OCClient.cpp $(LIB_SRC0) $(CXX_INC)

clean:
	rm -f -v test_properties test_OCReflect test_OCClient 

