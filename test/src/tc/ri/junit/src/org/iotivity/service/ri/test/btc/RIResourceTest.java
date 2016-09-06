/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * // * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.service.ri.test.btc;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcPlatformStatus;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcResource.OnDeleteListener;
import org.iotivity.base.OcResource.OnGetListener;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResource.OnPostListener;
import org.iotivity.base.OcResource.OnPutListener;
import org.iotivity.service.ri.test.helper.RIHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class RIResourceTest extends InstrumentationTestCase {
    private final String LOG_TAG = this.getClass().getSimpleName();

    private RIHelper m_riHelper;
    private String m_host = "";
    private OcResource m_resource;
    private OcRepresentation m_rep = new OcRepresentation();

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(getInstrumentation()
                .getTargetContext());
        m_resource = null;
        m_resource = m_riHelper.constructResource();

        if (m_resource == null) {
            fail("constructResourceObject failed");
        }

        Log.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        Log.i(LOG_TAG, "tearDown Finished");
    }

    OnGetListener onGet = new OnGetListener() {

        @Override
        public void onGetCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {
            Log.i(LOG_TAG, "Inside onGet");
        }

        @Override
        public void onGetFailed(Throwable message) {
            Log.i(LOG_TAG, "onGet failed, reason: " + message);
        }


    };

    OnPutListener onPut = new OnPutListener() {

        @Override
        public void onPutFailed(Throwable message) {
            Log.i(LOG_TAG, "onPut failed, reason: " + message);
        }

        @Override
        public void onPutCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {
            Log.i(LOG_TAG, "Inside onPut");
        }
    };

    OnPostListener onPost = new OnPostListener() {

        @Override
        public void onPostFailed(Throwable message) {
            Log.i(LOG_TAG, "onPost failed, reason: " + message);
        }

        @Override
        public void onPostCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {
            Log.i(LOG_TAG, "Inside onPost");
        }
    };

    OnDeleteListener onDelete = new OnDeleteListener() {

        @Override
        public void onDeleteFailed(Throwable message) {
            Log.i(LOG_TAG, "onDelete failed, reason: " + message);
        }

        @Override
        public void onDeleteCompleted(List<OcHeaderOption> arg0) {
            Log.i(LOG_TAG, "Inside onDelete");
        }
    };

    OnObserveListener onObserve = new OnObserveListener() {

        @Override
        public void onObserveFailed(Throwable message) {
            Log.i(LOG_TAG, "onObserve failed, reason: " + message);
        }

        @Override
        public void onObserveCompleted(List<OcHeaderOption> arg0,
                                       OcRepresentation arg1, int arg2) {
            Log.i(LOG_TAG, "Inside onObserve");
        }
    };

    /**
     * @objective Test get function with positive basic way
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener)
     * @test_data 1. queryParamsMap map with query paramter and value
     * `			2. onGetListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call get() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testGetResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.get(qpMap, onGet);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test get function with negative basic way using null event handler
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener)
     * @test_data 1. queryParamsMap map with query paramter and value
     * `			2. onGetListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call get() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testGetResourceHandler_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.get(qpMap, null);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test get function with negative basic way using null queryParamsMap
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener)
     * @test_data 1. queryParamsMap map with query paramter and value
     * `			2. onGetListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call get() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testGetResourceQueryParam_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.get(null, onGet);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test get function with positive basic way(with high qos)
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener, QualityOfService qualityOfService)
     * @test_data 1. queryParamsMap map with query paramter and value
     * `			2. onGetListener event handler
     * 3. qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call get() API(with high qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testGetResourceWithHighQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.get(qpMap, onGet, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test get function with positive basic way(with low qos)
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener, QualityOfService qualityOfService)
     * @test_data 1. queryParamsMap map with query paramter and value
     * `			2. onGetListener event handler
     * 3. qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call get() API(with low qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testGetResourceWithLowQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.get(qpMap, onGet, QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test put function with positive basic way
     * @target void put(OcRepresentation representation,
     * Map<String, String> queryParamsMap,
     * OnPutListener onPutListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPutListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call put() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(m_rep, qpMap, onPut);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test put function with negative basic way using null representation
     * @target put(OcRepresentation representation,
     *Map<String, String> queryParamsMap,
     *OnPutListener onPutListener)
     * @test_data 1. representation null
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPutListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call put() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResourceRepresentation_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(null, qpMap, onPut);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test put function with negative basic way using null event handler
     * @target put(OcRepresentation representation,
     *Map<String, String> queryParamsMap,
     *OnPutListener onPutListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPutListener null
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call put() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResourceHandler_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(m_rep, qpMap, null);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test put function with negative basic way using null queryParamsMap
     * @target put(OcRepresentation representation,
     *Map<String, String> queryParamsMap,
     *OnPutListener onPutListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap null
     * `			3. OnPutListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call put() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResourceQueryParam_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(m_rep, null, onPut);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test put function with positive basic way(with high qos)
     * @target void put(OcRepresentation representation,
     * Map<String, String> queryParamsMap,
     * OnPutListener onPutListener,
     * QualityOfService qualityOfService)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPutListener event handler
     * `				4. qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call put() API(with high qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResourceWithHighQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(m_rep, qpMap, onPut, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test get function with positive basic way(with low qos)
     * @target void put(OcRepresentation representation,
     * Map<String, String> queryParamsMap,
     * OnPutListener onPutListener,
     * QualityOfService qualityOfService)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPutListener event handler
     * `				4. qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure call put() API(with low qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPutResourceWithLowQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.put(m_rep, qpMap, onPut, QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test post function with positive basic way
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. onPostListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call post() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(m_rep, qpMap, onPost);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test post function with negative basic way using null representation
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener)
     * @test_data 1. representation null
     * 2. queryParamsMap map with query paramter and value
     * `			3. onPostListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call post() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResourceRepresentation_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(null, qpMap, onPost);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test post function with negative basic way using null event handler
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. OnPostListener null
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call post() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResourceHandler_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(m_rep, qpMap, null);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test post function with negative basic way using null queryParamsMap
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap null
     * `			3. OnPostListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call post() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResourceQueryParam_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(m_rep, null, onPost);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test post function with positive basic way(with high qos)
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener,
     * QualityOfService qualityOfService)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. onPostListener event handler
     * `				4. qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call post() API(with high qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResourceWithHighQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(m_rep, qpMap, onPost, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test post function with positive basic way(with low qos)
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener,
     * QualityOfService qualityOfService)
     * @test_data 1. representation representation of the resource
     * 2. queryParamsMap map with query paramter and value
     * `			3. onPostListener event handler
     * `				4. qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure call post() API(with low qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testPostResourceWithLowQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.post(m_rep, qpMap, onPost, QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test deleteResource function with positive basic way
     * @target void deleteResource(OnDeleteListener onDeleteListener)
     * @test_data OnDeleteListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call deleteResource() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testDeleteResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.deleteResource(onDelete);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test deleteResource function with negative basic way using null handler
     * @target void deleteResource(OnDeleteListener onDeleteListener)
     * @test_data OnDeleteListener null
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call deleteResource() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testDeleteResourceHandler_NV_N() {
        try {
            m_resource.deleteResource(null);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test deleteResource function with positive basic way(with high qos)
     * @target void deleteResource(OnDeleteListener onDeleteListener,QualityOfService qualityOfService)
     * @test_data 1. OnDeleteListener event handler
     * 2. qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call deleteResource() API using resource(with high qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testDeleteResourceWithHighQos_SRC_P() {
        try {
            m_resource.deleteResource(onDelete, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test deleteResource function with positive basic way(with low qos)
     * @target void deleteResource(OnDeleteListener onDeleteListener,QualityOfService qualityOfService)
     * @test_data 1. OnDeleteListener event handler
     * 2. qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call deleteResource() API using resource(with low qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testDeleteResourceWithLowQos_SRC_P() {
        try {
            m_resource.deleteResource(onDelete, QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test observe function with positive basic way
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @test_data 1. observeType observe type
     * 2. queryParamsMap map with query paramter and value
     * `			3. onObserveListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call observe() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test observe function with negative basic way using null observe type
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @test_data 1. observeType null
     * 2. queryParamsMap map with query paramter and value
     * `			3. onObserveListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call observe() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResourceObserveType_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(null, qpMap, onObserve);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test observe function with negative basic way using null event handler
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @test_data 1. observeType observe type
     * 2. queryParamsMap map with query paramter and value
     * `			3. onObserveListener null
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call observe() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResourceHandler_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, null);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test observe function with negative basic way using null queryParamsMap
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @test_data 1. observeType observe type
     * 2. queryParamsMap null
     * `			3. onObserveListener event handler
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call observe() API using resource
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResourceQueryParam_NV_N() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, null, onObserve);
            fail("OcException should have occured");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test observe function with positive basic way(with high qos)
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener,
     * QualityOfService qualityOfService)
     * @test_data 1. observeType observe type
     * 2. queryParamsMap map with query paramter and value
     * `			3. onObserveListener event handler
     * `			4. qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure Call observe() API(with high qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResourceWithHighQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test observe function with positive basic way(with low qos)
     * @target void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener,
     * QualityOfService qualityOfService)
     * @test_data 1. observeType observe type
     * 2. queryParamsMap map with query paramter and value
     * `			3. onObserveListener event handler
     * `			4. qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * @procedure call observe() API(with low qos) using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @since 2016-09-05
     **/
    public void testObserveResourceWithLowQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve, QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test cancelObserve function with positive basic way
     * @target void cancelObserve()
     * @test_data None
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * 3. Call observe() API
     * @procedure Call cancelObserve() API using resource
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @see void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @since 2016-09-05
     **/
    public void testCancelObserveResource_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in observe");
        }

        try {
            m_resource.cancelObserve();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test cancelObserve function with positive basic way(with high qos)
     * @target void cancelObserve(QualityOfService qualityOfService)
     * @test_data qualityOfService High
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * 3. Call observe() API
     * @procedure Call cancelObserve() API using resource(with high qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @see void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @since 2016-09-05
     **/
    public void testCancelObserveResourceWithHighQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in observe");
        }

        try {
            m_resource.cancelObserve(QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test cancelObserve function with positive basic way(with low qos)
     * @target void cancelObserve(QualityOfService qualityOfService)
     * @test_data qualityOfService Low
     * @pre_condition 1. configure platform
     * 2. construct resource object
     * 3. Call observe() API
     * @procedure Call cancelObserve() API using resource(with low qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResource constructResourceObject(
     * String host,
     * String uri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * boolean isObservable,
     * List<String> resourceTypeList,
     * List<String> interfaceList)
     * @see void observe(ObserveType observeType,
     * Map<String, String> queryParamsMap,
     * OnObserveListener onObserveListener)
     * @since 2016-09-05
     **/
    public void testCancelObserveResourceWithLowQos_SRC_P() {
        Map<String, String> qpMap = new HashMap<String, String>();

        try {
            m_resource.observe(ObserveType.OBSERVE, qpMap, onObserve);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in observe");
        }

        try {
            m_resource.cancelObserve(QualityOfService.LOW);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }
}