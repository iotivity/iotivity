package org.iotivity.base.examples.fridgeclient;

import org.iotivity.base.OcPlatform;

/**
 * StringConstant contains the fridgeclient specific constant values.  To add another supported
 * Resource or Interface type to this app, begin by adding the new strings here, and then
 * find the places throughout the app where Resource-specific case switches occur, and add
 * the newly-supported type there.
 */
public interface StringConstants {
    public static final String RESOURCE_URI = "/device";
    public static final String DEVICE_NAME = "device_name";
    public static final String LIGHT = "/light";
    public static final String LEFT_DOOR = "/door/left";
    public static final String RIGHT_DOOR = "/door/right";
    public static final String RANDOM_DOOR = "/door/random";
    public static final String MESSAGE = "message";
    public static final String API_VERSION = "v.1.2";
    public static final String CLIENT_TOKEN = "AaBbYyZz";
    public static final String ON = "on";
    public static final String OPEN = "open";
    public static final String SIDE = "side";
    public static final String INTENT = "org.iotivity.base.examples.fridgeclient";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE;

    public static final int WAIT_TIME = 1000;
    public static final int API_VERSION_KEY = 2048;
    public static final int CLIENT_VERSION_KEY = 3000;

    public static final boolean ENABLE_PRINTING = true; // change to false to disable printing
                                                        // of messages on the console and the screen
}
