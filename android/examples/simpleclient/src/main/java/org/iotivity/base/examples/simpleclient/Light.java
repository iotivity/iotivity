/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
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

package org.iotivity.base.examples.simpleclient;

/**
 * Light
 *
 * This class is used by simpleclient to create an object and update the values depending on the server response
 */
public class Light {
    private boolean mState;
    private int mPower;
    private String mName;

    // constructor
    Light() {
        mState = false;
        mPower = 0;
        mName = "";
    }

    // accessor methods
    public boolean getState() {
        return mState;
    }
    void setState(boolean state) {
        mState = state;
    }
    int getPower() {
        return mPower;
    }
    void setPower(int power) {
        mPower = power;
    }
    String getName() {
        return mName;
    }
    void setName(String name) {
        mName = name;
    }
}




