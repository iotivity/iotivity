package org.iotivity.base;

public abstract class NativeInstance
{
    protected long nativeHandle = 0;

    public NativeInstance()
    {
    }

    public long getHandle() { return nativeHandle;}

}
