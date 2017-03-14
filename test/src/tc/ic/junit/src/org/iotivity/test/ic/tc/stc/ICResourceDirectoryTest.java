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

package org.iotivity.test.ic.tc.stc;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

import java.util.EnumSet;
import java.util.List;

import android.test.InstrumentationTestCase;
import android.content.Context;
import android.test.AndroidTestCase;
import android.util.Log;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcPresenceHandle;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcRDClient;
import org.iotivity.cloud.*;
import org.iotivity.configuration.*;

import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;
import org.iotivity.test.ic.tc.helper.*;

public class ICResourceDirectoryTest extends InstrumentationTestCase
        implements IConfiguration {

    private Context                          m_Context;
    private ICResourceDirectoryCommonAdapter m_ICResourceDirectoryAdapter;
    private OcPresenceHandle                 m_OcPresenceHandle;
    private OcAccountManagerHelper           mCloudHelper;
    private OcAccountManager                 mAccountManager;

    protected void setUp() throws Exception {
        super.setUp();

        m_Context = getInstrumentation().getTargetContext();
        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        OcAccountManagerHelper.init(m_Context);
        m_ICResourceDirectoryAdapter = new ICResourceDirectoryCommonAdapter();
        mCloudHelper = new OcAccountManagerHelper();
        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;
        ICResourceDirectoryCommonAdapter.sOnDeleteResourceCompleted = false;
        // signUp and SignIn for all TC
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2016-08-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishResourceToRD' positively to Publish Virtual * *
     *            Resource to RD
     * @target OCStackResult publishResourceToRD(const std::string& host,
     *         OCConnectivityType connectivityType, OC::ResourceHandles&
     *         resourceHandles, PublishResourceCallback callback,
     *         QualityOfService qos))
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Check ALL APIs
     *            work properly
     * @post_condition SignOut()
     * @expected It will Publish Virtual Resource to Resource Directory
     */
    public void testPublishResourcesToRDWithResourceHandeler_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishResourceToRD' positively to Publish Virtual
     *            Resource to RD
     * @target OCStackResult publishResourceToRD(const std::string& host,
     *         OCConnectivityType connectivityType, PublishResourceCallback
     *         callback, QualityOfService qos)
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Check ALL APIs
     *            work properly
     * @post_condition SignOut
     * @expected It will Publish Virtual Resource to Resource Directory
     */
    public void testPublishResourcesToRDWithResourceHandelerandQOS_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishResourceToRD' positively to Publish Virtual
     *            Resource to RD
     * @target OCStackResult publishResourceToRD(const std::string& host,
     *         OCConnectivityType connectivityType, OC::ResourceHandles&
     *         resourceHandles, PublishResourceCallback callback)
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Check ALL APIs
     *            work properly
     * @post_condition SignOut
     * @expected It will Publish Virtual Resource to Resource Directory
     */
    public void testPublishResourcesToRDWithResourceList_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    sResourceHandleList, m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'deleteResourceFromRD' positively to Publish Virtual
     *            Resource to RD
     * @target deleteResourceFromRD(const std::string& host,OCConnectivityType
     *         connectivityType, PublishResourceCallback callback)
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Perform string
     *            deleteResourceFromRD() API with hendler 3. Check ALL APIs work
     *            properly
     * @expected It will delete Virtual Resource from Resource Directory
     */
    public void testDeleteResourcesToRDWithRESHandle_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICResourceDirectoryAdapter);
            assertTrue("deleteResourceFromRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sOnDeleteResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'deleteResourceFromRD' positively to delete resource from
     *            RD
     * @target OCStackResult deleteResourceFromRD(const std::string& host,
     *         OCConnectivityType connectivityType, DeleteResourceCallback
     *         callback, QualityOfService qos)
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Perform string
     *            deleteResourceFromRD() API with QOS 3. Check ALL APIs work
     *            properly
     * @post_condition SignOut
     * @expected It will delete Virtual Resource from Resource Directory
     */
    public void testDeleteResourcesToRDWithQOS_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    m_ICResourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("deleteResourceFromRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sOnDeleteResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-24
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'deleteResourceFromRD' positively to delete resource from
     *            RD
     * @target OCStackResult deleteResourceFromRD(const std::string& host,      
     *                                          OCConnectivityType
     *         connectivityType, OC::ResourceHandles& resourceHandles,          
     *                                      DeleteResourceCallback callback)
     * @test_data 1. host Host address 2. connectivityType CT_DEFAULT as
     *            connectivity 3. callback onPublish as callback
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string publishResourceToRD() API 2. Perform string
     *            deleteResourceFromRD() API 3. Check ALL APIs work properly
     * @post_condition SignOut
     * @expected It will delete Virtual Resource from Resource Directory
     */
    public void testDeleteResourcesToRDWithResourceList_SRC_FSV_P() {
        assertTrue("Create resource failed", ICHelper.createResource());
        try {
            // publish virtual resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted = false;

            // Publish Local Resource
            OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    sResourceHandleList, m_ICResourceDirectoryAdapter);
            assertTrue("publishResourceToRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sIsOnPublishResourceCompleted);

            OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    sResourceHandleList, m_ICResourceDirectoryAdapter);
            assertTrue("deleteResourceFromRD callback is not worked",
                    ICResourceDirectoryCommonAdapter.sOnDeleteResourceCompleted);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
}
