/**
 * ***************************************************************
 * <p/>
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p/>
 * ****************************************************************
 */


package org.iotivity.service.easysetup.mediator;

import android.util.Log;

public class RemoteEnrollee{

    public static final String TAG = RemoteEnrollee.class.getName();
    private long m_nativeHandle;

    private native void nativeStartProvision();
    private native void nativeStopProvision();
    private native void nativeRegisterProvisioningHandler(IProvisionStatusNativeHandler listener);

    /* constructor will be invoked from the native layer */
    private RemoteEnrollee(long nativeHandle){
        this.m_nativeHandle = nativeHandle;
    }

    /* Register native Listener for the Provisioning state */
    public void registerProvisioningHandler( IProvisionStatusNativeHandler provisioningListener) throws ESException{
       // this.provisioningListener = provisioningListener;
        nativeRegisterProvisioningHandler(provisioningListener);
        Log.i(TAG, "JNI Callback is registered for getting provisioning status");
    }

    /* native StartProvision */
    public void startProvision() throws ESException{
    nativeStartProvision();
}

    /* native stopProvision */
    public void stopProvision() throws  ESException{
    nativeStopProvision();
}

}
