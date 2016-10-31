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

package org.iotivity.cloud.ic.test.btc;
import static org.iotivity.cloud.ic.test.helper.ICHelperStaticUtil.*;
import static org.iotivity.cloud.ic.test.helper.MessageQueue.*;

import java.util.EnumSet;
import java.util.*;

import org.iotivity.cloud.ic.test.helper.ICHelper;
import org.iotivity.cloud.ic.test.helper.MessageQueue;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcResource.*;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.ModeType;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;


import android.content.Context;
import android.test.AndroidTestCase;

public class ICMessageQueue extends AndroidTestCase {
	
	private static final String ALL_INTERFACE_TYPE                 = "0.0.0.0";
        QualityOfService m_qualityOfService;
	OcResource m_ocResource;
	
	OcRepresentation m_rep = new OcRepresentation();
	OnMQTopicCreatedListener m_onTopicCreatedListener = new OnMQTopicCreatedListener() {
		public void onTopicResourceCreated(OcResource m_myResource) {
		}

		public void onCreateTopicFailed(Throwable ex, String m_uri) {
		}
	};
	OnMQTopicFoundListener m_onTopicFoundListener = new OnMQTopicFoundListener() {
		public void onTopicDiscoveried(OcResource m_myResource){}

		public void onDiscoveryTopicFailed(Throwable ex, String m_uri){}
	};
	OnPostListener m_onPostListener = new OnPostListener() {
		public void onPostCompleted(List<OcHeaderOption> m_headerOptionList,
				OcRepresentation m_ocRepresentation){}
		public void onPostFailed(Throwable ex){}
	};
	OnObserveListener m_onObserveListener = new OnObserveListener(){
		public void onObserveCompleted(List<OcHeaderOption> m_headerOptionList,
                OcRepresentation m_ocRepresentation,
                int m_sequenceNumber){}

		public void onObserveFailed(Throwable ex){}
		
	};
	

	protected void setUp() throws Exception {
		super.setUp();
		
        MessageQueue messageQueueHelper = new MessageQueue();
        m_qualityOfService = QualityOfService.LOW;
        m_ocResource = messageQueueHelper.findClintResource(getContext());
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test createMQTopic API Positively
	 * @target createMQTopic(OcRepresentation ocRepresentation, java.lang.String
	 *         uri, java.util.Map<java.lang.String,java.lang.String>
	 *         queryParamsMap, OcResource.OnMQTopicCreatedListener
	 *         onTopicCreatedListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onTopicCreatedListener callback
	 * @pre_condition none
	 * @procedure 1. Call createMQTopic API
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testcreateMQTopic_SRC_FSV_P() {
		try {
			m_ocResource.createMQTopic(m_rep, TOPIC_URI, MY_MAP,
					m_onTopicCreatedListener, IC_OC_QUALITY_OF_SERVICE);
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test createMQTopic API negatively
	 * @target createMQTopic(OcRepresentation ocRepresentation, java.lang.String
	 *         uri, java.util.Map<java.lang.String,java.lang.String>
	 *         queryParamsMap, OcResource.OnMQTopicCreatedListener
	 *         onTopicCreatedListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. NULL callback
	 * @pre_condition none
	 * @procedure 1. Call createMQTopic API 2. Check it's return value
	 * @post_condition none
	 * @expected It will throw exception
	 */
	public void testcreateMQTopic_ESV_ETC_N() {
		try {
			m_ocResource.createMQTopic(m_rep, NULL_VAL, MY_MAP, m_onTopicCreatedListener,
					m_qualityOfService);
		fail(SHOULD_THROW_EXCEPTION);			
		} catch (OcException ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test discoveryMQTopics API Positively
	 * @target public void discoveryMQTopics(Map<String, String> queryParamsMap,
	 *         OnMQTopicFoundListener onTopicFoundListener, QualityOfService
	 *         qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onTopicFoundListener callback
	 * @pre_condition none
	 * @procedure 1. Call discoveryMQTopics API 
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testdiscoveryMQTopics_SRC_FSV_P() {
		try {
			m_ocResource.discoveryMQTopics(MY_MAP, m_onTopicFoundListener,
					IC_OC_QUALITY_OF_SERVICE);
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test discoveryMQTopics API negatively
	 * @target public void discoveryMQTopics(Map<String, String> queryParamsMap,
	 *         OnMQTopicFoundListener onTopicFoundListener, QualityOfService
	 *         qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onTopicFoundListener callback
	 * @pre_condition none
	 * @procedure 1. Call discoveryMQTopics API 
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testdiscoveryMQTopics_ESV_ETC_N() {
		try {
			m_ocResource.discoveryMQTopics(MY_MAP, m_onTopicFoundListener, m_qualityOfService);			
		} catch (OcException ex) {
			fail(SHOULD_NOT_THROW_EXCEPTION);
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test requestMQPublish API Positively
	 * @target public void requestMQPublish(Map<String, String> queryParamsMap,
	 *         OnPostListener onPostListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onPostListener callback
	 * @pre_condition none
	 * @procedure 1. Call requestMQPublish API
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testrequestMQPublish_SRC_FSV_P() {
		try {
			m_ocResource.requestMQPublish(MY_MAP, m_onPostListener,
					IC_OC_QUALITY_OF_SERVICE);
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test requestMQPublish API negatively
	 * @target public void requestMQPublish(Map<String, String> queryParamsMap,
	 *         OnPostListener onPostListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onPostListener callback
	 * @pre_condition none
	 * @procedure 1. Call requestMQPublish API 
	 * @post_condition none
	 * @expected It will throw exception
	 */
	public void testrequestMQPublish_ESV_ETC_N() {
		try {
			m_ocResource.requestMQPublish(null, m_onPostListener, m_qualityOfService);
			fail(SHOULD_THROW_EXCEPTION);			
		} catch (OcException ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test publishMQTopic API Positively
	 * @target public void publishMQTopic(OcRepresentation ocRepresentation,
	 *         Map<String, String> queryParamsMap, OnPostListener
	 *         onPostListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. m_onPostListener callback
	 * @pre_condition none
	 * @procedure 1. Call publishMQTopic API 
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testpublishMQTopic_SRC_FSV_P() {
		try {
			m_ocResource.publishMQTopic(m_rep, MY_MAP, m_onPostListener,
					IC_OC_QUALITY_OF_SERVICE);
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test publishMQTopic API negatively
	 * @target public void publishMQTopic(OcRepresentation ocRepresentation,
	 *         Map<String, String> queryParamsMap, OnPostListener
	 *         onPostListener, QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. onPublishResourceListener callback
	 * @pre_condition none
	 * @procedure 1. Call publishMQTopic API
	 * @post_condition none
	 * @expected It will throw exception
	 */
	public void testpublishMQTopic_ESV_ETC_N() {
		try {
			m_ocResource.publishMQTopic(m_rep, MY_MAP, m_onPostListener, m_qualityOfService);			
		} catch (OcException ex) {
			fail(SHOULD_NOT_THROW_EXCEPTION);			
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test subscribeMQTopic API Positively
	 * @target subscribeMQTopic(Map<String, String> queryParamsMap,
	 *         OnObserveListener onObserveListener, QualityOfService
	 *         qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 3. onPublishResourceListener callback
	 * @pre_condition none
	 * @procedure 1. Call subscribeMQTopic API 2. Check it's return value
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testsubscribeMQTopic_SRC_FSV_P() {
		try {
			m_ocResource.subscribeMQTopic(MY_MAP, m_onObserveListener,
					IC_OC_QUALITY_OF_SERVICE);
			
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test subscribeMQTopic API negatively
	 * @target subscribeMQTopic(Map<String, String> queryParamsMap,
	 *         OnObserveListener onObserveListener, QualityOfService
	 *         qualityOfService)
	 * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
	 *            connectivity 
	 * @pre_condition none
	 * @procedure 1. Call subscribeMQTopic API 2. Check it's return value
	 * @post_condition none
	 * @expected It will throw exception
	 */
	public void testsubscribeMQTopic_ESV_ETC_N() {
		try {
			m_ocResource.subscribeMQTopic(MY_MAP, m_onObserveListener, m_qualityOfService);			
		} catch (OcException ex) {
			fail(SHOULD_NOT_THROW_EXCEPTION);
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test unsubscribeMQTopic API Positively
	 * @target public void unsubscribeMQTopic(QualityOfService qualityOfService)
	 * @test_data 1. host Host address 2. IC_OC_QUALITY_OF_SERVICE
	 * @pre_condition 1. subscribeMQTopic API
	 * @procedure 1. subscribeMQTopic API 2. Call unsubscribeMQTopic API
	 * @post_condition none
	 * @expected It will working fine without throw exception
	 */
	public void testunsubscribeMQTopic_SRC_FSV_P() {
		try {
			m_ocResource.subscribeMQTopic(MY_MAP, m_onObserveListener,
					IC_OC_QUALITY_OF_SERVICE);
			m_ocResource.unsubscribeMQTopic(IC_OC_QUALITY_OF_SERVICE);
		} catch (OcException ex) {
			
			fail("Exception occurred: " + ex.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-09-08
	 * @see none
	 * @objective test unsubscribeMQTopic API negatively
	 * @target public void unsubscribeMQTopic(QualityOfService qualityOfService)
	 * @test_data 	1. host Host address 
	 * 				2. unsubscribeMQTopic
	 * @pre_condition none
	 * @procedure 1. Call unsubscribeMQTopic API
	 * @post_condition none
	 * @expected It will throw exception
	 */
	public void testunsubscribeMQTopic_ESV_ETC_N() {
		try {
			m_ocResource.unsubscribeMQTopic(m_qualityOfService);
			fail(SHOULD_THROW_EXCEPTION);			
		} catch (OcException ex) {
			assertTrue(true);
		}
	}

}
