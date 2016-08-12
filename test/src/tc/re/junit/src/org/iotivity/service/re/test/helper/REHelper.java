//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package org.iotivity.service.re.test.helper;

import static org.iotivity.service.client.RcsRemoteResourceObject.OnCacheUpdatedListener;
import static org.iotivity.service.client.RcsRemoteResourceObject.OnStateChangedListener;
import static org.iotivity.service.client.RcsRemoteResourceObject.ResourceState;

import org.iotivity.service.RcsException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.client.RcsAddress;
import org.iotivity.service.client.RcsDiscoveryManager;
import org.iotivity.service.client.RcsDiscoveryManager.OnResourceDiscoveredListener;
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.client.RcsRemoteResourceObject.CacheState;
import org.iotivity.service.client.RcsRemoteResourceObject.OnRemoteAttributesReceivedListener;
import org.iotivity.service.server.RcsGetResponse;
import org.iotivity.service.server.RcsRequest;
import org.iotivity.service.server.RcsResourceObject.OnAttributeUpdatedListener;
import org.iotivity.service.server.RcsSetResponse;
import org.iotivity.service.server.RcsResourceObject.GetRequestHandler;
import org.iotivity.service.server.RcsResourceObject.SetRequestHandler;

import android.util.Log;

import java.util.ArrayList;

public class REHelper {
  private static final String                LOG_TAG                             = "RETest "
                                                                                     + REHelper.class
                                                                                         .getSimpleName();
  static boolean                             isResourceFound                     = false;
  private static final String                DEFAULT_IP_ADDRESS                  = "107.109.214.139";
  private static final int                   MAX_WAIT_TIME                       = 10;

  private RcsDiscoveryManager.DiscoveryTask  mDiscoveryTask;
  public RcsRemoteResourceObject             mResourceObj;
  private ArrayList<RcsRemoteResourceObject> m_ResourceList                      = new ArrayList<RcsRemoteResourceObject>();

  private OnResourceDiscoveredListener       mOnResourceDiscoveredListener       = new OnResourceDiscoveredListener() {

                                                                                   @Override
                                                                                   public void onResourceDiscovered(
                                                                                       RcsRemoteResourceObject foundResource) {
                                                                                     m_ResourceList
                                                                                         .add(foundResource);
                                                                                     mResourceObj = foundResource;
                                                                                     isResourceFound = true;
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "onResourceDiscovered");
                                                                                   }
                                                                                 };

  private OnStateChangedListener             mOnStateChangedListener             = new OnStateChangedListener() {

                                                                                   @Override
                                                                                   public void onStateChanged(
                                                                                       ResourceState resourceState) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "onStateChanged");
                                                                                   }
                                                                                 };

  private OnRemoteAttributesReceivedListener mOnRemoteAttributesReceivedListener = new OnRemoteAttributesReceivedListener() {
                                                                                   @Override
                                                                                   public void onAttributesReceived(
                                                                                       RcsResourceAttributes attrs,
                                                                                       int eCode) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "onAttributesReceived");
                                                                                   }
                                                                                 };

  private OnCacheUpdatedListener             mOnCacheUpdatedListener             = new OnCacheUpdatedListener() {
                                                                                   @Override
                                                                                   public void onCacheUpdated(
                                                                                       RcsResourceAttributes attrs) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "onCacheUpdated");
                                                                                   }
                                                                                 };

  public GetRequestHandler                   mGetRequestHandler                  = new GetRequestHandler() {
                                                                                   @Override
                                                                                   public RcsGetResponse onGetRequested(
                                                                                       RcsRequest request,
                                                                                       RcsResourceAttributes attrs) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "Got a Get request from client, send default response "
                                                                                             + "URI : "
                                                                                             + request
                                                                                                 .getResourceUri());

                                                                                     return RcsGetResponse
                                                                                         .defaultAction();
                                                                                   }
                                                                                 };

  public SetRequestHandler                   mSetRequestHandler                  = new SetRequestHandler() {
                                                                                   @Override
                                                                                   public RcsSetResponse onSetRequested(
                                                                                       RcsRequest request,
                                                                                       RcsResourceAttributes attrs) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "Got a Set request from client, send default response URI: "
                                                                                             + request
                                                                                                 .getResourceUri());

                                                                                     return RcsSetResponse
                                                                                         .defaultAction();
                                                                                   }
                                                                                 };

  public OnAttributeUpdatedListener          mAttributeUpdatedListener           = new OnAttributeUpdatedListener() {

                                                                                   @Override
                                                                                   public void onAttributeUpdated(
                                                                                       RcsValue oldRcsValue,
                                                                                       RcsValue newRcsValue) {
                                                                                     Log.i(
                                                                                         LOG_TAG,
                                                                                         "Inside onAttributeUpdated.");
                                                                                   }
                                                                                 };

  public Boolean disocverResources(StringBuilder logMsg) {
    int waitedTime = 0;

    if (m_ResourceList != null) {
      m_ResourceList.clear();
    }

    try {
      mDiscoveryTask = RcsDiscoveryManager.getInstance().discoverResource(
          RcsAddress.multicast(), mOnResourceDiscoveredListener);

      if (mResourceObj != null) {
        mResourceObj.destroy();
        mResourceObj = null;
      }

      waitedTime = waitForResourceFound(MAX_WAIT_TIME);

      if (waitedTime > MAX_WAIT_TIME) {
        logMsg.append("Waited for " + Integer.valueOf(waitedTime).toString()
            + " sec, but can't got resource. ");
        return false;
      } else {
        logMsg.append("Found resource successfully. ");
      }
    } catch (RcsException e) {
      logMsg.append("Throws exception when called discoverResource API. "
          + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  public void distroyResources() {
    if (mResourceObj != null) {
      mResourceObj.destroy();
      mResourceObj = null;
    }

    if (mDiscoveryTask != null) {
      mDiscoveryTask.cancel();
      mDiscoveryTask = null;
    }
  }

  public static RcsAddress getDefaultRcsAddressForUnicast() {
    return RcsAddress.unicast(DEFAULT_IP_ADDRESS);
  }

  public ArrayList<RcsRemoteResourceObject> getFoundResourceList() {
    return m_ResourceList;
  }

  public Boolean startMonitoring(StringBuilder logMsg) {
    try {
      if (mResourceObj != null) {
        Log.d(LOG_TAG, "Object is not null");
      } else {
        Log.d(LOG_TAG, "Object is null");
      }

      if (mResourceObj.isMonitoring()) {
        mResourceObj.startMonitoring(mOnStateChangedListener);
        logMsg
            .append("Monitoring started successfully when already monitoring started.");
      } else {
        mResourceObj.startMonitoring(mOnStateChangedListener);
        logMsg.append("Monitoring started successfully.");
      }
    } catch (RcsException e) {
      logMsg.append("Can't startMonitoring because of "
          + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  public Boolean stopMonitoring(StringBuilder logMsg) {
    try {
      if (mResourceObj.isMonitoring()) {
        mResourceObj.stopMonitoring();
        logMsg.append("Stopped Resource Monitoring");
      } else {
        mResourceObj.stopMonitoring();
        logMsg.append("Stopped Resource Monitoring without stating.");
      }
    } catch (RcsException e) {
      logMsg.append("Can't stopMonitoring because of "
          + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  public Boolean getRemoteAttributes(Boolean withListener, StringBuilder logMsg) {
    try {
      if (withListener) {
        mResourceObj.getRemoteAttributes(mOnRemoteAttributesReceivedListener);
        logMsg.append("getRemoteAttributes called successfully.");
      } else {
        mResourceObj.getRemoteAttributes(null);
        logMsg
            .append("getRemoteAttributes called successfully without listner");
        return false;
      }
    } catch (NullPointerException ne) {
      if (!withListener) {
        logMsg
            .append("Got NullPointerException for getRemoteAttributes API without callback. Exception: "
                + ne);
      }
    } catch (RcsException e) {
      if (withListener) {
        logMsg
            .append("Throws exception when getRemoteAttributes API called. Exception is: "
                + e.getLocalizedMessage());
        return false;
      }
    }

    return true;
  }

  public Boolean getState(StringBuilder logMsg) {
    ResourceState resourceState;

    try {
      resourceState = mResourceObj.getState();
      logMsg.append("Got state: " + resourceState.toString() + " ");
    } catch (Exception e) {
      logMsg.append("Exception occured when executing getState(). Exception: "
          + e.getLocalizedMessage());
      return false;
    }

    if (resourceState == ResourceState.NONE
        || resourceState == ResourceState.REQUESTED
        || resourceState == ResourceState.ALIVE
        || resourceState == ResourceState.DESTROYED
        || resourceState == ResourceState.LOST_SIGNAL) {
      logMsg.append("ResourceState returned correct state. ");
    } else {
      logMsg.append("ResourceState should return correct state. ");
      return false;
    }

    return true;
  }

  public Boolean setRemoteAttributes(RcsResourceAttributes attribute,
      StringBuilder logMsg, Boolean expected, Boolean withListener) {
    try {
      if (withListener) {
        mResourceObj.setRemoteAttributes(attribute,
            mOnRemoteAttributesReceivedListener);
      } else {
        mResourceObj.setRemoteAttributes(attribute, null);
      }

      logMsg.append("setRemoteAttributes API called successfully. ");

      if (!expected) {
        if (withListener) {
          logMsg
              .append("Although the setRemoteAttributes called with empty attribute.");
        } else {
          logMsg
              .append("Although the setRemoteAttributes called with null listner.");
        }

        return !expected;
      }
    } catch (NullPointerException ne) {
      if (!withListener) {
        logMsg
            .append("Got NullPointerException for setRemoteAttributes API without callback. Exception: "
                + ne);
      } else {
        logMsg
            .append("Got NullPointerException for setRemoteAttributes API with null attribute. Exception: "
                + ne);
      }
    } catch (RcsException e) {
      if (expected) {
        logMsg
            .append("Throws exception when setRemoteAttributes API called. Exception is: "
                + e.getLocalizedMessage());
        return !expected;
      }
    }

    return expected;
  }

  public Boolean startCaching(Boolean withListener, StringBuilder logMsg) {
    String msg = "";

    if (withListener) {
      msg = "with listener";
    } else {
      msg = "without listener";
    }

    try {
      if (mResourceObj.isCaching()) {
        logMsg.append("Caching (" + msg
            + ")started successfully when already monitoring started.");
      } else {
        logMsg.append("Caching (" + msg + ") started successfully.");
      }

      if (withListener) {
        mResourceObj.startCaching(mOnCacheUpdatedListener);
      } else {
        mResourceObj.startCaching();
      }

    } catch (RcsException e) {
      logMsg.append("Can't startMonitoring because of "
          + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  public Boolean getCacheState(StringBuilder logMsg) {
    CacheState cState = null;
    try {
      cState = mResourceObj.getCacheState();
      logMsg.append("Cache State: " + cState.name());
    } catch (RcsException e) {
      logMsg
          .append("Exception occured when executing getCacheState(). Exception: "
              + e.getLocalizedMessage());
    }

    if (cState != CacheState.NONE || cState != CacheState.UNREADY
        || cState != CacheState.READY || cState != CacheState.LOST_SIGNAL) {
      logMsg.append("CacheState should return correct state");
      return false;
    }

    return true;
  }

  public Boolean getCachedAttributes(StringBuilder logMsg) {
    try {
      RcsResourceAttributes attributes = mResourceObj.getCachedAttributes();

      if (attributes.size() <= 0) {
        logMsg
            .append("getCachedAttributes() API returned NULL resource attributes");
        return false;
      }

      logMsg.append("getCachedAttributes: ");

      for (String key : attributes.keySet()) {
        logMsg.append(key + " : " + attributes.get(key));
      }

    } catch (RcsException e) {
      logMsg
          .append("Throws exception when getCachedAttributes API called. Exception is: "
              + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  public Boolean getCachedAttribute(String key, StringBuilder logMsg,
      Boolean expected) {
    try {
      if (mResourceObj != null) {
        RcsValue value = mResourceObj.getCachedAttribute(key);
        Log.d(LOG_TAG, "getCachedAttribute(" + key + "): " + value);

        if (value != null) {
          if (value.asInt() < 0) {
            logMsg
                .append("getCachedAttribute API should return greater than or equal to 0 but actual is: "
                    + value + " ");
            return !expected;
          }
        } else {
          if (expected) {
            logMsg.append("getCachedAttribute API returns null reference");
            return !expected;
          }
        }
      } else {
        logMsg.append("Get Null Resource.");
        return !expected;
      }

    } catch (RcsException e) {
      logMsg
          .append("Throws exception when getCachedAttribute API called with key: "
              + key + ". Exception is: " + e.getLocalizedMessage());
      return !expected;
    }

    return expected;
  }

  public Boolean stopCaching(StringBuilder logMsg) {
    try {
      if (mResourceObj != null) {
        if (mResourceObj.isCaching()) {
          logMsg.append("Caching already started.");
        }

        mResourceObj.stopCaching();

        logMsg.append("Stop cashing successfully.");
      } else {
        logMsg.append("Get Null Resource.");
      }
    } catch (RcsException e) {
      logMsg
          .append("Throws exception when stopCaching API called. Exception is: "
              + e.getLocalizedMessage());
      return false;
    }

    return true;
  }

  private int waitForResourceFound(int timeOut) {
    int progessTime = 0;

    while (!isResourceFound) {
      Log.d(LOG_TAG, "Waiting for Resource " + progessTime);

      try {
        Thread.sleep(1000);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }

      if (++progessTime > timeOut) {
        break;
      }
    }

    isResourceFound = false;

    return progessTime;
  }

  public void waitInSecond(int time) {
    int timeInSec = time * 1000;
    try {
      Thread.sleep(timeInSec);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
  }
}
