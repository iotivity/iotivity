/******************************************************************
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
******************************************************************/

package org.iotivity.service.es.test.stc;

import static org.iotivity.service.es.test.helper.ESUtility.ES_EXCEPTION_SHOULD_BE_THROWN;
import static org.iotivity.service.es.test.helper.ESUtility.EXCEPTION_SHOULD_BE_THROWN;
import static org.iotivity.service.es.test.helper.ESUtility.EXCEPTION_SHOULD_NOT_BE_THROWN;

import org.iotivity.base.OcResource;
import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.GetEnrolleeStatus;
import org.iotivity.service.easysetup.mediator.GetStatusCallback;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.service.es.test.helper.ESDevicePropProvCallback;
import org.iotivity.service.es.test.helper.ESEnrolleeHelper;
import org.iotivity.service.es.test.helper.ESGetConfigurationCallback;
import org.iotivity.service.es.test.helper.ESPropertiesHelper;
import org.iotivity.service.es.test.helper.ESUtility;

import android.test.AndroidTestCase;
import android.util.Log;

public class ESProvisionDevicePropertiesTest extends AndroidTestCase {

    private ESEnrolleeHelper eSEnrolleeHelper;
    static int               count = 0;
    private boolean          iSGetStatusCallbackCalled;

    protected void setUp() throws Exception {
        super.setUp();
        eSEnrolleeHelper = new ESEnrolleeHelper();
        ESDevicePropProvCallback.initialCallbackValuse();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        eSEnrolleeHelper = null;
        ESDevicePropProvCallback.initialCallbackValuse();
        count = 0;
    }

    /**
     * @throws ESException
     * @since 2016-08-14
     * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *      authtype, WIFI_ENCTYPE enctype)
     * @see public void setDevConfProp(String language, String country)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test mediator can provision device properties
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback)
     * @test_data deviceProp DevicePropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop 3.create wifi
     *                prop
     * @procedure 1. Call provisionDeviceProperties API.
     * @post_condition none
     * @expected can provision device properties
     */
    public void testESProvisionDeviceProperties_P_RSV() {
        OcResource remoteEnrolleeResource = eSEnrolleeHelper
                .findEnrolleeResource(getContext());
        assertNotNull("Got null remote enrollee resource",
                remoteEnrolleeResource);
        RemoteEnrollee remoteEnrollee = EasySetup.getInstance(getContext())
                .createRemoteEnrollee(remoteEnrolleeResource);
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        DeviceProp deviceProp = ESPropertiesHelper.createDeviceProperties();
        assertNotNull("Got null deviceProp", deviceProp);

        try {
            remoteEnrollee.provisionDeviceProperties(deviceProp,
                    new ESDevicePropProvCallback());
            ESUtility.waitUntilEnrolleeResponse(
                    ESDevicePropProvCallback.isDevicePropProvCallbackCalled);
        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        assertTrue("DevicePropProvCallback not called",
                ESDevicePropProvCallback.isDevicePropProvCallbackCalled);
        assertTrue(
                "Provisioning failed"
                        + ESDevicePropProvCallback.eSResult.toString(),
                ESDevicePropProvCallback.isProvisionDevicePropSuccess);
    }
    
    
    /**
     * @since 2016-08-19
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionDeviceProperties negatively
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback) throws ESException
     * @test_data null deviceProp DevicePropProvisioningCallback
     * @pre_condition create remote enrollee
     * @procedure Call provisionDeviceProperties API with null deviceProp
     * @post_condition none
     * @expected ES Exception should be thrown
     */
    public void testESProvisionDevicePropertiesWithNullDeviceProp_ETC_N() {
        OcResource remoteEnrolleeResource = eSEnrolleeHelper
                .findEnrolleeResource(getContext());
        assertNotNull("Got null remote enrollee resource",
                remoteEnrolleeResource);
        RemoteEnrollee remoteEnrollee = EasySetup.getInstance(getContext())
                .createRemoteEnrollee(remoteEnrolleeResource);
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        
        try {
            remoteEnrollee.provisionDeviceProperties(null,
                    new ESDevicePropProvCallback());
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(true);
        }
    }

    /**
     * @throws ESException
     * @since 2016-08-14
     * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *      authtype, WIFI_ENCTYPE enctype)
     * @see public void setDevConfProp(String language, String country)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionDeviceProperties positively
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback) throws ESException
     * @test_data deviceProp DevicePropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure 1. Call provisionDeviceProperties API.
     * @post_condition none
     * @expected provisionDeviceProperties call without exception
     * 
     */
    public void testESGetConfigurationStatus_P_RSV() {
        OcResource remoteEnrolleeResource = eSEnrolleeHelper
                .findEnrolleeResource(getContext());
        assertNotNull("Got null remote enrollee resource",
                remoteEnrolleeResource);
        RemoteEnrollee remoteEnrollee = EasySetup.getInstance(getContext())
                .createRemoteEnrollee(remoteEnrolleeResource);
        assertNotNull("Got null remote enrollee", remoteEnrollee);

        try {
            remoteEnrollee.getConfiguration(new ESGetConfigurationCallback());
            ESUtility.waitUntilEnrolleeResponse(
                    ESGetConfigurationCallback.isConfigurationCallbackCalled);

        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }       
    }

    /**
     * @throws ESException
     * @since 2016-08-14
     * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *      authtype, WIFI_ENCTYPE enctype)
     * @see public void setDevConfProp(String language, String country)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test getStatus positively
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback) throws ESException
     * @test_data deviceProp DevicePropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure 1. Call provisionDeviceProperties API.
     * @post_condition none
     * @expected can get provisioning status
     * 
     */
    public void testESProvisionGetStatus_P_RSV() {
        OcResource remoteEnrolleeResource = eSEnrolleeHelper
                .findEnrolleeResource(getContext());
        assertNotNull("Got null remote enrollee resource",
                remoteEnrolleeResource);
        RemoteEnrollee remoteEnrollee = EasySetup.getInstance(getContext())
                .createRemoteEnrollee(remoteEnrolleeResource);
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        DeviceProp deviceProp = ESPropertiesHelper.createDeviceProperties();
        assertNotNull("Got null deviceProp", deviceProp);
        try {
            remoteEnrollee.getStatus(new GetStatusCallback() {

                @Override
                public void onProgress(GetEnrolleeStatus enrolleeStatus) {
                    enrolleeStatus.getEnrolleeStatus();
                    iSGetStatusCallbackCalled = true;
                    Log.d("GetEnrolleeStatus", enrolleeStatus
                            .getEnrolleeStatus().getLastErrCode().toString());
                }

            });
        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }       
    }
}
