package org.iotivity.base;

public class PlatformConfig extends NativeInstance
{
    final private static String TAG = "PlatformConfig";
    static {
        System.loadLibrary("ocstack-jni");
    }

    // ENUM definition. Need to match with OCApi.h
    public class ServiceType
    {
        static final public int INPROC  = 0;
        static final public int OUTPROC = 1;
    };

    public class ModeType
    {
        static final public int SERVER = 0;
        static final public int CLIENT = 1;
        static final public int BOTH   = 2;

    };

    public class QualityOfService
    {
        static final public int LO_QOS  = 0;
        static final public int ME_QOS  = 1;
        static final public int HI_QOS  = 2;
        static final public int NA_QOS  = 3;
    }

    public PlatformConfig(int serviceType,
                          int mode,
                          String ipAddress,
                          int port,
                          int QoS)
    {
        super.nativeHandle = createNativeInstance(serviceType, mode, ipAddress, port, QoS);
    }
   

    protected native long createNativeInstance(int serviceType,
             int mode,
             String ipAddress,
             int port,
             int QoS);
}
