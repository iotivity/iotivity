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
SET(TESTAPP "oicapp-test")

FILE(GLOB SRCS src/*.c src/*.cpp)

SET(VENDOR "samsung")
SET(PACKAGE ${TESTAPP})
SET(PKGNAME "com.${VENDOR}.${PACKAGE}")
SET(PREFIX "/usr/apps/com.samsung.oicapp-test")
SET(BINDIR "${PREFIX}/bin")
SET(RESDIR "${PREFIX}/res")
SET(DATADIR "${PREFIX}/data")
SET(LOCALEDIR "${RESDIR}/locale")
SET(ICONDIR "${RESDIR}/icons/default/small")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED dlog appcore-efl ecore-x elementary efl-assist capi-network-wifi)
INCLUDE_DIRECTORIES(${pkgs_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

ADD_DEFINITIONS("-DVENDOR=\"${VENDOR}\"")
ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PKGNAME}\"")
ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DICONDIR=\"${ICONDIR}\"")
ADD_DEFINITIONS("-DEDJDIR=\"${EDJDIR}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${TESTAPP} ${SRCS})
TARGET_LINK_LIBRARIES(${TESTAPP} ${pkgs_LDFLAGS} ${PROJECT_NAME} ${OCTBSTACK} pthread)

INSTALL(TARGETS ${TESTAPP} DESTINATION ${BINDIR})

# install desktop file & icon
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.png DESTINATION "/usr/share/icons/default/small")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.xml DESTINATION "/usr/share/packages")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.rule DESTINATION /etc/smack/accesses2.d)
