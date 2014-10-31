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
SET(TESTAPP "oicapp-switch")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/oic-resource/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/oic-resource/csdk/stack/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/oic-resource/csdk/ocsocket/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/oic-resource/csdk/ocrandom/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/oic-resource/csdk/logger/include)
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR}/boost)

# added - T.G.M
SET(TGM_SRC
						../../../tgm/src/Action.cpp
						../../../tgm/src/ActionManager.cpp
						../../../tgm/src/ActionRule.cpp
						../../../tgm/src/ActionSet.cpp
						../../../tgm/src/Capability.cpp
						../../../tgm/src/Group.cpp
						../../../tgm/src/GroupManager.cpp
						../../../tgm/src/PresenceManager.cpp
						../../../tgm/src/Rule.cpp
						../../../sdk/src/TGMClient.cpp)

# added - T.G.M
SET(JSON_SRC ../../../lib/jsoncpp/src/jsoncpp.cpp)
 

# fixed - T.G.M
FILE(GLOB SRCS src/*.c src/*.cpp ${JSON_SRC} ${TGM_SRC})

SET(VENDOR "samsung")
SET(PACKAGE ${TESTAPP})
SET(PKGNAME "com.${VENDOR}.${PACKAGE}")
SET(PREFIX "/usr/apps/com.samsung.oicapp-switch")
SET(BINDIR "${PREFIX}/bin")
SET(RESDIR "${PREFIX}/res")
SET(DATADIR "${PREFIX}/data")
SET(LOCALEDIR "${RESDIR}/locale")
SET(ICONDIR "${RESDIR}/icons/default/small")

# added - T.G.M
SET(IMGDIR "${RESDIR}/images")
SET(MUSICDIR "${RESDIR}/musics")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

# fixed - T.G.M
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED dlog appcore-efl ecore-x elementary efl-assist capi-network-wifi appsvc capi-media-player)

INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(../../../tgm/inc)
INCLUDE_DIRECTORIES(../../../sdk/inc)
INCLUDE_DIRECTORIES(../../../lib/inc)
INCLUDE_DIRECTORIES(../../../lib/jsoncpp/inc)

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
TARGET_LINK_LIBRARIES(${TESTAPP} ${pkgs_LDFLAGS} ${OCLIB} ${OCTBSTACK} pthread)

INSTALL(TARGETS ${TESTAPP} DESTINATION ${BINDIR})

# install desktop file & icon
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.png DESTINATION "/usr/share/icons/default/small")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.xml DESTINATION "/usr/share/packages")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.rule DESTINATION /etc/smack/accesses2.d)

# added - T.G.M
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${IMGDIR} FILES_MATCHING PATTERN "*.png")
