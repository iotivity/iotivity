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

import java.util.EnumSet;

/**
 * Data structure to provide the configuration.
 */
public class PlatformConfig {
    private ServiceType mServiceType;
    private ModeType mModeType;
    private QualityOfService mQualityOfService;
    private String mSvrDbPath; //TODO: Instead of SVRDB file, it should be Persistent Storage.
                              //this is only for 0.9.2
    private String mIntrospectPath;
    private int mTransportType;

    /**
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
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService,
                          String dbPath,
                          String introspectionPath) {
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
        mIntrospectPath = introspectionPath;
    }

    /**
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
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService,
                          String dbPath) {
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
    }

    /**
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
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService) {
        this(serviceType, modeType, qualityOfService, "");
    }


    /**
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     */
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService) {
        this(serviceType, modeType, qualityOfService, "");
    }

    /**
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     */
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath) {
        this(serviceType, modeType, qualityOfService, dbPath, null);
    }

    /**
     * @param serviceType        indicate IN_PROC or OUT_OF_PROC
     * @param modeType           indicate whether we want to do server, client or both
     * @param qualityOfService   quality of service
     * @param dbPath             Persistent storage file for SVR Database.
     * @param introspectionPath  Persistent storage file for introspection data.
     */
    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          QualityOfService qualityOfService,
                          String dbPath,
                          String introspectionPath) {
        mServiceType = serviceType;
        mModeType = modeType;
        mQualityOfService = qualityOfService;
        mSvrDbPath = dbPath;
        mIntrospectPath = introspectionPath;
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
