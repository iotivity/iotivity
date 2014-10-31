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

SET(TGM "TGMClient")
SET(TGM_SERVER "tgmserver")
SET(TGM_SDK "TGMSDKLibrary")
SET(TGM_JSON "jsoncpp")

SET(TGM_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

INCLUDE_DIRECTORIES(${TGM_BASE_DIR}/lib/inc)
INCLUDE_DIRECTORIES(${TGM_BASE_DIR}/lib/jsoncpp/inc)

SET(EXTRA_CXXFLAGS "-std=c++0x -D_DEBUG")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")

FILE(GLOB TGM_LIB_SRCS ${TGM_BASE_DIR}/lib/jsoncpp/src/*.cpp)
ADD_LIBRARY(${TGM_JSON} STATIC ${TGM_LIB_SRCS})

#ADD_SUBDIRECTORY(${TGM_BASE_DIR}/sdk/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/sdk)
#ADD_SUBDIRECTORY(${TGM_BASE_DIR}/tgm/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/tgm)
ADD_SUBDIRECTORY(${TGM_BASE_DIR}/sampleapp/tizen/musicapp ${CMAKE_CURRENT_BINARY_DIR}/musicapp)
ADD_SUBDIRECTORY(${TGM_BASE_DIR}/sampleapp/tizen/switch ${CMAKE_CURRENT_BINARY_DIR}/switch)