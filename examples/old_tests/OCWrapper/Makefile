CXX=g++
CXX_FLAGS=-std=c++11 -Wall -pthread

final: server client serverApp

serverApp: InProcServerWrapper.o testServerApp.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o OCPlatform.o
	$(CXX) $(CXX_FLAGS) -o serverApp InProcServerWrapper.o testServerApp.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o OCPlatform.o

server: InProcServerWrapper.o testServer.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o
	$(CXX) $(CXX_FLAGS) -o server InProcServerWrapper.o testServer.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o

client: InProcServerWrapper.o testClient.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o
	$(CXX) $(CXX_FLAGS) -o client InProcServerWrapper.o testClient.o ocstack_stub.o OCResource.o InProcClientWrapper.o OCReflect.o

OCReflect.o: ../../../OCLib/OCReflect.cpp
	$(CXX) $(CXX_FLAGS) -c ../../../OCLib/OCReflect.cpp -I../../../csdk/ -I../../../include -I../

OCPlatform.o: ../../../OCLib/OCPlatform.cpp
	$(CXX) $(CXX_FLAGS) -c ../../../OCLib/OCPlatform.cpp -I../../../csdk/ -I../../../include/ -I../ -I../client/ -I../server/

OCResource.o: ../../../OCLib/OCResource.cpp
	$(CXX) $(CXX_FLAGS) -c ../../../OCLib/OCResource.cpp -I../../../csdk/ -I../../../include/ -I../ -I../client/ -I../server/
	
ocstack_stub.o : ../../../csdk/ocstack_stub.c
	gcc -c ../../../csdk/ocstack_stub.c -I../../../csdk/

InProcServerWrapper.o: ../server/InProcServerWrapper.cpp
	$(CXX) $(CXX_FLAGS) -c ../server/InProcServerWrapper.cpp -I../../../csdk/ -I../../../include -I../

InProcClientWrapper.o: ../client/InProcClientWrapper.cpp
	$(CXX) $(CXX_FLAGS) -c ../client/InProcClientWrapper.cpp -I../../../csdk/ -I../../../include -I../

testServer.o : testServer.cpp
	$(CXX) $(CXX_FLAGS) -c testServer.cpp -I../../../csdk/ -I../../../include -I../ -I../client/ -I../server/

testServerApp.o : testServerApp.cpp
	$(CXX) $(CXX_FLAGS) -c testServerApp.cpp -I../../../csdk/ -I../../../include -I../ -I../client/ -I../server/

testClient.o : testClient.cpp
	$(CXX) $(CXX_FLAGS) -c testClient.cpp -I../../../csdk/ -I../../../include -I../ -I../client/ -I../server/

clean: 
	rm *.o server client serverApp

