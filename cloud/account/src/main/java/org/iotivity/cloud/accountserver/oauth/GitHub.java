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
import org.iotivity.cloud.util.JSONUtil;
import org.iotivity.cloud.util.Logger;

/**
 *
 * This class provides APIs relating authorization for GitHub.
 *
 */
public class GitHub extends OAuthServer {

    final static private String client_id    = "ea9c18f540323b0213d0";
    final static private String secret       = "4bc0cd9fe21269507eb8eba3a32664a0f598dbc9";
    final static private String resource_url = "https://api.github.com/user";

    @Override
    public String requestAccessToken(String authCode) {

        String accessToken = null;

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

            accessToken = oAuthResponse.getAccessToken();

        } catch (OAuthSystemException | OAuthProblemException e) {
            e.printStackTrace();
        }

        return accessToken;
    }

    @Override
    public String requestGetUserInfo(String accessToken) {

        String userInfo = null;

        if (accessToken == null) {
            Logger.w("accessToken is null!");
            return null;
        }

        try {

            OAuthClientRequest request = new OAuthBearerClientRequest(
                    resource_url).setAccessToken(accessToken)
                    .buildQueryMessage();

            OAuthClient oAuthClient = new OAuthClient(new URLConnectionClient());
            
            OAuthResourceResponse resourceResponse = oAuthClient.resource(
                    request, OAuth.HttpMethod.GET, OAuthResourceResponse.class);

            userInfo = resourceResponse.getBody();
            Logger.d("userInfo: " + userInfo);

        } catch (OAuthSystemException | OAuthProblemException e) {
            e.printStackTrace();
        }

        String userIdKey = "login";
        String userId = JSONUtil.parseJSON(userInfo, userIdKey);

        return userId;
    }
}
