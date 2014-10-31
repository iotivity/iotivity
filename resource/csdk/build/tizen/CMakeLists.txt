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
SET(CURRENT_SRC_DIR "${BASE_SRC_DIR}/csdk")

SET(LCOAP "coap")
SET(OCTBSTACK_DIR "${CURRENT_SRC_DIR}/stack")
SET(OCCOAP_DIR "${CURRENT_SRC_DIR}/occoap")
SET(OCSOCK_DIR "${CURRENT_SRC_DIR}/ocsocket")
SET(LOGGER_DIR "${CURRENT_SRC_DIR}/logger")
SET(RANDOM_DIR "${CURRENT_SRC_DIR}/ocrandom")
SET(LCOAP_DIR "${CURRENT_SRC_DIR}/libcoap-4.1.1")
SET(CJSON_DIR "${BASE_SRC_DIR}/../oic-utilities/tb/cJSON")

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

INCLUDE_DIRECTORIES(${LOGGER_DIR}/include)
INCLUDE_DIRECTORIES(${RANDOM_DIR}/include)
INCLUDE_DIRECTORIES(${OCSOCK_DIR}/include)
INCLUDE_DIRECTORIES(${LCOAP_DIR})
INCLUDE_DIRECTORIES(${OCCOAP_DIR}/include)
INCLUDE_DIRECTORIES(${OCTBSTACK_DIR}/include)
INCLUDE_DIRECTORIES(${OCTBSTACK_DIR}/include/internal)
INCLUDE_DIRECTORIES(${CJSON_DIR})

SET(BASE_CFLAGS "${CMAKE_C_FLAGS} -DWITH_POSIX")
SET(EXTRA_CFLAGS "-Os -fpic")
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -std=c99")
#SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -O0 -g3 -pedantic -DTB_LOG")
SET(CMAKE_C_FLAGS "${BASE_CFLAGS} ${EXTRA_CFLAGS}")

SET(OCCOAP_SRC
	${OCCOAP_DIR}/src/occoap.c
	${OCCOAP_DIR}/src/occoaphelper.c)

SET(OCTBSTACK_SRC
	${OCTBSTACK_DIR}/src/ocstack.c
	${OCTBSTACK_DIR}/src/occlientcb.c
	${OCTBSTACK_DIR}/src/ocresource.c
	${OCTBSTACK_DIR}/src/ocobserve.c
	${OCTBSTACK_DIR}/src/occollection.c)

SET(CJSON_SRC ${CJSON_DIR}/cJSON.c)

SET(OCTBSTACK_SRCS
	${OCCOAP_SRC}
	${OCTBSTACK_SRC}
	${CJSON_SRC})
ADD_LIBRARY(${OCTBSTACK} SHARED ${OCTBSTACK_SRCS})
SET_TARGET_PROPERTIES(${OCTBSTACK} PROPERTIES VERSION ${FULLVER} SOVERSION ${MAJORVER})
TARGET_LINK_LIBRARIES(${OCTBSTACK} ${LCOAP})

INSTALL(TARGETS ${OCTBSTACK} DESTINATION ${LIB_INSTALL_DIR})

FILE(GLOB LOGGER_HEADER ${LOGGER_DIR}/include/*.h)
FOREACH(hfile ${LOGGER_HEADER})
	INSTALL(FILES ${hfile} DESTINATION include/oic/logger)
ENDFOREACH(hfile)

FILE(GLOB RANDOM_HEADER ${RANDOM_DIR}/include/*.h)
FOREACH(hfile ${RANDOM_HEADER})
	INSTALL(FILES ${hfile} DESTINATION include/oic/ocrandom)
ENDFOREACH(hfile)

FILE(GLOB OCSOCK_HEADER ${OCSOCK_DIR}/include/*.h)
FOREACH(hfile ${OCSOCK_HEADER})
	INSTALL(FILES ${hfile} DESTINATION include/oic/ocsocket)
ENDFOREACH(hfile)

FILE(GLOB OCTBSTACK_HEADER ${OCTBSTACK_DIR}/include/*.h)
FOREACH(hfile ${OCTBSTACK_HEADER})
	INSTALL(FILES ${hfile} DESTINATION include/oic/stack)
ENDFOREACH(hfile)

ADD_SUBDIRECTORY(${CURRENT_SRC_DIR}/libcoap-4.1.1/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/libcoap-4.1.1)
