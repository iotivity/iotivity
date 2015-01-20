/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

/**
 * Data structure to provide the configuration. ServiceType: indicate IN_PROC or OUT_OF_PROC
 * ModeType : indicate whether we want to do server, client or both ipAddress : ip address of
 * server. if you specify 0.0.0.0 : it listens on any interface. port : port of server. : if you
 * specify 0 : next available random port is used. : if you specify 5683 : client discovery can
 * work even if they don't specify port.
 */
public class PlatformConfig {

    private ServiceType mServiceType;
    private ModeType mModeType;
    private String mIpAddress;
    private int mPort;
    private QualityOfService mQualityOfService;

    public PlatformConfig(ServiceType serviceType,
                          ModeType modeType,
                          String ipAddress,
                          int port,
                          QualityOfService qualityOfService) {
        this.mServiceType = serviceType;
        this.mModeType = modeType;
        this.mIpAddress = ipAddress;
        this.mPort = port;
        this.mQualityOfService = qualityOfService;
    }

    public ServiceType getServiceType() {
        return mServiceType;
    }

    public ModeType getModeType() {
        return mModeType;
    }

    public String getIpAddress() {
        return mIpAddress;
    }

    public int getPort() {
        return mPort;
    }

    public QualityOfService getQualityOfService() {
        return mQualityOfService;
    }
}
