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
import org.iotivity.base.OcResource;
import org.iotivity.cloud.*;
import org.iotivity.configuration.*;
import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;
import org.iotivity.test.ic.tc.helper.*;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

public class ICMessageQueueTest extends InstrumentationTestCase
        implements IConfiguration {

    private static final String    ALL_INTERFACE_TYPE = "0.0.0.0";
    private ICMessageAdapter       mIcMessageAdapter;
    QualityOfService               mQualityOfService;
    private Context                mContext;
    OcRepresentation               mRep               = new OcRepresentation();
    ICMessageQueue                 mMessageQueueHelper;
    private OcAccountManagerHelper mCloudHelper;
    private OcAccountManager       mAccountManager;

    protected void setUp() throws Exception {
        super.setUp();
        mMessageQueueHelper = new ICMessageQueue();
        mQualityOfService = QualityOfService.LOW;
        mIcMessageAdapter = new ICMessageAdapter();

        mContext = getInstrumentation().getContext();
        mCloudHelper = new OcAccountManagerHelper();

        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, ALL_INTERFACE_TYPE, // bind to all
                                                            // available
                // interfaces
                0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);

        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        OcAccountManagerHelper.init(mContext);

        MY_MAP.put("rt", "light");

        // signUp and SignIn for all TC
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        mMessageQueueHelper.getMQBroker();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'createMQTopic' API With valid scenario
     * @target createMQTopic(const OCRepresentation& rep, const std::string&
     *         topicUri, const QueryParamsMap& queryParametersMap,
     *         MQTopicCallback attributeHandler, QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn
     * @procedure Call createMQTopic API with valid parameter
     * @post_condition SignOut
     * @expected createMQTopic API works properly without any exception..
     */
    public void testCreateMQTopic_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'createMQTopic' API With valid scenario
     * @target createMQTopic(const OCRepresentation& rep, const std::string&
     *         topicUri, const QueryParamsMap& queryParametersMap,
     *         MQTopicCallback attributeHandler, QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn
     * @procedure Call createMQTopic API with valid parameter
     * @post_condition SignOut
     * @expected createMQTopic API works properly without any exception..
     */
    public void testCreateMQTopicByType_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'discoveryMQTopics' API With valid scenario
     * @target OCStackResult discoveryMQTopics(const QueryParamsMap&
     *         queryParametersMap, MQTopicCallback attributeHandler,
     *         QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn 4. Create Topic
     * @procedure Call discoveryMQTopics API with valid parameter
     * @post_condition SignOut
     * @expected discoveryMQTopics API works properly without any exception..
     */
    public void testDiscoveryMQTopics_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishMQTopic' API With valid scenario
     * @target OCStackResult publishMQTopic(const QueryParamsMap&
     *         queryParametersMap, MQTopicCallback
     *         attributeHandler,QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn
     * @procedure 1. Perform createMQTopic() API 2. Perform discoveryMQTopics()
     *            API 3. Call publishMQTopic API with valid parameter 4. Check
     *            all APIs work properly
     * @post_condition SignOut
     * @expected publishMQTopic API works properly without any exception..
     */
    public void testdiscoveryMQTopicsByType_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("resourceUri is null", ICMessageAdapter.sResourceUri);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishMQTopic' API With valid scenario
     * @target OCStackResult publishMQTopic(const QueryParamsMap&
     *         queryParametersMap, MQTopicCallback
     *         attributeHandler, QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn
     * @procedure 1. Perform createMQTopic() API 2. Perform discoveryMQTopics()
     *            API 3. Call publishMQTopic API with valid parameter 4. Check
     *            all APIs work properly
     * @post_condition SignOut
     * @expected publishMQTopic API works properly without any exception..
     */
    public void testPublishMQTopic_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("resourceUri is null", ICMessageAdapter.sResourceUri);

            mRep = mMessageQueueHelper.generateRep();
            assertNotNull("m_rep is null", mRep);

            sMQbrokerResource.publishMQTopic(mRep, MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnPostCompleted is failed",
                    ICMessageAdapter.sOnPostCompleted);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-26
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'publishMQTopic' API With valid scenario
     * @target OCStackResult publishMQTopic(const QueryParamsMap&
     *         queryParametersMap, MQTopicCallback attributeHandler,
     *         QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn
     * @procedure 1. Perform createMQTopic() API 2. Perform discoveryMQTopics()
     *            API 3. Call requestMQPublish API with valid parameter 4. Check
     *            all APIs work properly
     * @post_condition SignOut
     * @expected publishMQTopic API works properly without any exception..
     */
    public void testRequestpublishMQTopic_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("resourceUri is null", ICMessageAdapter.sResourceUri);

            mRep = mMessageQueueHelper.generateRep();
            assertNotNull("m_rep is null", mRep);

            sMQbrokerResource.requestMQPublish(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnPostCompleted is failed",
                    ICMessageAdapter.sOnPostCompleted);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-03
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'subscribeMQTopic' API With valid scenario
     * @target OCStackResult subscribeMQTopic(const QueryParamsMap&
     *         queryParametersMap,MQTopicCallback attributeHandler,
     *         QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn 4. discover Topic
     * @procedure 1. Perform createMQTopic() API 2. Perform discoveryMQTopics()
     *            API 3. Call subscribeMQTopic API with valid parameter 4. Check
     *            all APIs work properly
     * @post_condition SignOut
     * @expected subscribeMQTopic API works properly without any exception..
     */
    public void testSubscribeMQTopic_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("resourceUri is null", ICMessageAdapter.sResourceUri);

            mRep = mMessageQueueHelper.generateRep();
            assertNotNull("m_rep is null", mRep);

            ICMessageAdapter.scurrentTopicResource.subscribeMQTopic(MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnPostCompleted is failed",
                    ICMessageAdapter.sOnObserveCompleted);
            assertNotNull("resourceUri is null",
                    ICMessageAdapter.sCurrentResourceRep);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-03
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'unsubscribeMQTopic' API With valid scenario
     * @target OCStackResult unsubscribeMQTopic(QualityOfService qos)
     * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4.
     *            MQTopicCallback 5. QualityOfService
     * @pre_condition 1. constructAccountManagerObject(host, connectivity_type)
     *                API 2. SignUp 3. SignIn 4. discover Topic
     * @procedure 1. Perform createMQTopic() API 2. Perform discoveryMQTopics()
     *            API 3. Call subscribeMQTopic() and unsubscribeMQTopic() APIs
     *            with valid parameter 4. Check all APIs work properly
     * @post_condition SignOut
     * @expected unsubscribeMQTopic API works properly without any exception..
     */
    public void testUnsubscribeMQTopic_SRC_P() {
        try {
            assertNotNull("MQbrokerResource is null", sMQbrokerResource);
            sMQbrokerResource.createMQTopic(mRep, MQ_DEFAULT_TOPIC_URI, MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("OnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("currentTopicResource is null",
                    ICMessageAdapter.scurrentTopicResource);

            sMQbrokerResource.discoveryMQTopics(MY_MAP, mIcMessageAdapter,
                    IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnTopicResourceCreated is failed",
                    ICMessageAdapter.sOnTopicResourceCreated);
            assertNotNull("resourceUri is null", ICMessageAdapter.sResourceUri);

            ICMessageAdapter.scurrentTopicResource.subscribeMQTopic(MY_MAP,
                    mIcMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            assertTrue("mOnPostCompleted is failed",
                    ICMessageAdapter.sOnObserveCompleted);
            assertNotNull("resourceUri is null",
                    ICMessageAdapter.sCurrentResourceRep);

            ICMessageAdapter.scurrentTopicResource
                    .unsubscribeMQTopic(IC_OC_QUALITY_OF_SERVICE);
        } catch (OcException ex) {
            fail("Exception occurred: " + ex.getLocalizedMessage());
        }
    }

}
