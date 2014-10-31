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
LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

SET(SS_SAMPLE_THSEN "THSensorApp")
SET(SS_SAMPLE_THSEN_PATH, "${SS_SAMPLE_PATH}/THSensorApp");

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/stack/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/ocsocket/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/ocrandom/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/logger/include)
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/boost)
INCLUDE_DIRECTORIES(${SS_SDK_PATH})
INCLUDE_DIRECTORIES(${SS_SAMPLE_THSEN_PATH}/include)

SET(EXTRA_CXXFLAGS "-std=c++0x -DLINUX")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")

ADD_EXECUTABLE(${SS_SAMPLE_THSEN} ${SS_SAMPLE_THSEN_PATH}/src/ThingResourceServer.cpp)
TARGET_LINK_LIBRARIES(${SS_SAMPLE_THSEN} ${SS_SDK} ${OCLIB} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${SS_SAMPLE_THSEN} DESTINATION ${BIN_INSTALL_DIR})

