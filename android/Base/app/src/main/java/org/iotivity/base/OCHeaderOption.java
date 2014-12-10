package org.iotivity.base;

public class OCHeaderOption extends NativeInstance {
    final private static String TAG = "OCHeaderOption";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public OCHeaderOption(long instPtr) {
        super.nativeHandle = instPtr;
    }

    public native int getOptionID();
    public native String getOptionData();
    public native long OCHeaderOptionConstructor(int optionID, String optionData);
}
