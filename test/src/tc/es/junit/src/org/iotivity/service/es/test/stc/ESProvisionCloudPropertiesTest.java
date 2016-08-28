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

import org.iotivity.base.OcResource;
import org.iotivity.service.easysetup.mediator.CloudProp;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.service.es.test.helper.ESCloudPropProvCallback;
import org.iotivity.service.es.test.helper.ESEnrolleeHelper;
import org.iotivity.service.es.test.helper.ESPropertiesHelper;
import org.iotivity.service.es.test.helper.ESUtility;
import static org.iotivity.service.es.test.helper.ESUtility.*;

import android.test.AndroidTestCase;

public class ESProvisionCloudPropertiesTest extends AndroidTestCase {
    private ESEnrolleeHelper eSEnrolleeHelper;

    protected void setUp() throws Exception {
        super.setUp();
        eSEnrolleeHelper = new ESEnrolleeHelper();
        ESCloudPropProvCallback.initialCallbackValuse();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        eSEnrolleeHelper = null;
        ESCloudPropProvCallback.initialCallbackValuse();
    }

    /**
     * @throws ESException
     * @since 2016-08-14
     * @see public void setCloudProp(String authCode, String authProvider,
     *      String ciServer)
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
    public void testESCloudProvision_P_RSV() {

        OcResource remoteEnrolleeResource = eSEnrolleeHelper
                .findEnrolleeResource(getContext());
        assertNotNull("Got null remote enrollee resource",
                remoteEnrolleeResource);
        RemoteEnrollee remoteEnrollee = EasySetup.getInstance(getContext())
                .createRemoteEnrollee(remoteEnrolleeResource);
        assertNotNull("Got null remote enrollee", remoteEnrollee);
        CloudProp cloudProp = ESPropertiesHelper.createCloudProperties();
        assertNotNull("Got null deviceProp", cloudProp);

        try {
            remoteEnrollee.provisionCloudProperties(cloudProp,
                    new ESCloudPropProvCallback());
            ESUtility.waitUntilEnrolleeResponse(
                    ESCloudPropProvCallback.isCloudPropProvCallbackCalled);

        } catch (Exception ex) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        assertTrue("CloudPropProvCallback not called",
                ESCloudPropProvCallback.isCloudPropProvCallbackCalled);
        assertTrue("Fail to find enrollee",
                ESCloudPropProvCallback.isEnrolleeFound);
        ESCloudPropProvCallback.waitUntilProvSuccess(
                ESCloudPropProvCallback.isCloudPropSuccess);
        assertTrue("Cloud provisioning fail : ",
                ESCloudPropProvCallback.isCloudPropSuccess);
    }
}