SET(TESTAPP "oicapp-test")

# added - T.G.M
SET(TGM_SRC
						../../../TGM/src/Action.cpp
						../../../TGM/src/ActionManager.cpp
						../../../TGM/src/ActionRule.cpp
						../../../TGM/src/ActionSet.cpp
						../../../TGM/src/Capability.cpp
						../../../TGM/src/Group.cpp
						../../../TGM/src/GroupManager.cpp
						../../../TGM/src/PresenceManager.cpp
						../../../TGM/src/Rule.cpp
						../../../SDK/src/TGMClient.cpp)

# added - T.G.M
SET(JSON_SRC ../../../lib/jsoncpp/jsoncpp.cpp)
 

# fixed - T.G.M
FILE(GLOB SRCS src/*.c src/*.cpp ${JSON_SRC} ${TGM_SRC})

SET(VENDOR "samsung")
SET(PACKAGE ${TESTAPP})
SET(PKGNAME "com.${VENDOR}.${PACKAGE}")
SET(PREFIX "/usr/apps/com.samsung.oicapp-test")
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
INCLUDE_DIRECTORIES(../../../TGM/inc)
INCLUDE_DIRECTORIES(../../../SDK/inc)
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
TARGET_LINK_LIBRARIES(${TESTAPP} ${pkgs_LDFLAGS} ${PROJECT_NAME} ${OCTBSTACK} pthread)

INSTALL(TARGETS ${TESTAPP} DESTINATION ${BINDIR})

# install desktop file & icon
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.png DESTINATION "/usr/share/icons/default/small")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.xml DESTINATION "/usr/share/packages")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.rule DESTINATION /etc/smack/accesses2.d)

# added - T.G.M
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${IMGDIR} FILES_MATCHING PATTERN "*.png")