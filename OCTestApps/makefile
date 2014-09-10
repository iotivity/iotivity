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
