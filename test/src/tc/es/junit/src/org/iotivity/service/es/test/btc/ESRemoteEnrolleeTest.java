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

package org.iotivity.service.es.test.btc;

import org.iotivity.service.es.test.helper.ESCloudPropProvCallback;
import org.iotivity.service.es.test.helper.ESDevicePropProvCallback;
import org.iotivity.service.es.test.helper.ESEnrolleeHelper;
import org.iotivity.service.es.test.helper.ESPropertiesHelper;
import static org.iotivity.service.es.test.helper.ESUtility.*;
import org.iotivity.service.easysetup.mediator.GetConfigurationCallback;
import org.iotivity.service.easysetup.mediator.GetConfigurationStatus;
import org.iotivity.service.easysetup.mediator.GetEnrolleeStatus;
import org.iotivity.service.easysetup.mediator.GetStatusCallback;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningCallback;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningStatus;
import org.iotivity.service.easysetup.mediator.CloudProp;
import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.ESException;

import android.test.AndroidTestCase;

public class ESRemoteEnrolleeTest extends AndroidTestCase {
    
    private ESEnrolleeHelper    eSEnrolleeHelper;

    protected void setUp() throws Exception {
        super.setUp();
        eSEnrolleeHelper = new ESEnrolleeHelper();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        eSEnrolleeHelper = null;
    }

    /**
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
    public void testESProvisionDeviceProperties_P_RSV() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        DeviceProp deviceProp = ESPropertiesHelper.createDeviceProperties();
        assertNotNull("Got null deviceProp", deviceProp);
        try {
            remoteEnrollee.provisionDeviceProperties(deviceProp,
                    new ESDevicePropProvCallback());
        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

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
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        try {
            remoteEnrollee.provisionDeviceProperties(null,
                    new ESDevicePropProvCallback());
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-14
     * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *      authtype, WIFI_ENCTYPE enctype)
     * @see public void setDevConfProp(String language, String country)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionDeviceProperties negatively
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback) throws ESException
     * @test_data deviceProp DevicePropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure Call provisionDeviceProperties API with null
     *            DevicePropProvisioningCallback
     * @post_condition none
     * @expected ES Exception should be thrown
     */
    public void testESProvisionDevicePropertiesWithNullDeviceProvCallback_NV_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        DeviceProp deviceProp = ESPropertiesHelper.createDeviceProperties();
        assertNotNull("Got null remote enrollee", deviceProp);
        try {
            remoteEnrollee.provisionDeviceProperties(deviceProp, null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-14
     * @see public void setCloudProp(String authCode, String authProvider,
     *      String ciServer)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionCloudProperties positively
     * @target public void provisionCloudProperties(CloudProp cloudProp,
     *         CloudPropProvisioningCallback callback) throws ESException
     * @test_data cloudProp CloudPropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae cloudProp
     * @procedure 1. Call provisionCloudProperties API.
     * @post_condition none
     * @expected provisionCloudProperties call without exception
     */
    public void testESProvisionCloudProperties_RSV_P() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        CloudProp cloudProp = ESPropertiesHelper.createCloudProperties();
        assertNotNull("Got null cloudProp", cloudProp);
        try {
            remoteEnrollee.provisionCloudProperties(cloudProp,
                    new ESCloudPropProvCallback());
        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-19
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionDeviceProperties negatively
     * @target public void provisionCloudProperties(CloudProp cloudProp,
     *         CloudPropProvisioningCallback callback) throws ESException
     * @test_data null cloudProp CloudPropProvisioningCallback
     * @pre_condition create remote enrollee
     * @procedure Call provisionCloudProperties API with null cloudProp
     * @post_condition none
     * @expected ES Exception should be thrown
     */
    public void testESProvisionCloudPropertiesWithNullCloudProp_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        try {
            remoteEnrollee.provisionCloudProperties(null,
                    new ESCloudPropProvCallback());
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-14
     * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *      authtype, WIFI_ENCTYPE enctype)
     * @see public void setDevConfProp(String language, String country)
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionDeviceProperties negatively
     * @target public void provisionDeviceProperties(DeviceProp deviceProp,
     *         DevicePropProvisioningCallback callback) throws ESException
     * @test_data cloudProp CloudPropProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure Call provisionCloudProperties API with null
     *            CloudPropProvisioningCallback
     * @post_condition none
     * @expected ES Exception should be thrown
     */
    public void testProvisionCloudPropertiesWithNullCloudPropProvisioningCallback_NV_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        CloudProp cloudProp = ESPropertiesHelper.createCloudProperties();
        assertNotNull("Got null remote enrollee", cloudProp);
        try {
            remoteEnrollee.provisionCloudProperties(cloudProp, null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-20
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test getStatus API positively
     * @target public void getStatus(GetStatusCallback callback) throws
     *         ESException
     * @test_data GetStatusCallback
     * @pre_condition create remote enrollee
     * @procedure 1. Call getStatus API
     * @post_condition none
     * @expected getStatus API called without thrown exception
     */
    public void testESGetStatus_RSV_P() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);

        try {
            remoteEnrollee.getStatus(new GetStatusCallback() {
                @Override
                public void onProgress(GetEnrolleeStatus arg0) {
                }
            });
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-20
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test getStatus API positively
     * @target public void getStatus(GetStatusCallback callback) throws
     *         ESException
     * @test_data null GetStatusCallback
     * @pre_condition create remote enrollee
     * @procedure 1. Call getStatus API with null callback
     * @post_condition none
     * @expected thrown ES exception
     */
    public void testESGetStatusWithNullCallback_NV_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);

        try {
            remoteEnrollee.getStatus(null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-20
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test getConfiguration API positively
     * @target public void getConfiguration(GetConfigurationCallback callback)
     *         throws ESException
     * @test_data GetConfigurationCallback
     * @pre_condition create remote enrollee
     * @procedure 1. Call getConfiguration API
     * @post_condition none
     * @expected getConfiguration API called without thrown exception
     */
    public void testESGetConfiguration_RSV_P() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);

        try {
            remoteEnrollee.getConfiguration(new GetConfigurationCallback() {
                @Override
                public void onProgress(GetConfigurationStatus arg0) {

                }
            });
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-20
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test getConfiguration API positively
     * @target public void getStatus(GetStatusCallback callback) throws
     *         ESException
     * @test_data null GetConfigurationCallback
     * @pre_condition create remote enrollee
     * @procedure 1. Call getConfiguration API with null callback
     * @post_condition none
     * @expected thrown ES exception
     */
    public void testESGetConfigurationWithNullCallback_NV_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);

        try {
            remoteEnrollee.getStatus(null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-22
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionSecurity positively
     * @target public void provisionSecurity(SecurityProvisioningCallback
     *         callback) throws ESException
     * @test_data SecurityProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure 1. Call provisionDeviceProperties API.
     * @post_condition none
     * @expected provisionDeviceProperties call without exception
     */
    public void testESSProvisionSecurity_RSV_P() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        try {
            remoteEnrollee
                    .provisionSecurity(new SecurityProvisioningCallback() {
                        @Override
                        public void onProgress(
                                SecurityProvisioningStatus arg0) {

                        }
                    });
        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

    }

    /**
     * 
     * @since 2016-08-22
     * @see public synchronized static EasySetup getInstance(Context context)
     * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
     *      enrolleeResource)
     * @objective test provisionSecurity positively
     * @target public void provisionSecurity(SecurityProvisioningCallback
     *         callback) throws ESException
     * @test_data SecurityProvisioningCallback
     * @pre_condition 1.create remote enrollee 2.cretae deviceprop
     * @procedure 1. Call provisionDeviceProperties API.
     * @post_condition none
     * @expected exception should be thrown
     */
    public void testESSProvisionSecurity_NV_ETC_N() {
        RemoteEnrollee remoteEnrollee = eSEnrolleeHelper
                .createRemoteEnrollee(getContext());
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        try {
            remoteEnrollee.provisionSecurity(null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception ex) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN,
                    ex instanceof ESException);
        }

    }
}