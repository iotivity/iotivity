//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/// @file Activator.java

package oic.plugin.hue;

import java.util.EnumSet;
import java.util.List;

import oic.plugin.hue.AccessPointListAdapter;
import oic.plugin.hue.HueSharedPreferences;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.philips.lighting.hue.sdk.PHAccessPoint;
import com.philips.lighting.hue.sdk.PHBridgeSearchManager;
import com.philips.lighting.hue.sdk.PHHueSDK;
import com.philips.lighting.hue.sdk.PHSDKListener;
import com.philips.lighting.model.PHBridge;
import com.philips.lighting.model.PHHueError;
import com.philips.lighting.model.PHHueParsingError;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class Activator extends Activity implements BundleActivator {

    private static BundleContext   context;
    private static Context         AppContext;
    private PHHueSDK               phHueSDK;
    public static final String     TAG = "Hue Plugin";
    private HueSharedPreferences   prefs;
    private AccessPointListAdapter adapter;

    class Light {
        public String m_power;
        public int    m_brightness;
        public int    m_color;
        public String m_name;

        public Light() {
            m_power = "off";
            m_brightness = 0;
            m_color = 0;
            m_name = "device.light";
        }
    }

    static int                     OBSERVE_TYPE_TO_USE = ObserveType.OBSERVE
                                                               .getValue();

    public static Light            myLight;
    public static OcResource       curResource;
    public static OcResourceHandle resourceHandle;
    static int                     oc                  = 0;
    static Activity                mActivity;

    static BundleContext getContext() {
        return context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = this;
    }

    public void start(BundleContext bundleContext) throws Exception {
        Activator.context = bundleContext;

        ServiceReference<Context> ref = Activator.context
                .getServiceReference(Context.class);
        AppContext = Activator.context.getService(ref);

        // Gets an instance of the Hue SDK.
        phHueSDK = PHHueSDK.getInstance();

        // Set the Device Name (name of your app). This will be stored in your
        // bridge whitelist entry.
        phHueSDK.setAppName("QuickStartApp");
        phHueSDK.setDeviceName(android.os.Build.MODEL);

        // Register the PHSDKListener to receive callbacks from the bridge.
        phHueSDK.getNotificationManager().registerSDKListener(listener);

        adapter = new AccessPointListAdapter(AppContext,
                phHueSDK.getAccessPointsFound());

        // Try to automatically connect to the last known bridge. For first time
        // use this will be empty so a bridge search is automatically started.
        prefs = HueSharedPreferences.getInstance(AppContext);
        String lastIpAddress = prefs.getLastConnectedIPAddress();
        String lastUsername = prefs.getUsername();

        // Automatically try to connect to the last connected IP Address. For
        // multiple bridge support a different implementation is required.
        if (lastIpAddress != null && !lastIpAddress.equals("")) {
            PHAccessPoint lastAccessPoint = new PHAccessPoint();
            lastAccessPoint.setIpAddress(lastIpAddress);
            lastAccessPoint.setUsername(lastUsername);

            if (!phHueSDK.isAccessPointConnected(lastAccessPoint)) {
                phHueSDK.connect(lastAccessPoint);
            }
        } else { // First time use, so perform a bridge search.
            doBridgeSearch();
            Log.w(TAG, "Searching for Bridges.");
        }

        myLight = new Light();
    }

    public void stop(BundleContext bundleContext) throws Exception {
        OcPlatform.unregisterResource(resourceHandle);
        context = null;
        myLight = null;
        // phHueSDK = null;
    }

    public void doBridgeSearch() {
        PHBridgeSearchManager sm = (PHBridgeSearchManager) phHueSDK
                .getSDKService(PHHueSDK.SEARCH_BRIDGE);
        // Start the UPNP Searching of local bridges.
        sm.search(true, true);
    }

    private PHSDKListener listener = new PHSDKListener() {

                                       @Override
                                       public void onAccessPointsFound(
                                               List<PHAccessPoint> accessPoint) {
                                           Log.w(TAG, "Access Points Found. "
                                                   + accessPoint.size());
                                           if (accessPoint != null
                                                   && accessPoint.size() > 0) {
                                               phHueSDK.getAccessPointsFound()
                                                       .clear();
                                               phHueSDK.getAccessPointsFound()
                                                       .addAll(accessPoint);
                                               runOnUiThread(new Runnable() {
                                                   @Override
                                                   public void run() {
                                                       adapter.updateData(phHueSDK
                                                               .getAccessPointsFound());
                                                   }
                                               });

                                               HueSharedPreferences prefs2 = HueSharedPreferences
                                                       .getInstance(AppContext);
                                               PHAccessPoint accessPoint2 = (PHAccessPoint) adapter
                                                       .getItem(0);
                                               accessPoint2.setUsername(prefs2
                                                       .getUsername());

                                               PHBridge connectedBridge = phHueSDK
                                                       .getSelectedBridge();

                                               if (connectedBridge != null) {
                                                   String connectedIP = connectedBridge
                                                           .getResourceCache()
                                                           .getBridgeConfiguration()
                                                           .getIpAddress();
                                                   Log.w(TAG, "Connected IP: "
                                                           + connectedIP);
                                                   if (connectedIP != null) { // We
                                                                              // are
                                                                              // already
                                                                              // connected
                                                                              // here:-
                                                       Log.w(TAG,
                                                               "Connected IP != null");
                                                       phHueSDK.disableHeartbeat(connectedBridge);
                                                       phHueSDK.disconnect(connectedBridge);
                                                   }
                                               }
                                               phHueSDK.connect(accessPoint2);
                                           }
                                       }

                                       @Override
                                       public void onCacheUpdated(
                                               List<Integer> arg0,
                                               PHBridge bridge) {
                                           Log.w(TAG, "On CacheUpdated.");
                                       }

                                       @Override
                                       public void onBridgeConnected(PHBridge b) {
                                           Log.w(TAG,
                                                   "Connected to the Bridge.");
                                           phHueSDK.setSelectedBridge(b);
                                           phHueSDK.enableHeartbeat(b,
                                                   PHHueSDK.HB_INTERVAL);
                                           phHueSDK.getLastHeartbeat()
                                                   .put(b.getResourceCache()
                                                           .getBridgeConfiguration()
                                                           .getIpAddress(),
                                                           System.currentTimeMillis());

                                           PlatformConfig cfg = new PlatformConfig(
                                                   getBaseContext(),
                                                   ServiceType.IN_PROC,
                                                   ModeType.CLIENT_SERVER,
                                                   "0.0.0.0", 0,
                                                   QualityOfService.LOW);

                                           OcPlatform.Configure(cfg);

                                           EntityHandlerHue entitycb = new EntityHandlerHue();
                                           try {
                                               resourceHandle = OcPlatform
                                                       .registerResource(
                                                               "/a/huebulb",
                                                               "device.light",
                                                               OcPlatform.DEFAULT_INTERFACE,
                                                               entitycb,
                                                               EnumSet.of(ResourceProperty.DISCOVERABLE));
                                           } catch (OcException e) {
                                               e.printStackTrace();
                                           }
                                       }

                                       @Override
                                       public void onAuthenticationRequired(
                                               PHAccessPoint accessPoint) {
                                           Log.w(TAG,
                                                   "Authentication Required.");
                                           phHueSDK.startPushlinkAuthentication(accessPoint);
                                           Log.w(TAG, "Access Point IP addr: "
                                                   + accessPoint.getIpAddress());
                                           Log.w(TAG,
                                                   "Access Point MAC addr: "
                                                           + accessPoint
                                                                   .getMacAddress());
                                           Log.w(TAG, "Access Point key: "
                                                   + accessPoint.getUsername());
                                       }

                                       @Override
                                       public void onConnectionResumed(
                                               PHBridge bridge) {
                                           // Log.v(TAG, "onConnectionResumed "
                                           // +
                                           // bridge.getResourceCache().getBridgeConfiguration().getIpAddress());
                                           phHueSDK.getLastHeartbeat()
                                                   .put(bridge
                                                           .getResourceCache()
                                                           .getBridgeConfiguration()
                                                           .getIpAddress(),
                                                           System.currentTimeMillis());
                                           for (int i = 0; i < phHueSDK
                                                   .getDisconnectedAccessPoint()
                                                   .size(); i++) {
                                               if (phHueSDK
                                                       .getDisconnectedAccessPoint()
                                                       .get(i)
                                                       .getIpAddress()
                                                       .equals(bridge
                                                               .getResourceCache()
                                                               .getBridgeConfiguration()
                                                               .getIpAddress())) {
                                                   phHueSDK.getDisconnectedAccessPoint()
                                                           .remove(i);
                                               }
                                           }
                                       }

                                       @Override
                                       public void onConnectionLost(
                                               PHAccessPoint accessPoint) {
                                           Log.v(TAG, "onConnectionLost : "
                                                   + accessPoint.getIpAddress());
                                           if (!phHueSDK
                                                   .getDisconnectedAccessPoint()
                                                   .contains(accessPoint)) {
                                               phHueSDK.getDisconnectedAccessPoint()
                                                       .add(accessPoint);
                                           }
                                       }

                                       @Override
                                       public void onError(int code,
                                               final String message) {
                                           Log.e(TAG, "on Error Called : "
                                                   + code + ":" + message);

                                           if (code == PHHueError.NO_CONNECTION) {
                                               Log.w(TAG, "On No Connection");
                                           } else if (code == PHHueError.AUTHENTICATION_FAILED
                                                   || code == 1158) {

                                           } else if (code == PHHueError.BRIDGE_NOT_RESPONDING) {
                                               Log.w(TAG,
                                                       "Bridge Not Responding . . . ");
                                           }
                                       }

                                       @Override
                                       public void onParsingErrors(
                                               List<PHHueParsingError> parsingErrorsList) {
                                           for (PHHueParsingError parsingError : parsingErrorsList) {
                                               Log.e(TAG,
                                                       "ParsingError : "
                                                               + parsingError
                                                                       .getMessage());
                                           }
                                       }
                                   };
}
