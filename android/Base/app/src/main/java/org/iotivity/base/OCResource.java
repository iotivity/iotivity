package org.iotivity.base;

public class OCResource extends NativeInstance {
    final private static String TAG = "OCResource";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public native OCStackResult get(AbstractGetCallback attributeHandler);
//    public native OCStackResult get(AbstractGetCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult get(String resourceType, String resourceInterface, AbstractGetCallback attributeHandler);
//    public native OCStackResult get(String resourceType, String resourceInterface, AbstractGetCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult put(OCRepresentation rep, AbstractPutCallback attributeHandler);
//    public native OCStackResult put(OCRepresentation rep, AbstractPutCallback attributeHandler, PlatformConfig.QualityOfService QoS);
//    public native OCStackResult put(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPutCallback attributeHandler);
//    public native OCStackResult put(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPutCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult post(OCRepresentation rep, AbstractPostCallback attributeHandler);
//    public native OCStackResult post(OCRepresentation rep, AbstractPostCallback attributeHandler, PlatformConfig.QualityOfService QoS);
//    public native OCStackResult post(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPostCallback attributeHandler);
//    public native OCStackResult post(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPostCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult observe(int observeType, AbstractObserveCallback observerHandler);
//    public native OCStackResult observe(int observeType, AbstractObserveCallback observerHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult cancelObserve();
//    public native OCStackResult cancelObserve(PlatformConfig.QualityOfService QoS);

    public native String uri();
    public native String host();

    public native String[] getResourceInterfaces();
    public native String[] getResourceTypes();

    public OCResource(long instPtr) {
        super.nativeHandle = instPtr;
    }
}
