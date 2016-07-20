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

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.util.Logger;

public class TokenValidator {

    Boolean verifyAccessToken(String token) {

        Boolean valid = false;

        // check token existed
        valid = AccountDBManager.getInstance().hasAccessToken(token);
        if (!valid) {
            Logger.w("accessToken is not existed..");
            return false;
        }

        // check token expired
        if (getElaspedSeconds(token) > TokenPolicy.EXPIRES_IN) {

            Logger.w("accessToken is expired..");
            return false;
        }

        return true;
    }

    public long getElaspedSeconds(String token) {

        String time = AccountDBManager.getInstance().getIssuedTime(token);

        DateFormat format = new SimpleDateFormat("yyyyMMddkkmm");
        Date currentTime = new Date();
        Date issuedTime = null;

        try {
            issuedTime = format.parse(time);
        } catch (ParseException e) {
            e.printStackTrace();
        }

        long difference = currentTime.getTime() - issuedTime.getTime();
        long elaspedSeconds = difference / 1000;
        // Logger.d("currentTime : " + currentTime.getTime());
        // Logger.d("issuedTime : " + issuedTime.getTime());
        // Logger.d("difference : " + difference);
        Logger.d("accessToken elasped time: " + elaspedSeconds + "s");

        return elaspedSeconds;
    }

    public long getRemainedSeconds(String token) {
        return TokenPolicy.EXPIRES_IN - getElaspedSeconds(token);
    }

    Boolean verifyRefreshToken(String token) {

        Boolean valid = false;
        // check token existed
        valid = AccountDBManager.getInstance().hasRefreshToken(token);
        if (!valid)
            Logger.w("refreshToken is not existed..");

        return valid;
    }

}
