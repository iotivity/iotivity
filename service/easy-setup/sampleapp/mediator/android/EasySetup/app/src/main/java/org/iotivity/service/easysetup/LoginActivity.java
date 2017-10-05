/**
 * ***************************************************************
 * <p/>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p/>
 * ****************************************************************
 */

package org.iotivity.service.easysetup;

import android.app.Activity;
import android.content.Intent;
import android.net.UrlQuerySanitizer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class LoginActivity extends Activity {
    private static final String TAG = "Easysetup Login: ";

    /* Samsung account */
    private WebView mWebView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        /* For Samsung account authentifaction */
        mWebView = (WebView) findViewById(R.id.webView);
        mWebView.setVisibility(View.VISIBLE);
        mWebView.setInitialScale(200);
        mWebView.getSettings().setJavaScriptEnabled(true);
        mWebView.getSettings().setBuiltInZoomControls(true);
        mWebView.setWebViewClient(new WebViewClientClass());

        mWebView.loadUrl("https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F");
    }

    private class WebViewClientClass extends WebViewClient {

        @Override
        public void onPageFinished(WebView view, String url) {
            Log.e(TAG, "called!!! url=" + url);

            if(url.contains("http://www.example.com/oauth_callback/")){

                mWebView.setVisibility(View.INVISIBLE);

                //parsing url
                UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
                sanitizer.setAllowUnregisteredParamaters(true);
                sanitizer.parseUrl(url);

                String mAuthCode = sanitizer.getValue("code");

                Intent intent = getIntent();
                intent.putExtra("authCode", mAuthCode);
                intent.putExtra("authProvider", "github");

                setResult(RESULT_OK, intent);

                finish();
            }
        }
    }
}
