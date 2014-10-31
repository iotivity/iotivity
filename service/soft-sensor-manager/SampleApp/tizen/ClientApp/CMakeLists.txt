/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(oic)

SET(CMAKE_SHARED_LINKER_FLAGS " -Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS " -Wl,--as-needed -Wl,--hash-style=both")
SET(EX_SERVER "simpleserver")
SET(EX_CLIENT "simpleclient")
SET(EX_CLIENTSERVER "simpleclientserver")
SET(OCTBSTACK "octbstack")

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(csdk/stack/include)
INCLUDE_DIRECTORIES(csdk/ocsocket/include)
INCLUDE_DIRECTORIES(csdk/ocrandom/include)
INCLUDE_DIRECTORIES(csdk/logger/include)
INCLUDE_DIRECTORIES(csdk/libcoap-4.1.1)
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/boost)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

#SET(EXTRA_CXXFLAGS "-std=c++11")
SET(EXTRA_CXXFLAGS "-g3 -std=c++0x")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")

SET(SRCS
	OCLib/OCReflect.cpp
	OCLib/OCPlatform.cpp
	OCLib/OCResource.cpp
	OCLib/OCUtilities.cpp
	OCLib/InProcServerWrapper.cpp
	OCLib/InProcClientWrapper.cpp)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})

ADD_EXECUTABLE(${EX_SERVER} examples/simpleserver.cpp)
TARGET_LINK_LIBRARIES(${EX_SERVER} ${PROJECT_NAME} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${EX_SERVER} DESTINATION ${BIN_INSTALL_DIR})

ADD_EXECUTABLE(${EX_CLIENT} examples/simpleclient.cpp)
TARGET_LINK_LIBRARIES(${EX_CLIENT} ${PROJECT_NAME} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${EX_CLIENT} DESTINATION ${BIN_INSTALL_DIR})

ADD_EXECUTABLE(${EX_CLIENTSERVER} examples/simpleclientserver.cpp)
TARGET_LINK_LIBRARIES(${EX_CLIENTSERVER} ${PROJECT_NAME} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${EX_CLIENTSERVER} DESTINATION ${BIN_INSTALL_DIR})

ADD_SUBDIRECTORY(csdk)
#ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(oic-sample)
