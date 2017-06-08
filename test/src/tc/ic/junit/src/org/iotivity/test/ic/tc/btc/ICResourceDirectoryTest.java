/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

package org.iotivity.test.ic.tc.btc;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

import java.util.EnumSet;

import android.content.Context;
import android.test.InstrumentationTestCase;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcPresenceHandle;
import org.iotivity.base.OcRDClient;
import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter;

public class ICResourceDirectoryTest extends InstrumentationTestCase {
    private Context                          m_Context;
    private ICResourceDirectoryCommonAdapter m_ICDesourceDirectoryAdapter;
    private OcPresenceHandle                 m_OcPresenceHandle;

    protected void setUp() throws Exception {
        super.setUp();
        m_Context = getInstrumentation().getTargetContext();
        ICHelper.icConfigurePlatform(m_Context);
        m_ICDesourceDirectoryAdapter = new ICResourceDirectoryCommonAdapter();
    }
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective test publishResourceToRD API Positively
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will Publish Virtual Resource to Resource Directory & return
     *           OK
     **/
    public void testPublishResourceToRD_SRC_FSV_P() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' to Publish Virtual Resource to RD
     *            with invalid host address
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host invalid host address 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */

    public void testPublishResourceToRDwithInvalidHost_USV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDwithEmptyHost_ESV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDwithNullHost_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_NULL,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'publishResourceToRD' negatively with NULL string value
     * @target publishResourceToRD(const std::string& host,OCConnectivityType
     *         connectivityType, PublishResourceCallback callback)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback null as callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDwithNullCallback_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    IC_ON_PUBLISHED_CALLBACK_HANDLER_NULL);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective test publishResourceToRD API Positively
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will Publish Virtual Resource to Resource Directory & return
     *           OK
     */
    public void testPublishResourceToRDWithQOS_SRC_FSV_P() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' to Publish Virtual Resource to RD
     *            with invalid host address
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. onPublishResourceListener callback 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */

    public void testPublishResourceToRDWithQOSwithInvalidHost_USV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * 
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDWithQOSWithEmptyHost_ESV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * 
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithQOSWithNullHost_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_NULL,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'publishResourceToRD' negatively with NULL string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback null as callback 4. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithQOSWithNullCallback_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    IC_ON_PUBLISHED_CALLBACK_HANDLER_NULL,
                    IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective test publishResourceToRD API Positively
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will Publish Virtual Resource to Resource Directory & return
     *           OK
     */
    public void testPublishResourceToRDWithResourceHandle_SRC_FSV_P() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' to Publish Virtual Resource to RD
     *            with invalid host address
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host invalid host address 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. ocResourceHandleList OCResourceHandleList
     *            4. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDWithResourceHandleWithInvalidHost_USV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDWithResourceHandleWithEmptyHost_ESV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address as null 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. ocResourceHandleList OCResourceHandleList
     *            4. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithResourceHandleWithNullHost_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_NULL,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'publishResourceToRD' negatively with NULL string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            callback null as callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithResourceHandleWithNullCallback_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    IC_ON_PUBLISHED_CALLBACK_HANDLER_NULL);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective test publishResourceToRD API Positively
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            onPublishResourceListener callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will Publish Virtual Resource to Resource Directory & return
     *           OK
     */
    public void testPublishResourceToRDWithResourceHandleAndQOS_SRC_FSV_P() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' to Publish Virtual Resource to RD
     *            with invalid host address
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. ocResourceHandleList OCResourceHandleList
     *            4. onPublishResourceListener callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDWithResourceHandleAndQOS_USV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            onPublishResourceListener callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testPublishResourceToRDWithResourceHandleAndQOS_ESV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-30
     * @see none
     * @objective Test 'publishResourceToRD' negatively with Empty string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host Host address as null 2. connectivityTypeSet CT_DEFAULT
     *            as connectivity 3. ocResourceHandleList OCResourceHandleList
     *            4. onPublishResourceListener callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithResourceHandleAndQOSWithAddressNull_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS_NULL,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'publishResourceToRD' negatively with NULL string value
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList OCResourceHandleList 4.
     *            callback null as callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testPublishResourceToRDWithResourceHandleAndQOSWithNullCallback_NV_N() {
        try {
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    IC_ON_PUBLISHED_CALLBACK_HANDLER_NULL,
                    IC_OC_QUALITY_OF_SERVICE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-31
     * @see none
     * @objective Test 'deleteResourceFromRD' positively to delete Published
     *            Resource from RD
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will delete Published Resource from Resource Directory &
     *           return OK
     */
    public void testDeleteResourceFromRD_SRC_FSV_P() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Unformatted value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener onDelete as callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testDeleteResourceFromRDWithInvalidHost_USV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Empty string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host empty host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener onDelete as callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDWithEmptyHost_ESV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with NULL string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener null
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDDWithNullCallback_NV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    IC_ON_DELETE_CALLBACK_HANDLER_NULL);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-31
     * @see none
     * @objective Test 'deleteResourceFromRD' positively to delete Published
     *            Resource from RD
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener callback 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will delete Published Resource from Resource Directory &
     *           return OK
     */
    public void testDeleteResourceFromRDWithQOS_SRC_FSV_P() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Unformatted value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener onDelete as callback
     *            4. qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testDeleteResourceFromRDWithQOSWithInvalidHost_USV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Empty string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host empty host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener onDelete as callback
     *            4. qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDWithQOSWithEmptyHost_ESV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with NULL string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. onDeleteResourceListener null 4.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */

    public void testDeleteResourceFromRDWithQOSWithNullCallback_NV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    IC_ON_DELETE_CALLBACK_HANDLER_NULL,
                    IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-31
     * @see none
     * @objective Test 'deleteResourceFromRD' positively to delete Published
     *            Resource from RD
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will delete Published Resource from Resource Directory &
     *           return OK
     */
    public void testDeleteResourceFromRDWithResourceHandler_SRC_FSV_P() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);

        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Unformatted value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testDeleteResourceFromRDWithResourceHandler_USV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Empty string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host empty host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener callback
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDWithResourceHandler_ESV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with NULL string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener)
     * @test_data 1. host host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener null
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDWithResourceHandlerWithNullCallback_NV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    IC_ON_DELETE_CALLBACK_HANDLER_NULL);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-31
     * @see none
     * @objective Test 'deleteResourceFromRD' positively to delete Published
     *            Resource from RD
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener callback 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will delete Published Resource from Resource Directory &
     *           return OK
     */
    public void testDeleteResourceFromRDWithResourceHanlderAndQOS_SRC_FSV_P() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

        } catch (OcException ex) {
            ex.printStackTrace();
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Unformatted value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host invalid host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener onDelete as callback 5.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_URI
     */
    public void testDeleteResourceFromRDWithResourceHanlderAndQOS_USV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with Empty string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host empty host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener onDelete as callback 5.
     *            qualityOfService QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */
    public void testDeleteResourceFromRDWithResourceHanlderAndQOSWithEmptyHost_ESV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    m_ICDesourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-24
     * @see none
     * @objective Test 'deleteResourceFromRD' negatively with NULL string value
     * @target deleteResourceFromRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         java.util.List<OcResourceHandle> ocResourceHandleList,
     *         OcPlatform.OnDeleteResourceListener onDeleteResourceListener,
     *         QualityOfService qualityOfService)
     * @test_data 1. host host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. ocResourceHandleList ocResourceHandler list 4.
     *            onDeleteResourceListener null 5. qualityOfService
     *            QualityOfService.HIGH
     * @pre_condition none
     * @procedure 1. Call deleteResourceFromRD API 2. Check it's return value
     * @post_condition none
     * @expected It will not success & return INVALID_PARAM
     */

    public void testDeleteResourceFromRDWithResourceHanlderAndQOSWithNullCallback_NV_N() {
        try {
            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), s_ocResourceList,
                    IC_ON_DELETE_CALLBACK_HANDLER_NULL,
                    IC_OC_QUALITY_OF_SERVICE);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'subscribeDevicePresence' negatively with Invalid Host
     *            address
     * @target OcPresenceHandle subscribeDevicePresence(java.lang.String host,
     *         java.util.List<java.lang.String> di,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcResource.OnObserveListener onObserveListener)
     * @test_data 1. host invalid host address 2. di device Identity 3.
     *            connectivityType CT_DEFAULT 4. onObserveCompleted callback
     * @pre_condition none
     * @procedure 1. Call subscribeDevicePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & will return JNI_EXCEPTION
     */
    public void testSubscribeDevicePresence_SRC_FSV_P() {
        try {
            OcPlatform.subscribeDevicePresence(IC_HOST_ADDRESS, s_deviceIdList,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

        } catch (OcException ex) {
            ex.printStackTrace();
            fail(IC_INVALID_PARAM_MSG);
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'subscribeDevicePresence' negatively with Invalid Host
     *            address
     * @target OcPresenceHandle subscribeDevicePresence(java.lang.String host,
     *         java.util.List<java.lang.String> di,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcResource.OnObserveListener onObserveListener)
     * @test_data 1. host invalid host address 2. di device Identity 3.
     *            connectivityType CT_DEFAULT 4. onObserveCompleted callback
     * @pre_condition none
     * @procedure 1. Call subscribeDevicePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & will return JNI_EXCEPTION
     */
    public void testSubscribeDevicePresenceWithInvalidHostAddress_USV_N() {
        try {
            OcPlatform.subscribeDevicePresence(IC_HOST_ADDRESS_INVALID,
                    s_deviceIdList, EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_URI, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'subscribeDevicePresence' negatively with Empty
     *            IC_HOST_ADDRESS
     * @target OcPresenceHandle subscribeDevicePresence(java.lang.String host,
     *         java.util.List<java.lang.String> di,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcResource.OnObserveListener onObserveListener)
     * @test_data 1. host empty host address 2. di device Identity 3.
     *            connectivityType CT_DEFAULT 4. onObserveCompleted callback
     * @pre_condition none
     * @procedure 1. Call subscribeDevicePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & should return JNI_EXCEPTION
     */
    public void testSubscribeDevicePresenceWithEmptyHost_ESV_N() {
        try {
            OcPlatform.subscribeDevicePresence(IC_HOST_ADDRESS_EMPTY,
                    LIST_DEVICE_ID, EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);

            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'subscribeDevicePresence' negatively with null OnObserve
     * @target OcPresenceHandle subscribeDevicePresence(java.lang.String host,
     *         java.util.List<java.lang.String> di,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcResource.OnObserveListener onObserveListener)
     * @test_data 1. host host address 2. di device Identity 3. connectivityType
     *            CT_DEFAULT 4. onObserveCompleted null
     * @pre_condition none
     * @procedure 1. Call subscribeDevicePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & should not return JNI_EXCEPTION
     */
    public void testSubscribeDevicePresenceWithNullCallback_NV_N() {
        try {
            OcPlatform.subscribeDevicePresence(IC_HOST_ADDRESS, s_deviceIdList,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    IC_ON_OBSERVE_CALLBACK_HANDLER_NULL);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'unsubscribePresence' negatively with Invalid Host
     *            address
     * @target OcPresenceHandle unsubscribePresence(OcResource.OnObserveListener
     *         onObserveListener)
     * @test_data 1. host invalid host address 2. di device Identity 3.
     *            connectivityType CT_DEFAULT 4. onObserveCompleted callback
     * @pre_condition none
     * @procedure 1. Call unsubscribePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & will return JNI_EXCEPTION
     */
    public void testunsubscribePresence_SRC_FSV_P() {
        try {
            IC_OCPRESENCEHANDLE = OcPlatform.subscribeDevicePresence(
                    IC_HOST_ADDRESS, s_deviceIdList,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICDesourceDirectoryAdapter);
            OcPlatform.unsubscribePresence(IC_OCPRESENCEHANDLE);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("unsubscribePresence crashed.");
        }
    }

    /**
     * @since 2016-08-25
     * @see none
     * @objective Test 'unsubscribePresence' negatively with Invalid Host
     *            address
     * @target OcPresenceHandle unsubscribePresence(OcResource.OnObserveListener
     *         onObserveListener)
     * @test_data 1. host invalid host address 2. di device Identity 3.
     *            connectivityType CT_DEFAULT 4. onObserveCompleted callback
     * @pre_condition none
     * @procedure 1. Call unsubscribePresence API 2. Check it's return value
     * @post_condition none
     * @expected It will not subscribes & will return JNI_EXCEPTION
     */
    public void testunsubscribePresenceWithNullCallback_NV_N() {
        try {
            OcPlatform.unsubscribePresence(IC_OCPRESENCEHANDLE);
            fail(IC_INVALID_PARAM_MSG);
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
}
