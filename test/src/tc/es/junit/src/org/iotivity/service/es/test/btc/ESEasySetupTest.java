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

import org.iotivity.service.es.test.helper.ESEnrolleeHelper;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;

import static org.iotivity.service.es.test.helper.ESUtility.*;

import android.test.AndroidTestCase;

public class ESEasySetupTest extends AndroidTestCase {

    private OcResource          ocResource;
    private ESEnrolleeHelper    eSEnrolleeHelper;
    private static final String RESOURCE_NAME          = "/a/light";
    private static final String PROV_RESOURCE_TYPE     = "ocf.wk.prov";
    private static final String INVALID_RESOURCE_TYPE  = "oic.es.invalid";
    private static final String INVALID_INTERFACE_TYPE = OcPlatform.DEFAULT_INTERFACE;
    private static final String BATCH_INTERFACE_TYPE   = OcPlatform.BATCH_INTERFACE;

    protected void setUp() throws Exception {
        super.setUp();
        eSEnrolleeHelper = new ESEnrolleeHelper();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        eSEnrolleeHelper = null;
        ocResource = null;
    }

    /**
     * @since 2016-08-14
     * @see none
     * @objective test getInstance API positively
     * @target public synchronized static EasySetup getInstance(Context context)
     * @test_data context
     * @pre_condition none
     * @procedure 1. Call getInstance API with application context. 2. Check
     *            easy setup intsance is not null.
     * @post_condition none
     * @expected initialization of the easy setup
     */
    public void testESGetInstance_SRC_RSV_P() {
        assertNotNull("Fail to cretae easy setup instance",
                EasySetup.getInstance(getContext()));
    }

    /**
     * @since 2016-08-14
     * @see none
     * @objective test getInstance API negatively
     * @target public synchronized static EasySetup getInstance(Context context)
     * @test_data null context
     * @pre_condition none
     * @procedure 1. Call getInstance API with null context.
     * @post_condition none
     * @expected exception thrown.
     */
    public void testESGetInstanceWithNullContext_ETC_NV_N() {
        try {
            EasySetup.getInstance(null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-14
     * @see none
     * @objective test createRemoteEnrollee API can create remote enrolle.
     * @target public synchronized RemoteEnrollee
     *         createRemoteEnrollee(OcResource enrolleeResource)
     * @test_data ocresource
     * @pre_condition create oc resource
     * @procedure 1. Call createRemoteEnrollee API with created resource. 2.
     *            Check enrollee object is not null.
     * @post_condition none
     * @expected Successful creation of remote enrollee object
     */
    public void testESCreateRemoteEnrollee_P_RSV() {
        ocResource = eSEnrolleeHelper.createEnrolleeResource(getContext(),
                RESOURCE_NAME, PROV_RESOURCE_TYPE, BATCH_INTERFACE_TYPE);
        assertNotNull("Resource object should not null", ocResource);
        assertNotNull("RemoteEnrollee should not null", EasySetup
                .getInstance(getContext()).createRemoteEnrollee(ocResource));
    }

    /**
     * @since 2016-08-18
     * @see none
     * @objective test createRemoteEnrollee API with resource containing invalid
     *            interface
     * @target public synchronized RemoteEnrollee
     *         createRemoteEnrollee(OcResource enrolleeResource)
     * @test_data context
     * @pre_condition none
     * @procedure 1. Call createRemoteEnrollee API with resource containing
     *            invalid resource type. 2. Check remote enrollee object is null
     * @post_condition none
     * @expected Null remote enrollee will be returned.
     */
    public void testESCreateRemoteEnrolleeWithInvalidInterfaceType_USV_SRC_N() {
        ocResource = eSEnrolleeHelper.createEnrolleeResource(getContext(),
                RESOURCE_NAME, INVALID_RESOURCE_TYPE, BATCH_INTERFACE_TYPE);
        assertNotNull("Resource object should not null", ocResource);
        assertNull("RemoteEnrollee should null", EasySetup
                .getInstance(getContext()).createRemoteEnrollee(ocResource));
    }

    /**
     * @since 2016-08-18
     * @see none
     * @objective test createRemoteEnrollee API with resource containing invalid
     *            interface
     * @target public synchronized RemoteEnrollee
     *         createRemoteEnrollee(OcResource enrolleeResource)
     * @test_data context
     * @pre_condition none
     * @procedure 1. Call createRemoteEnrollee API with resource containing
     *            invalid interface. 2. Check remote enrollee object is null.
     * @post_condition none
     * @expected Null remote enrollee will be returned.
     */
    public void testESCreateRemoteEnrolleeWithInvalidResourceType_USV_SRC_N() {
        ocResource = eSEnrolleeHelper.createEnrolleeResource(getContext(),
                RESOURCE_NAME, PROV_RESOURCE_TYPE, INVALID_INTERFACE_TYPE);
        assertNotNull("Resource object should not null", ocResource);
        assertNull("RemoteEnrollee should null", EasySetup
                .getInstance(getContext()).createRemoteEnrollee(ocResource));
    }

    /**
     * @since 2016-08-18
     * @see none
     * @objective test createRemoteEnrollee API with resource containing invalid
     *            interface
     * @target public synchronized RemoteEnrollee
     *         createRemoteEnrollee(OcResource enrolleeResource)
     * @test_data context
     * @pre_condition none
     * @procedure 1. Call createRemoteEnrollee API with null resource
     * @post_condition none
     * @expected should throw exception
     */
    public void testESCreateRemoteEnrolleeWithNullResource_ETC_N() {
        ocResource = null;
        try {
            EasySetup.getInstance(getContext())
                    .createRemoteEnrollee(ocResource);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }
}
