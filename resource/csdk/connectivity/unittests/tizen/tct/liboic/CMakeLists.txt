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
    capi-network-bluetooth capi-network-wifi com.samsung.oicca dlog gobject-2.0 glib-2.0
)

INCLUDE_DIRECTORIES(
    ${${CAPI_LIB}_INCLUDE_DIRS}
    /usr/include/glib-2.0
    /usr/lib/glib-2.0/include
    /usr/lib/
    /usr/include/network
)
ADD_DEFINITIONS("-D__TIZEN__")
ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.c ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES} -loicinterface -lm -lpthread -lrt -ldl -lstdc++ -lgobject-2.0 -lgio-2.0 -lglib-2.0 -lcapi-network-wifi -ldlog -lcapi-network-bluetooth
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
