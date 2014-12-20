//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package OC.Cm;

import java.util.HashMap;
import java.util.Map;

import OC.Cm.Notification.INotificationListener;

/**
 * This class provides methods to instantiate interfaces using which other SHP
 * functionalities can be accessed
 * 
 */
public class ControlManager {

    /**
     * This method loads the native libraries required for running the SHP
     * Framework from Java environment
     * 
     * @param useSqliteDB
     *            determines if SqliteDb library should be used for database
     *            implementation
     * @param useFileDB
     *            determines if File based Database library should be used for
     *            database implementation
     * @param useRemoteConnector
     *            determines if Remote Connector should be supported
     */
    public static void loadNativeLibaries() {
        try {
            System.loadLibrary("gnustl_shared");
            System.loadLibrary("coap");
            System.loadLibrary("octbstack");
            System.loadLibrary("oc_logger");
            System.loadLibrary("oc");
            System.loadLibrary("cmjni");
        } catch (UnsatisfiedLinkError e) {

        }
    }

    private static Map<String, OC.Cm.ControlManager> nativeHandlers            = new HashMap<String, OC.Cm.ControlManager>();
    private long                                     nativeControlManager;
    private long                                     nativeNotificationManager = 0;

    private static native long constructNative();

    private static native void deleteNative(long cmInstance);

    private native boolean start(long cmInstance, long myDevice)
            throws Exception;

    private native boolean stop(long cmInstance, boolean isInternalStop)
            throws Exception;

    private native boolean isStarted(long cmInstance) throws Exception;

    // private native boolean isSCSConnected( long cmInstance ) throws
    // Exception;
    private native OC.Cm.CMStates getCMState(long cmInstance) throws Exception;

    // private native boolean getSimulatorFlag(long cmInstance) throws
    // Exception;
    // private native void setSimulatorFlag(long cmInstance, boolean flag)
    // throws Exception;

    // private native Sec.Cm.CMModes getCMMode(long cmInstance) throws
    // Exception;
    // private native boolean setCMMode(long cmInstance, int mode, String
    // changedIP, boolean isEasySetupThroughDeviceDiscovery, long peerDevice)
    // throws Exception;

    private native boolean setConfiguration(long cmInstance, long configHandle)
            throws Exception;

    private native long getConfiguration(long cmInstance) throws Exception;

    private native int getTimer(long cmInstance, int timerType)
            throws Exception;

    private native void setTimer(long cmInstance, int timerType, int value)
            throws Exception;

    private native long setCMListener(long cmInstance,
            OC.Cm.ICMListener listener) throws Exception;

    private native void removeCMListener(long cmInstance, long listener)
            throws Exception;

    private native long getDeviceFinder(long cmInstance) throws Exception;

    // private native long getSubscriptionManager(long cmInstance) throws
    // Exception;
    private native boolean validateConfiguration(long cmInstance,
            OC.Cm.Configuration pConfig) throws Exception;

    private native boolean validateMyDevice(long cmInstance,
            OC.Cm.Device pDevice) throws Exception;

    // private native boolean configureSqliteSubscriptionManager(long
    // cmInstance);
    // private native boolean configureFileBasedSubscriptionManager(long
    // cmInstance);
    private native boolean setCMLogLevel(long cmInstance, int shpLogType)
            throws Exception;

    // private native boolean deviceUpdateToSHS( long cmInstance, long myDevice
    // ) throws Exception;

    private native static void configureClassLoader(ClassLoader loader);

    private native long getContext(long cmInstance);

    private native long addNotificationListener(long cmInstance,
            OC.Cm.Notification.INotificationListener notificationListener)
            throws Exception;

    private native void removeNotificationListener(long cmInstance,
            long listener) throws Exception;

    OC.Cm.DeviceFinder.IDeviceFinder        mDeviceFinder        = null;

    OC.Cm.Notification.ISubscriptionManager mSubscriptionManager = null;

    private Map<Long, OC.Cm.ICMListener>    nativeListenersMap   = new HashMap<Long, ICMListener>();

    public synchronized static ControlManager getInstance(String name) {
        ControlManager instance = nativeHandlers.get(name);
        if (null == instance) {
            long handle = constructNative();
            instance = new ControlManager(handle);;

            nativeHandlers.put(name, instance);
        }

        return instance;
    }

    public synchronized static String getContextName(long context) {
        for (String contextName : nativeHandlers.keySet()) {
            if (nativeHandlers.get(contextName).getContext() == context) {
                return contextName;
            }
        }

        return null;
    }

    public synchronized static void destroyInstance(String name) {
        ControlManager instance = nativeHandlers.get(name);

        if (instance != null) {
            nativeHandlers.remove(name);
            deleteNative(instance.nativeControlManager);

            instance.mDeviceFinder = null;
            instance.mSubscriptionManager = null;
        }
    }

    public boolean start(OC.Cm.Device myDevice) throws Exception {
        return start(nativeControlManager, myDevice.getNativeHandle());
    }

    public boolean stop() throws Exception {
        return stop(nativeControlManager, false);
    }

    public boolean stop(boolean isInternalStop) throws Exception {
        return stop(nativeControlManager, isInternalStop);
    }

    public boolean isStarted() throws Exception {
        return isStarted(nativeControlManager);
    }

    /*
     * public boolean isSCSConnected() throws Exception { return
     * isSCSConnected(nativeControlManager); }
     */
    public OC.Cm.CMStates getCMState() throws Exception {
        return getCMState(nativeControlManager);
    }

    public OC.Cm.Configuration getConfiguration() throws Exception {
        if (null == m_configuration) {
            m_configuration = new Configuration();
            setConfiguration(m_configuration);
        }

        return m_configuration;
    }

    public boolean setConfiguration(OC.Cm.Configuration config)
            throws Exception {
        m_configuration = config;
        return setConfiguration(nativeControlManager, config.nativeHandle);
    }

    public void setCMListener(OC.Cm.ICMListener listener) throws Exception {
        long nativeListenr = this.setCMListener(nativeControlManager, listener);
        nativeListenersMap.put(nativeListenr, listener);
    }

    public void removeCMListener(OC.Cm.ICMListener listener) throws Exception {
        if (null != listener) {
            long keyFound = 0;

            for (Long key : nativeListenersMap.keySet()) {
                OC.Cm.ICMListener liste = nativeListenersMap.get(key);

                if (liste == listener) {
                    keyFound = key;
                    break;
                }
            }

            if (keyFound != 0) {
                this.removeCMListener(nativeControlManager, keyFound);
                nativeListenersMap.remove(keyFound);
            }
        }
    }

    public OC.Cm.DeviceFinder.IDeviceFinder getDeviceFinder() throws Exception {
        if (null == mDeviceFinder) {
            long handle = this.getDeviceFinder(nativeControlManager);

            mDeviceFinder = new OC.Cm.DeviceFinder.IDeviceFinder(handle);
        }

        return mDeviceFinder;
    }

    /*
     * public OC.Cm.Notification.ISubscriptionManager getSubscriptionManager()
     * throws Exception { if (null == mSubscriptionManager) { long handle =
     * this.getSubscriptionManager(nativeControlManager) ;
     * 
     * mSubscriptionManager = new
     * OC.Cm.Notification.ISubscriptionManager(handle); }
     * 
     * return mSubscriptionManager; }
     */

    private ControlManager(long handle) {
        nativeControlManager = handle;
    }

    /**
     * Returns context of the current framework instance
     * 
     * @return context
     */
    public long getContext() {
        return this.getContext(nativeControlManager);
    }

    public static void configCurrentClassLoader(ClassLoader loader) {
        configureClassLoader(loader);
    }

    /**
     * Returns mode of SHP
     * 
     * @return SHPModes
     */
    /*
     * public Sec.Cm.CMModes getCMMode() throws Exception { return
     * getCMMode(nativeControlManager); }
     */

    /**
     * Set SHP mode
     * 
     * @param mode
     *            SHP mode
     * 
     * @param peerDevice
     *            Peer Device Information object
     */
    /*
     * public boolean setSHPMode(int mode, String changedIP, boolean
     * isEasySetupThroughDeviceDiscovery, Sec.Cm.Device peerDevice) throws
     * Exception { return this.setSHPMode(nativeControlManager, mode, changedIP,
     * isEasySetupThroughDeviceDiscovery, peerDevice.getNativeHandle()); }
     * 
     * public void setSHPMode(int mode, String changedIP) throws Exception {
     * this.setSHPMode(nativeControlManager, mode, changedIP, true, 0); }
     */

    /*
     * public void configureSqliteSubscriptionManager() {
     * this.configureSqliteSubscriptionManager(nativeControlManager); }
     * 
     * public void configureFileBasedSubscriptionManager() {
     * this.configureFileBasedSubscriptionManager(nativeControlManager); }
     * 
     * public boolean getSimulatorFlag() throws Exception { return
     * getSimulatorFlag(nativeControlManager); }
     * 
     * public void setSimulatorFlag(boolean flag) throws Exception {
     * setSimulatorFlag(nativeControlManager, flag); }
     */

    /**
     * Returns set Timer Value
     * 
     * @return value int
     */
    public int getTimer(OC.Cm.TimerType timerType) throws Exception {
        return getTimer(nativeControlManager, timerType.getValue());
    }

    /**
     * Sets Timer Value
     * 
     * @param timerType
     *            Type of Timer
     * 
     * @param value
     *            int value
     */
    public void setTimer(OC.Cm.TimerType timerType, int value) throws Exception {
        this.setTimer(nativeControlManager, timerType.getValue(), value);
    }

    /**
     * Sending Device Update request to SHS Server
     * 
     * @param device
     *            SHP Device
     */
    /*
     * public boolean deviceUpdateToSHS(Sec.Cm.Device myDevice) throws Exception
     * { return deviceUpdateToSHS(nativeControlManager,
     * myDevice.getNativeHandle()); }
     */

    public boolean setCMLogLevel(OC.Cm.CMLogType shpLogType) throws Exception {
        return this.setCMLogLevel(nativeControlManager, shpLogType.getValue());
    }

    private OC.Cm.Configuration m_configuration;

    /**
     * Add Notification Listener
     * 
     * @param cmNotificationListener
     */
    public void addNotificationListener(
            INotificationListener cmNotificationListener) {

        if (cmNotificationListener != null) {

            try {
                nativeNotificationManager = this.addNotificationListener(
                        nativeControlManager, cmNotificationListener);

            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                nativeNotificationManager = 0;
            }
        }

    }

    /**
     * To remove the NotificationListener
     */
    public void removeNotificationListener() {

        if (nativeNotificationManager != 0) {
            try {
                this.removeNotificationListener(nativeControlManager,
                        nativeNotificationManager);
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            nativeNotificationManager = 0;
        }
    }

}
