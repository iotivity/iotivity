/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
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
package org.iotivity.cloud.accountserver.oauth;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.HashMap;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.util.Logger;

/**
 *
 * This class provides factory method for OAuth2.0 Servers.
 *
 */
public class OAuthServerFactory {

    private Class<?> authServerClass = null;
    private Object   authServerObj   = null;

    @SuppressWarnings("unchecked")
    public String requestAccessToken(String authCode, String authServerUrl) {
        String accessToken = null;
        HashMap<String, String> authServerInfo = null;

        try {
            Method method = authServerClass.getMethod("requestAccessToken",
                    new Class[] { String.class, String.class });
            authServerInfo = (HashMap<String, String>) method
                    .invoke(authServerObj, authCode, authServerUrl);
            String errorMessage = authServerInfo.get(Constants.ERROR_MESSAGE);
            if (errorMessage != null) {
                Logger.d("Exception Error Message from Auth Server : "
                        + errorMessage);
            } else {
                accessToken = authServerInfo.get(Constants.KEY_ACCESS_TOKEN_GH);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return accessToken;
    }

    @SuppressWarnings("unchecked")
    public String requestGetUserInfo(String accessToken, String apiServerUrl) {
        String userId = null;
        HashMap<String, String> authServerInfo = null;

        try {
            Method method = authServerClass.getMethod("requestGetUserInfo",
                    new Class[] { String.class, String.class });
            authServerInfo = (HashMap<String, String>) method
                    .invoke(authServerObj, accessToken, apiServerUrl);
            String errorMessage = authServerInfo.get(Constants.ERROR_MESSAGE);
            if (errorMessage != null) {
                Logger.d("Exception Error Message from Auth Server : "
                        + errorMessage);
            } else {
                userId = authServerInfo.get(Constants.KEY_USER_ID);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return userId;
    }

    /**
     * API for creating OAuth server object
     * 
     * @param authServer
     *            authorization server
     * @return OAuthServer - object to handle authorization
     * @throws Exception
     */
    public boolean create(String authServer) {

        // TODO : 3rd-party provider check logic required
        String authServerName = checkAuthServerName(authServer);
        String fileName = authServerName + ".jar";
        File jarFile = new File(fileName);
        URLClassLoader classLoader = null;

        try {
            URL urls = new URL("jar:" + jarFile.toURI() + "!/");

            Logger.d("urls: " + urls.toString());

            classLoader = new URLClassLoader(new URL[] { urls });
            Class<?> gitHubClass = classLoader
                    .loadClass(this.getClass().getPackage().getName() + "."
                            + authServerName);

            authServerClass = gitHubClass;
            Object object = gitHubClass.newInstance();
            authServerObj = object;

        } catch (Exception e) {
            e.printStackTrace();
            return false;

        } finally {
            try {
                classLoader.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        return true;
    }

    private String checkAuthServerName(String authServer) {

        String authServerName = null;

        if (authServer.equalsIgnoreCase(Constants.GITHUB)) {
            authServerName = Constants.GITHUB;
        } else if (authServer.equalsIgnoreCase(Constants.SAMSUNG)) {
            authServerName = Constants.SAMSUNG;
        }

        return authServerName;
    }
}
