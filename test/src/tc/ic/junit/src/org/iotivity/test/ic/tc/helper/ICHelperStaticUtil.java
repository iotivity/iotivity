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

package org.iotivity.test.ic.tc.helper;

import java.util.*;

import org.iotivity.base.*;
import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource.*;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcPresenceHandle;

public final class ICHelperStaticUtil {
    public static final String IC_HOST_ADDRESS = "coap+tcp://192.168.43.174:5683";
    public static final String IC_HOST_ADDRESS_INVALID = "AAA.BBB:GGG";
    public static final String IC_HOST_ADDRESS_EMPTY = "";
    public static final String IC_HOST_ADDRESS_NULL = null;
    public static final String IC_INVALID_PARAM_MSG = "Exception shoud occur due to invalid parameter";
    public static final OcConnectivityType IC_CONNECTIVITY_TYPE = OcConnectivityType.CT_DEFAULT;
    public static final QualityOfService IC_OC_QUALITY_OF_SERVICE = QualityOfService.LOW;
    public static final String IC_OC_QUALITY_OF_SERVICE_NULL = null;

    public static final OcRDClient.OnPublishResourceListener IC_ON_PUBLISHED_CALLBACK_HANDLER_NULL = null;
    public static final OcRDClient.OnDeleteResourceListener IC_ON_DELETE_CALLBACK_HANDLER_NULL = null;
    public static final OcResource.OnObserveListener IC_ON_OBSERVE_CALLBACK_HANDLER_NULL = null;
    public static OcPresenceHandle IC_OCPRESENCEHANDLE = null;

    public static List<OcResourceHandle> s_ocResourceList = new ArrayList<OcResourceHandle>();
    public static OcResourceHandle s_ocResource = null;
    public static List<String> s_deviceIdList = new ArrayList<String>();

    public static final String AUTH_PROVIDER = "AnyAuthProvider";
    public static final String AUTH_CODE = "AnyAuthCode";
    public static final String ACCESS_TOKEN = "AnyAccessToken";
    public static final String REFRESH_TOKEN = "AnyRefreshToken";
    public static final String USER_ID = "AnyUserId";
    public static final String DEVICE_ID = "AnyDeviceId";
    public static final String GROUP_ID = "208ee67c-a535-e4a4-68ca-3cea984964ca";
    public static final String USER_UUID = "AnyUserUuid";
    public static final String TOPIC_URI = "/ps/nweTopic";
    public static final String Resource_URI = "/a/light";
    public static final String RESOURCE_TYPE = "core.light";
    public static final String KEY = "members";
    public static final String VALUE = "SRBD_IOT";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE;
    public static final String MQ_BROKER_URI = "/oic/ps";
    public static final String MQ_DEFAULT_TOPIC_URI = "/oic/ps/cleanroom13";
    public static final String MQ_DEFAULT_TOPIC_URI2 = "/oic/ps/cleanroom14";
    public static Map<String, String> MY_MAP = new HashMap<String, String>() {
        {
            put("a", "b");
        }
    };
    public static final Map<String, String> EMPTY_MY_MAP = Collections.emptyMap();
    public static List<String> LIST_DEVICE_ID = new ArrayList<String>() {
        {
            add("sggg");
        }
    };
    public static final List<String> EMPTY_LIST_DEVICE_ID = Collections.emptyList();
    public static List<String> RESOURCE_LIST = new ArrayList<String>();
    public static List<String> INTERFACE_LIST = new ArrayList<String>();

    public static final OnPostListener IC_OcAccountManager_OnPostListener_NULL = null;
    public static final OnGetListener IC_OcAccountManager_OnGetListener_NULL = null;
    public static final OnDeleteListener IC_OcAccountManager_OnDeleteListener_NULL = null;
    public static final OnObserveListener IC_OcAccountManager_OnObserveListener_NULL = null;
    public static final OnMQTopicCreatedListener IC_MessageQueue_OnTopicCreateListener_NULL = null;

    public static final String INVALID_PARAMETER = "";
    public static final String EMPTY_VALUE = "";
    public static final String NULL_VAL = null;

    public static final int WAITING_TIME_IN_SECOND = 1000;
    public static final int MAX_WAIT_IN_CALLBACK = 5;
    public static final String SHOULD_NOT_THROW_EXCEPTION = "Should not through exception";
    public static final String SHOULD_THROW_EXCEPTION = "Should through exception";

    public static final String TAG = "OIC_CLOUD_TEST";
    public static List<OcResourceHandle> sResourceHandleList = new LinkedList<>();
    public static OcResource sMQbrokerResource = null;

}
