package org.iotivity.base.examples.simpleclient;

/**
 * StringConstant contains the simpleclient specific constant values.  To add another supported
 * Resource or Interface type to this app, begin by adding the new strings here, and then
 * find the places throughout the app where Resource-specific case switches occur, and add
 * the newly-supported type there.
 */
public interface StringConstants {
    public static final String RESOURCE_URI0 = "/light0";
    public static final String RESOURCE_URI1 = "/light1";
    public static final String CREATED_URI = "createduri";
    public static final String STATE = "state";
    public static final String NAME = "name";
    public static final String POWER = "power";
    public static final String MESSAGE = "message";
}
