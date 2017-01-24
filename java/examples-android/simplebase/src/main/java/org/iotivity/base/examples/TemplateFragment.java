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
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

/**
 * This class the template for the test.
 */
public class TemplateFragment extends Fragment implements View.OnClickListener {

    private Activity mActivity;
    private Context  mContext;

    Button           buttonA;
    Button           buttonB;
    Button           buttonC;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_template, container, false);

        buttonA = (Button) rootView.findViewById(R.id.btn_A);
        buttonB = (Button) rootView.findViewById(R.id.btn_B);
        buttonC = (Button) rootView.findViewById(R.id.btn_C);

        buttonA.setOnClickListener(this);
        buttonB.setOnClickListener(this);
        buttonC.setOnClickListener(this);

        return rootView;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = getActivity();
        mContext = mActivity.getBaseContext();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View view) {
        StringBuilder sb = new StringBuilder(getString(R.string.action_onclick));
        switch (view.getId()) {
            case R.id.btn_A:
                sb.append(getString(R.string.button_A));
                break;
            case R.id.btn_B:
                sb.append(getString(R.string.button_B));
                break;
            case R.id.btn_C:
                sb.append(getString(R.string.button_C));
                break;
        }
        Common.showToast(mContext, sb.toString());
    }
}