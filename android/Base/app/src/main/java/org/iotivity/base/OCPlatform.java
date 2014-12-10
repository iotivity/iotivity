package org.iotivity.base;

public class OCPlatform {
    final private static String TAG = "OCPlatform";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public static native void configure(PlatformConfig cfg);
    public static native int findResource(String host, String Uri, AbstractFindCallback found);

}
