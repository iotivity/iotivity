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

package oic.plugin.gear.noti;

import java.util.EnumSet;
import java.util.UUID;

import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
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
import android.util.Log;

import com.samsung.android.sdk.SsdkUnsupportedException;
import com.samsung.android.sdk.richnotification.Srn;
import com.samsung.android.sdk.richnotification.SrnRichNotification;
import com.samsung.android.sdk.richnotification.SrnRichNotificationManager;
import com.samsung.android.sdk.richnotification.SrnRichNotificationManager.ErrorType;
import com.samsung.android.sdk.richnotification.SrnRichNotificationManager.EventListener;

public class Activator extends Activity implements BundleActivator,
        EventListener {

    final private static String TAG = "GearNoti";

    public enum TemplateTypes {
        SMALL_HEADER;
    }

    private static BundleContext             context;
    public static Context                    AppContext;
    public static SrnRichNotificationManager mRichNotificationManager;

    class Notify {
        public String m_power;
        public String m_name;

        public Notify() {
            m_power = "";
            m_name = "device.notify";
        }
    }

    static int                     OBSERVE_TYPE_TO_USE = ObserveType.OBSERVE
                                                               .getValue();

    public static Notify           myNotify;
    public static OcResource       curResource;
    public static OcResourceHandle resourceHandle;
    static int                     oc                  = 0;
    static Activity                mActivity;

    static BundleContext getContext() {
        return context;
    }

    public void start(BundleContext bundleContext) throws Exception {
        Activator.context = bundleContext;

        ServiceReference<Context> ref = Activator.context
                .getServiceReference(Context.class);
        AppContext = Activator.context.getService(ref);

        myNotify = new Notify();

        Srn srn = new Srn();
        try {
            // Initializes an instance of Srn.
            srn.initialize(AppContext);
            Log.w(TAG, "SRN initialized");
        } catch (SsdkUnsupportedException e) {
            Log.w(TAG, "SsdkUnsupportedException");
        }

        mRichNotificationManager = new SrnRichNotificationManager(AppContext);

        if (mRichNotificationManager.isConnected()) {
            // Starts the manager.
            mRichNotificationManager.start();
            Log.w(TAG, "RichNoti Manager Started");

            SrnRichNotification myRichNotification = new SrnRichNotification(
                    AppContext);
            Log.w(TAG, "RichNoti instance created");

            PlatformConfig cfg = new PlatformConfig(this, ServiceType.IN_PROC,
                    ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);

            OcPlatform.Configure(cfg);

            EntityHandlerNoti entitycb = new EntityHandlerNoti();

            resourceHandle = OcPlatform.registerResource("/a/galaxy/gear",
                    "device.notify", "oc.mi.def", entitycb,
                    EnumSet.of(ResourceProperty.DISCOVERABLE));
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mRichNotificationManager.registerRichNotificationListener(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mRichNotificationManager.unregisterRichNotificationListener(this);
    }

    @Override
    public void onError(UUID arg0, ErrorType arg1) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onRead(UUID arg0) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onRemoved(UUID arg0) {
        // TODO Auto-generated method stub

    }

    @Override
    public void stop(BundleContext arg0) throws Exception {
        // TODO Auto-generated method stub
        OcPlatform.unregisterResource(resourceHandle);
        context = null;
        mRichNotificationManager = null;
        myNotify = null;

    }

}
