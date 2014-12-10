package org.iotivity.base;

public class OCRepresentation extends NativeInstance {
    final private static String TAG = "OCRepresentation";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public OCRepresentation(long instPtr) {
        super.nativeHandle = instPtr;
    }
    public OCRepresentation() {
        super.nativeHandle = OCRepresentationConstructor();
    }

    public native long OCRepresentationConstructor();

    public native int getValueInt(String str);
    public native boolean getValueBool(String str);
    public native String getValueString(String str);

    public native void setValueInt (String str, int val);
    public native void setValueBool (String str, boolean val);
    public native void setValueString (String str, String val);

    public native String getUri();

    public native boolean hasAttribute(String str);
}
