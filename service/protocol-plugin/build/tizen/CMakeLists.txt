SET(PP_CPLUFF "cpluff")

SET(PP_BASE_DIR "${BASE_SRC_DIR}/protocol-plugin")
SET(PP_CPLUFF_DIR "${PP_BASE_DIR}/lib/cpluff")

INCLUDE_DIRECTORIES(${PP_CPLUFF_DIR})
INCLUDE_DIRECTORIES(${PP_CPLUFF_DIR}/libcpluff)

ADD_DEFINITIONS("-DWITH_TLS_PSK")
ADD_DEFINITIONS("-DCP_C_API=CP_EXPORT")
ADD_DEFINITIONS("-DNDEBUG")
ADD_DEFINITIONS("-DCP_DATADIR=\"${SHARE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DCP_HOST=\"\"")
ADD_DEFINITIONS("-DHAVE_CONFIG_H")

SET(EXTRA_CFLAGS "-fPIC")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

SET(CPLUFF_SRCS
	${PP_CPLUFF_DIR}/libcpluff/psymbol.c
	${PP_CPLUFF_DIR}/libcpluff/pscan.c
	${PP_CPLUFF_DIR}/libcpluff/ploader.c
	${PP_CPLUFF_DIR}/libcpluff/pinfo.c
	${PP_CPLUFF_DIR}/libcpluff/pcontrol.c
	${PP_CPLUFF_DIR}/libcpluff/serial.c
	${PP_CPLUFF_DIR}/libcpluff/logging.c
	${PP_CPLUFF_DIR}/libcpluff/context.c
	${PP_CPLUFF_DIR}/libcpluff/cpluff.c
	${PP_CPLUFF_DIR}/libcpluff/util.c
	${PP_CPLUFF_DIR}/kazlib/list.c
	${PP_CPLUFF_DIR}/kazlib/hash.c
	${PP_CPLUFF_DIR}/libcpluff/thread_posix.c
)

ADD_LIBRARY(${PP_CPLUFF} STATIC ${CPLUFF_SRCS})
TARGET_LINK_LIBRARIES(${PP_CPLUFF} dl expat)

ADD_SUBDIRECTORY(${PP_BASE_DIR}/plugin-manager/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/plugin-manager)
ADD_SUBDIRECTORY(${PP_BASE_DIR}/plugins/mqtt/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/mqtt)
ADD_SUBDIRECTORY(${PP_BASE_DIR}/plugins/hue/build/tizen ${CMAKE_CURRENT_BINARY_DIR}/hue)
#ADD_SUBDIRECTORY(${PP_BASE_DIR}/sample-app/linux/fan-control ${CMAKE_CURRENT_BINARY_DIR}/fan-control)
