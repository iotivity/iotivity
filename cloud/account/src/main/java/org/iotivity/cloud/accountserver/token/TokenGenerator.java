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
package org.iotivity.cloud.accountserver.token;

import org.apache.oltu.oauth2.as.issuer.MD5Generator;
import org.apache.oltu.oauth2.as.issuer.OAuthIssuer;
import org.apache.oltu.oauth2.as.issuer.OAuthIssuerImpl;
import org.apache.oltu.oauth2.common.exception.OAuthSystemException;
import org.iotivity.cloud.util.Logger;

public class TokenGenerator {

    String generateAccessToken() {

        OAuthIssuer oauthIssuerImpl = new OAuthIssuerImpl(new MD5Generator());

        String accessToken = null;

        try {

            accessToken = oauthIssuerImpl.accessToken();
            Logger.d("accessToken : " + accessToken);

        } catch (OAuthSystemException e) {

            e.printStackTrace();
        }

        return accessToken;
    }

    String generateRefreshToken() {

        OAuthIssuer oauthIssuerImpl = new OAuthIssuerImpl(new MD5Generator());

        String refreshToken = null;

        try {

            refreshToken = oauthIssuerImpl.refreshToken();
            Logger.d("refreshToken : " + refreshToken);

        } catch (OAuthSystemException e) {

            e.printStackTrace();
        }

        return refreshToken;
    }
}
