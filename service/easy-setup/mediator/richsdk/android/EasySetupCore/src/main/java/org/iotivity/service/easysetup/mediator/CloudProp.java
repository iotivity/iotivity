/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

/**
 * This class contains on cloud provisioning information.
 * It implements ProvisioningInfo interface and provide configuration object specific to cloud registration
 */
public class CloudProp {

    private final String mAuthCode;
    private final String mAuthProvider;
    private final String mCiServer;

    public CloudProp(String authCode, String authProvider, String ciServer) {
        mAuthCode = authCode;
        mAuthProvider = authProvider;
        mCiServer = ciServer;
    }

    /**
     * This method returns the authCode used for the first registration to IoTivity cloud
     * @return AuthCode for sign-up to IoTivity cloud
     */
    public String getAuthCode() {
        return mAuthCode;
    }

    /**
     * This method returns the auth provider which issued the given AuthCode
     * @return Auth provider which issued the given AuthCode
     */
    public String getAuthProvider() {
        return mAuthProvider;
    }

	/**
     * This method returns the Cloud Interface server's URL to be registered
     * @return CI server's URL to be registered
     */
    public String getCiServer() {
        return mCiServer;
    }
}
