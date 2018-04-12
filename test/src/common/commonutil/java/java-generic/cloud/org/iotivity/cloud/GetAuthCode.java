/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.CookieHandler;
import java.net.CookieManager;
import java.net.CookiePolicy;
import java.net.CookieStore;
import java.net.HttpCookie;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.List;
import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;

import javax.net.ssl.X509TrustManager;
import java.security.cert.X509Certificate;

import org.iotivity.configuration.IConfiguration;
import org.iotivity.configuration.ReadConfigPropFile;

public class GetAuthCode implements IConfiguration {

    private static String            s_mLogInCookies       = "_ga=GA1.2.428254156.1503511513; _gat=1; tz=Asia%2FDhaka";
    private static String            s_mUserAgent    = "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0";
    private static CookieManager     s_mCookieManager = null;
    private static HttpURLConnection s_mUrlConnection          = null;

    private static String getResponse(URL url, String postData) {
        String body = "";
        String cookies = "";

        Reader inputBufferReader = null;
        String headerName = "";

        try {
            SSLContext sc = SSLContext.getInstance("TLS");
            sc.init(null, new TrustManager[] { new TrustAllX509TrustManager() },
                    new java.security.SecureRandom());
            HttpsURLConnection
                    .setDefaultSSLSocketFactory(sc.getSocketFactory());
            HttpsURLConnection
                    .setDefaultHostnameVerifier(new HostnameVerifier() {
                        public boolean verify(String string, SSLSession ssls) {
                            return true;
                        }
                    });

            s_mUrlConnection = (HttpURLConnection) url.openConnection();
            s_mUrlConnection.setInstanceFollowRedirects(false);

            s_mUrlConnection.setRequestProperty("User-Agent", s_mUserAgent);
            s_mUrlConnection.setRequestProperty("Accept", "*/*");

            s_mUrlConnection.setRequestProperty("charset", "utf-8");
            s_mUrlConnection.setConnectTimeout(30000);
            List<HttpCookie> listCookie = s_mCookieManager.getCookieStore()
                    .getCookies();

            for (HttpCookie cCookie : listCookie) {
                cookies += cCookie.getName() + "=" + cCookie.getValue() + ";";
            }
            if (!cookies.isEmpty()) {
                cookies += s_mLogInCookies;
                s_mUrlConnection.setRequestProperty("Cookie", cookies);
            }

            if (postData != null) {
                s_mUrlConnection.setRequestProperty("Content-Type",
                        "application/x-www-form-urlencoded");
                s_mUrlConnection.setRequestProperty("Content-Length",
                        String.valueOf(postData.length()));
                s_mUrlConnection.setRequestMethod("POST");
                s_mUrlConnection.setDoOutput(true);
                s_mUrlConnection.getOutputStream().write(postData.getBytes("UTF-8"));
            } else {
                s_mUrlConnection.setRequestMethod("GET");
                s_mUrlConnection.setRequestProperty("Content-Type", "text/plain");
            }

            for (int i = 1; (headerName = s_mUrlConnection
                    .getHeaderFieldKey(i)) != null; i++) {
                if (headerName.equals("Set-Cookie")) {
                    String cookie = s_mUrlConnection.getHeaderField(i);
                    if (!cookie.isEmpty()) {
                        cookie = cookie.substring(0, cookie.indexOf(";"));
                        String cookieName = cookie.substring(0,
                                cookie.indexOf("="));
                        String cookieValue = cookie.substring(
                                cookie.indexOf("=") + 1, cookie.length());
                        s_mCookieManager.getCookieStore().add(
                                s_mUrlConnection.getURL().toURI(),
                                new HttpCookie(cookieName, cookieValue));
                    }
                }

            }
            inputBufferReader = new BufferedReader(
                    new InputStreamReader(s_mUrlConnection.getInputStream(), "UTF-8"));

            StringBuilder sb = new StringBuilder();
            for (int c; (c = inputBufferReader.read()) >= 0;)
                sb.append((char) c);
            body = sb.toString();
            inputBufferReader.close();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        return body;
    }

    public static String getAuthenticationCode(String htmlbody) {
        String auth_token = htmlbody.substring(htmlbody.indexOf("<a href=") + 1,
                htmlbody.indexOf("a>"));
        String authcode = auth_token.substring(
                auth_token.indexOf("?code=") + "?code=".length(),
                auth_token.indexOf("\">"));
        return authcode;
    }

    public static String redirectUrl() {
        String pageResponse = null;
        boolean redirect = false;
        int status;
        try {
            status = s_mUrlConnection.getResponseCode();

            if (status != HttpURLConnection.HTTP_OK) {
                if (status == HttpURLConnection.HTTP_MOVED_TEMP
                        || status == HttpURLConnection.HTTP_MOVED_PERM
                        || status == HttpURLConnection.HTTP_SEE_OTHER)
                    redirect = true;
            }

            System.out.println("Response Code ... " + status);

            if (redirect) {
                // get redirect url from "location" header field
                String newUrl = s_mUrlConnection.getHeaderField("Location");
                pageResponse = getResponse(new URL(newUrl), null);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return pageResponse;
    }

    public static String getAuthenticationToken(String htmlbody) {
        String auth_token = htmlbody.substring(htmlbody.indexOf(
                "<input name=\"authenticity_token\" type=\"hidden\" value=\"")
                + 1);
        auth_token = auth_token.substring(
                auth_token.indexOf("value=\"") + "value=\"".length(),
                auth_token.indexOf("\" />"));
        return auth_token;
    }

    public static String login() {
        String accessToken = null;
        String authCode = "";
        String pageResponse = "";
        try {
            s_mCookieManager = new CookieManager(null, CookiePolicy.ACCEPT_ALL);
            URL url = new URL(INIT_GET_LOGIN_URL);
            pageResponse = getResponse(url, null);

            String authText = getAuthenticationToken(pageResponse);
            String authURLText = URLEncoder.encode(authText, "UTF-8");

            String demoPostData = PAR_POST_VAL + AUTHENTICITY_TOKEN
                    + authURLText + "&login="
                    + URLEncoder.encode(ReadConfigPropFile.s_mGitHubUserId,
                            "UTF-8")
                    + "&password=" + URLEncoder.encode(
                            ReadConfigPropFile.s_mGitHubUserPassword, "UTF-8");

            pageResponse = "";
            pageResponse = getResponse(new URL(LOGIN_URL), demoPostData);
            pageResponse = redirectUrl();

            pageResponse = getResponse(new URL(IOTIVITY_CLOUD_URL), null);
            pageResponse = redirectUrl();

            authCode = getAuthenticationCode(pageResponse);

            if (authCode.isEmpty()) {
                System.out.println("AuthCode Not Found !!!");
            } else {
                System.out.println("AUTH CODE:" + authCode);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return authCode;
    }

    public static class TrustAllX509TrustManager implements X509TrustManager {
        public X509Certificate[] getAcceptedIssuers() {
            return new X509Certificate[0];
        }

        public void checkClientTrusted(
                java.security.cert.X509Certificate[] certs, String authType) {
        }

        public void checkServerTrusted(
                java.security.cert.X509Certificate[] certs, String authType) {
        }
    }

}
