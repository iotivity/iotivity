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

package oic.plugin.wemo;

import java.util.ArrayList;
import java.util.EnumSet;

import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.belkin.wemo.localsdk.WeMoDevice;
import com.belkin.wemo.localsdk.WeMoSDKContext;

public class Activator extends Activity implements BundleActivator,
        WeMoSDKContext.NotificationListener {
    final private static String  TAG             = "SimpleClient";

    private static BundleContext context;
    private Context              AppContext;
    public static WeMoSDKContext mWeMoSDKContext = null;
    WeMoDevice                   wemoDevice;

    static class SmartPlug {
        public String m_power;
        public String m_name;

        public SmartPlug() {
            m_power = "";
            m_name = "";
        }
    }

    static int                     OBSERVE_TYPE_TO_USE = ObserveType.OBSERVE
                                                               .getValue();

    public static SmartPlug        mySmartPlug;
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
        /*
         * OSGi bundle context
         */
        context = bundleContext;

        /*
         * Android Application Context
         */
        ServiceReference<Context> ref = context
                .getServiceReference(Context.class);
        this.AppContext = ((Context) context.getService(ref));

        /*
         * WeMo Context
         */
        this.mWeMoSDKContext = new WeMoSDKContext(this.AppContext);
        this.mWeMoSDKContext.addNotificationListener(this);
        refresh();

        mySmartPlug = new SmartPlug();
    }

    public void stop(BundleContext bundleContext) throws Exception {
        OcPlatform.unregisterResource(resourceHandle);
        context = null;
        mWeMoSDKContext = null;
        mySmartPlug = null;

    }

    public void onNotify(final String event, final String udn) {
        runOnUiThread(new Runnable() {
            public void run() {
                Activator.this.wemoDevice = Activator.this.mWeMoSDKContext
                        .getWeMoDeviceByUDN(udn);
                if (event.equals("refresh")) {
                    ArrayList<String> udns = Activator.this.mWeMoSDKContext
                            .getListOfWeMoDevicesOnLAN();
                    ArrayList<WeMoDevice> wemoDevices = new ArrayList();
                    for (String udn : udns) {
                        WeMoDevice listDevice = Activator.this.mWeMoSDKContext
                                .getWeMoDeviceByUDN(udn);
                        if ((listDevice != null) && (listDevice.isAvailable())) {
                            wemoDevices.add(listDevice);
                            Log.i("WeMoPlugin", "WeMo Decive Name: "
                                    + listDevice.getFriendlyName());
                            Log.i("WeMoPlugin", "WeMo Decive UDN: "
                                    + listDevice.getUDN());
                            Log.i("WeMoPlugin", "WeMo Decive Type: "
                                    + listDevice.getType());
                            Log.i("WeMoPlugin", "WeMo Decive Serial #: "
                                    + listDevice.getSerialNumber());
                        }
                    }
                } else if (event.equals(WeMoSDKContext.ADD_DEVICE)) {
                    ArrayList<String> udns = Activator.this.mWeMoSDKContext
                            .getListOfWeMoDevicesOnLAN();
                    ArrayList<WeMoDevice> wemoDevices = new ArrayList();
                    for (String udn : udns) {
                        WeMoDevice listDevice = Activator.this.mWeMoSDKContext
                                .getWeMoDeviceByUDN(udn);
                        if ((listDevice != null) && (listDevice.isAvailable())) {
                            PlatformConfig cfg = new PlatformConfig(
                                    this,
                                    ServiceType.IN_PROC,
                                    ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                                    QualityOfService.LOW);

                            OcPlatform.Configure(cfg);

                            EntityHandlerWemo entitycb = new EntityHandlerWemo();
                            try {
                                resourceHandle = OcPlatform
                                        .registerResource(
                                                "/a/wemo",
                                                "device.smartplug",
                                                "oc.mi.def",
                                                entitycb,
                                                EnumSet.of(ResourceProperty.DISCOVERABLE));
                            } catch (OcException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
            }
        });
    }

    private void refresh() {
        this.mWeMoSDKContext.refreshListOfWeMoDevicesOnLAN();
    }
}
