/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import android.content.Context;
import android.app.Activity;
import java.util.EnumSet;
/**
 * Data structure to provide the configuration.
 */
public class PlatformConfig {
    private Activity mActivity;
    private Context mContext;
    private ServiceType mServiceType;
    private ModeType mModeType;
    private QualityOfService mQualityOfService;
    private String mSvrDbPath; //TODO: Instead of SVRDB file, it should be Persistent Storage.
                              //this is only for 0.9.2
    private String mIntrospectPath;
    private int mTransportType;

    /**
     * @param activity           app activity
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param ipAddress          not used
     * @param port               not used
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     * @param introspectionPath  Persistent storage file for introspection data.
     *
     * @deprecated ipAddress and port are no longer used
     */
    @Deprecated
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService,
                          String dbPath,
                          String introspectionPath) {
        mActivity = activity;
        mContext = context;
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
        mIntrospectPath = introspectionPath;
    }

    /**
     * @param activity         app activity
     * @param context          app context
     * @param serviceType      indicate IN_PROC or OUT_OF_PROC
     * @param modeType         indicate whether we want to do server, client or both
     * @param ipAddress        not used
     * @param port             not used
     * @param qualityOfService quality of service
     * @param dbPath           Persistent storage file for SVR Database.
     *
     * @deprecated ipAddress and port are no longer used
     */
    @Deprecated
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService,
                          String dbPath) {
        mActivity = activity;
        mContext = context;
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
    }

    /**
     * @param context          app context
     * @param serviceType      indicate IN_PROC or OUT_OF_PROC
     * @param modeType         indicate whether we want to do server, client or both
     * @param ipAddress        not used
     * @param port             not used
     * @param qualityOfService quality of service
     * @param dbPath           Persistent storage file for SVR Database.
     *
     * @deprecated ipAddress and port are no longer used
     */
    @Deprecated
    public PlatformConfig(Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService,
                          String dbPath) {
        this(null, context, serviceType, modeType, ipAddress, port, qualityOfService, dbPath);
    }

    /**
     * @param context          app context
     * @param serviceType      indicate IN_PROC or OUT_OF_PROC
     * @param modeType         indicate whether we want to do server, client or both
     * @param ipAddress        not used
     * @param port             not used
     * @param qualityOfService quality of service
     *
     * @deprecated ipAddress and port are no longer used
     */
    @Deprecated
    // Avoid breaking building java samples due to persistent storage SVR DB changes.
    public PlatformConfig(Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService) {
        this(null, context, serviceType, modeType, ipAddress, port, qualityOfService, "");
    }

    /**
     * @param activity         app activity
     * @param context          app context
     * @param serviceType      indicate IN_PROC or OUT_OF_PROC
     * @param modeType         indicate whether we want to do server, client or both
     * @param ipAddress        not used
     * @param port             not used
     * @param qualityOfService quality of service
     *
     * @deprecated ipAddress and port are no longer used
     */
    @Deprecated
    // Avoid breaking building java samples due to persistent storage SVR DB changes.
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService) {
        this(activity, context, serviceType, modeType, ipAddress, port, qualityOfService, "");
    }

    /**
     * @param activity           app activity
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     * @param introspectionPath  Persistent storage file for introspection data.
     */
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath,
                          String introspectionPath) {
        mActivity = activity;
        mContext = context;
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
        mIntrospectPath = introspectionPath;
    }

    /**
     * @param activity           app activity
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     */
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath) {
        this(activity, context, serviceType, modeType, qualityOfService, dbPath, null);
    }

    /**
     * @param activity           app activity
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     */
    public PlatformConfig(Activity activity,
                          Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService) {
        this(activity, context, serviceType, modeType, qualityOfService, "", null);
    }

    /**
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     * @param introspectionPath  Persistent storage file for introspection data.
     */
    public PlatformConfig(Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath,
                          String introspectionPath) {
        this(null, context, serviceType, modeType, qualityOfService, dbPath, introspectionPath);
    }

    /**
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     */
    public PlatformConfig(Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath) {
        this(null, context, serviceType, modeType, qualityOfService, dbPath, null);
    }

    /**
     * @param context            app context
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     */
    public PlatformConfig(Context context,
                          ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService) {
        this(null, context, serviceType, modeType, qualityOfService, "", null);
    }

    public Context getContext() {
        return mContext;
    }

    public ServiceType getServiceType() {
        return mServiceType;
    }

    public ModeType getModeType() {
        return mModeType;
    }

    public QualityOfService getQualityOfService() {
        return mQualityOfService;
    }

    public String getSvrDbPath() {
        return mSvrDbPath;
    }

    public String getIntrospectionPath() {
        return mIntrospectPath;
    }

    public Activity getActivity() {
        return mActivity;
    }

    public void setAvailableTransportType(EnumSet<OcConnectivityType> type) {
        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (type.contains(connType))
                mTransportType |= connType.getValue();
        }
    }

    public int getAvailableTransportType() {
        return mTransportType;
    }
}
