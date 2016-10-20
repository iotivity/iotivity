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

package org.iotivity.base.examples.cloudprovisioningclient;

import android.app.Activity;
import android.content.Intent;
import android.net.UrlQuerySanitizer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import org.iotivity.base.examples.cloudprovisioningclient.R;

/**
 * This class is for login to the provider.
 * Can be get auth code via web page.
 */
public class LoginActivity extends Activity {
    private static final String TAG = "OIC_SIMPLE_LOGIN";

    private WebView mWebView = null;

    @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_login);

            mWebView = (WebView) findViewById(R.id.webView);
            mWebView.setInitialScale(200);
            mWebView.getSettings().setJavaScriptEnabled(true);
            mWebView.getSettings().setBuiltInZoomControls(true);
            mWebView.setWebViewClient(new WebViewClientClass());

            mWebView.loadUrl("https://github.com/login?return_to=%2Flogin%2Foauth" +
                    "%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri" +
                    "%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F");
        }

    private class WebViewClientClass extends WebViewClient {

        @Override
            public void onPageFinished(WebView view, String url) {
                Log.i(TAG, "called url=" + url);

                if (url.contains("http://www.example.com/oauth_callback")) {

                    mWebView.setVisibility(View.INVISIBLE);

                    // parsing url
                    UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
                    sanitizer.setAllowUnregisteredParamaters(true);
                    sanitizer.parseUrl(url);

                    String mAuthCode = sanitizer.getValue("code");

                    Intent intent = getIntent();
                    intent.putExtra("authCode", mAuthCode);
                    setResult(RESULT_OK, intent);

                    finish();
                }
            }
    }
}
