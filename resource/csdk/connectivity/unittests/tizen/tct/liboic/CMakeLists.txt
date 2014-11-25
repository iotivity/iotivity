SET(PKG_NAME "liboic")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "liboic")
SET(TC_SOURCES
    #utc-liboic.c
    wifi/utc-wifi-liboic.c
    #bt/utc-bt-liboic.c
)

#PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
#    ${CAPI_LIB}
#)
PKG_CHECK_MODULES(pkgs REQUIRED
    capi-network-bluetooth capi-network-wifi com.samsung.oicca dlog
)

INCLUDE_DIRECTORIES(
    ${${CAPI_LIB}_INCLUDE_DIRS}
)
ADD_DEFINITIONS("-D__TIZEN__")
ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.c ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES} /usr/lib/liboicinterface.a -lm -lpthread -lrt -ldl -lcapi-network-wifi -lcapi-network-bluetooth -ldlog
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
