//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
package org.iotivity.ResourceHosting;
import java.lang.System;
import java.lang.String;;
/**
 * To execute resource hosting function for android sample application .
 * 
 * @author Copyright 2015 Samsung Electronics All Rights Reserved.
 * @see className class : ResourceHosting</br>
 *
 */

public class ResourceHosting {

    /**
     * jni function - OicCorrdinatorstart() method.
     * 
     * @see Class class :
     *      org_iotivity_resourcehosting_ResourceHosting</br>
     * @see Method method : OICCoordinatorStart</br>
     * @see Signature signature : ()V</br>
     */
    public native int OICCoordinatorStart();

    /**
     * jni function - OICCoordinatorStop() method.
     * 
     * @see Class class :
     *      org_iotivity_resourcehosting_ResourceHosting</br>
     * @see Method method : OICCoordinatorStop</br>
     * @see signature signature : ()V</br>
     */
    public native int OICCoordinatorStop();

    /**
     * jni function - ResourceHostingInit() method in order to execute
     * OICCoordinatorStart() method.
     * 
     * @see Class class :
     *      org_iotivity_resourcehosting_ResourceHosting</br>
     * @see Method method : ResourceHostingInit</br>
     * @param addr
     *            ipAddress
     * @see signature signature : (Ljava/lang/String;)V</br>
     */
    public native int ResourceHostingInit(String addr);

    /**
     * jni function - ResourceHostingTerminate() method in order to terminate
     * resource hosting
     * 
     * @see Class class :
     *      org_iotivity_resourcehosting_ResourceHosting</br>
     * @see Method method : ResourceHostingTerminate</br>
     * @see signature signature : ()V</br>
     */
    public native int ResourceHostingTerminate();

    static {
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("oc_logger_core");
        System.loadLibrary("oc_logger");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("NotificationManager");
    	System.loadLibrary("ResourceHosing_JNI");
    }
}
