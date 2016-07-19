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

import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.util.Logger;

public class TokenManager {

    private TokenGenerator tokenGenerator;
    private TokenValidator tokenValidator;

    public TokenManager() {
        tokenGenerator = new TokenGenerator();
        tokenValidator = new TokenValidator();
    }

    public Token generateToken() {

        String accessToken = tokenGenerator.generateAccessToken();
        String refreshToken = tokenGenerator.generateRefreshToken();

        Token token = new Token();
        token.setToken(accessToken, refreshToken);

        return token;
    }

    public Boolean verifyAccessToken(String token) {

        return tokenValidator.verifyAccessToken(token);
    }

    public Boolean verifyRefreshToken(String token) {

        return tokenValidator.verifyRefreshToken(token);
    }

    public long getRemainExpiredTime(String token) {

        return tokenValidator.getRemainedSeconds(token);
    }

    public Token refreshToken(String token) {

        String userId = null;

        Token refreshedToken = new Token();

        Boolean verifyRefreshToken = false;
        verifyRefreshToken = tokenValidator.verifyRefreshToken(token);

        if (!verifyRefreshToken) {
            Logger.w("RefreshToken is not existed!");
            return null;
        }

        refreshedToken = generateToken();

        userId = AccountDBManager.getInstance().getUserIdByRefreshToken(token);

        AccountDBManager.getInstance().updateUserToken(userId, token,
                refreshedToken.getAccessToken(),
                refreshedToken.getRefreshToken());

        return refreshedToken;
    }

}
