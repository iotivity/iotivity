# //******************************************************************
# //
# // Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
CXX=g++
CXX_FLAGS=-std=c++0x -Wall -pthread

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

