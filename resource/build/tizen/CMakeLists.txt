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

SET(OCTBSTACK "octbstack")
SET(OCLIB "oc")
SET(BASE_SRC_DIR "${CMAKE_SOURCE_DIR}/../..")

INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/include)
INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/csdk/stack/include)
INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/csdk/ocsocket/include)
INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/csdk/ocrandom/include)
INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/csdk/logger/include)
INCLUDE_DIRECTORIES(${BASE_SRC_DIR}/csdk/libcoap-4.1.1)
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/boost)

#SET(EXTRA_CXXFLAGS "-std=c++11")
SET(EXTRA_CXXFLAGS "-g3 -std=c++0x -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS " -Wl,--hash-style=both")

SET(SRCS
	${BASE_SRC_DIR}/src/OCReflect.cpp
	${BASE_SRC_DIR}/src/OCPlatform.cpp
	${BASE_SRC_DIR}/src/OCResource.cpp
	${BASE_SRC_DIR}/src/OCUtilities.cpp
	${BASE_SRC_DIR}/src/InProcServerWrapper.cpp
	${BASE_SRC_DIR}/src/InProcClientWrapper.cpp)
ADD_LIBRARY(${OCLIB} STATIC ${SRCS})

INSTALL(TARGETS ${OCLIB} DESTINATION ${LIB_INSTALL_DIR})

FILE(GLOB OC_HEADER ${BASE_SRC_DIR}/include/*.h)
FOREACH(hfile ${OC_HEADER})
	INSTALL(FILES ${hfile} DESTINATION include/oic)
ENDFOREACH(hfile)

ADD_SUBDIRECTORY(${BASE_SRC_DIR}/csdk/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/csdk)
ADD_SUBDIRECTORY(${BASE_SRC_DIR}/examples/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/examples)
