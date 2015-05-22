/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file    ThingsManagerCallback.java
 *
 * @brief    This file provides a class for a set of callback functions for group management,
 *        synchronization of group, configuration of things, and diagnostics
 *        about things.
 *
 */

package org.iotivity.service.tm;

import java.util.Vector;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;

import android.util.Log;

/**
 * This class provides a set of callback functions for group management,
 * synchronization of group, configuration of things, and diagnostics
 * about things.
 *
 */
class ThingsManagerCallback {

    private IFindCandidateResourceListener resourceListener;
    private ISubscribePresenceListener     presenceListener;
    private IFindGroupListener             groupListener;
    private IConfigurationListener         configurationListener;
    private IDiagnosticsListener           diagnosticsListener;
    private IActionListener                actionListener;
    private static ThingsManagerCallback   thingsManagerCallbackInterfaceObj;

    private ThingsManagerCallback() {
    }

    /**
     * Function for Getting instance of ThingsManagerCallback object.
     *
     * @return ThingsManagerCallback instance.
     *
     */
    public static synchronized ThingsManagerCallback getInstance() {
        if (null == thingsManagerCallbackInterfaceObj) {
            thingsManagerCallbackInterfaceObj = new ThingsManagerCallback();
        }
        return thingsManagerCallbackInterfaceObj;
    }

    /**
     * Register listener for findCandidateResource callback.
     *
     * @param listener
     *            interface for getting notification when resources are
     *            discovered in network.
     *
     */
    public void registerFindCandidateResourceListener(
            IFindCandidateResourceListener listener) {
        resourceListener = listener;
    }

    /**
     * Register listener for subscribeCollectionPresence callback.
     *
     * @param listener
     *            interface for getting notification regarding child resource
     *            presence status.
     *
     */
    public void registerSubscribePresenceListener(
            ISubscribePresenceListener listener) {
        presenceListener = listener;
    }

    /**
     * Register listener for find group callback.
     *
     * @param listener
     *            interface for getting notification on whether the group is
     *            found or not.
     *
     */
    public void registerGroupListener(IFindGroupListener listener) {
        groupListener = listener;
    }

    /**
     * Register listener for updateConfigurations and getConfigurations
     * callback.
     *
     * @param listener
     *            interface for getting notification on configuration values
     *            information or when configuration value is updated for
     *            multiple things of a target group or a single thing.
     *
     */
    public void registerConfigurationListener(IConfigurationListener listener) {
        configurationListener = listener;
    }

    /**
     * Register listener for doBootstrap, reboot and factoryReset callbacks.
     *
     * @param listener
     *            interface for receiving asynchronous response for diagnostic
     *            feature APIs.
     *
     */
    public void registerDiagnosticsListener(IDiagnosticsListener listener) {
        diagnosticsListener = listener;
    }

    /**
     * Register listener for getActionSet, executeActionSet and deleteActionSet
     * callback.
     *
     * @param listener
     *            interface for receiving the callback for the GET, PUT and
     *            POST requested actions.
     *
     */
    public void registerActionListener(IActionListener listener) {
        actionListener = listener;
    }

    /**
     * Unregister listener for findCandidateResource callback.
     *
     */
    public void unregisterFindCandidateResourceListener() {
        resourceListener = null;
    }

    /**
     * Unregister listener for subscribeCollectionPresence callback.
     *
     */
    public void unregisterSubscribePresenceListener() {
        presenceListener = null;
    }

    /**
     * Unregister listener for find group callback.
     *
     */
    public void unregisterGroupListener() {
        groupListener = null;
    }

    /**
     * Unregister listener for updateConfigurations and getConfigurations
     * callback.
     *
     */
    public void unregisterConfigurationListener() {
        configurationListener = null;
    }

    /**
     * Unregister listener for doBootstrap, reboot and factoryReset callbacks.
     *
     */
    public void unregisterDiagnosticsListener() {
        diagnosticsListener = null;
    }

    /**
     * Unregister listener for getActionSet, executeActionSet and
     * deleteActionSet callback.
     *
     */
    public void unregisterActionListener() {
        actionListener = null;
    }

    /**
     * This callback method will be called whenever resource is discovered in
     * the network.
     *
     * @param resources
     *            List of resources discovered in the network
     *
     */
    public void onResourceCallback(Vector<OcResource> resources) {
        if (null != resourceListener)
            resourceListener.onResourceCallback(resources);
    }

    /**
     * This callback method is called when a response for the executeActionSet
     * or deleteActionSet request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onPostResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != actionListener)
            actionListener.onPostResponseCallback(headerOptions, rep,
                    errorValue);
    }

    /**
     * This callback method is called when a response for the addActionSet
     * request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onPutResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != actionListener)
            actionListener
                    .onPutResponseCallback(headerOptions, rep, errorValue);
    }

    /**
     * This callback method is called when a response for the getActionSet
     * request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onGetResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != actionListener)
            actionListener
                    .onGetResponseCallback(headerOptions, rep, errorValue);
    }

    /**
     * This callback method is called when a response for the
     * updateConfigurations request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onUpdateConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue) {
        if (null != configurationListener)
            configurationListener.onUpdateConfigurationsCallback(headerOptions,
                    rep, errorValue);
    }

    /**
     * This callback method is called when a response for the getConfigurations
     * request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onGetConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue) {
        if (null != configurationListener)
            configurationListener.onGetConfigurationsCallback(headerOptions,
                    rep, errorValue);
    }

    /**
     * This callback method is called when a response for the doBootstrap
     * request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onBootStrapCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != configurationListener) {
            Log.i("ThingsManagerCallback : onBootStrapCallback",
                    "Received Callback from JNI");
            configurationListener.onBootStrapCallback(headerOptions, rep,
                    errorValue);
        }
    }

    /**
     * This callback method is called when a response for the reboot request
     * just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onRebootCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != diagnosticsListener) {
            Log.i("ThingsManagerCallback : onRebootCallback",
                    "Received Callback from JNI");
            diagnosticsListener
                    .onRebootCallback(headerOptions, rep, errorValue);
        }
    }

    /**
     * This callback method is called when a response for the factoryReset
     * request just arrives.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onFactoryResetCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        if (null != diagnosticsListener) {
            Log.i("ThingsManagerCallback : onFactoryResetCallback",
                    "Received Callback from JNI");
            diagnosticsListener.onFactoryResetCallback(headerOptions, rep,
                    errorValue);
        }
    }

    /**
     * This callback method is called to notify whether group is found or not.
     *
     * @param resource
     *            Resource URI
     *
     */
    public void onGroupFindCallback(OcResource resource) {
        if (null != groupListener) {
            Log.i("ThingsManagerCallback : onGroupFindCallback",
                    "Received Callback from JNI");
            groupListener.onGroupFindCallback(resource);
        }
    }

    /**
     * This callback method is called for child resource presence status.
     *
     * @param resource
     *            URI of resource.
     * @param errorValue
     *            error code.
     *
     */
    public void onPresenceCallback(String resource, int errorValue) {
        if (null != presenceListener) {
            Log.i("ThingsManagerCallback : onPresenceCallback",
                    "Received Callback from JNI");
            presenceListener.onPresenceCallback(resource, errorValue);
        }
    }

}
