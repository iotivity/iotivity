CXX=g++
#CXX=clang -lstdc++ -g

CXX_FLAGS=-std=c++0x -Wall -pthread

CXX_INC=-I../OCProject/include/ -I../OCProject/OCLib/ -I../OCProject/OCLib/OCWrapper/ -I../OCProject/OCLib/OCWrapper/server -I../OCProject/OCLib/OCWrapper/client/ -I../OCProject/csdk/

LIB_SRC0=../OCProject/OCLib/OCPlatform.cpp ../OCProject/OCLib/OCWrapper/server/InProcServerWrapper.cpp ../OCProject/OCLib/OCWrapper/client/InProcClientWrapper.cpp ../OCProject/csdk/ocstack_stub.c ../OCProject/OCLib/OCReflect.cpp ../OCProject/OCLib/OCResource.cpp


platform:
	$(CXX) $(CXX_FLAGS) -o OCPlatformTestApp OCPlatformTestApp.cpp $(LIB_SRC0) $(CXX_INC)

server:
	$(CXX) $(CXX_FLAGS) -o OCServerTestApp OCServerTestApp.cpp $(LIB_SRC0) $(CXX_INC)

client:
	$(CXX) $(CXX_FLAGS) -o OCClientTestApp OCClientTestApp.cpp $(LIB_SRC0) $(CXX_INC)
