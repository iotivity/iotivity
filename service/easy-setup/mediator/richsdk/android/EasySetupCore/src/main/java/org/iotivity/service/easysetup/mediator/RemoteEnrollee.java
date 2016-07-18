/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ****************************************************************
 */


package org.iotivity.service.easysetup.mediator;

import android.util.Log;

public class RemoteEnrollee{

    public static final String TAG = RemoteEnrollee.class.getName();
    private long m_nativeHandle;

    private native void nativeGetConfiguration(GetConfigurationCallback callback);
    private native void nativeConfigureSecurity(SecurityProvisioningCallback callback);
    private native void nativeProvisionDeviceProperties(String ssid, String pwd, int authType, int encType,
                                              String language, String country, DevicePropProvisioningCallback callback);
    private native void nativeProvisionCloudProperties(String authCode, String autoProvider,
                                               String ciServer, CloudPropProvisioningCallback callback);

    /* constructor will be invoked from the native layer */
    private RemoteEnrollee(long nativeHandle){
        this.m_nativeHandle = nativeHandle;
    }

    public void getConfiguration(GetConfigurationCallback callback) throws ESException
    {
        if(callback != null)
        {
            nativeGetConfiguration(callback);
            return;
        }
        Log.d(TAG, "GetConfigurationCallback is null ");
    }

    public void configureSecurity(SecurityProvisioningCallback callback) throws ESException
    {
        if(callback != null)
        {
            nativeConfigureSecurity(callback);
            return;
        }
        Log.d(TAG, "SecurityProvisioningCallback is null ");
    }

    public void provisionDeviceProperties(DeviceProp deviceProp, DevicePropProvisioningCallback callback) throws ESException
    {
        if(callback != null)
        {
            nativeProvisionDeviceProperties(deviceProp.getSsid(), deviceProp.getPwd(),
                              deviceProp.getAuthType().getValue(), deviceProp.getEncType().getValue(),
                              deviceProp.getLanguage(), deviceProp.getCountry(), callback);
            return;
        }
        Log.d(TAG, "DevicePropProvisioningCallback is null ");
    }

    /* native provisionCloudProperties */
    public void provisionCloudProperties(CloudProp cloudProp , CloudPropProvisioningCallback callback) throws ESException{
        if(callback != null)
        {
            nativeProvisionCloudProperties(cloudProp.getAuthCode(), cloudProp.getAuthProvider(),
                               cloudProp.getCiServer(), callback);
            return;
        }
        Log.d(TAG, "CloudPropProvisioningCallback is null ");
    }
}
