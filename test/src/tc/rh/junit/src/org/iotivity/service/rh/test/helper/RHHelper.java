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

package org.iotivity.service.rh.test.helper;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnDeleteListener;
import org.iotivity.base.OcResource.OnGetListener;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResource.OnPostListener;
import org.iotivity.base.OcResource.OnPutListener;

import android.util.Log;

import org.iotivity.ResourceHosting.ResourceHosting;


public class RHHelper implements OcPlatform.OnResourceFoundListener,
    OnGetListener, OnDeleteListener, OnObserveListener, OnPutListener,
    OnPostListener {
		
  				/************************** Declare class variables *************************/
  	
  public final String                       TAG                                  = "RHTest";
  private ResourceHosting                   m_ResourceHosting;
  public String                             m_IpAddress;
  private static int                        m_FirstResTemp;
  private static int                        m_SecondResTemp;
  private static int                        m_FirstResHum;
  private static int                        m_SecondResHum;

  private boolean                           m_OnPutCalled                        = false;
  private boolean                           m_OnGetCalled                        = false;
  private boolean                           m_OnObserveCalled                    = false;
  private boolean                           m_OnDeleteCalled                     = false;
  private static final String               RH_KEY_HUMIDITY                      = "humidity";
  private static final String               RH_KEY_TEMPERATURE                   = "temperature";
  private static final String               RH_TEMP_SENSOR3_URI                  = "TempHumSensor3";
  private static final String               RH_TEMP_SENSOR_URI                   = "/a/TempHumSensor";
  private static final String               RH_TEMP_SENSOR2_URI                  = "/a/TempHumSensor2";
  public  static final String               RH_NON_HOSTABLE_RESOURCE_URI         = "/a/TempHumSensor3";
  

  public int                                m_PostTemp                           = 0;
  public int                                m_PostHum                            = 0;

  public Boolean                            m_deleteCompleted                    = false;
  public Boolean                            m_PostCompleted                      = false;

  public static OcResource                  m_FirstResource;
  public static OcResource                  m_SecondResource;
  public static OcResource                  m_curResourceNonHostable;
  public OcPlatform.OnResourceFoundListener OnResourceFoundListener;

  public static final int                   OCSTACK_OK                           = 0;
  public static final int                   OCSTACK_ERROR                        = 255;
  public static final int                   RESOURCEHOSTING_DO_NOT_THREADRUNNING = -2;
  public static int 						m_result 							 = 0;
  public static final String 				RESOURCEHOSTING_EMPTY_STRING 		 = "";
  public static final String 				RESOURCEHOSTING_NULL_VALUE 			 = null;
  public static final int                   RH_THREAD_SLEEP_MAX 				 = 1000;
  public static final int                   RH_THREAD_SLEEP_MIN 				 = 100;
  public static final int                   RH_INITIALIZE_INT 				 	 = 0;
  public static final int                   RH_KEY_TEMPERATURE_PUT_VAL 			 = 25;
  public static final int                   RH_KEY_HUMIDITY_PUT_VAL 			 = 0;
  public static final int                   RH_LOOP_COUNT_VAL		 			 = 5;
  public static final String  				RH_FIND_RESOURCE_QUERY  			 = "/oic/res?rt=oic.r.resourcehosting";
  public static final int                   RH_MAX_WAIT_TIME		 			 = 10;
  public static final int                   RH_MIN_WAIT_TIME		 			 = 5;
  
  
  public RHHelper(){
	  m_ResourceHosting = new ResourceHosting();
  }
  	  
   /**
   * @brief   	This function is used to initialize all resources 
   * @param 	none
   * @return	OC_STACK_OK upon success otherwise OC_STACK_ERROR
   */
  public int initOICStack() 
  {
	    m_result = OCSTACK_ERROR;
	    
	    try 
	    {
	      m_IpAddress = getIpAddress();
	      m_result = m_ResourceHosting.ResourceHostingInit(m_IpAddress);
	    } 
	    catch (Exception e) 
	    {
	      System.out.println("Failed to init Resource Hosting: " + e.getLocalizedMessage());
	      e.printStackTrace();
	      return m_result;
	    }
	    
	    return m_result;
  }
  
  
  /**
   * @brief   	This function is used to get local ip address of the test devices
   * @param 	none
   * @return	localIpAddress on success
   */
  public String getIpAddress()
  {
	  try 
	  {
		  for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) 
		  {
			  NetworkInterface intf = en.nextElement();
	          for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();) 
	          {
	        	  InetAddress inetAddress = enumIpAddr.nextElement();
	        	  if (!inetAddress.isLoopbackAddress()) 
	        	  {
	        	  	return inetAddress.getHostAddress().toString();
	        	  }
	          }
	      }
	   } 
	  catch (Exception ex) 
	  {
		  System.out.println("Failed to get local ip address: " + ex.getLocalizedMessage());
		  ex.printStackTrace();
	  }
	  
	  return null;
	}

  public void initiateData() {
    m_FirstResTemp = RH_INITIALIZE_INT;
    m_SecondResTemp = RH_INITIALIZE_INT;
    m_FirstResHum = RH_INITIALIZE_INT;
    m_SecondResHum = RH_INITIALIZE_INT;
  }


   /**
   * @brief   	This function will start the resource hosting and the discovery for remote resource which want to be hosted
   * @param 	none
   * @return	OC_STACK_OK upon success otherwise OC_STACK_ERROR
   */
  public int coordinatorStart() 
  {
	  m_result = OCSTACK_ERROR;
    
	  try 
	  {
		  m_result = m_ResourceHosting.OICCoordinatorStart();
	  } 
	  catch (Exception e) 
	  {
		  System.out.println("Failed to Start OIC Coordinator: " + e.getLocalizedMessage());
		  return m_result;
	  }
    
	  return m_result;
  }
  
  public Boolean coordinatorStartMultipleTimes(StringBuilder errMsg) {
    int result = RH_INITIALIZE_INT;

    try {
      result = m_ResourceHosting.OICCoordinatorStart();
    } catch (Exception e) {
      errMsg.append("Failed to Start OIC Coordinator. "
          + e.getLocalizedMessage());
      return false;
    }

    if (result != RESOURCEHOSTING_DO_NOT_THREADRUNNING) {
      errMsg
          .append("Start OIC Coordinator. Returned " + String.valueOf(result)
              + " Expected "
              + String.valueOf(RESOURCEHOSTING_DO_NOT_THREADRUNNING));
      return false;
    }

    return true;
  }


  /**
   * @brief   	This function will stop the resource hosting and delete all hosting resource
   * @param 	none
   * @return	OC_STACK_OK upon success otherwise OC_STACK_ERROR
   */
  public int coordinatorStop() 
  {
	  m_result = OCSTACK_ERROR;
    
	  try 
	  {
		  m_result = m_ResourceHosting.OICCoordinatorStop();
	  } 
	  catch (Exception e) 
	  {
		  System.out.println("Failed to Stop OIC Coordinator: " + e.getLocalizedMessage());
		  return m_result;
	  }
    
      return m_result;
  }
  
  public Boolean coordinatorStopMultipleTimes(StringBuilder errMsg) {
    int result = RH_INITIALIZE_INT;

    try {
      result = m_ResourceHosting.OICCoordinatorStop();
    } catch (Exception e) {
      errMsg
          .append("Failed to Stop OIC Coordinator. " + e.getLocalizedMessage());
      return false;
    }

    if (result != RESOURCEHOSTING_DO_NOT_THREADRUNNING) {
      errMsg
          .append("Stop OIC Coordinator. Returned " + String.valueOf(result)
              + " Expected "
              + String.valueOf(RESOURCEHOSTING_DO_NOT_THREADRUNNING));
      return false;
    }

    return true;
  }


  /**
  * @brief   	This function is used to de-initialized all resources that created due to init
  * @param 		none
  * @return		OC_STACK_OK upon success otherwise OC_STACK_ERROR
  */
  public int resourceHostingTerminate() 
  {
	  m_result = OCSTACK_ERROR;
    
	  try 
	  {
		  m_result = m_ResourceHosting.ResourceHostingTerminate();
	  } 
	  catch (Exception e) 
	  {
		  System.out.println("Failed to Stop OIC Coordinator: " + e.getLocalizedMessage());
		  return m_result;
	  }
    
	  return m_result;
  }  
  
  @Override
  public void onResourceFound(OcResource resource) {
    Log.d(TAG, "===== onResourceFound called =====");
		
	
    String resourceURI;
    String hostAddress;

    if (resource != null) {
      Log.i(TAG, "==============================");
      Log.i(TAG, "DISCOVERED Resource(Consumer): ");

      resourceURI = resource.getUri();
      hostAddress = resource.getHost();

      Log.i(TAG, "Discovered Resource URI: " + resourceURI + " Host: "
          + hostAddress);

      if (resource.getUri().contains(RH_TEMP_SENSOR2_URI)) {
        Log.i(TAG, "****** Found Second Resource ******");
        m_SecondResource = resource;
      } else if (resource.getUri().contains(RH_TEMP_SENSOR3_URI)) {
        Log.i(TAG, "****** Found Third Resource ******");

        m_curResourceNonHostable = resource;
      } else if (resource.getUri().contains(RH_TEMP_SENSOR_URI)) {
        Log.i(TAG, "****** Found First Resource ******");

        m_FirstResource = resource;
      }
    } else {
      Log.d(TAG, "Resource is invalid");
    }
  }

  @Override
  public void onGetCompleted(List<OcHeaderOption> options, OcRepresentation rep) {
    Log.i(TAG, "===== onGetCompleted called =====");

    Log.i(TAG, "GET request was successful\n" + "URI: " + rep.getUri() + "\n"
        + "Temperature: " + rep.getValueInt(RH_KEY_TEMPERATURE) + "\n"
        + "Humidity: " + rep.getValueInt(RH_KEY_HUMIDITY) + "\n");

    getTempAndHumFromRep(rep);

    m_OnGetCalled = true;
  }

  private void getTempAndHumFromRep(OcRepresentation rep) {
    if (rep.getUri().contains(RH_TEMP_SENSOR2_URI)) {
      m_SecondResTemp = rep.getValueInt(RH_KEY_TEMPERATURE);
      m_SecondResHum = rep.getValueInt(RH_KEY_HUMIDITY);
      Log.i(TAG, "======== onGet Second Resource ========\nTemp: "
          + m_SecondResTemp + " Hum: " + m_SecondResHum);
    } else if (rep.getUri().contains(RH_TEMP_SENSOR3_URI)) {
      Log.d(TAG, "Got resource for " + RH_TEMP_SENSOR3_URI);
    } else if (rep.getUri().contains(RH_TEMP_SENSOR_URI)) {
      m_FirstResTemp = rep.getValueInt(RH_KEY_TEMPERATURE);
      m_FirstResHum = rep.getValueInt(RH_KEY_HUMIDITY);
      Log.i(TAG, "======== onGet FirstResource ========\nTemp: "
          + m_FirstResTemp + " Hum: " + m_FirstResHum);
    }
  }

  @Override
  public void onPutCompleted(List<OcHeaderOption> options, OcRepresentation rep) {
    Log.i(TAG, "===== onPutCompleted called =====");

    int humidity = rep.getValueInt(RH_KEY_HUMIDITY);
    int temperature = rep.getValueInt(RH_KEY_TEMPERATURE);

    Log.i(TAG, "Put successful for temperature: " + temperature + " humidity: "
        + humidity);

    m_OnPutCalled = true;
  }

  @Override
  public void onPostCompleted(List<OcHeaderOption> headerOptionList,
      OcRepresentation rep) {
    Log.i(TAG, "===== onPostCompleted called =====");
    m_PostCompleted = true;

    int humidity = rep.getValueInt(RH_KEY_HUMIDITY);
    int temperature = rep.getValueInt(RH_KEY_TEMPERATURE);

    Log.i(TAG, "Post successful for temperature: " + temperature + "humidity: "
        + humidity);
  }

  @Override
  public void onPostFailed(Throwable arg0) {
    Log.i(TAG, "===== onPostFailed called =====");

    m_PostCompleted = false;
  }

  @Override
  public void onObserveCompleted(List<OcHeaderOption> options,
      OcRepresentation rep, int seqNum) {
    Log.i(TAG, "===== onObserveCompleted called =====");

    Log.i(TAG, "========================================================\n"
        + "Receive OBSERVE RESULT:\n" + "URI: " + rep.getUri() + "\n"
        + "SequenceNumber: " + String.valueOf(seqNum) + "\n" + "Temperature: "
        + String.valueOf(rep.getValueInt(RH_KEY_TEMPERATURE)) + "\n"
        + "Humidity: " + String.valueOf(rep.getValueInt(RH_KEY_HUMIDITY)) + "\n");

    getTempAndHumFromRep(rep);

    m_OnObserveCalled = true;
  }

  @Override
  public void onDeleteCompleted(List<OcHeaderOption> options) {
    Log.i(TAG, "===== onDeleteCompleted called =====");

    m_deleteCompleted = true;

    m_OnDeleteCalled = true;
  }

  @Override
  public void onPutFailed(Throwable arg0) {
    Log.i(TAG, "=== Called onPutFailed ===");
  }

  @Override
  public void onObserveFailed(Throwable arg0) {
    Log.i(TAG, "=== Called onObserveFailed ===");
  }

  @Override
  public void onDeleteFailed(Throwable arg0) {
    m_deleteCompleted = false;
    Log.i(TAG, "=== Called onDeleteFailed ===");
  }

  @Override
  public void onGetFailed(Throwable arg0) {
    Log.i(TAG, "=== Called onGetFailed ===");
  }

  // Test as Consumer

  public void waitInSecond(int sec) {
    Log.d(TAG, "Waiting for " + String.valueOf(sec) + " Seconds ...");

    int milSec = sec * RH_THREAD_SLEEP_MAX;

    try {
      Thread.sleep(milSec);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }

    Log.d(TAG, "===== End waiting =====");
  }

  public void waitForResource(int time) {
    Log.d(TAG, "Starts waiting for " + String.valueOf(time) + " Seconds ...");

    int i = RH_INITIALIZE_INT;
    while (i < time) {
      try {
        Thread.sleep(RH_THREAD_SLEEP_MAX);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }

      if (m_FirstResource != null && m_SecondResource != null) {
        break;
      }

      i++;
    }

    Log.d(TAG, "Waited for " + i + " Sec...");

    Log.d(TAG, "===== End waiting =====");
  }

  public void waitForCallback(int time, RequestType type) {
    Log.i(TAG, "Starts waiting for " + String.valueOf(time) + " Seconds ...");

    int i = RH_INITIALIZE_INT;
    while (i < time) {
      try {
        Thread.sleep(RH_THREAD_SLEEP_MAX);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }

      switch (type) {
        case OBSERVE:
          if (m_OnObserveCalled)
            return;
          break;

        case PUT:
          if (m_OnPutCalled)
            return;
          break;

        case GET:
          if (m_OnGetCalled)
            return;
          break;

        case DELETE:
          if (m_OnDeleteCalled)
            return;
          break;

        case POST:
          if (m_PostCompleted)
            return;
          break;

        default:
          break;
      }

      i++;
    }

    Log.d(TAG, "Waited for " + i + " Second");

    Log.i(TAG, "===== End waiting =====");
  }

  public Boolean findResource(String host, String resourceName) {
	
	try 
    {
      EnumSet<OcConnectivityType> propertySet = EnumSet.of(OcConnectivityType.CT_DEFAULT);
      OcPlatform.findResource(host, resourceName, propertySet, this);
    } 
    catch (OcException e) 
    {
    	System.out.println("Can't find Resource: " + e.getMessage());
	    e.printStackTrace();
	    Log.d(TAG, e.getMessage());
	    return false;
    }

    return true;
  }

 
  public Boolean startObserve(OcResource resource) {
	  
	  m_OnObserveCalled = false;

	  if (resource != null) 
	  {
		  Log.i(TAG, "===== startObserve for resource " + resource.getUri() + " =====");

		  Map<String, String> queryParamsMap = new HashMap<String, String>();
		  try 
		  {
			  resource.observe(ObserveType.OBSERVE, queryParamsMap, this);
		  } 
		  catch (OcException e) 
		  {
			  System.out.println(e.getMessage());
			  Log.d(TAG, e.getMessage());
			  return false;
		  }
	  } 
	  else 
	  {
		  System.out.println("Resource is null");
		  Log.d(TAG, "Resource is null");
		  return false;
	  }

	  return true;
  }

  public Boolean startGet(OcResource resource) {
    m_OnGetCalled = false;

    if (resource != null) 
    {
      Log.i(TAG, "**** start Get for resource: " + resource.getUri() + " ****");

      Map<String, String> queryParamsMap = new HashMap<String, String>();
      try 
      {
        resource.get(queryParamsMap, this);
      } 
      catch (OcException e) 
      {
        System.out.println(e.getMessage());
        Log.d(TAG, e.getMessage());
        return false;
      }
    } 
    else 
    {
    	System.out.println("Resource is null");
    	Log.d(TAG, "Resource is null");
    	return false;
    }

    return true;
  }

  public Boolean startPut(OcResource resource) {
    m_OnPutCalled = false;

    if (resource != null) {
      Log.d(TAG, "===== start Put =====");

      m_FirstResource = resource;
      OcRepresentation rep = new OcRepresentation();
      rep.setValueInt(RH_KEY_TEMPERATURE, RH_KEY_TEMPERATURE_PUT_VAL);
      rep.setValueInt(RH_KEY_HUMIDITY, RH_KEY_HUMIDITY_PUT_VAL);
      Map<String, String> queryParamsMap = new HashMap<String, String>();

      try 
      {
    	  resource.put(rep, queryParamsMap, this);
      } 
      catch (OcException e) 
      {
    	  System.out.println(e.getMessage());
    	  Log.d(TAG, e.getMessage());
    	  return false;
      }
    } 
    else 
    {
    	System.out.println("Resource is null");
    	return false;
    }

    return true;
  }

  public Boolean startPost(OcResource resource) {
    m_PostCompleted = false;

    if (resource != null) {
      Log.d(TAG, "===== start Post for resourceUri: " + resource.getUri() + " =====");

      OcRepresentation rep = new OcRepresentation();
      rep.setValueInt(RH_KEY_TEMPERATURE, m_PostTemp);
      rep.setValueInt(RH_KEY_HUMIDITY, m_PostHum);
      Map<String, String> queryParamsMap = new HashMap<String, String>();

      try 
      {
        resource.post(rep, queryParamsMap, this);
      } 
      catch (OcException e) 
      {
    	  System.out.println(e.getMessage());
    	  Log.d(TAG, e.getMessage());
    	  return false;
      }
    } 
    else 
    {
    	System.out.println("Resource is null");
    	return false;
    }

    return true;
  }

  public Boolean verifyObserveDataFromFirstSensor(int expectedTemperature,int expectedHumidity) {
    if (m_FirstResTemp != expectedTemperature) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedTemperature) + " : actual = "  + String.valueOf(m_FirstResHum));
    	return false;
    }

    if (m_FirstResHum != expectedHumidity) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedHumidity) + " : actual = " + String.valueOf(m_FirstResHum));
    	return false;
    }

    return true;
  }

  public Boolean verifyObserveDataFromFirstSensor() {
    int expectedTemperature = RH_INITIALIZE_INT;
    int expectedHumidity = RH_INITIALIZE_INT;

    if (m_FirstResHum != expectedTemperature) {
    	System.out.println("TempHumSensor Temperature expected = "+ String.valueOf(expectedTemperature) + " : actual = " + String.valueOf(m_FirstResHum));
      	return false;
    }

    if (m_FirstResHum != expectedHumidity) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedHumidity) + " : actual = " + String.valueOf(m_FirstResHum));
      	return false;
    }

    return true;
  }

  public Boolean verifyObserveDataFromSecondSensor(int expectedTemperature,int expectedHumidity) {
    if (m_SecondResTemp != expectedTemperature) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedTemperature) + " : actual = " + String.valueOf(m_SecondResTemp));
    	return false;
    }

    if (m_SecondResHum != expectedHumidity) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedHumidity) + " : actual = " + String.valueOf(m_SecondResHum));
      	return false;
    }

    return true;
  }

  public Boolean verifyObserveDataFromSecondSensor() {
    int expectedTemperature = RH_INITIALIZE_INT;
    int expectedHumidity = RH_INITIALIZE_INT;

    if (m_SecondResTemp != expectedTemperature) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedTemperature) + " : actual = " + String.valueOf(m_SecondResTemp));
    	return false;
    }

    if (m_SecondResHum != expectedHumidity) {
    	System.out.println("TempHumSensor Temperature expected = " + String.valueOf(expectedHumidity) + " : actual = " + String.valueOf(m_SecondResHum));
    	return false;
    }

    return true;
  }

  public Boolean verifySensor(OcResource resource, String uri) {
    if (resource == null) {
    	System.out.println(uri + " not found");
    	return false;
    }

    if (!resource.getUri().equals(uri)) {
    	System.out.println("Expected URI " + uri + " Actual URI " + resource.getUri());
    	return false;
    }

    return true;
  }

  public Boolean verifyTempHumFirstSensor() {
    
	  if (m_FirstResource == null) {
		  System.out.println("TempHumSensor resource not found..");
		  return false;
	  }

	  if (!m_FirstResource.getUri().contains(RH_TEMP_SENSOR_URI)) {
		  System.out.println("Resource with uri " + RH_TEMP_SENSOR_URI + " not found");
		  return false;
	  }

    return true;
  }

  public Boolean verifyTempHumSecondSensor() {
    if (m_SecondResource == null) {
      System.out.println("TempHumSensor2 resource not found..");
      return false;
    }

    if (!m_SecondResource.getUri().contains(RH_TEMP_SENSOR2_URI)) {
      System.out.println("Resource with uri " + RH_TEMP_SENSOR2_URI + " not found");
      return false;
    }

    return true;
  }

  public enum RequestType {
    OBSERVE, PUT, GET, DELETE, POST
  }
}
