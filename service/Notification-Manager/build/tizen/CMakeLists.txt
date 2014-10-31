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
LINK_DIRECTORIES(${LIB_INSTALL_DIR})

SET(NOTI_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

#INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/oic)
#INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/oic/stack)
#INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/oic/ocsocket)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/stack/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/ocsocket/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/ocrandom/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/resource/csdk/logger/include)
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/boost)
INCLUDE_DIRECTORIES(${NOTI_BASE_DIR}/NotificationManager/include)

SET(NOTIFICATIONMANAGER "NotificationManager")
SET(EX_NOTIPROVIDER "SampleProvider")
SET(EX_NOTICONSUMER "SampleConsumer")

SET(EXTRA_CXXFLAGS "-std=c++0x")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")

FILE(GLOB MANAGER_SRCS ${NOTI_BASE_DIR}/NotificationManager/src/*.cpp)

ADD_EXECUTABLE(${NOTIFICATIONMANAGER} ${MANAGER_SRCS})
TARGET_LINK_LIBRARIES(${NOTIFICATIONMANAGER} ${OCLIB} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${NOTIFICATIONMANAGER} DESTINATION ${BIN_INSTALL_DIR})

ADD_EXECUTABLE(${EX_NOTIPROVIDER} ${NOTI_BASE_DIR}/SampleApp/linux/sampleProvider/SampleProvider.cpp)
TARGET_LINK_LIBRARIES(${EX_NOTIPROVIDER} ${OCLIB} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${EX_NOTIPROVIDER} DESTINATION ${BIN_INSTALL_DIR})

ADD_EXECUTABLE(${EX_NOTICONSUMER} ${NOTI_BASE_DIR}/SampleApp/linux/sampleConsumer/SampleConsumer.cpp)
TARGET_LINK_LIBRARIES(${EX_NOTICONSUMER} ${OCLIB} ${OCTBSTACK} pthread)
INSTALL(TARGETS ${EX_NOTICONSUMER} DESTINATION ${BIN_INSTALL_DIR})
