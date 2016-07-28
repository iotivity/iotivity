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

import java.util.HashMap;
import org.apache.oltu.oauth2.client.OAuthClient;
import org.apache.oltu.oauth2.client.URLConnectionClient;
import org.apache.oltu.oauth2.client.request.OAuthBearerClientRequest;
import org.apache.oltu.oauth2.client.request.OAuthClientRequest;
import org.apache.oltu.oauth2.client.response.GitHubTokenResponse;
import org.apache.oltu.oauth2.client.response.OAuthResourceResponse;
import org.apache.oltu.oauth2.common.OAuth;
import org.apache.oltu.oauth2.common.OAuthProviderType;
import org.apache.oltu.oauth2.common.exception.OAuthProblemException;
import org.apache.oltu.oauth2.common.exception.OAuthSystemException;
import org.apache.oltu.oauth2.common.message.types.GrantType;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.util.JSONUtil;
import org.iotivity.cloud.util.Log;

/**
 *
 * This class provides a sample to make .jar that communicates with OAuth2
 * provider.
 *
 */
public class Github implements OAuthServer {

    // do not use 'client_id' and 'secret' variables.
    // should use values that are obtained from github.
    final static private String client_id    = "ea9c18f540323b0213d0";
    final static private String secret       = "4bc0cd9fe21269507eb8eba3a32664a0f598dbc9";
    final static private String resource_url = "https://api.github.com/user";

    @Override
    public HashMap<String, String> requestAccessToken(String authCode,
            String authServerUrl) {

        HashMap<String, String> authServerInfo = new HashMap<String, String>();

        try {

            OAuthClientRequest request = OAuthClientRequest
                    .tokenProvider(OAuthProviderType.GITHUB)
                    .setGrantType(GrantType.AUTHORIZATION_CODE)
                    .setClientId(client_id).setClientSecret(secret)
                    .setCode(authCode).buildBodyMessage();

            OAuthClient oAuthClient = new OAuthClient(
                    new URLConnectionClient());
            GitHubTokenResponse oAuthResponse = oAuthClient.accessToken(request,
                    GitHubTokenResponse.class);

            authServerInfo.put(Constants.KEY_ACCESS_TOKEN_GH,
                    oAuthResponse.getAccessToken());

        } catch (OAuthSystemException | OAuthProblemException e) {
            authServerInfo.put(Constants.ERROR_MESSAGE, e.getMessage());
        }

        return authServerInfo;
    }

    @Override
    public HashMap<String, String> requestGetUserInfo(String accessToken,
            String apiServerUrl) {

        HashMap<String, String> authServerInfo = new HashMap<String, String>();

        String userInfo = null;

        if (accessToken == null) {
            Log.w("accessToken is null!");
            return null;
        }

        try {

            OAuthClientRequest request = new OAuthBearerClientRequest(
                    resource_url).setAccessToken(accessToken)
                            .buildQueryMessage();

            OAuthClient oAuthClient = new OAuthClient(
                    new URLConnectionClient());
            OAuthResourceResponse resourceResponse = oAuthClient.resource(
                    request, OAuth.HttpMethod.GET, OAuthResourceResponse.class);

            userInfo = resourceResponse.getBody();
            Log.d("userInfo: " + userInfo);

        } catch (OAuthSystemException | OAuthProblemException e) {
            authServerInfo.put(Constants.ERROR_MESSAGE, e.getMessage());
        }

        String userIdKey = "login";
        JSONUtil<HashMap<String, String>> util = new JSONUtil<HashMap<String, String>>();
        HashMap<String, String> parsedData = util.parseJSON(userInfo,
                HashMap.class);
        String userId = parsedData.get(userIdKey);
        authServerInfo.put(Constants.KEY_USER_ID, userId);

        return authServerInfo;
    }
}
