/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/

package org.iotivity.cloud;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.CookieHandler;
import java.net.CookiePolicy;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.net.CookieManager;

import android.os.AsyncTask;
import android.util.Log;

import org.iotivity.configuration.IConfiguration;
import org.iotivity.configuration.ReadConfigPropFile;

public class GetAuthCode extends AsyncTask<Void, String, String>
        implements IConfiguration {

    static HttpURLConnection urlConnection = null;
    static String            authCode      = null;

    @Override
    protected String doInBackground(Void... uri) {
        String responseString = null;
        try {

            CookieManager cookieManager = new CookieManager(null,
                    CookiePolicy.ACCEPT_ALL);
            CookieHandler.setDefault(cookieManager);

            // get initial login page
            responseString = getRequest(INIT_GET_LOGIN_URL);
            int responseCode = urlConnection.getResponseCode();
            String authText = getAuthenticationToken(responseString);
            String authURLText = URLEncoder.encode(authText, "UTF-8");

            // prepare post data
            String demoPostData = PAR_POST_VAL + AUTHENTICITY_TOKEN
                    + authURLText + "&login="
                    + ReadConfigPropFile.s_GitHubUserId + "&password="
                    + ReadConfigPropFile.s_GitHubUserPassword;

            // Post request for login
            responseString = postRequest(LOGIN_URL, demoPostData);
            responseCode = urlConnection.getResponseCode();
            Log.d(COMMON_TAG, "postRequest Response Code : " + responseCode);

            // get request for auth code
            responseString = getRequest(IOTIVITY_CLOUD_URL);
            responseCode = urlConnection.getResponseCode();
            Log.d(COMMON_TAG, "getRequest Response Code : " + responseCode);

            authCode = getAuthenticationCode(responseString);

            if (authCode.isEmpty()) {
                System.out.println("AuthCode Not Found !!!");
            } else
                System.out.println("AUTH CODE:" + authCode);

        } catch (Exception ex) {
            System.out.println("Exception Occurred:" + ex.getMessage());
        }

        return authCode;
    }

    // Function for Get Request
    public static String getRequest(String sUrl) {
        BufferedReader reader = null;
        String strRet = null;
        try {
            URL url = new URL(sUrl);

            // Create the request to OpenWeatherMap, and open the connection
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod("GET");
            urlConnection.connect();

            // Read the input stream into a String
            InputStream inputStream = urlConnection.getInputStream();
            StringBuffer buffer = new StringBuffer();
            if (inputStream == null) {
                // Nothing to do.
                return null;
            }
            reader = new BufferedReader(new InputStreamReader(inputStream));

            String line;
            while ((line = reader.readLine()) != null) {
                buffer.append(line + "\n");
            }

            if (buffer.length() == 0) {
                // Stream was empty. No point in parsing.
                return null;
            }

            strRet = buffer.toString();
        } catch (Exception ex) {
            System.out.println("Exception Occured:" + ex.getMessage());
        }

        return strRet;
    }

    // Function for Post Request
    public static String postRequest(String sUrl, String urlParameters) {
        BufferedReader reader = null;
        String strRet = null;
        try {
            URL url = new URL(sUrl);

            // Create the request to OpenWeatherMap, and open the connection
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod("POST");
            urlConnection.setDoOutput(true);

            DataOutputStream write = new DataOutputStream(
                    urlConnection.getOutputStream());
            write.writeBytes(urlParameters);
            write.flush();
            write.close();

            // Read the input stream into a String
            InputStream inputStream = urlConnection.getInputStream();
            StringBuffer buffer = new StringBuffer();
            if (inputStream == null) {
                // Nothing to do.
                return null;
            }
            reader = new BufferedReader(new InputStreamReader(inputStream));

            String line;
            while ((line = reader.readLine()) != null) {
                buffer.append(line + "\n");
            }

            if (buffer.length() == 0) {
                return null;
            }

            strRet = buffer.toString();
        } catch (Exception ex) {
            System.out.println("Exception Occured:" + ex.getMessage());
        }

        return strRet;
    }

    public String getAuthenticationToken(String htmlbody) {
        String auth_token = htmlbody.substring(htmlbody.indexOf(
                "<input name=\"authenticity_token\" type=\"hidden\" value=\"")
                + 1);
        auth_token = auth_token.substring(
                auth_token.indexOf("value=\"") + "value=\"".length(),
                auth_token.indexOf("\" />"));
        return auth_token;
    }

    // get Authentication Code
    public String getAuthenticationCode(String htmlbody) {
        String auth_token = htmlbody.substring(htmlbody.indexOf("<a href=") + 1,
                htmlbody.indexOf("a>"));
        String authcode = auth_token.substring(
                auth_token.indexOf("?code=") + "?code=".length(),
                auth_token.indexOf("\">"));
        return authcode;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);

    }
}
