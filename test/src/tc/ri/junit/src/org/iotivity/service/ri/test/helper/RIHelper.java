/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.service.ri.test.helper;

import android.content.Context;

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

import android.util.Log;

public class RIHelper {

    private static RIHelper riHelperInstance = null;

    public static final String EMPTY_STRING = "";

    public static final String KEY_PLATFORM_ID = "pi";
    public static final String KEY_MANUFACTURER_NAME = "mnmn";
    public static final String KEY_MANUFACTURER_URL = "mnml";
    public static final String KEY_MODEL_NO = "mnmo";
    public static final String KEY_MANUFACTURE_DATE = "mndt";
    public static final String KEY_PLATFORM_VERSION = "mnpv";
    public static final String KEY_OS_VERSION = "mnos";
    public static final String KEY_HW_VERSION = "mnhw";
    public static final String KEY_FIRMWARE_VERSION = "mnfv";
    public static final String KEY_SUPPORT_URL = "mnsl";
    public static final String KEY_SYSTEM_TIME = "st";
    
    public static final String PLATFORM_ID = "436f6e66-6f72-6d61-6e63-6553696d756c";
    public static final String MANUFACTURER_NAME = "Vendor";
    public static final String MANUFACTURER_URL = "www.default-vendor.com";
    public static final String MODEL_NO = "ABCDE00004";
    public static final String MANUFACTURE_DATE = "2016-06-01";
    public static final String PLATFORM_VERSION = "0.0.1";
    public static final String OS_VERSION = "10";
    public static final String HW_VERSION = "1.1.0";
    public static final String FIRMWARE_VERSION = "1.1.1";
    public static final String SUPPORT_URL = "support.default-vendor.com";
    public static final String SYSTEM_TIME = "2016-06-20T10:10:10Z";

    public static final String KEY_DEVICE_NAME = "n"; 
    public static final String DEVICE_NAME = "IotivitySmartRoom";
    public static final String DEVICE_TYPE_AC = "AirCondition";

    public static final String RESOURCE_URI_TEMPERATURE = "/a/temperature";
    public static final String RESOURCE_TYPE_TEMPERATURE = "oic.r.temperature";
    public static final String RESOURCE_URI_LIGHT = "/a/light";
    public static final String RESOURCE_TYPE_LIGHT = "core.light";
    public static final String RESOURCE_URI_FAN = "/a/fan";
    public static final String RESOURCE_TYPE_FAN = "core.fan";

    public static final String TEMPERATURE_RESOURCE_QUERY = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + RESOURCE_TYPE_TEMPERATURE;
    
    private static final String HOST = "coap://192.168.1.2:5000";

    public final int INT_ZERO = 0;
    public final int INT_ONE = 1;
    public final int INT_TWO = 2;
    public final int INT_MINUS_ONE = -1;

    public final int CALLBACK_WAIT_DEFAULT = 5;
    public final int CALLBACK_WAIT_MAX = 10;
    public final int CALLBACK_WAIT_MIN = 1;
    public final int SUCCESS_RESPONSE = 0;
    
    public static final int     COAP_RESPONSE_CODE_SUCCESS               = 205;
    public static final int     COAP_RESPONSE_CODE_CREATED               = 201;
    public static final int     COAP_RESPONSE_CODE_DELETED               = 202;
    public static final int     COAP_RESPONSE_CODE_UPDATED               = 204;
    public static final int     COAP_RESPONSE_CODE_RETRIEVED             = 205;
    public static final int     COAP_RESPONSE_CODE_ERROR                 = 400;
    public static final int     COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED = 401;
    public static final int     COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND    = 404;
    public static final int     COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED    = 405;
    
    public static final String KEY_TEMPERATURE    = "temperature";
    public static final String KEY_HOUR    = "hour";
        
    private final String LOG_TAG = this.getClass().getSimpleName();    

    private static Context s_helperContext;

    private OcResourceHandle m_resourceHandle = null;
    public EnumSet<ResourceProperty> m_resourceProperty;
    
    private OcRepresentation m_representation = new OcRepresentation();
    
    public int m_temp;
    public int m_hour;
    
    public static boolean s_isServerOk;
    
    public static String s_errorMsg;

    private RIHelper() {
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
    public static RIHelper getInstance(Context context) {
        Lock mutex = new ReentrantLock();

        if (riHelperInstance == null) {
            mutex.lock();
            if (riHelperInstance == null) {
                Log.i("RIHelper", "Inside Helper");
                riHelperInstance = new RIHelper();
            }
            mutex.unlock();
        }

        s_helperContext = context;

        return riHelperInstance;
    }

    /**
     * @brief Function is for configuring platform
     */
    public void configClientServerPlatform() {
        PlatformConfig cfg = new PlatformConfig(s_helperContext, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
        OcPlatform.Configure(cfg);
    }

    /**
     * @return OcPlatformInfo
     * @brief Function is for getting platform information
     */
    public OcPlatformInfo getPlatformInfo() {
        configClientServerPlatform();

        OcPlatformInfo platformInfo = new OcPlatformInfo(PLATFORM_ID, MANUFACTURER_NAME, MANUFACTURER_URL);
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
                Arrays.asList(new String[]{
                        DEVICE_TYPE_AC}));
        return deviceInfo;
    }

    void setTemperatureRep()
    {
    	try {
			m_representation.setValue(RIHelper.KEY_TEMPERATURE, m_temp);
			m_representation.setValue(RIHelper.KEY_HOUR, m_hour);
		} catch (OcException e) {
			Log.i(LOG_TAG, "Exception occured iside setValue");
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
        	Log.i(LOG_TAG, "\t\trequestFlag : Request");
        	Log.i(LOG_TAG, "\t\t\trequestType : " + requestType);

            // If the request type is GET
            if (requestType == RequestType.GET) {                               
                setTemperatureRep();
                response.setResourceRepresentation(m_representation);
                response.setErrorCode(COAP_RESPONSE_CODE_RETRIEVED);
                
                try {
                    OcPlatform.sendResponse(response);
                    s_isServerOk = true;
                    Log.i(LOG_TAG, "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();  
                    Log.i(LOG_TAG, "Server Response failed");
                    s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Server: Exception occured inside sendResponse for get request. ";
                }

            } else if (requestType == RequestType.PUT) {
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();
                response.setErrorCode(COAP_RESPONSE_CODE_CREATED);
                try {
					if(incomingRepresentation.getValue(KEY_TEMPERATURE) == null || 
							incomingRepresentation.getValue(KEY_HOUR) == null)
					{
	                    s_isServerOk = false;
	                    s_errorMsg = s_errorMsg +"Representation in put does not have temperature or hour attribute. ";
	                    return;
					}
				} catch (OcException e1) {
					Log.i(LOG_TAG, "Exception occured inside getValue");
					s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Exception occured inside getValue. ";                    
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
                    s_isServerOk = true;
                    Log.i(LOG_TAG, "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();  
                    Log.i(LOG_TAG, "Server Response failed");
                    s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Server: Exception occured inside sendResponse for put request. ";
                }
            } else if (requestType == RequestType.POST) {
            	OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();
            	response.setErrorCode(COAP_RESPONSE_CODE_UPDATED);
                try {
					if(incomingRepresentation.getValue(KEY_TEMPERATURE) == null || 
							incomingRepresentation.getValue(KEY_HOUR) == null)
					{
	                    s_isServerOk = false;
	                    s_errorMsg = s_errorMsg +"Representation in post does not have temperature or hour attribute. ";
	                    return;
					}
				} catch (OcException e1) {
					Log.i(LOG_TAG, "Exception occured inside getValue");
					s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Exception occured inside getValue. ";                    
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
                    s_isServerOk = true;
                    Log.i(LOG_TAG, "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();  
                    Log.i(LOG_TAG, "Server Response failed");
                    s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Server: Exception occured inside sendResponse for post request. ";
                }
            } else if (requestType == RequestType.DELETE) {          
            	
            	response.setErrorCode(COAP_RESPONSE_CODE_DELETED);
            	response.setResponseResult(EntityHandlerResult.RESOURCE_DELETED);
            	try {
                    OcPlatform.sendResponse(response);
                    s_isServerOk = true;
                    Log.i(LOG_TAG, "\tServer response sent successfully");
                } catch (Exception e) {
                    e.printStackTrace();  
                    Log.i(LOG_TAG, "Server Response failed");
                    s_isServerOk = false;
                    s_errorMsg = s_errorMsg +"Server: Exception occured inside sendResponse for delete request. ";
                    return;
                }
            	try {
					OcPlatform.unregisterResource(m_resourceHandle);
					Log.i(LOG_TAG, "\tResource unregistered successfully");
				} catch (OcException e) {
					e.printStackTrace();
					s_isServerOk = false;
					s_errorMsg = s_errorMsg +"Server: Exception occured inside unregisterResource for delete request. ";
				}
            }
        }
    }

    /**
     * Entity Handler for resource register
     */
    OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

        @Override
        public EntityHandlerResult handleEntity(final OcResourceRequest request) {
            EntityHandlerResult result = EntityHandlerResult.OK;

            Log.i(LOG_TAG, "In Server entity handler:");

            if(request != null)
            {
            	handleResponse(request);
            }
            else
            {
            	Log.i(LOG_TAG, "In Server entity handler:");
            }
            return result;
        }
    };

    /**
     * @return OcResourceHandle
     * @brief Function is for registering resource
     */
    public OcResourceHandle registerResource(String resourceUri, String resourceType) {
        configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(resourceUri, resourceType, OcPlatform.DEFAULT_INTERFACE,
                    entityHandler, m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            Log.i(LOG_TAG, "Exception occured inside registerResource");
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

        resourceTypeList.add(RIHelper.RESOURCE_TYPE_TEMPERATURE);
        interfaceList.add(OcPlatform.DEFAULT_INTERFACE);
        OcResource resource = null;
        try {
            resource = OcPlatform.constructResourceObject(HOST, RIHelper.RESOURCE_URI_TEMPERATURE, EnumSet.of(OcConnectivityType.CT_DEFAULT), true,
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

