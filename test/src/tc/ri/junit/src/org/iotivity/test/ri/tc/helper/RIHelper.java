/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.test.ri.tc.helper;

import java.util.Arrays;
import java.util.EnumSet;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResourceHandle;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

import org.iotivity.test.ri.common.RIHelperCommon;

public class RIHelper extends RIHelperCommon implements IRIConstants {

    private static RIHelper          s_mRiHelperInstance           = null;

    private final String             LOG_TAG                    = this
            .getClass().getSimpleName();

    private OcResourceHandle         m_resourceHandle           = null;
    public EnumSet<ResourceProperty> m_resourceProperty;
    public static final String       TEMPERATURE_RESOURCE_QUERY = OcPlatform.WELL_KNOWN_QUERY
            + "?rt=" + RESOURCE_TYPE_TEMPERATURE;

    private OcRepresentation         m_representation           = null;

    public int                       m_temp;
    public int                       m_hour;

    public static boolean            s_mIsServerOk;

    public static String             s_mErrorMsg;

    private RIHelper(IoTivityTc iotivityTcObj) {
        super(iotivityTcObj);
        m_resourceProperty = EnumSet.of(ResourceProperty.ACTIVE);
        m_resourceProperty.add(ResourceProperty.DISCOVERABLE);
        m_resourceProperty.add(ResourceProperty.OBSERVABLE);
        m_temp = 10;
        m_hour = 10;
    }

    /**
     * @return singleton instance of RIHelper
     * @brief Function is for getting singleton instance of RIHelper
     */
    public static RIHelper getInstance(IoTivityTc iotivityTcObj) {

        new OcRepresentation();

        Lock mutex = new ReentrantLock();

        if (s_mRiHelperInstance == null) {
            mutex.lock();
            if (s_mRiHelperInstance == null) {
                IoTivityLog.i("RIHelper", "Inside Helper");
                s_mRiHelperInstance = new RIHelper(iotivityTcObj);
            }
            mutex.unlock();
        }

        return s_mRiHelperInstance;
    }

    /**
     * @return OcPlatformInfo
     * @brief Function is for getting platform information
     */
    public OcPlatformInfo getPlatformInfo() {
        configClientServerPlatform();

        OcPlatformInfo platformInfo = new OcPlatformInfo(PLATFORM_ID,
                MANUFACTURER_NAME, MANUFACTURER_URL);
        platformInfo.setModelNumber(MODEL_NO);
        platformInfo.setDateOfManufacture(MANUFACTURE_DATE);
        platformInfo.setPlatformVersion(PLATFORM_VERSION);
        platformInfo.setOperatingSystemVersion(OS_VERSION);
        platformInfo.setHardwareVersion(HW_VERSION);
        platformInfo.setFirmwareVersion(FIRMWARE_VERSION);
        platformInfo.setSupportUrl(SUPPORT_URL);
        platformInfo.setSystemTime(SYSTEM_TIME);

        return platformInfo;
    }

    /**
     * @return OcDeviceInfo
     * @brief Function is for getting device information
     */
    public OcDeviceInfo getDeviceInfo() {
        configClientServerPlatform();

        OcDeviceInfo deviceInfo = new OcDeviceInfo(DEVICE_NAME,
                Arrays.asList(new String[] { DEVICE_TYPE_AC }));
        return deviceInfo;
    }

    void setTemperatureRep() {
        try {
            m_representation.setValue(KEY_TEMPERATURE, m_temp);
            m_representation.setValue(KEY_HOUR, m_hour);
        } catch (OcException e) {
            IoTivityLog.i(LOG_TAG, "Exception occured iside setValue");
            e.printStackTrace();
        }
    }

    private void handleResponse(OcResourceRequest request) {
        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        // Get the request type and request flag
        RequestType requestType = request.getRequestType();
        EnumSet<RequestHandlerFlag> requestFlag = request
                .getRequestHandlerFlagSet();

        if (requestFlag.contains(RequestHandlerFlag.REQUEST)) {
            IoTivityLog.i(LOG_TAG, "\t\trequestFlag : Request");
            IoTivityLog.i(LOG_TAG, "\t\t\trequestType : " + requestType);

            // If the request type is GET
            if (requestType == RequestType.GET) {
                setTemperatureRep();
                response.setResourceRepresentation(m_representation);

                try {
                    OcPlatform.sendResponse(response);
                    s_mIsServerOk = true;
                    IoTivityLog.i(LOG_TAG,
                            "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();
                    IoTivityLog.i(LOG_TAG, "Server Response failed");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Server: Exception occured inside sendResponse for get request. ";
                }

            } else if (requestType == RequestType.PUT) {
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();
                try {
                    if (incomingRepresentation.getValue(KEY_TEMPERATURE) == null
                            || incomingRepresentation
                                    .getValue(KEY_HOUR) == null) {
                        s_mIsServerOk = false;
                        s_mErrorMsg = s_mErrorMsg
                                + "Representation in put does not have temperature or hour attribute. ";
                        return;
                    }
                } catch (OcException e1) {
                    IoTivityLog.i(LOG_TAG, "Exception occured inside getValue");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Exception occured inside getValue. ";
                    e1.printStackTrace();
                    return;
                }

                try {
                    m_temp = incomingRepresentation.getValue(KEY_TEMPERATURE);
                    m_hour = incomingRepresentation.getValue(KEY_HOUR);
                } catch (OcException e1) {
                    e1.printStackTrace();
                    return;
                }

                setTemperatureRep();
                response.setResourceRepresentation(m_representation);

                try {
                    OcPlatform.sendResponse(response);
                    s_mIsServerOk = true;
                    IoTivityLog.i(LOG_TAG,
                            "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();
                    IoTivityLog.i(LOG_TAG, "Server Response failed");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Server: Exception occured inside sendResponse for put request. ";
                }
            } else if (requestType == RequestType.POST) {
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();
                try {
                    if (incomingRepresentation.getValue(KEY_TEMPERATURE) == null
                            || incomingRepresentation
                                    .getValue(KEY_HOUR) == null) {
                        s_mIsServerOk = false;
                        s_mErrorMsg = s_mErrorMsg
                                + "Representation in post does not have temperature or hour attribute. ";
                        return;
                    }
                } catch (OcException e1) {
                    IoTivityLog.i(LOG_TAG, "Exception occured inside getValue");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Exception occured inside getValue. ";
                    e1.printStackTrace();
                    return;
                }

                try {
                    m_temp = incomingRepresentation.getValue(KEY_TEMPERATURE);
                    m_hour = incomingRepresentation.getValue(KEY_HOUR);
                } catch (OcException e1) {
                    e1.printStackTrace();
                    return;
                }

                setTemperatureRep();
                response.setResourceRepresentation(m_representation);

                try {
                    OcPlatform.sendResponse(response);
                    s_mIsServerOk = true;
                    IoTivityLog.i(LOG_TAG,
                            "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();
                    IoTivityLog.i(LOG_TAG, "Server Response failed");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Server: Exception occured inside sendResponse for post request. ";
                }
            } else if (requestType == RequestType.DELETE) {

                response.setResponseResult(
                        EntityHandlerResult.RESOURCE_DELETED);
                try {
                    OcPlatform.sendResponse(response);
                    s_mIsServerOk = true;
                    IoTivityLog.i(LOG_TAG,
                            "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();
                    IoTivityLog.i(LOG_TAG, "Server Response failed");
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Server: Exception occured inside sendResponse for delete request. ";
                    return;
                }
                try {
                    OcPlatform.unregisterResource(m_resourceHandle);
                    IoTivityLog.i(LOG_TAG,
                            "\tResource unregistered successfully");
                } catch (OcException e) {
                    e.printStackTrace();
                    s_mIsServerOk = false;
                    s_mErrorMsg = s_mErrorMsg
                            + "Server: Exception occured inside unregisterResource for delete request. ";
                }
            }
        }
    }

    /**
     * Entity Handler for resource register
     */
    OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

        @Override
        public EntityHandlerResult handleEntity(
                final OcResourceRequest request) {
            EntityHandlerResult result = EntityHandlerResult.OK;

            IoTivityLog.i(LOG_TAG, "In Server entity handler:");

            if (request != null) {
                handleResponse(request);
            } else {
                IoTivityLog.i(LOG_TAG, "In Server entity handler:");
            }
            return result;
        }
    };

    /**
     * @return OcResourceHandle
     * @brief Function is for registering resource
     */
    public OcResourceHandle registerResource(String resourceUri,
            String resourceType) {
        configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(resourceUri,
                    resourceType, OcPlatform.DEFAULT_INTERFACE, entityHandler,
                    m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            IoTivityLog.i(LOG_TAG, "Exception occured inside registerResource");
        }
        return m_resourceHandle;
    }

    /**
     * @return OcResource
     * @brief Function is for constructing resource object
     */
    public OcResource constructResource() {
        configClientServerPlatform();

        List<String> resourceTypeList = new ArrayList<String>();
        List<String> interfaceList = new ArrayList<String>();

        resourceTypeList.add(RESOURCE_TYPE_TEMPERATURE);
        interfaceList.add(OcPlatform.DEFAULT_INTERFACE);
        OcResource resource = null;
        try {
            resource = OcPlatform.constructResourceObject(HOST,
                    RESOURCE_URI_TEMPERATURE,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), true,
                    resourceTypeList, interfaceList);

        } catch (Exception e) {
            e.printStackTrace();
        }
        return resource;
    }

    /**
     * @brief function for waiting
     * @param[in] second - seconds to wait
     */
    public void waitInSecond(int second) {
        try {
            Thread.sleep(second * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
