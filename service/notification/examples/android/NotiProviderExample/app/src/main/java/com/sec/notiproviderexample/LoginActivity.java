/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package com.sec.notiproviderexample;
import android.app.Activity;
import android.content.Intent;
import android.net.UrlQuerySanitizer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;


/**
 * This class is for login to the provider.
 * Can be get auth code via web page.
 */
public class LoginActivity extends Activity {
    private static final String TAG = "OIC_SIMPLE_LOGIN";

    private WebView mWebView = null;
    private final String samsungAccount = "https://account.samsung.com/mobile/account/check.do?serviceID=166135d296&actionID=StartOAuth2&countryCode="+ "US" + "&languageCode=" + "en";
    private final String mAuthProvider = "samsung-us";


    @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_login);

            mWebView = (WebView) findViewById(R.id.webView);
            mWebView.setInitialScale(200);
            mWebView.getSettings().setJavaScriptEnabled(true);
            mWebView.getSettings().setBuiltInZoomControls(true);
            mWebView.setWebViewClient(new WebViewClientClass());

            mWebView.loadUrl(samsungAccount);
        }
    public void onDestroy() {
        super.onDestroy();
    }

    private class WebViewClientClass extends WebViewClient {

        @Override
            public void onPageFinished(WebView view, String url) {
                Log.i(TAG, "onPageFinished!!! Response received: called url=" + url);

                    if (url.contains("code") && url.contains("code_expires_in")) {

                    mWebView.setVisibility(View.INVISIBLE);

                    // parsing url
                    UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
                    sanitizer.setAllowUnregisteredParamaters(true);
                    sanitizer.parseUrl(url);

                    String mAuthCode = sanitizer.getValue("code");
                    Log.i(TAG, "onPageFinished!!! authCode=" + mAuthCode);

                    Intent intent = getIntent();
                    intent.putExtra("authCode", mAuthCode);
                    intent.putExtra("authProvider", mAuthProvider);
                    setResult(RESULT_OK, intent);

                    finish();
                }
            }
    }
}
