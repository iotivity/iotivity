package org.iotivity.base.examples.simpleserver;

import org.iotivity.base.OcPlatform;
/**
 * StringConstants contains the simpleserver specific constant values.  To add another supported
 * Resource or Interface type to this app, begin by adding the new strings here, and then
 * find the places throughout the app where Resource-specific case switches occur, and add
 * the newly-supported type there.
 */
public interface StringConstants {
    public static final String RESOURCE_URI0 = "/a/light";
    public static final String RESOURCE_URI1 = "/a/light2";
    public static final String RESOURCE_TYPENAME = "core.light";
    public static final String OIC_SERVER_JSON_DB_FILE =  "oic_svr_db_server.json";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE; //resource interface
    public static final String CREATED_URI = "createduri";
    public static final String STATE = "state";
    public static final String NAME = "name";
    public static final String POWER = "power";
    public static final String MESSAGE = "message";
    public static final int ERROR_CODE = 200;
}
