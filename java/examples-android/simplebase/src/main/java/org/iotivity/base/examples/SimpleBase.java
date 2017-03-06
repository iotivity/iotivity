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

package org.iotivity.base.examples;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.util.Log;

/**
 * SimpleBase is a sample OCF Base layer application.
 * It can handle message and bluetooth manually.
 */
public class SimpleBase extends Activity implements DrawerFragment.DrawerCallbacks {

    private static final String TAG = "OIC_SIMPLE_BASE";
    private DrawerFragment      mDrawerFragment;

    private CharSequence        mTitle;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_simplebase);

        mDrawerFragment = (DrawerFragment) getFragmentManager()
                .findFragmentById(R.id.navigation_drawer);
        mTitle = getTitle();

        mDrawerFragment.setUp(R.id.navigation_drawer,
                (DrawerLayout) findViewById(R.id.drawer_layout));
    }

    @Override
    public void onDrawerItemSelected(int position) {
        Fragment fragment;
        switch (position) {
            case 0:
                mTitle = getString(R.string.title_message);
                fragment = new MessageFragment();
                break;
            case 1:
                mTitle = getString(R.string.title_bluetooth);
                fragment = new BluetoothFragment();
                break;
            case 2:
                mTitle = getString(R.string.title_cloud);
                fragment = new CloudFragment();
                break;
            case 3:
                mTitle = getString(R.string.title_template);
                fragment = new TemplateFragment();
                break;
            default:
                mTitle = getString(R.string.title_message);
                fragment = new MessageFragment();
                break;
        }
        getFragmentManager().beginTransaction()
                            .replace(R.id.container, fragment)
                            .commit();

        getActionBar().setTitle(mTitle);
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Log.d(TAG, "onNewIntent with changes sending broadcast IN ");

        Intent i = new Intent();
        i.setAction(intent.getAction());
        i.putExtra(NfcAdapter.EXTRA_NDEF_MESSAGES,
                intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES));
        sendBroadcast(i);
        Log.d(TAG, "Initialize Context again resetting");
    }
}
